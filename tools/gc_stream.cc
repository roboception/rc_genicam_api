/*
 * This file is part of the rc_genicam_api package.
 *
 * Copyright (c) 2017-2024 Roboception GmbH
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
#include <rc_genicam_api/nodemap_out.h>

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

void printHelp()
{
  // show help

  std::cout << "gc_stream -h | [-c] [-f <fmt>] [-t] [<interface-id>:]<device-id> [n=<n>] [@<file>] [<key>=<value>] ..." << std::endl;
  std::cout << std::endl;
  std::cout << "Stores images from the specified device after applying the given optional GenICam parameters." << std::endl;
  std::cout << std::endl;
  std::cout << "Options:" << std::endl;
  std::cout << "-h         Prints help information and exits" << std::endl;
  std::cout << "-c         Print ChunkDataControl category for all received buffers" << std::endl;
  std::cout << "-t         Testmode, which does not store images and provides extended statistics" << std::endl;
  std::cout << "-f pnm|png Format for storing images. Default is pnm" << std::endl;
  std::cout << std::endl;
  std::cout << "Parameters:" << std::endl;
  std::cout << "<interface-id> Optional GenICam ID of interface for connecting to the device" << std::endl;
  std::cout << "<device-id>    GenICam device ID, serial number or user defined name of device" << std::endl;
  std::cout << "n=<n>          Optional number of images to be received (default is 1)" << std::endl;
  std::cout << "@<file>        Optional file with parameters as store with parameter 'gc_info -p ...'" << std::endl;
  std::cout << "<key>=<value>  Optional GenICam parameters to be changed in the given order" << std::endl;
#ifdef _WIN32
  std::cout << std::endl;
  std::cout << "Streaming can be aborted by hitting the 'Enter' key." << std::endl;
#endif
}

/**
  Get status of digital input and output lines as separate bit fields if available.
*/

std::string getDigitalIO(const std::shared_ptr<GenApi::CNodeMapRef> &nodemap)
{
  try
  {
    int64_t line_status=rcg::getInteger(nodemap, "ChunkLineStatusAll", 0, 0, true);

    std::string out;
    std::string in;

    std::vector<std::string> io;
    rcg::getEnum(nodemap, "LineSelector", io, true);

    for (int i=static_cast<int>(io.size())-1; i>=0; i--)
    {
      rcg::setEnum(nodemap, "LineSelector", io[i].c_str(), true);

      std::string mode=rcg::getString(nodemap, "LineMode", true);

      if (mode == "Input") in+=std::to_string((line_status>>i)&0x1);
      if (mode == "Output") out+=std::to_string((line_status>>i)&0x1);
    }

    if (out.size() > 0 || in.size() > 0)
    {
      if (out.size() == 0) out.push_back('0');
      if (in.size() == 0) in.push_back('0');

      std::ostringstream ret;
      ret << "_" << out << "_" << in;
      return ret.str();
    }
  }
  catch (const std::exception &)
  {
    // just ignore and return empty string
  }

  return std::string();
}

/**
  Store image in given buffer.
*/

std::string storeBuffer(rcg::ImgFmt fmt, const std::shared_ptr<GenApi::CNodeMapRef> &nodemap,
                        const std::string &component, const rcg::Buffer *buffer, uint32_t part,
                        size_t yoffset=0, size_t height=0)
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

  if (buffer->getContainsChunkdata())
  {
    name << getDigitalIO(nodemap);
  }

  // store image (see e.g. the sv tool of cvkit for show images)

  std::string full_name;
  if (!buffer->getIsIncomplete() && buffer->getImagePresent(part))
  {
    rcg::Image image(buffer, part);
    full_name=storeImage(name.str(), fmt, image, yoffset, height);
  }
  else if (buffer->getIsIncomplete())
  {
    std::cerr << "storeBuffer(): Received incomplete buffer" << std::endl;
  }
  else if (!buffer->getImagePresent(part))
  {
    std::cerr << "storeBuffer(): Received buffer without image" << std::endl;
  }

  return full_name;
}

/**
  This method expects in the given buffer an image of format Coord3D_C16 and
  ChunkScan3d parameters in the nodemap. The chunk adapter must have already
  been attached to the nodemap. If this function succeeds, then a floating
  point disparity image and a parameter file is stored and the name of the
  disparity image returned.
*/

std::string storeBufferAsDisparity(const std::shared_ptr<GenApi::CNodeMapRef> &nodemap,
                                   const rcg::Buffer *buffer, uint32_t part)
{
  std::string dispname;

  if (!buffer->getIsIncomplete() && buffer->getImagePresent(part) &&
      buffer->getPixelFormat(part) == Coord3D_C16 && buffer->getContainsChunkdata())
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

    // prepare file name

    std::ostringstream name;

    uint64_t t_sec = buffer->getTimestampNS()/1000000000;
    uint64_t t_nsec = buffer->getTimestampNS()%1000000000;

    name << "image_" << t_sec << "." << std::setfill('0') << std::setw(9) << t_nsec;
    name << "_Disparity";

    // Append out1 and out2 status to file name: _<out1>_<out2>

    name << getDigitalIO(nodemap);

    // store image

    rcg::Image image(buffer, part);
    dispname=storeImageAsDisparityPFM(name.str(), image, inv, static_cast<float>(scale), static_cast<float>(offset));
  }
  else if (buffer->getIsIncomplete())
  {
    std::cerr << "storeBuffer(): Received incomplete buffer" << std::endl;
  }
  else if (!buffer->getImagePresent(part))
  {
    std::cerr << "storeBuffer(): Received buffer without image" << std::endl;
  }

  return dispname;
}

/**
  Stores 3D parameters into parameter file if possible.
*/

void storeParameter(const std::shared_ptr<GenApi::CNodeMapRef> &nodemap,
                    const std::string &component, const rcg::Buffer *buffer,
                    size_t height=0, bool dispinfo=false)
{
  if (buffer->getContainsChunkdata())
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

    // Append out1 and out2 status to file name: _<out1>_<out2>

    name << getDigitalIO(nodemap);
    name << "_param.txt";

    // get 3D parameter

    rcg::setString(nodemap, "ChunkComponentSelector", component.c_str());

    int width=static_cast<int>(rcg::getInteger(nodemap, "ChunkWidth"));
    if (height == 0) height=rcg::getInteger(nodemap, "ChunkHeight");
    double f=rcg::getFloat(nodemap, "ChunkScan3dFocalLength");
    double t=rcg::getFloat(nodemap, "ChunkScan3dBaseline");
    double u=rcg::getFloat(nodemap, "ChunkScan3dPrincipalPointU");
    double v=rcg::getFloat(nodemap, "ChunkScan3dPrincipalPointV");
    double exp=rcg::getFloat(nodemap, "ChunkExposureTime")/1000000.0;
    double gain=rcg::getFloat(nodemap, "ChunkGain");
    int inv=-1;
    double scale=0, offset=0;

    if (dispinfo)
    {
      if (rcg::getBoolean(nodemap, "ChunkScan3dInvalidDataFlag"))
      {
        inv=static_cast<int>(rcg::getFloat(nodemap, "ChunkScan3dInvalidDataValue"));
      }

      scale=rcg::getFloat(nodemap, "ChunkScan3dCoordinateScale");
      offset=rcg::getFloat(nodemap, "ChunkScan3dCoordinateOffset");
    }

    // create parameter file

    if (width > 0 && height > 0 && f > 0 && t > 0)
    {
      std::ofstream out(rcg::ensureNewFileName(name.str()));

      out << "# Created by gc_stream" << std::endl;
      out << std::fixed << std::setprecision(5);
      out << "camera.A=[" << f << " 0 " << u << "; 0 " << f << " " << v << "; 0 0 1]" << std::endl;
      out << "camera.height=" << height << std::endl;
      out << "camera.width=" << width << std::endl;
      out << "camera.exposure_time=" << exp << std::endl;
      out << "camera.gain=" << gain << std::endl;
      out << "rho=" << f*t << std::endl;
      out << "t=" << t << std::endl;

      try
      {
        float v=static_cast<float>(rcg::getFloat(nodemap, "ChunkRcNoise", 0, 0, true));
        out << "camera.noise=" << v << std::endl;
      }
      catch (const std::exception &)
      { }

      try
      {
        float v=static_cast<float>(rcg::getFloat(nodemap, "ChunkRcBrightness", 0, 0, true));
        out << "camera.brightness=" << v << std::endl;
      }
      catch (const std::exception &)
      { }

      try
      {
        float v=static_cast<float>(rcg::getFloat(nodemap, "ChunkRcOut1Reduction", 0, 0, true));
        out << "camera.out1_reduction=" << v << std::endl;
      }
      catch (const std::exception &)
      { }

      for (int i=0; i<4; i++)
      {
        try
        {
          rcg::setEnum(nodemap, "ChunkLineSelector", ("Out"+std::to_string(i)).c_str(), true);
          float v=static_cast<float>(rcg::getFloat(nodemap, "ChunkRcLineRatio", 0, 0, true));
          out << "camera.out" << i << "_ratio=" << v << std::endl;
        }
        catch (const std::exception &)
        { }
      }

      if (scale > 0)
      {
        out << "disp.inv=" << inv << std::endl;
        out << "disp.scale=" << scale << std::endl;
        out << "disp.offset=" << offset << std::endl;
      }

      out.close();
    }
  }
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
    bool print_chunk_data=false;
    bool store=true;
    rcg::ImgFmt fmt=rcg::PNM;
    int i=1;

    // get parameters

    while (i < argc && argv[i][0] == '-')
    {
      std::string param=argv[i];

      if (param == "-h")
      {
        printHelp();
        return 0;
      }
      else if (param == "-c")
      {
        print_chunk_data=true;
        i++;
      }
      else if (param == "-t")
      {
        store=false;
        i++;
      }
      else if (param == "-f")
      {
        i++;

        if (i < argc)
        {
          std::string imgfmt=argv[i];
          if (imgfmt == "pnm")
          {
            fmt=rcg::PNM;
          }
          else if (imgfmt == "png")
          {
            fmt=rcg::PNG;
          }
          else
          {
            throw std::invalid_argument(std::string("Invalid argument of '-f': ")+argv[i]);
          }

          i++;
        }
        else
        {
          throw std::invalid_argument("Argument expected after '-f'!");
        }
      }
      else
      {
        throw std::invalid_argument("Unknown parameter: "+param);
      }
    }

    if (i < argc)
    {
      // find specific device accross all systems and interfaces and open it

      std::shared_ptr<rcg::Device> dev=rcg::getDevice(argv[i]);

      if (dev)
      {
        i++;
        dev->open(rcg::Device::CONTROL);
        std::shared_ptr<GenApi::CNodeMapRef> nodemap=dev->getRemoteNodeMap();
        std::vector<std::pair<std::string, std::string> > chunk_param;

        // try to enable chunks by default (can be disabed by the user)

        rcg::setBoolean(nodemap, "ChunkModeActive", true);

        // set values as given on the command line

        int n=1;
        while (i < argc)
        {
          std::string key=argv[i++];

          if (key.size() > 0 && key[0] == '@')
          {
            // load streamable parameters from file into nodemap

            try
            {
              rcg::loadStreamableParameters(nodemap, key.substr(1).c_str(), true);
            }
            catch (const std::exception &ex)
            {
              std::cerr << "Warning: Loading of parameters from file '" << key.substr(1) <<
                "' failed at least partially" << std::endl;
              std::cerr << ex.what() << std::endl;
            }
          }
          else
          {
            // split argument in key and value

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
                try
                {
                  rcg::setString(nodemap, key.c_str(), value.c_str(), true);
                }
                catch (const std::exception &)
                {
                  // chunk parameters may fail, try to apply them after
                  // attaching the buffer

                  if (key.compare(0, 5, "Chunk") == 0)
                  {
                    chunk_param.push_back(std::pair<std::string, std::string>(key, value));
                  }
                  else
                  {
                    throw;
                  }
                }
              }
              else
              {
                rcg::callCommand(nodemap, key.c_str(), true);
              }
            }
          }
        }

        // disable component Intensity if component IntensityCombined is enabled

        if (rcg::setEnum(nodemap, "ComponentSelector", "IntensityCombined", false) &&
            rcg::getBoolean(nodemap, "ComponentEnable", true, true))
        {
          if (rcg::setEnum(nodemap, "ComponentSelector", "Intensity", false) &&
              rcg::getBoolean(nodemap, "ComponentEnable", true, true))
          {
            rcg::setBoolean(nodemap, "ComponentEnable", false, true);

            std::cout << std::endl;
            std::cout << "NOTE: Disabling component 'Intensity' as 'IntensityCombined' is enabled."
                      << std::endl;
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
          stream[0]->attachBuffers(true);
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
                if (buffers_received == 0)
                {
                  time_start=std::chrono::steady_clock::now();
                }

                buffers_received++;

                if (!buffer->getIsIncomplete())
                {
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

                        if (component == "Disparity" && fmt == rcg::PNM)
                        {
                          name=storeBufferAsDisparity(nodemap, buffer, part);

                          if (name.size() != 0)
                          {
                            std::cout << "Image '" << name << "' stored" << std::endl;
                            storeParameter(nodemap, component, buffer);
                          }
                        }

                        // otherwise, store as ordinary image

                        if (name.size() == 0)
                        {
                          if (component == "IntensityCombined" || component == "RawCombined")
                          {
                            // splitting left and right image of combined format of
                            // Roboceptions rc_visard camera

                            std::string comp_name = component.substr(0, component.size() - 8);

                            size_t h2=buffer->getHeight(part)/2;
                            name=storeBuffer(fmt, nodemap, comp_name, buffer, part, 0, h2);

                            if (name.size() > 0)
                            {
                              std::cout << "Image '" << name << "' stored" << std::endl;
                            }

                            name=storeBuffer(fmt, nodemap, comp_name.append("Right"), buffer, part, h2, h2);

                            if (name.size() > 0)
                            {
                              std::cout << "Image '" << name << "' stored" << std::endl;
                            }
                          }
                          else
                          {
                            name=storeBuffer(fmt, nodemap, component, buffer, part);

                            if (name.size() > 0)
                            {
                              std::cout << "Image '" << name << "' stored" << std::endl;
                            }
                          }

                          // store 3D parameters for intensity and disparity
                          // components (nothing is done if chunk parameters are
                          // not available)

                          if (component == "Intensity")
                          {
                            storeParameter(nodemap, component, buffer);
                          }
                          else if (component == "Disparity")
                          {
                            storeParameter(nodemap, component, buffer, 0, true);
                          }
                          else if (component == "IntensityCombined" || component == "RawCombined")
                          {
                            std::string comp_name = component.substr(0, component.size() - 8);
                            size_t h2=buffer->getHeight(part)/2;
                            storeParameter(nodemap, comp_name, buffer, h2, false);
                          }
                        }

                        // report success

                        if (name.size() > 0)
                        {
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

                  // optinally print chunk data

                  if (print_chunk_data)
                  {
                    // apply chunk parameters

                    for (size_t i=0; i<chunk_param.size(); i++)
                    {
                      rcg::setString(nodemap, chunk_param[i].first.c_str(),
                        chunk_param[i].second.c_str(), true);
                    }

                    // print chunk data

                    std::cout << std::endl;

                    if (!rcg::printNodemap(nodemap, "ChunkDataControl", 100, false))
                    {
                      std::cout << "Cannot find node 'ChunkDataControl'" << std::endl;
                    }

                    std::cout << std::endl;
                  }
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

          auto time_stop=std::chrono::steady_clock::now();

          stream[0]->stopStreaming();
          stream[0]->close();

          // report received and incomplete buffers

          std::cout << std::endl;
          std::cout << "Received buffers:   " << buffers_received << std::endl;
          std::cout << "Incomplete buffers: " << buffers_incomplete << std::endl;

          std::cout << "Buffers per second: " << std::setprecision(3)
                    << 1000.0*buffers_received/std::chrono::duration_cast<std::chrono::milliseconds>(time_stop-time_start).count()
                    << std::endl;

          if (!store)
          {
            if (rcg::getBoolean(nodemap, "PtpEnable") || rcg::getBoolean(nodemap, "GevIEEE1588"))
            {
              if (buffers_received-buffers_incomplete > 0)
              {
                latency_ns/=buffers_received-buffers_incomplete;
              }

              std::cout << "Mean latency:       " << std::setprecision(5) << latency_ns/1.0e6
                        << " ms" << std::endl;
            }
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
        std::cerr << "Device '" << argv[i] << "' not found!" << std::endl;
        ret=1;
      }
    }
    else
    {
      printHelp();
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
