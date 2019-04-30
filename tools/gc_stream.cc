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
#include <rc_genicam_api/config.h>

#include <rc_genicam_api/pixel_formats.h>

#include <Base/GCException.h>

#include <signal.h>

#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <atomic>
#include <thread>
#include <chrono>

#ifdef _WIN32
#undef min
#undef max
#endif

namespace
{

/**
  This method checks if the given file name already exists and produces a new
  file name if this happens.
*/

std::string ensureNewName(std::string name)
{
  // check if given name is already used

  std::ifstream file(name);

  if (file.is_open())
  {
    file.close();

    // split name in prefix and suffix

    std::string suffix;

    size_t i=name.rfind('.');
    if (i != name.npos && name.size()-i <= 4)
    {
      suffix=name.substr(i);
      name=name.substr(0, i);
    }

    // add number for finding name that is nor used

    int n=1;
    while (n < 100)
    {
      std::ostringstream s;
      s << name << "_" << n << suffix;

      file.open(s.str());
      if (!file.is_open())
      {
        name=s.str();
        break;
      }

      file.close();
      n++;
    }
  }

  return name;
}

/**
  Store image given in buffer in PGM or PPM format.
*/

std::string storeBuffer(const std::shared_ptr<GenApi::CNodeMapRef> &nodemap,
                        const std::shared_ptr<GenApi::CChunkAdapter> &chunkadapter,
                        const std::string &component, const rcg::Buffer *buffer, uint32_t part)
{
  // prepare file name

  std::ostringstream name;

  uint64_t t_sec = buffer->getTimestampNS()/1000000000;
  uint64_t t_nsec = buffer->getTimestampNS()%1000000000;

  name << "image_" << t_sec << "." << std::setfill('0') << std::setw(9) << t_nsec;

  if (component.size() > 0)
  {
    name << '_' << component;
  }

  if (chunkadapter)
  {
    // Append out1 and out2 status to file name: _<out1>_<out2>
    std::int64_t line_status=rcg::getInteger(nodemap, "ChunkLineStatusAll");
    bool out1 = line_status & 0x01;
    bool out2 = line_status & 0x02;
    name << "_" << std::noboolalpha << out1 << "_" << out2;
  }

  // store image (see e.g. the sv tool of cvkit for show images)

  if (!buffer->getIsIncomplete() && buffer->getImagePresent(part))
  {
    size_t width=buffer->getWidth(part);
    size_t height=buffer->getHeight(part);
    const unsigned char *p=static_cast<const unsigned char *>(buffer->getBase(part));

    size_t px=buffer->getXPadding(part);

    uint64_t format=buffer->getPixelFormat(part);
    switch (format)
    {
      case Mono8: // store 8 bit monochrome image
      case Confidence8:
      case Error8:
        {
          name << ".pgm";
          std::ofstream out(ensureNewName(name.str()), std::ios::binary);

          out << "P5" << std::endl;
          out << width << " " << height << std::endl;
          out << 255 << "\n";

          std::streambuf *sb=out.rdbuf();

          for (size_t k=0; k<height && out.good(); k++)
          {
            for (size_t i=0; i<width; i++)
            {
              sb->sputc(static_cast<char>(*p++));
            }

            p+=px;
          }

          out.close();
        }
        break;

      case Coord3D_C16: // store 16 bit monochrome image
        {
          name << ".pgm";
          std::ofstream out(ensureNewName(name.str()), std::ios::binary);

          out << "P5" << std::endl;
          out << width << " " << height << std::endl;
          out << 65535 << "\n";

          std::streambuf *sb=out.rdbuf();

          // copy image data, pgm is always big endian

          if (buffer->isBigEndian())
          {
            for (size_t k=0; k<height && out.good(); k++)
            {
              for (size_t i=0; i<width; i++)
              {
                sb->sputc(static_cast<char>(*p++));
                sb->sputc(static_cast<char>(*p++));
              }

              p+=px;
            }
          }
          else
          {
            for (size_t k=0; k<height && out.good(); k++)
            {
              for (size_t i=0; i<width; i++)
              {
                sb->sputc(static_cast<char>(p[1]));
                sb->sputc(static_cast<char>(p[0]));
                p+=2;
              }

              p+=px;
            }
          }

          out.close();
        }
        break;

      case YCbCr411_8: // convert and store as color image
        {
          name << ".ppm";
          std::ofstream out(ensureNewName(name.str()), std::ios::binary);

          out << "P6" << std::endl;
          out << width << " " << height << std::endl;
          out << 255 << "\n";

          std::streambuf *sb=out.rdbuf();

          size_t pstep=(width>>2)*6+px;
          for (size_t k=0; k<height && out.good(); k++)
          {
            for (size_t i=0; i<width; i+=4)
            {
              uint8_t rgb[12];
              rcg::convYCbCr411toQuadRGB(rgb, p, static_cast<int>(i));

              for (int j=0; j<12; j++)
              {
                sb->sputc(static_cast<char>(rgb[j]));
              }
            }

            p+=pstep;
          }

          out.close();
        }
        break;

      default:
        std::cerr << "storeBuffer(): Unknown pixel format: "
                  << GetPixelFormatName(static_cast<PfncFormat>(buffer->getPixelFormat(part)))
                  << std::endl;
        return std::string();
        break;
    }
  }
  else if (buffer->getIsIncomplete())
  {
    std::cerr << "storeBuffer(): Received incomplete buffer" << std::endl;
    return std::string();
  }
  else if (!buffer->getImagePresent(part))
  {
    std::cerr << "storeBuffer(): Received buffer without image" << std::endl;
    return std::string();
  }

  return name.str();
}

/**
  This method expects in the given buffer an image of format Coord3D_C16 and
  ChunkScan3d parameters in the nodemap. The chunk adapter must have already
  been attached to the nodemap. If this function succeeds, then a floating
  point disparity image and a parameter file is stored and the name of the
  disparity image returned.
*/

std::string storeBufferAsDisparity(const std::shared_ptr<GenApi::CNodeMapRef> &nodemap,
                                   const std::shared_ptr<GenApi::CChunkAdapter> &chunkadapter,
                                   const rcg::Buffer *buffer, uint32_t part)
{
  std::string dispname;

  if (!buffer->getIsIncomplete() && buffer->getImagePresent(part) &&
      buffer->getPixelFormat(part) == Coord3D_C16 && chunkadapter)
  {
    // get necessary information from ChunkScan3d parameters

    int inv=-1;

    rcg::setString(nodemap, "ChunkComponentSelector", "Disparity");

    if (rcg::getBoolean(nodemap, "ChunkScan3dInvalidDataFlag"))
    {
      inv=static_cast<int>(rcg::getFloat(nodemap, "ChunkScan3dInvalidDataValue"));
    }

    double scale=rcg::getFloat(nodemap, "ChunkScan3dCoordinateScale");
    double offset=rcg::getFloat(nodemap, "ChunkScan3dCoordinateOffset");
    double f=rcg::getFloat(nodemap, "ChunkScan3dFocalLength");
    double t=rcg::getFloat(nodemap, "ChunkScan3dBaseline");
    double u=rcg::getFloat(nodemap, "ChunkScan3dPrincipalPointU");
    double v=rcg::getFloat(nodemap, "ChunkScan3dPrincipalPointV");

    // proceed if required information is given

    if (scale > 0 && f > 0 && t > 0)
    {
      // prepare file name

      std::ostringstream name;

      uint64_t t_sec = buffer->getTimestampNS()/1000000000;
      uint64_t t_nsec = buffer->getTimestampNS()%1000000000;

      name << "image_" << t_sec << "." << std::setfill('0') << std::setw(9) << t_nsec;

      // convert values and store disparity image

      size_t px=buffer->getXPadding(part);
      size_t width=buffer->getWidth(part);
      size_t height=buffer->getHeight(part);
      const unsigned char *p=static_cast<const unsigned char *>(buffer->getBase(part))+
                             2*(width+px)*(height+1);

      name << "_Disparity";

      // Append out1 and out2 status to file name: _<out1>_<out2>
      std::int64_t line_status=rcg::getInteger(nodemap, "ChunkLineStatusAll");
      bool out1 = line_status & 0x01;
      bool out2 = line_status & 0x02;
      name << "_" << std::noboolalpha << out1 << "_" << out2;

      dispname=name.str()+".pfm";

      std::ofstream out(ensureNewName(dispname), std::ios::binary);

      out << "Pf" << std::endl;
      out << width << " " << height << std::endl;
      out << 1 << "\n";

      std::streambuf *sb=out.rdbuf();

      // get 16 bit data, scale and add offset and store as big endian

      bool msbfirst=true;

      {
        int pp=1;
        char *cc=reinterpret_cast<char *>(&pp);
        msbfirst=(cc[0] != 1);
      }

      for (size_t k=0; k<height && out.good(); k++)
      {
        p-=(width+px)<<2;
        for (size_t i=0; i<width; i++)
        {
          int val;
          if (buffer->isBigEndian())
          {
            val=(static_cast<int>(p[0])<<8)|p[1];
          }
          else
          {
            val=(static_cast<int>(p[1])<<8)|p[0];
          }

          p+=2;

          float d=std::numeric_limits<float>::infinity();
          if (val != inv)
          {
            d=static_cast<float>(val*scale+offset);
          }

          char *c=reinterpret_cast<char *>(&d);

          if (msbfirst)
          {
            sb->sputc(c[0]);
            sb->sputc(c[1]);
            sb->sputc(c[2]);
            sb->sputc(c[3]);
          }
          else
          {
            sb->sputc(c[3]);
            sb->sputc(c[2]);
            sb->sputc(c[1]);
            sb->sputc(c[0]);
          }

          p+=px;
        }
      }

      out.close();

      // store parameters that are necessary for 3D reconstruction in separate
      // parameter file

      name << "_param.txt";

      out.open(ensureNewName(name.str()));

      out << "# Created by gc_stream" << std::endl;
      out << std::fixed << std::setprecision(5);
      out << "camera.A=[" << f << " 0 " << u << "; 0 " << f << " " << v << "; 0 0 1]" << std::endl;
      out << "camera.height=" << height << std::endl;
      out << "camera.width=" << width << std::endl;
      out << "rho=" << f*t << std::endl;
      out << "t=" << t << std::endl;

      out.close();
    }
  }

  return dispname;
}

// simple mechanism to set the boolean flag when the user presses enter in the
// terminal

std::atomic<bool> user_interrupt(false);

void interruptHandler(int)
{
  std::cout << "Stopping ..." << std::endl;

  user_interrupt=true;
}

#ifdef _WIN32

void checkUserInterrupt()
{
  char a;
  std::cin.get(a);

  std::cout << "Stopping ..." << std::endl;

  user_interrupt=true;
}

#endif

}

int main(int argc, char *argv[])
{
  int ret=0;

  signal(SIGINT, interruptHandler);

  try
  {
    bool store=true;
    int i=1;

    if (i < argc)
    {
      if (std::string(argv[i]) == "-t")
      {
        store=false;
        i++;
      }
    }

    if (i < argc && std::string(argv[i]) != "-h")
    {
      // find specific device accross all systems and interfaces and open it

      std::shared_ptr<rcg::Device> dev=rcg::getDevice(argv[i++]);

      if (dev)
      {
        dev->open(rcg::Device::CONTROL);
        std::shared_ptr<GenApi::CNodeMapRef> nodemap=dev->getRemoteNodeMap();

        // get chunk adapter (this switches chunk mode on if possible and
        // returns a null pointer if this is not possible)

        std::shared_ptr<GenApi::CChunkAdapter> chunkadapter=rcg::getChunkAdapter(nodemap, dev->getTLType());

        // set values as given on the command line

        int n=1;
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

          if (key == "n") // set number of images
          {
            n=std::max(1, std::stoi(value));
          }
          else // set key=value pair through GenICam
          {
            if (value.size() > 0)
            {
              rcg::setString(nodemap, key.c_str(), value.c_str(), true);
            }
            else
            {
              rcg::callCommand(nodemap, key.c_str(), true);
            }
          }
        }

        // print enabled streams

        {
          std::vector<std::string> component;

          rcg::getEnum(nodemap, "ComponentSelector", component, false);

          if (component.size() > 0)
          {
            std::cout << std::endl;
            std::cout << "Available components (1 means enabled, 0 means disabled):" << std::endl;
            std::cout << std::endl;

            for (size_t k=0; k<component.size(); k++)
            {
              rcg::setEnum(nodemap, "ComponentSelector", component[k].c_str(), true);

              std::cout << component[k] << ": ";
              std::cout << rcg::getBoolean(nodemap, "ComponentEnable", true, true);
              std::cout << std::endl;
            }

            std::cout << std::endl;
          }
        }

        // open stream and get n images

        std::vector<std::shared_ptr<rcg::Stream> > stream=dev->getStreams();

        if (stream.size() > 0)
        {
#ifdef _WIN32
          // start background thread for checking user input
          std::thread thread_cui(checkUserInterrupt);
          thread_cui.detach();
#endif

          // opening first stream

          stream[0]->open();
          stream[0]->startStreaming();

          std::cout << "Package size: " << rcg::getString(nodemap, "GevSCPSPacketSize") << std::endl;

#ifdef _WIN32
          std::cout << "Press 'Enter' to abort grabbing." << std::endl;
#endif
          std::cout << std::endl;

          int buffers_received=0;
          int buffers_incomplete=0;
          auto time_start=std::chrono::steady_clock::now();
          double latency_ns=0;

          for (int k=0; k<n && !user_interrupt; k++)
          {
            // grab next image with timeout of 3 seconds

            int retry=5;
            while (retry > 0 && !user_interrupt)
            {
              const rcg::Buffer *buffer=stream[0]->grab(3000);

              if (buffer != 0)
              {
                buffers_received++;

                if (!buffer->getIsIncomplete())
                {
                  // attach buffer to nodemap for accessing chunk data if possible

                  if (chunkadapter)
                  {
                    chunkadapter->AttachBuffer(
                      reinterpret_cast<std::uint8_t *>(buffer->getGlobalBase()),
                        static_cast<int64_t>(buffer->getSizeFilled()));
                  }

                  // store images in all parts

                  if (store)
                  {
                    uint32_t npart=buffer->getNumberOfParts();
                    for (uint32_t part=0; part<npart; part++)
                    {
                      if (buffer->getImagePresent(part))
                      {
                        std::string name;

                        // get component name

                        std::string component=rcg::getComponetOfPart(nodemap, buffer, part);

                        // try storing disparity as float image with meta information

                        if (component == "Disparity")
                        {
                          name=storeBufferAsDisparity(nodemap, chunkadapter, buffer, part);
                        }

                        // otherwise, store as ordinary image

                        if (name.size() == 0)
                        {
                          name=storeBuffer(nodemap, chunkadapter, component, buffer, part);
                        }

                        // report success

                        if (name.size() > 0)
                        {
                          std::cout << "Image '" << name << "' stored" << std::endl;
                          retry=0;
                        }
                      }
                    }
                  }
                  else
                  {
                    // just print timestamp of received buffer

                    uint64_t t_sec = buffer->getTimestampNS()/1000000000;
                    uint64_t t_nsec = buffer->getTimestampNS()%1000000000;

                    std::cout << "Received buffer with timestamp: " << t_sec << "."
                              << std::setfill('0') << std::setw(9) << t_nsec << std::endl;
                    retry=0;

                    // accumulate mean latency

                    auto current=std::chrono::system_clock::now();
                    latency_ns+=
                      static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(current.time_since_epoch()).count())-
                      static_cast<double>(buffer->getTimestampNS());
                  }

                  // detach buffer from nodemap

                  if (chunkadapter) chunkadapter->DetachBuffer();
                }
                else
                {
                  std::cerr << "Incomplete buffer received" << std::endl;
                  buffers_incomplete++;
                }
              }
              else
              {
                std::cerr << "Cannot grab images" << std::endl;
                break;
              }

              retry--;
            }
          }

          stream[0]->stopStreaming();
          stream[0]->close();

          // report received and incomplete buffers

          std::cout << std::endl;
          std::cout << "Received buffers:   " << buffers_received << std::endl;
          std::cout << "Incomplete buffers: " << buffers_incomplete << std::endl;

          auto time_stop=std::chrono::steady_clock::now();
          std::cout << "Buffers per second: " << std::setprecision(3)
                    << 1000.0*buffers_received/std::chrono::duration_cast<std::chrono::milliseconds>(time_stop-time_start).count()
                    << std::endl;

          if (!store)
          {
            if (buffers_received-buffers_incomplete > 0)
            {
              latency_ns/=buffers_received-buffers_incomplete;
            }

            std::cout << "Mean latency:       " << std::setprecision(5) << latency_ns/1.0e6
                      << " ms (only meaningful if camera is synchronized e.g. via PTP!)" << std::endl;
          }

          // return error code if no images could be received

          if (buffers_incomplete == buffers_received)
          {
            ret=1;
          }
        }
        else
        {
          std::cerr << "No streams available" << std::endl;
          ret=1;
        }

        dev->close();
      }
      else
      {
        std::cerr << "Device '" << argv[1] << "' not found!" << std::endl;
        ret=1;
      }
    }
    else
    {
      // show help

      std::cout << argv[0] << " -h | [-t] [<interface-id>:]<device-id> [n=<n>] [<key>=<value>] ..." << std::endl;
      std::cout << std::endl;
      std::cout << "Stores images from the specified device after applying the given optional GenICam parameters." << std::endl;
      std::cout << std::endl;
      std::cout << "Options:" << std::endl;
      std::cout << "-h   Prints help information and exits" << std::endl;
      std::cout << "-t   Testmode, which does not store images and provides extended statistics" << std::endl;
      std::cout << std::endl;
      std::cout << "Parameters:" << std::endl;
      std::cout << "<interface-id> Optional GenICam ID of interface for connecting to the device" << std::endl;
      std::cout << "<device-id>    GenICam device ID, serial number or user defined name of device" << std::endl;
      std::cout << "n=<n>          Optional number of images to be received (default is 1)" << std::endl;
      std::cout << "<key>=<value>  Optional GenICam parameters to be changed in the given order" << std::endl;
#ifdef _WIN32
      std::cout << std::endl;
      std::cout << "Streaming can be aborted by hitting the 'Enter' key." << std::endl;
#endif
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

  rcg::System::clearSystems();

  return ret;
}
