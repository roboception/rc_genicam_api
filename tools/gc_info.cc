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
#include <rc_genicam_api/nodemap_out.h>
#include <rc_genicam_api/nodemap_edit.h>

#include <iostream>

int main(int argc, char *argv[])
{
  int ret=0;

  try
  {
    if (argc >= 2 && std::string(argv[1]) != "-h")
    {
      if (std::string(argv[1]) == "-L" || std::string(argv[1]) == "-l")
      {
        bool extended=(std::string(argv[1]) == "-L");

        // list all systems, interfaces and devices

        std::vector<std::shared_ptr<rcg::System> > system=rcg::System::getSystems();

        for (size_t i=0; i<system.size(); i++)
        {
          system[i]->open();

          std::cout << "Transport Layer " << system[i]->getID() << std::endl;
          std::cout << "Vendor:         " << system[i]->getVendor() << std::endl;
          std::cout << "Model:          " << system[i]->getModel() << std::endl;
          std::cout << "Vendor version: " << system[i]->getVersion() << std::endl;
          std::cout << "TL type:        " << system[i]->getTLType() << std::endl;
          std::cout << "Name:           " << system[i]->getName() << std::endl;
          std::cout << "Pathname:       " << system[i]->getPathname() << std::endl;
          std::cout << "Display name:   " << system[i]->getDisplayName() << std::endl;
          std::cout << "GenTL version   " << system[i]->getMajorVersion() << "."
                    << system[i]->getMinorVersion() << std::endl;
          std::cout << std::endl;

          std::vector<std::shared_ptr<rcg::Interface> > interf=system[i]->getInterfaces();

          if (extended)
          {
            rcg::printNodemap(system[i]->getNodeMap(), "Root");
            std::cout << std::endl;
          }

          for (size_t k=0; k<interf.size(); k++)
          {
            interf[k]->open();

            std::cout << "    Interface     " << interf[k]->getID() << std::endl;
            std::cout << "    Display name: " << interf[k]->getDisplayName() << std::endl;
            std::cout << "    TL type:      " << interf[k]->getTLType() << std::endl;
            std::cout << std::endl;

            std::vector<std::shared_ptr<rcg::Device> > device=interf[k]->getDevices();

            if (extended)
            {
              rcg::printNodemap(interf[k]->getNodeMap(), "Root");
              std::cout << std::endl;
            }

            for (size_t j=0; j<device.size(); j++)
            {
              std::cout << "        Device             " << device[j]->getID() << std::endl;
              std::cout << "        Vendor:            " << device[j]->getVendor() << std::endl;
              std::cout << "        Model:             " << device[j]->getModel() << std::endl;
              std::cout << "        TL type:           " << device[j]->getTLType() << std::endl;
              std::cout << "        Display name:      " << device[j]->getDisplayName() << std::endl;
              std::cout << "        User defined name: " << device[j]->getUserDefinedName() << std::endl;
              std::cout << "        Access status:     " << device[j]->getAccessStatus() << std::endl;
              std::cout << "        Serial number:     " << device[j]->getSerialNumber() << std::endl;
              std::cout << "        Version:           " << device[j]->getVersion() << std::endl;
              std::cout << "        TS Frequency:      " << device[j]->getTimestampFrequency() << std::endl;
              std::cout << std::endl;
            }

            interf[k]->close();
          }

          system[i]->close();
        }
      }
      else if (std::string(argv[1]) == "-s")
      {
        // list all systems, interfaces and devices

        std::vector<std::shared_ptr<rcg::System> > system=rcg::System::getSystems();

        std::cout << "Interface\tSerial Number\tVendor\tModel\tName" << std::endl;

        for (size_t i=0; i<system.size(); i++)
        {
          system[i]->open();

          std::vector<std::shared_ptr<rcg::Interface> > interf=system[i]->getInterfaces();

          for (size_t k=0; k<interf.size(); k++)
          {
            interf[k]->open();

            std::vector<std::shared_ptr<rcg::Device> > device=interf[k]->getDevices();

            for (size_t j=0; j<device.size(); j++)
            {
              std::cout << interf[k]->getID() << '\t'
                        << device[j]->getSerialNumber() << '\t'
                        << device[j]->getVendor() << '\t'
                        << device[j]->getModel() << '\t'
                        << device[j]->getDisplayName() << std::endl;
            }

            interf[k]->close();
          }

          system[i]->close();
        }
      }
      else
      {
        int k=1;

        // get parameters, if any

        int module_event_timeout=-1;
        const char *xml=0;
        const char *paramfile=0;
        bool local_nodemap=false;
        bool edit=false;

        while (k+1 < argc && argv[k][0] == '-')
        {
          if (std::string(argv[k]) == "-o")
          {
            k++;
            xml=argv[k++];

            if (std::string(xml) == ".")
            {
              xml="";
            }
          }
          else if (std::string(argv[k]) == "-m")
          {
            k++;
            module_event_timeout=std::stoi(std::string(argv[k++]));
          }
          else if (std::string(argv[k]) == "-p")
          {
            k++;
            paramfile=argv[k++];
          }
          else if (std::string(argv[k]) == "-d")
          {
            k++;
            local_nodemap=true;
          }
          else if (std::string(argv[k]) == "-e")
          {
            k++;
            edit=true;
          }
          else
          {
            std::cerr << "Unknown parameter: " << argv[k] << std::endl;
            ret=1;
            break;
          }
        }

        if (ret == 0)
        {
          if (k < argc)
          {
            // separate optional node name from device id

            std::string devid=argv[k++];
            std::string node="Root";
            int depth=1000;

            {
              size_t j=devid.find('?');

              if (j != std::string::npos)
              {
                if (devid.substr(j+1) != "-")
                {
                  node=devid.substr(j+1);
                  devid=devid.substr(0, j);
                  depth=1;

                  if (node.size() == 0)
                  {
                    node="Root";
                  }
                }
                else
                {
                  node="";
                  devid=devid.substr(0, j);
                }
              }
            }

            // find specific device accross all systems and interfaces

            std::shared_ptr<rcg::Device> dev=rcg::getDevice(devid.c_str());

            if (dev)
            {
              // open device and optionally change some settings

              if (k < argc || edit || paramfile)
              {
                dev->open(rcg::Device::CONTROL);
              }
              else
              {
                dev->open(rcg::Device::READONLY);
              }

              // get nodemap

              std::shared_ptr<GenApi::CNodeMapRef> nodemap;
              if (local_nodemap)
              {
                nodemap=dev->getNodeMap(xml);
              }
              else
              {
                nodemap=dev->getRemoteNodeMap(xml);
              }

              // register and enable module events

              if (module_event_timeout >= 0)
              {
                dev->enableModuleEvents();

                std::shared_ptr<GenApi::CNodeMapRef> lnodemap;
                lnodemap=dev->getNodeMap();

                std::vector<std::string> list;
                rcg::getEnum(lnodemap, "EventSelector", list, false);

                for (size_t i=0; i<list.size(); i++)
                {
                  rcg::setEnum(lnodemap, "EventSelector", list[i].c_str(), true);
                  rcg::setEnum(lnodemap, "EventNotification", "On", false);
                }
              }

              if (nodemap)
              {
                // interpret all remaining parameters

                while (k < argc)
                {
                  std::string p=argv[k++];

                  if (p.size() > 0 && p[0] == '@')
                  {
                    // load streamable parameters from file into nodemap

                    rcg::loadStreamableParameters(nodemap, p.substr(1).c_str(), true);
                  }
                  else if (p.find('=') != std::string::npos)
                  {
                    // split argument in key and value

                    size_t j=p.find('=');
                    std::string value=p.substr(j+1);
                    std::string key=p.substr(0, j);

                    // set key=value pair through GenICam

                    rcg::setString(nodemap, key.c_str(), value.c_str(), true);
                  }
                  else
                  {
                    // call the command
                    rcg::callCommand(nodemap, p.c_str(), true);
                  }
                }

                if (module_event_timeout >= 0)
                {
                  std::cout << "Waiting for events" << std::endl;

                  int64_t eventid=dev->getModuleEvent(1000*module_event_timeout);

                  if (eventid < 0)
                  {
                    std::cout << "Received no module events" << std::endl;
                  }

                  while (eventid >= 0)
                  {
                    std::cout << "Received module event with ID: " << eventid << std::endl;
                    eventid=dev->getModuleEvent(0);
                  }

                  std::cout << std::endl;
                }

                if (edit)
                {
                  if (!rcg::editNodemap(nodemap, node.c_str()))
                  {
                    std::cerr << "Unknown node: " << node << std::endl;
                    ret=1;
                  }
                }
                else if (node.size() > 0)
                {
                  if (depth > 1)
                  {
                    // report all features

                    std::cout << "Device:            " << dev->getID() << std::endl;
                    std::cout << "Vendor:            " << dev->getVendor() << std::endl;
                    std::cout << "Model:             " << dev->getModel() << std::endl;
                    std::cout << "TL type:           " << dev->getTLType() << std::endl;
                    std::cout << "Display name:      " << dev->getDisplayName() << std::endl;
                    std::cout << "User defined name: " << dev->getUserDefinedName() << std::endl;
                    std::cout << "Serial number:     " << dev->getSerialNumber() << std::endl;
                    std::cout << "Version:           " << dev->getVersion() << std::endl;
                    std::cout << "TS Frequency:      " << dev->getTimestampFrequency() << std::endl;
                    std::cout << std::endl;

                    std::vector<std::shared_ptr<rcg::Stream> > stream=dev->getStreams();

                    std::cout << "Available streams:" << std::endl;
                    for (size_t i=0; i<stream.size(); i++)
                    {
                      std::cout << "  Stream ID: " << stream[i]->getID() << std::endl;
                    }

                    std::cout << std::endl;

                    if (local_nodemap)
                    {
                      std::cout << "Local device nodemap:" << std::endl;
                    }
                    else
                    {
                      std::cout << "Remote device nodemap:" << std::endl;
                    }

                    rcg::printNodemap(nodemap, node.c_str(), depth, true);
                  }
                  else
                  {
                    // report requested node only

                    if (!rcg::printNodemap(nodemap, node.c_str(), depth, true))
                    {
                      std::cerr << "Unknown node: " << node << std::endl;
                      ret=1;
                    }
                  }
                }

                if (paramfile)
                {
                  rcg::saveStreamableParameters(nodemap, paramfile, true);
                }
              }
              else
              {
                std::cerr << "Nodemap not available!" << std::endl;
              }

              dev->close();
            }
            else
            {
              std::cerr << "Device '" << devid << "' not found!" << std::endl;
              ret=1;
            }
          }
          else
          {
            std::cerr << "Device name not given!" << std::endl;
            ret=1;
          }
        }
      }
    }
    else
    {
      std::cout << argv[0] << " -h | -L | -l | -s | ([-o <xml-output-file>|.] [-m <timeout>] [-p <file>] [-d] [-e] [<interface-id>:]<device-id>[?<node>] [@<file>] [<key>=<value>] ...)" << std::endl;
      std::cout << std::endl;
      std::cout << "Provides information about GenICam transport layers, interfaces and devices." << std::endl;
      std::cout << std::endl;
      std::cout << "Options: " << std::endl;
      std::cout << "-h   Prints help information and exits" << std::endl;
      std::cout << "-L   List all available devices on all interfaces (extended format)" << std::endl;
      std::cout << "-l   List all available devices on all interfaces" << std::endl;
      std::cout << "-s   List all available devices on all interfaces (short format)" << std::endl;
      std::cout << "-o   Store XML description from specified device" << std::endl;
      std::cout << "-m   Registers for module events and waits for the given number of seconds for such events" << std::endl;
      std::cout << "-d   Use local device nodemap, instead of remote nodemap" << std::endl;
      std::cout << "-e   Open nodemap editor instead of printing nodemap" << std::endl;
      std::cout << "-p   Store all streamable parameters to the given file, after applying all parameters" << std::endl;
      std::cout << std::endl;
      std::cout << "Parameters:" << std::endl;
      std::cout << "<interface-id> Optional GenICam ID of interface for connecting to the device" << std::endl;
      std::cout << "<device-id>    GenICam device ID, serial number or user defined name of device" << std::endl;
      std::cout << "<node>         Optional name of category or parameter to be reported. '-' for none. Default is 'Root'." << std::endl;
      std::cout << "@<file>        Optional file with parameters as store with parameter '-p'" << std::endl;
      std::cout << "<key>=<value>  Optional GenICam parameters to be changed in the given order before reporting" << std::endl;
      ret=1;
    }
  }
  catch (const std::exception &ex)
  {
    std::cerr << ex.what() << std::endl;
    ret=2;
  }

  rcg::System::clearSystems();

  return ret;
}
