/*
 * This file is part of the rc_genicam_api package.
 *
 * Copyright (c) 2017 Roboception GmbH
 * All rights reserved
 *
 * Author: Heiko Hirschmueller
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <rc_genicam_api/system.h>
#include <rc_genicam_api/interface.h>
#include <rc_genicam_api/device.h>
#include <rc_genicam_api/stream.h>
#include <rc_genicam_api/buffer.h>
#include <rc_genicam_api/image.h>
#include <rc_genicam_api/imagelist.h>
#include <rc_genicam_api/config.h>

#include <rc_genicam_api/pixel_formats.h>

#include <Base/GCException.h>

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <algorithm>

#ifdef WIN32
#undef min
#undef max
#endif

namespace
{

/*
  Print help text on standard output.
*/

void printHelp(const char *prgname)
{
  // show help

  std::cout << prgname << " [-o <output-filename>] [<interface-id>:]<device-id>" << std::endl;
  std::cout << std::endl;
  std::cout << "Gets the first synchronized image set consisting of left, disparity, confidence" << std::endl;
  std::cout << "and error image, creates a point cloud and stores it in ply ascii format" << std::endl;
  std::cout << "in the current directory." << std::endl;
  std::cout << std::endl;
  std::cout << "<device-id> Device from which images will taken" << std::endl;
}

/*
  Get the i-th 16 bit value.

  @param p         Pointer to first byte of array of 16 bit values.
  @param bigendian True if values are given as big endian. Otherwise, litte
                   endian is assumed.
  @param i         Index of 16 bit inside the given array.
*/

inline uint16_t getUint16(const uint8_t *p, bool bigendian, size_t i)
{
  uint16_t ret;

  if (bigendian)
  {
    size_t j=i<<1;
    ret=static_cast<uint16_t>(((p[j]<<8)|p[j+1]));
  }
  else
  {
    size_t j=i<<1;
    ret=static_cast<uint16_t>(((p[j+1]<<8)|p[j]));
  }

  return ret;
}

/*
  Computes a point cloud from the given synchronized left and disparity image
  pair and stores it in ply ascii format.

  @param name  Name of output file. If empty, a standard file name with
               timestamp is used.
  @param f     Focal length factor (to be multiplicated with image width).
  @param t     Baseline in m.
  @param scale Disparity scale factor.
  @param left  Left camera image. The image must have format Mono8 or
               YCbCr411_8.
  @param disp  Corresponding disparity image, possibly downscaled by an integer
               factor. The image must be in format Coord3D_C16.
  @param conf  Optional corresponding confidence image in the same size as
               disp. The image must be in format Confidence8.
  @param error Optional corresponding error image in the same size as disp. The
               image must be in format Error8.
*/

void storePointCloud(std::string name, double f, double t, double scale,
                     std::shared_ptr<const rcg::Image> left,
                     std::shared_ptr<const rcg::Image> disp,
                     std::shared_ptr<const rcg::Image> conf=0,
                     std::shared_ptr<const rcg::Image> error=0)
{
  // get size and scale factor between left image and disparity image

  size_t width=disp->getWidth();
  size_t height=disp->getHeight();
  bool bigendian=disp->isBigEndian();
  size_t ds=(left->getWidth()+disp->getWidth()-1)/disp->getWidth();

  // convert focal length factor into focal length in (disparity) pixels

  f*=width;

  // get pointer to disparity data and size of row in bytes

  const uint8_t *dps=disp->getPixels();
  size_t dstep=disp->getWidth()*sizeof(uint16_t)+disp->getXPadding();

  // count number of valid disparities and store vertice index in a temporary
  // index image

  size_t vi=0;
  const uint32_t vinvalid=0xffffffff;
  std::vector<uint32_t> vindex(width*height);

  uint32_t n=0;
  for (size_t k=0; k<height; k++)
  {
    int j=0;
    for (size_t i=0; i<width; i++)
    {
      vindex[vi]=vinvalid;
      if ((dps[j]|dps[j+1]) != 0) vindex[vi]=n++;

      j+=2;
      vi++;
    }

    dps+=dstep;
  }

  dps=disp->getPixels();

  // count number of triangles

  const uint16_t vstep=static_cast<uint16_t>(std::ceil(2/scale));

  int tn=0;
  for (size_t k=1; k<height; k++)
  {
    for (size_t i=1; i<width; i++)
    {
      uint16_t v[4];
      v[0]=getUint16(dps, bigendian, i-1);
      v[1]=getUint16(dps, bigendian, i);
      v[2]=getUint16(dps+dstep, bigendian, i-1);
      v[3]=getUint16(dps+dstep, bigendian, i);

      uint16_t vmin=65535;
      uint16_t vmax=0;
      int valid=0;

      for (int jj=0; jj<4; jj++)
      {
        if (v[jj])
        {
          vmin=std::min(vmin, v[jj]);
          vmax=std::max(vmax, v[jj]);
          valid++;
        }
      }

      if (valid >= 3 && vmax-vmin <= vstep)
      {
        tn+=valid-2;
      }
    }

    dps+=dstep;
  }

  dps=disp->getPixels();

  // get pointer to optional confidence and error data and size of row in bytes

  const uint8_t *cps=0, *eps=0;
  size_t cstep=0, estep=0;

  if (conf)
  {
    cps=conf->getPixels();
    cstep=conf->getWidth()*sizeof(uint8_t)+conf->getXPadding();
  }

  if (error)
  {
    eps=error->getPixels();
    estep=error->getWidth()*sizeof(uint8_t)+error->getXPadding();
  }

  // open output file and write ASCII PLY header

  if (name.size() == 0)
  {
    std::ostringstream os;
    double timestamp=left->getTimestampNS()/1000000000.0;
    os << "rc_visard_" << std::setprecision(16) << timestamp << ".ply";
    name=os.str();
  }

  std::ofstream out(name);

  out << "ply" << std::endl;
  out << "format ascii 1.0" << std::endl;
  out << "comment Created with gc_pointcloud from Roboception GmbH" << std::endl;
  out << "comment Camera [1 0 0; 0 1 0; 0 0 1] [0 0 0]" << std::endl;
  out << "element vertex " << n << std::endl;
  out << "property float32 x" << std::endl;
  out << "property float32 y" << std::endl;
  out << "property float32 z" << std::endl;
  out << "property float32 scan_size" << std::endl; // i.e. size of 3D point

  if (cps != 0)
  {
    out << "property float32 scan_conf" << std::endl; // optional confidence
  }

  if (eps != 0)
  {
    out << "property float32 scan_error" << std::endl; // optional error in 3D along line of sight
  }

  out << "property uint8 diffuse_red" << std::endl;
  out << "property uint8 diffuse_green" << std::endl;
  out << "property uint8 diffuse_blue" << std::endl;
  out << "element face " << tn << std::endl;
  out << "property list uint8 uint32 vertex_indices" << std::endl;
  out << "end_header" << std::endl;

  // create colored point cloud

  for (size_t k=0; k<height; k++)
  {
    for (size_t i=0; i<width; i++)
    {
      // convert disparity from fixed comma 16 bit integer into float value

      double d=scale*getUint16(dps, bigendian, i);

      // if disparity is valid and color can be obtained

      if (d)
      {
        // reconstruct 3D point from disparity value

        double x=(i+0.5-0.5*width)*t/d;
        double y=(k+0.5-0.5*height)*t/d;
        double z=f*t/d;

        // compute size of reconstructed point

        double x2=(i-0.5*width)*t/d;
        double size=2*1.4*std::abs(x2-x);

        // get corresponding color value

        uint8_t rgb[3];
        rcg::getColor(rgb, left, static_cast<uint32_t>(ds), static_cast<uint32_t>(i),
                      static_cast<uint32_t>(k));

        // store colored point, optionally with confidence and error

        out << x << " " << y << " " << z << " " << size << " ";

        if (cps != 0)
        {
          out << cps[i]/255.0 << " ";
        }

        if (eps != 0)
        {
          out << eps[i]*scale*f*t/(d*d) << " ";
        }

        out << static_cast<int>(rgb[0]) << " ";
        out << static_cast<int>(rgb[1]) << " ";
        out << static_cast<int>(rgb[2]) << std::endl;
      }
    }

    dps+=dstep;
    cps+=cstep;
    eps+=estep;
  }

  dps=disp->getPixels();

  // create triangles

  uint32_t *ips=vindex.data();
  for (size_t k=1; k<height; k++)
  {
    for (size_t i=1; i<width; i++)
    {
      uint16_t v[4];
      v[0]=getUint16(dps, bigendian, i-1);
      v[1]=getUint16(dps, bigendian, i);
      v[2]=getUint16(dps+dstep, bigendian, i-1);
      v[3]=getUint16(dps+dstep, bigendian, i);

      uint16_t vmin=65535;
      uint16_t vmax=0;
      int valid=0;

      for (int jj=0; jj<4; jj++)
      {
        if (v[jj])
        {
          vmin=std::min(vmin, v[jj]);
          vmax=std::max(vmax, v[jj]);
          valid++;
        }
      }

      if (valid >= 3 && vmax-vmin <= vstep)
      {
        int j=0;
        uint32_t fc[4];

        if (ips[i-1] != vinvalid)
        {
          fc[j++]=ips[i-1];
        }

        if (ips[width+i-1] != vinvalid)
        {
          fc[j++]=ips[width+i-1];
        }

        if (ips[width+i] != vinvalid)
        {
          fc[j++]=ips[width+i];
        }

        if (ips[i] != vinvalid)
        {
          fc[j++]=ips[i];
        }

        out << "3 " << fc[0] << ' ' << fc[1] << ' ' << fc[2] << std::endl;

        if (j == 4)
        {
          out << "3 " << fc[2] << ' ' << fc[3] << ' ' << fc[0] << std::endl;
        }
      }
    }

    ips+=width;
    dps+=dstep;
  }

  out.close();
}

}

int main(int argc, char *argv[])
{
  try
  {
    // optional parameters

    std::string name="";

    int i=1;

    while (i+1 < argc)
    {
      if (std::string(argv[i]) == "-o")
      {
        i++;
        name=argv[i++];
      }
      else
      {
        std::cout << "Unknown parameter: " << argv[i] << std::endl;
        std::cout << std::endl;

        printHelp(argv[0]);

        return 1;
      }
    }

    if (i >= argc)
    {
      printHelp(argv[0]);
      return 1;
    }

    // find specific device accross all systems and interfaces and open it

    std::shared_ptr<rcg::Device> dev=rcg::getDevice(argv[i]);

    if (dev)
    {
      dev->open(rcg::Device::CONTROL);
      std::shared_ptr<GenApi::CNodeMapRef> nodemap=dev->getRemoteNodeMap();

      // get chunk adapter (this switches chunk mode on if possible and
      // returns a null pointer if this is not possible)

      std::shared_ptr<GenApi::CChunkAdapter> chunkadapter=rcg::getChunkAdapter(nodemap, dev->getTLType());

      // get focal length, baseline and disparity scale factor

      double f=rcg::getFloat(nodemap, "FocalLengthFactor", 0, 0, false);
      double t=rcg::getFloat(nodemap, "Baseline", 0, 0, true);
      double scale=rcg::getFloat(nodemap, "Scan3dCoordinateScale", 0, 0, true);

      // check for special exposure alternate mode of rc_visard and
      // corresponding filter and set tolerance accordingly

      // (The exposure alternate mode is typically used with a random dot
      // projector connected to Out1. Alternate means that Out1 is high for
      // every second image. The rc_visard calculates disparities from images
      // with Out1=High. However, if the alternate filter is set to OnlyLow,
      // then it is gueranteed that Out1=Low (i.e. projector off) for all
      // rectified images. Thus, rectified images and disparity images are
      // always around 40 ms appart, which must be taken into account for
      // synchronization.)

      uint64_t tol=0;

      try
      {
        rcg::setEnum(nodemap, "LineSelector", "Out1", true);
        std::string linesource=rcg::getEnum(nodemap, "LineSource", true);
        std::string filter=rcg::getEnum(nodemap, "AcquisitionAlternateFilter", true);

        if (linesource == "ExposureAlternateActive" && filter == "OnlyLow")
        {
          tol=50*1000*1000; // set tolerance to 50 ms
        }
      }
      catch (const std::exception &)
      {
        // ignore possible errors
      }

      // sanity check of some parameters

      rcg::checkFeature(nodemap, "Scan3dOutputMode", "DisparityC");
      rcg::checkFeature(nodemap, "Scan3dCoordinateOffset", "0");
      rcg::checkFeature(nodemap, "Scan3dInvalidDataFlag", "1");
      rcg::checkFeature(nodemap, "Scan3dInvalidDataValue", "0");

      // set to color format if available

      rcg::setEnum(nodemap, "PixelFormat", "YCbCr411_8", false);

      // enable left image, disparity, confidence and error image and disable
      // all others

      {
        std::vector<std::string> component;

        rcg::getEnum(nodemap, "ComponentSelector", component, true);

        for (size_t k=0; k<component.size(); k++)
        {
          rcg::setEnum(nodemap, "ComponentSelector", component[k].c_str(), true);

          bool enable=(component[k] == "Intensity" || component[k] == "Disparity" ||
                       component[k] == "Confidence" || component[k] == "Error");
          rcg::setBoolean(nodemap, "ComponentEnable", enable, true);
        }
      }

      // try getting synchronized data (which only has an effect if the device
      // and GenTL producer support multipart)

      rcg::setString(nodemap, "AcquisitionMultiPartMode", "SynchronizedComponents");

      // open image stream

      std::vector<std::shared_ptr<rcg::Stream> > stream=dev->getStreams();

      if (stream.size() > 0)
      {
        // opening first stream

        stream[0]->open();
        stream[0]->startStreaming();

        // prepare buffers for time synchronization of images (buffer at most
        // 25 images in each list)

        rcg::ImageList left_list(50);
        rcg::ImageList disp_list(25);
        rcg::ImageList conf_list(25);
        rcg::ImageList error_list(25);

        bool run=true;
        int async=0, maxasync=50; // maximum number of asynchroneous images before giving up

        while (run && async < maxasync)
        {
          async++;

          // grab next image with timeout

          const rcg::Buffer *buffer=stream[0]->grab(5000);
          if (buffer != 0)
          {
            // check for a complete image in the buffer

            if (!buffer->getIsIncomplete())
            {
              // attach buffer to nodemap for accessing chunk data if possible

              if (chunkadapter)
              {
                chunkadapter->AttachBuffer(
                  reinterpret_cast<std::uint8_t *>(buffer->getGlobalBase()),
                                                   static_cast<int64_t>(buffer->getSizeFilled()));
              }

              // go through all parts in case of multi-part buffer

              size_t partn=buffer->getNumberOfParts();
              for (uint32_t part=0; part<partn; part++)
              {
                if (buffer->getImagePresent(part))
                {
                  // store image in the corresponding list

                  uint64_t left_tol=0;
                  uint64_t disp_tol=0;

                  std::string component=rcg::getComponetOfPart(nodemap, buffer, part);

                  if (component == "Intensity")
                  {
                    left_list.add(buffer, part);
                    disp_tol=tol;
                  }
                  else if (component == "Disparity")
                  {
                    disp_list.add(buffer, part);
                    left_tol=tol;
                  }
                  else if (component == "Confidence")
                  {
                    conf_list.add(buffer, part);
                    left_tol=tol;
                  }
                  else if (component == "Error")
                  {
                    error_list.add(buffer, part);
                    left_tol=tol;
                  }

                  // get corresponding left and disparity images

                  uint64_t timestamp=buffer->getTimestampNS();
                  std::shared_ptr<const rcg::Image> left=left_list.find(timestamp, left_tol);
                  std::shared_ptr<const rcg::Image> disp=disp_list.find(timestamp, disp_tol);

                  // get confidence and error images that correspond to the
                  // disparity image

                  std::shared_ptr<const rcg::Image> conf;
                  std::shared_ptr<const rcg::Image> error;

                  if (disp)
                  {
                    conf=conf_list.find(disp->getTimestampNS());
                    error=error_list.find(disp->getTimestampNS());
                  }

                  if (left && disp && conf && error)
                  {
                    // compute and store point cloud from synchronized image pair

                    storePointCloud(name, f, t, scale, left, disp, conf, error);

                    // remove all images from the buffer with the current or an
                    // older time stamp

                    async=0;
                    left_list.removeOld(timestamp);
                    disp_list.removeOld(timestamp);
                    conf_list.removeOld(timestamp);
                    error_list.removeOld(timestamp);

                    // in this example, we exit the grabbing loop after receiving
                    // the first synchronized image pair

                    run=false;
                  }
                }
              }

              // detach buffer from nodemap

              if (chunkadapter) chunkadapter->DetachBuffer();
            }
          }
          else
          {
            std::cerr << "Cannot grab images" << std::endl;
            break;
          }
        }

        // report if synchronization failed

        if (async >= maxasync && run)
        {
          std::cerr << "Cannot grab synchronized left and disparity image" << std::endl;
        }

        // stopping and closing image stream

        stream[0]->stopStreaming();
        stream[0]->close();
      }
      else
      {
        std::cerr << "No streams available" << std::endl;
      }

      // closing the communication to the device

      dev->close();
    }
    else
    {
      std::cerr << "Device '" << argv[1] << "' not found!" << std::endl;
    }
  }
  catch (const std::exception &ex)
  {
    std::cerr << ex.what() << std::endl;
  }
  catch (const GENICAM_NAMESPACE::GenericException &ex)
  {
    std::cerr << "Exception: " << ex.what() << std::endl;
  }
  catch (...)
  {
    std::cerr << "Unknown exception!" << std::endl;
  }

  rcg::System::clearSystems();

  return 0;
}
