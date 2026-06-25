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
#include <rc_genicam_api/pointcloud.h>
#include <rc_genicam_api/config.h>

#include <rc_genicam_api/pixel_formats.h>

#include <Base/GCException.h>

#include <iostream>
#include <iomanip>

#ifdef _WIN32
#undef min
#undef max
#endif

/**
  This is a straight forward example for streaming 3D data via rc_genicam_api.

  This example is meant for rc_visard (NG), rc_viscore and rc_cube devices from
  Roboception GmbH. It may need adaptation for usage with other GenICam
  complient 3D cameras as some settings are specific for Roboception devices.

  NOTE: Lines that are commented out are only suitable for specific cameras.
  This is explained in the individual comments.
*/

int main(int argc, char *argv[])
{
  int ret=0;

  try
  {
    // use first parameter as serial number or name of device

    if (argc != 2)
    {
      std::cerr << argv[0] << " <serial-number-or-name>" << std::endl;
      return 1;
    }

    std::string id=argv[1];

    {
      // if rc_genicam_api is used together with SGM Producer from Roboception
      // ...

      // the absolute path of the producer must be specified in the environment
      // variable GENICAM_GENTL64_PATH, which is usually done system wide, but
      // could be specified here if necessary

      // the GPU memory for SGM can be limited with an environment variable
      // before calling any function of rc_genicam_api (3312 MB is sufficient
      // in most use cases and even allows matching in full quality with
      // rc_viscore, i.e. 12 MPixel, with a minimum depth range, see
      // documentation of SGM Producer)

      setenv("RC_SGM_MAXMEM", "3312", 1);

      // logging SGM Producer activity can be activated with

//      setenv("RC_SGM_PRODUCER_LOG", "/tmp/stream3d.log", 1);
//      setenv("RC_SGM_PRODUCER_LEVEL", "5", 1);
    }

    // find specific device accross all systems and interfaces and open it

    std::shared_ptr<rcg::Device> dev=rcg::getDevice(id.c_str());

    if (dev)
    {
      // open connection to device with control rights

      dev->open(rcg::Device::CONTROL);

      // get nodemap for setting initial parameters

      std::shared_ptr<GenApi::CNodeMapRef> nodemap=dev->getRemoteNodeMap();

      // enable chunk data

      rcg::setBoolean(nodemap, "ChunkModeActive", true, true);

      // the rc_viscore supports limiting the bandwidth for, e.g., using it
      // over an 1G switch

//      rcg::setInteger(nodemap, "DeviceLinkThroughputLimit", 112500000, true);
//      rcg::setEnum(nodemap, "DeviceLinkThroughputLimitMode", "On", true);

      // activating the intensity and disparity image and exlicitely
      // deactivating the other components

      {
        std::vector<std::string> list;
        rcg::getEnum(nodemap, "ComponentSelector", list, true);

        for (size_t i=0; i<list.size(); i++)
        {
          rcg::setEnum(nodemap, "ComponentSelector", list[i].c_str(), true);
          rcg::setBoolean(nodemap, "ComponentEnable",
            (list[i] == "Intensity" || list[i] == "Disparity"), true);
        }
      }

      // if SGM Producer is used, it is also possible to reduce define a
      // region of interest

      rcg::setEnum(nodemap, "ComponentSelector", "Intensity", true);
//      rcg::setInteger(nodemap, "OffsetX", 0, true);
//      rcg::setInteger(nodemap, "OffsetY", 0, true);
//      rcg::setInteger(nodemap, "Width", 2472, true);
//      rcg::setInteger(nodemap, "Height", 2064, true);

      // setting frame rate

      rcg::setFloat(nodemap, "AcquisitionFrameRate", 20, false);

      // set Gamma to a useful value (depending on the camera and scene)

//      rcg::setFloat(nodemap, "Gamma", 0.5, true);

      // using auto exposure with maximum of 20 ms, after that gain is increased

      rcg::setEnum(nodemap, "ExposureAuto", "Continuous", true); // or use AdaptiveOut1
      rcg::setFloat(nodemap, "ExposureTimeAutoMax", 18000, true);

      // image region that is used for auto exposure control,
      // 0 means whole image

      rcg::setInteger(nodemap, "ExposureRegionOffsetX", 0, true);
      rcg::setInteger(nodemap, "ExposureRegionOffsetY", 0, true);
      rcg::setInteger(nodemap, "ExposureRegionWidth", 0, true);
      rcg::setInteger(nodemap, "ExposureRegionHeight", 0, true);

      // alternatively to auto exposure, use HDR

//      rcg::setEnum(nodemap, "ExposureAuto", "HDR", true);

      // alternatively set exposure time and gain manually, e.g. 5 ms, 6 dB

//      rcg::setFloat(nodemap, "ExposureTime", 5000, true);
//      rcg::setFloat(nodemap, "Gain", 6, true);

      // rc_visard NG and rc_viscore support software (and hardware) triggering

      // rcg::setEnum(nodemap, "TriggerSource", "Software", true);
      // rcg::setEnum(nodemap, "TriggerMode", "On", true);
      //
      // send triggers from another thread with:
      //
      // rcg::callCommand(nodemap, "TriggerSoftware", true);

      // Request pre-synchronized data to avoid cumbersome synchronization of
      // intensity and disparity images here in this program

      if (!rcg::setEnum(nodemap, "AcquisitionMultiPartMode", "SynchronizedAlternateComponents", false))
      {
        rcg::setEnum(nodemap, "AcquisitionMultiPartMode", "SynchronizedComponents", true);
      }

      // for rc_viscore or rc_visard with projector, the projector is
      // controlled through GPIO out1

      rcg::setEnum(nodemap, "LineSelector", "Out1", true);
      rcg::setEnum(nodemap, "LineSource", "ExposureAlternateActive", true);

      // the rc_viscore allows to dim the projector by defining a fraction of
      // the exposure time in which it should be turned on

//      rcg::setFloat(nodemap, "RcLineRatio", 1.0, true); // value between 0 and 1

      // useful depth image settings (values below are defaults)

      rcg::setEnum(nodemap, "DepthQuality", "High", true); // best speed / quality compromise
      rcg::setFloat(nodemap, "DepthMinDepth", 0.1, true); // in meter
      rcg::setFloat(nodemap, "DepthMaxDepth", 100, true); // in meter
      rcg::setBoolean(nodemap, "DepthDoubleShot", false, true);
      rcg::setFloat(nodemap, "DepthExposureAdaptTimeout", 1.0, true);

      rcg::setEnum(nodemap, "DepthAcquisitionMode", "Continuous", true);

      // in case of DepthAcquisitionMode=SingleFrame(Out1), depth images
      // can be triggered with
      //
      // rcg::callCommand(nodemap, "DepthAcquisitionTrigger", true);

      // enable metadata and start streaming images

      rcg::setBoolean(nodemap, "ChunkModeActive", true, true);

      std::vector<std::shared_ptr<rcg::Stream> > stream=dev->getStreams();

      if (stream.size() > 0)
      {
        stream[0]->open();
        stream[0]->attachBuffers(true); // for getting chunk data
        stream[0]->startStreaming();

        int n=1; // number of images that we want to receive

        for (int k=0; k<n; k++)
        {
          // grab next image with timeout of 5 seconds

          const rcg::Buffer *buffer=stream[0]->grab(5000);

          if (buffer != 0)
          {
            // incomplete buffers mean network problems, most commonly due to
            // overload of a switch or because the network buffer on the
            // computer is too small

            if (!buffer->getIsIncomplete())
            {
              // getting metadata for interpreting the disparity image and
              // 3D reconstruction

              rcg::setEnum(nodemap, "ChunkComponentSelector", "Disparity", true);

              double f=rcg::getFloat(nodemap, "ChunkScan3dFocalLength", 0, 0, true);
              double t=rcg::getFloat(nodemap, "ChunkScan3dBaseline", 0, 0, true);
              double scale=rcg::getFloat(nodemap, "ChunkScan3dCoordinateScale", 0, 0, true);

              // for Roboception devices, the principal point is always in the
              // image center, offset and inv are 0
              //
              // double u=rcg::getFloat(nodemap, "ChunkScan3dPrincipalPointU", 0, 0, true);
              // double v=rcg::getFloat(nodemap, "ChunkScan3dPrincipalPointV", 0, 0, true);
              // double offset=rcg::getFloat(nodemap, "ChunkScan3dCoordinateOffset", 0, 0, true);
              // double inv=rcg::getFloat(nodemap, "ChunkScan3dInvalidDataValue", 0, 0, true);

              // print effective depth range, which can be less then requested
              // due to camera capability and memory restrictions

              try
              {
                double zmin=rcg::getFloat(nodemap, "ChunkRcMinDepth", 0, 0, true);
                double zmax=rcg::getFloat(nodemap, "ChunkRcMaxDepth", 0, 0, true);

                std::cout << std::endl;
                std::cout << "Effective depth range: " << zmin << " m - "
                  << zmax << " m" << std::endl;
              }
              catch (const std::exception &)
              {
                // silently ignore
              }

              // getting intensity and disparity images from buffer

              std::shared_ptr<rcg::Image> image;
              std::shared_ptr<rcg::Image> disp;

              uint32_t npart=buffer->getNumberOfParts();
              for (uint32_t part=0; part<npart; part++)
              {
                if (buffer->getImagePresent(part)) // check if part contains an image
                {
                  std::string component=rcg::getComponetOfPart(nodemap, buffer, part);

                  if (component == "Intensity")
                  {
                    // The image size and length of one line can be requested with:
                    //
                    // width = buffer->getWidth(part)
                    // full_line_size = width + buffer->getXPadding(part)
                    // height = buffer->getHeight(part)
                    //
                    // The raw pixel data starts at 'buffer->getBase(part)'.
                    // Images can come in a lot of different formats. The actual
                    // pixel format is 'buffer->getPixelFormat(part)'.
                    //
                    // The conversion of some common formats to plain RGB or
                    // monochrome image can be done with 'convertImage()' function
                    // in module 'image.h'.
                    //
                    // Here, we just copy the image into an own object.

                    image=std::make_shared<rcg::Image>(buffer, part);
                  }
                  else if (component == "Disparity")
                  {
                    /*
                      // NOTE: Depending on the depth quality setting, the
                      // resolution of the disparity image can be lower than the
                      // resolution of the intensity image.

                      // Here is example code to convert the disparity image
                      // into a point cloud:

                      size_t width=buffer->getWidth(part);
                      size_t dstep=width*sizeof(uint16_t)+buffer->getXPadding(part);
                      size_t height=buffer->getHeight(part);
                      bool bigendian=buffer->isBigEndian();
                      const uint8_t *p=reinterpret_cast<const uint8_t *>(buffer->getBase(part));

                      for (size_t k=0; k<height; k++)
                      {
                        for (size_t i=0; i<width; i++)
                        {
                          // get 16 bit disparity value, considering endianess

                          uint16_t d16;

                          size_t j=i<<1;
                          if (bigendian)
                          {
                            d16=static_cast<uint16_t>(((p[j]<<8)|p[j+1]));
                          }
                          else
                          {
                            d16=static_cast<uint16_t>(((p[j+1]<<8)|p[j]));
                          }

                          // convert disparity from 16 bit integer into float value

                          double d=scale*d16+offset;

                          // if disparity is valid and color can be obtained

                          if (d != inv)
                          {
                            // reconstruct 3D point in meter from disparity value

                            double x=(i+0.5-u)*t/d;
                            double y=(k+0.5-v)*t/d;
                            double z=f*t/d;

                            // use 3D point x, y, z ...
                          }
                        }

                        p+=dstep;
                      }
                    */

                    // Here, we just copy the image into an own object.

                    disp=std::make_shared<rcg::Image>(buffer, part);
                  }
                }
              }

              if (!image)
              {
                std::cerr << "Missing intensity image" << std::endl;
                ret=1;
                continue;
              }

              if (!disp)
              {
                std::cerr << "Missing disparity image" << std::endl;
                ret=1;
                continue;
              }

              // here we use the internal function to store the 3D data as
              // colored mesh in PLY format

              std::ostringstream name;
              double timestamp=image->getTimestampNS()/1000000000.0;
              name << "pointcloud_" << std::setprecision(16) << timestamp << ".ply";

              storePointCloud(name.str().c_str(), f/disp->getWidth(), t, scale, image, disp);

              std::cout << "Stored pointcloud: " << name.str() << std::endl;
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

      // turn off projector

      rcg::setEnum(nodemap, "LineSource", "Low", true);

      // close device

      dev->close();
    }
    else
    {
      std::cerr << "Device '" << id << "' not found!" << std::endl;
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
