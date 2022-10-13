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
#include <rc_genicam_api/pointcloud.h>
#include <rc_genicam_api/config.h>

#include <rc_genicam_api/pixel_formats.h>

#include <Base/GCException.h>

namespace
{

/*
  Print help text on standard output.
*/

void printHelp(const char *prgname)
{
  // show help

  std::cout << prgname << " -h | [-o <output-filename>] [<interface-id>:]<device-id> [<key>=<value>] ..." << std::endl;
  std::cout << std::endl;
  std::cout << "Gets the first synchronized image set of the Roboception rc_visard, consisting of left, disparity, confidence and error image, creates a point cloud and stores it in ply ascii format." << std::endl;
  std::cout << std::endl;
  std::cout << "Options:" << std::endl;
  std::cout << "-h        Prints help information and exits" << std::endl;
  std::cout << "-o <file> Set name of output file (default is 'rc_visard_<timestamp>.ply')" << std::endl;
  std::cout << std::endl;
  std::cout << "Parameters:" << std::endl;
  std::cout << "<interface-id> Optional GenICam ID of interface for connecting to the device" << std::endl;
  std::cout << "<device-id>    GenICam device ID, serial number or user defined name of device" << std::endl;
  std::cout << "<key>=<value>  Optional GenICam parameters to be changed in the given order" << std::endl;
}

}

int main(int argc, char *argv[])
{
  int ret=0;

  try
  {
    // optional parameters

    std::string name="";

    int i=1;

    while (i < argc && argv[i][0] == '-')
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

    if (i >= argc || std::string(argv[i]) == "-h")
    {
      printHelp(argv[0]);
      return 1;
    }

    // find specific device accross all systems and interfaces and open it

    std::shared_ptr<rcg::Device> dev=rcg::getDevice(argv[i++]);

    if (dev)
    {
      dev->open(rcg::Device::CONTROL);
      std::shared_ptr<GenApi::CNodeMapRef> nodemap=dev->getRemoteNodeMap();

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

      // set values as given on the command line

      while (i < argc)
      {
        // split argument in key and value

        std::string key=argv[i++];
        std::string value;

        size_t k=key.find('=');
        if (k != std::string::npos)
        {
          value=key.substr(k+1);
          key=key.substr(0, k);
        }

        if (value.size() > 0)
        {
          rcg::setString(nodemap, key.c_str(), value.c_str(), true);
        }
        else
        {
          rcg::callCommand(nodemap, key.c_str(), true);
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

                    rcg::storePointCloud(name, f, t, scale, left, disp, conf, error);

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
          ret=1;
        }

        // stopping and closing image stream

        stream[0]->stopStreaming();
        stream[0]->close();
      }
      else
      {
        std::cerr << "No streams available" << std::endl;
        ret=1;
      }

      // closing the communication to the device

      dev->close();
    }
    else
    {
      std::cerr << "Device '" << argv[1] << "' not found!" << std::endl;
      ret=1;
    }
  }
  catch (const std::exception &ex)
  {
    std::cerr << ex.what() << std::endl;
    ret=2;
  }
  catch (const GENICAM_NAMESPACE::GenericException &ex)
  {
    std::cerr << "Exception: " << ex.what() << std::endl;
    ret=2;
  }
  catch (...)
  {
    std::cerr << "Unknown exception!" << std::endl;
    ret=2;
  }

  rcg::System::clearSystems();

  return ret;
}
