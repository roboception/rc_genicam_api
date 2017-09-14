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

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>

namespace
{

/*
  Computes a point cloud from the given synchronized left and disparity image
  pair and stores it in ply ascii format.

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

void storePointCloud(double f, double t, double scale,
                     std::shared_ptr<const rcg::Image> left,
                     std::shared_ptr<const rcg::Image> disp,
                     std::shared_ptr<const rcg::Image> conf=0,
                     std::shared_ptr<const rcg::Image> error=0)
{
  // get size and scale factor between left image and disparity image

  uint32_t width=disp->getWidth();
  uint32_t height=disp->getHeight();
  bool bigendian=disp->isBigEndian();
  int ds=(left->getWidth()+disp->getWidth()-1)/disp->getWidth();

  // convert focal length factor into focal length in (disparity) pixels

  f*=width;

  // get pointer to disparity data and size of row in bytes

  const uint8_t *dps=disp->getPixels();
  size_t dstep=disp->getWidth()*sizeof(uint16_t)+disp->getXPadding();

  // count number of valid disparities

  int n=0;
  for (uint32_t k=0; k<height; k++)
  {
    int j=0;
    for (uint32_t i=0; i<width; i++)
    {
      if ((dps[j]|dps[j+1]) != 0) n++;
      j+=2;
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

  std::ostringstream name;
  double timestamp=left->getTimestampNS()/1000000000.0;
  name << "rc_vizard_" << std::setprecision(16) << timestamp << ".ply";

  std::ofstream out(name.str());

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
  out << "end_header" << std::endl;

  // create point cloud

  for (uint32_t k=0; k<height; k++)
  {
    for (uint32_t i=0; i<width; i++)
    {
      // convert disparity from fixed comma 16 bit integer into float value

      double d;
      if (bigendian)
      {
        uint32_t j=i<<1;
        d=scale*((dps[j]<<8)|dps[j+1]);
      }
      else
      {
        uint32_t j=i<<1;
        d=scale*((dps[j+1]<<8)|dps[j]);
      }

      // if disparity is valid and color can be obtained

      if (d > 0)
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
        rcg::getColor(rgb, left, ds, i, k);

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

  out.close();
}

}

int main(int argc, char *argv[])
{
  try
  {
    if (argc >= 2)
    {
      // find specific device accross all systems and interfaces and open it

      std::shared_ptr<rcg::Device> dev=rcg::getDevice(argv[1]);

      if (dev)
      {
        dev->open(rcg::Device::CONTROL);
        std::shared_ptr<GenApi::CNodeMapRef> nodemap=dev->getRemoteNodeMap();

        // get focal length, baseline and disparity scale factor

        double f=rcg::getFloat(nodemap, "FocalLengthFactor", 0, 0, false);
        double t=rcg::getFloat(nodemap, "Baseline", 0, 0, true);
        double scale=rcg::getFloat(nodemap, "Scan3dCoordinateScale", 0, 0, true);

        // sanity check of some parameters

        rcg::checkFeature(nodemap, "Scan3dOutputMode", "DisparityC");
        rcg::checkFeature(nodemap, "Scan3dCoordinateOffset", "0");
        rcg::checkFeature(nodemap, "Scan3dInvalidDataFlag", "1");
        rcg::checkFeature(nodemap, "Scan3dInvalidDataValue", "0");

        // set to color format if available

        rcg::setEnum(nodemap, "PixelFormat", "YCbCr411_8", false);

        // enable left image and disparity image and disable all other streams

        {
          std::vector<std::string> component;

          rcg::getEnum(nodemap, "ComponentSelector", component, true);

          for (size_t i=0; i<component.size(); i++)
          {
            rcg::setEnum(nodemap, "ComponentSelector", component[i].c_str(), true);

            bool enable=(component[i] == "Intensity" || component[i] == "Disparity" ||
                         component[i] == "Confidence" || component[i] == "Error");
            rcg::setBoolean(nodemap, "ComponentEnable", enable, true);
          }
        }

        // open image stream

        std::vector<std::shared_ptr<rcg::Stream> > stream=dev->getStreams();

        if (stream.size() > 0)
        {
          // opening first stream

          stream[0]->open();
          stream[0]->startStreaming();

          // prepare buffers for time synchronization of images (buffer at most
          // 25 images in each list)

          rcg::ImageList left_list(25);
          rcg::ImageList disp_list(25);
          rcg::ImageList conf_list(25);
          rcg::ImageList error_list(25);

          bool run=true;
          int async=0, maxasync=50; // maximum number of asynchroneous images before giving up

          while (run && async < maxasync)
          {
            async++;

            // grab next image with timeout of 0.5 seconds

            const rcg::Buffer *buffer=stream[0]->grab(500);
            if (buffer != 0)
            {
              // check for a complete image in the buffer

              if (!buffer->getIsIncomplete() && buffer->getImagePresent())
              {
                // store image in the corresponding list

                uint64_t pixelformat=buffer->getPixelFormat();
                if (pixelformat == Mono8 || pixelformat == YCbCr411_8)
                {
                  left_list.add(buffer);
                }
                else if (pixelformat == Coord3D_C16)
                {
                  disp_list.add(buffer);
                }
                else if (pixelformat == Confidence8)
                {
                  conf_list.add(buffer);
                }
                else if (pixelformat == Error8)
                {
                  error_list.add(buffer);
                }

                // check if both lists contain an image with the current time
                // stamp

                uint64_t timestamp=buffer->getTimestampNS();

                std::shared_ptr<const rcg::Image> left=left_list.find(timestamp);
                std::shared_ptr<const rcg::Image> disp=disp_list.find(timestamp);
                std::shared_ptr<const rcg::Image> conf=conf_list.find(timestamp);
                std::shared_ptr<const rcg::Image> error=error_list.find(timestamp);

                if (left && disp && conf && error)
                {
                  // compute and store point cloud from synchronized image pair

                  storePointCloud(f, t, scale, left, disp, conf, error);

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
    else
    {
      // show help

      std::cout << argv[0] << " [interface-id>:]<device-id>" << std::endl;
      std::cout << std::endl;
      std::cout << "Gets the first synchronized image set consisting of left, disparity, confidence" << std::endl;
      std::cout << "and error image, creates a point cloud and stores it in ply ascii format" << std::endl;
      std::cout << "in the current directory." << std::endl;
      std::cout << std::endl;
      std::cout << "<device-id> Device from which images will taken" << std::endl;
    }
  }
  catch (const std::exception &ex)
  {
    std::cerr << ex.what() << std::endl;
  }

  rcg::System::clearSystems();

  return 0;
}
