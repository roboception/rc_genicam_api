/*
 * This file is part of the rc_genicam_api package.
 *
 * Copyright (c) 2026 Roboception GmbH
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
#include <rc_genicam_api/image_store.h>
#include <rc_genicam_api/config.h>

#include <rc_genicam_api/pixel_formats.h>

#include <Base/GCException.h>

#include <iostream>

#ifdef _WIN32
#undef min
#undef max
#endif

/**
  This is a straight forward example for streaming images via rc_genicam_api.
  This 2D example should work with most cameras.
*/

int main(int argc, char *argv[])
{
  int ret=0;

  try
  {
    // use first parameter as serial number or name of camera

    if (argc != 2)
    {
      std::cerr << argv[0] << " <camera-serial-number-or-name>" << std::endl;
      return 1;
    }

    std::string id=argv[1];

    // find specific device accross all systems and interfaces and open it

    std::shared_ptr<rcg::Device> dev=rcg::getDevice(id.c_str());

    if (dev)
    {
      // open connection to device with control rights

      dev->open(rcg::Device::CONTROL);

      // get nodemap for setting initial parameters

      std::shared_ptr<GenApi::CNodeMapRef> nodemap=dev->getRemoteNodeMap();

      // try to enable chunks (allowed to fail in case chunks are not supported)

      if (!rcg::setBoolean(nodemap, "ChunkModeActive", true, false))
      {
        std::cout << "The camera does not support chunk data." << std::endl;
      }

      // setting some common GenICam parameters (we specify false to continue
      // even if setting them fails)
      //
      // explore all parameters of the camera with 'gc_info -e <serial-or-name>'

      // check camera for supported formats
      //
      // rcg::setEnum(nodemap, "PixelFormat", "Mono8", false);
      rcg::setEnum(nodemap, "ExposureAuto", "Continuous", false);

      // alternative for manual exposure and gain
      //
      // rcg::setEnum(nodemap, "ExposureAuto", "Off", false);
      // rcg::setFloat(nodemap, "ExposureTime", 5000, false); // exposure time in us
      // rcg::setFloat(nodemap, "Gain", 0, false); // gain in dB

      // open first stream and start streaming

      std::vector<std::shared_ptr<rcg::Stream> > stream=dev->getStreams();

      if (stream.size() > 0)
      {
        stream[0]->open();
        stream[0]->attachBuffers(true); // for getting chunk data if enabled
        stream[0]->startStreaming();

        int n=3; // number of images that we want to receive

        for (int k=0; k<n; k++)
        {
          // grab next image with timeout of 3 seconds

          const rcg::Buffer *buffer=stream[0]->grab(3000);

          if (buffer != 0)
          {
            // incomplete buffers mean network problems, most commonly due to
            // overload of a switch or because the network buffer on the
            // computer is too small

            if (!buffer->getIsIncomplete())
            {
              // here we can access chunk data, i.e. meta data of this image
              //
              // NOTE: The following code will return 0 if chunk data is not
              // enabled or the requested parameters do not exist due to giving
              // 'false' as last parameter. Change to 'true' if an exception
              // should be thrown.

              std::cout << std::endl;
              std::cout << "Timestamp:     " << rcg::getInteger(nodemap, "ChunkTimestamp", 0, 0, false) << " ns" << std::endl;
              std::cout << "Exposure time: " << rcg::getFloat(nodemap, "ChunkExposureTime", 0, 0, false) << " us" << std::endl;
              std::cout << "Gain:          " << rcg::getFloat(nodemap, "ChunkGain", 0, 0, false) << " dB" << std::endl;

              // a buffer can contain multiple parts, e.g. images, for normal
              // cameras there is only one image per buffer

              uint32_t npart=buffer->getNumberOfParts();
              for (uint32_t part=0; part<npart; part++)
              {
                if (buffer->getImagePresent(part)) // check if part contains an image
                {
                  // The image size and length of one line can be requested with:
                  //
                  // width = buffer->getWidth(part)
                  // full_line_size = width + buffer->getXPadding(part)
                  // height = buffer->getHeight(part)
                  //
                  // The raw pixel data starts at 'buffer->getBase(part)'.
                  // Images can come in a lot of different formats. The actual
                  // pixel format is 'buffer->getPixelFormat(part)'. For formats
                  // with more than one byte per value 'buffer->isBigEndian()'
                  // must be considered to interpret the data.
                  //
                  // The conversion of some common formats to plain RGB or
                  // monochrome image can be done with 'convertImage()' function
                  // in module 'image.h'.
                  //
                  // Here, we just copy the image into an own object.

                  rcg::Image image(buffer, part);

                  // try to store image as PNG with fallback to PNM if PNG
                  // support was not compiled into rc_genicam_api

                  std::string name;

                  try
                  {
                    name=storeImage("image", rcg::PNG, image);
                  }
                  catch (const std::exception &)
                  {
                    name=storeImage("image", rcg::PNM, image);
                  }

                  std::cout << "Stored image: " << name << std::endl;
                }
              }
            }
            else
            {
              std::cerr << "Incomplete buffer received. Check network buffer size or reduce bandwidth." << std::endl;
            }
          }
          else
          {
            std::cerr << "Timeout while waiting for image." << std::endl;
            break;
          }
        }

        // stop streaming and close stream

        stream[0]->stopStreaming();
        stream[0]->close();
      }
      else
      {
        std::cerr << "No streams available." << std::endl;
        ret=1;
      }

      // close device

      dev->close();
    }
    else
    {
      std::cerr << "Camera '" << id << "' not found!" << std::endl;
      ret=1;
    }
  }
  catch (const std::exception &ex)
  {
    std::cerr << "Exception: " << ex.what() << std::endl;
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

  // clear systems before exiting program, otherwise segmentation fault can
  // happen with some GenICam producers

  rcg::System::clearSystems();

  return ret;
}
