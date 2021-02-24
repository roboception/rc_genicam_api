/*
 * This file is part of the rc_genicam_api package.
 *
 * Copyright (c) 2020 Roboception GmbH
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
#include <rc_genicam_api/config.h>

#include <GenApi/Filestream.h>

#include <iostream>
#include <fstream>

int main(int argc, char *argv[])
{
  int ret=0;

  try
  {
    if (argc >= 2 && std::string(argv[1]) != "-h")
    {
      // get device id

      int k=1;
      if (k < argc)
      {
        std::string devid=argv[k++];

        // find specific device accross all systems and interfaces and show some
        // information

        std::shared_ptr<rcg::Device> dev=rcg::getDevice(devid.c_str());

        if (dev)
        {
          // open device and optionally change some settings

          if (k < argc)
          {
            dev->open(rcg::Device::CONTROL);
          }

          std::shared_ptr<GenApi::CNodeMapRef> nodemap=dev->getRemoteNodeMap();

          // process command line parameters

          if (k < argc)
          {
            std::string devfile=argv[k++];

            if (devfile == "-f")
            {
              std::vector<std::string> list;
              rcg::getEnum(nodemap, "FileSelector", list, true);

              for (size_t i=0; i<list.size(); i++)
              {
                std::cout << list[i] << std::endl;
              }
            }
            else
            {
              std::string op;
              std::string file;

              if (k+1 < argc)
              {
                op=argv[k++];
                file=argv[k++];
              }

              if (op == "-w")
              {
                std::ifstream in(file);
                std::ostringstream buffer;
                buffer << in.rdbuf();
                std::string data=buffer.str();

                std::cout << "Input file length: " << data.size() << std::endl;

                GenApi::FileProtocolAdapter rf;
                rf.attach(nodemap->_Ptr);

                if (rf.openFile(devfile.c_str(), std::ios::out))
                {
                  size_t n=rf.write(data.c_str(), 0, data.size(), devfile.c_str());

                  std::cout << "Status: " << rcg::getString(nodemap, "FileOperationStatus") << std::endl;

                  if (n != data.size())
                  {
                    std::cerr << "Error: Can only write " << n << " of " << data.size() << " bytes" << std::endl;
                  }

                  rf.closeFile(devfile.c_str());
                }
                else
                {
                  std::cerr << "ERROR: Failed to open remote file!" << std::endl;
                }
              }
              else if (op == "-r" || op == "")
              {
                // load file completely into memory

                GenApi::FileProtocolAdapter rf;
                rf.attach(nodemap->_Ptr);

                if (rf.openFile(devfile.c_str(), std::ios::in))
                {
                  size_t n=rcg::getInteger(nodemap, "FileSize", 0, 0, true);
                  std::cout << "File size: " << n << std::endl;
                  std::vector<char> buffer(n);

                  n=rf.read(buffer.data(), 0, buffer.size(), devfile.c_str());

                  std::cout << "Status: " << rcg::getString(nodemap, "FileOperationStatus") << std::endl;

                  if (n == buffer.size())
                  {
                    if (op == "-r")
                    {
                      // store in file

                      std::ofstream out(file);
                      out.write(buffer.data(), buffer.size());
                      out.close();
                    }
                    else
                    {
                      // print on stdout

                      std::cout.write(buffer.data(), buffer.size());
                    }
                  }
                  else
                  {
                    std::cerr << "Error: Can only read " << n << " of " << buffer.size() << " bytes" << std::endl;
                  }

                  rf.closeFile(devfile.c_str());
                }
                else
                {
                  std::cerr << "Error: Cannot open remote file: " << devfile << std::endl;
                }
              }
              else
              {
                std::cerr << "Error: Expected parameter '-r' or '-w'!" << std::endl;
                ret=1;
              }
            }
          }
          else
          {
            std::cerr << "Error: More parameters expected!" << std::endl;
            ret=1;
          }

          dev->close();
        }
        else
        {
          std::cerr << "Error: Device '" << devid << "' not found!" << std::endl;
          ret=1;
        }
      }
      else
      {
        std::cerr << "Error: Device name not given!" << std::endl;
        ret=1;
      }
    }
    else
    {
      std::cout << argv[0] << " -h | [<interface-id>:]<device-id> -f | (<device-file> [-w|-r <file>])" << std::endl;
      std::cout << std::endl;
      std::cout << "Downloading or uploading a file via GenICam." << std::endl;
      std::cout << std::endl;
      std::cout << "-h            Prints help information and exits" << std::endl;
      std::cout << "-f            Lists names of files on the device" << std::endl;
      std::cout << "-w <file>     Writes the given local file into the selected file on the device" << std::endl;
      std::cout << "-r <file>     Reads the selected file on the device and stores it as local file" << std::endl;
      std::cout << std::endl;
      std::cout << "The selected file is printed on std out if none of -f, -w and -r are given." << std::endl;
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
    std::cerr << ex.what() << std::endl;
    ret=2;
  }

  rcg::System::clearSystems();

  return ret;
}
