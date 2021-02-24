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
#include <rc_genicam_api/config.h>

#include <iostream>

int main(int argc, char *argv[])
{
  int ret=0;

  try
  {
    if (argc > 1 && std::string(argv[1]) != "-h")
    {
      // list all available GigE Vision devices

      if (std::string(argv[1]) == "-l")
      {
        std::cout << "Available GigE Vision devices:" << std::endl;

        std::vector<std::shared_ptr<rcg::System> > system=rcg::System::getSystems();

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
              if (device[j]->getTLType() == "GEV")
              {
                std::cout << "  " << interf[k]->getID() << ":" << device[j]->getSerialNumber() << " (";

                std::string uname=device[j]->getDisplayName();

                if (uname.size() > 0)
                {
                  std::cout << uname << ", ";
                }

                std::cout << device[j]->getID() << ")" << std::endl;
              }
            }

            interf[k]->close();
          }

          system[i]->close();
        }
      }
      else
      {
        // discover and open a specific device

        int i=1;
        std::shared_ptr<rcg::Device> dev=rcg::getDevice(argv[i++]);

        if (dev != 0)
        {
          bool showsummary=false;
          bool iponly=false;

          // open device with control or read only access, depending on the
          // given parameters

          if (i == argc || (i+1 == argc && std::string(argv[i]) == "--iponly"))
          {
            dev->open(rcg::Device::READONLY);
            showsummary=true;
          }
          else
          {
            dev->open(rcg::Device::CONTROL);
          }

          // change setting according to given parameters

          std::shared_ptr<GenApi::CNodeMapRef> nodemap=dev->getRemoteNodeMap();

          while (i < argc)
          {
            std::string p=argv[i++];

            if (p == "--iponly")
            {
              iponly=true;
            }
            else if (p[0] == '-' && i < argc)
            {
              showsummary=true;

              if (p == "-n") // change user defined device name
              {
                rcg::setString(nodemap, "DeviceUserID", argv[i++], true);
              }
              else if (p == "-d") // switch dhcp on or off
              {
                rcg::setString(nodemap, "GevCurrentIPConfigurationDHCP", argv[i++], true);
              }
              else if (p == "-p") // switch persistent IP on or off
              {
                rcg::setString(nodemap, "GevCurrentIPConfigurationPersistentIP", argv[i++], true);
              }
              else if (p == "-t") // switch precision time protocol on or off
              {
                rcg::setString(nodemap, "PtpEnable", argv[i++], true);
              }
              else if (p == "-i") // set persistent IP address
              {
                rcg::setIPV4Address(nodemap, "GevPersistentIPAddress", argv[i++], true);
              }
              else if (p == "-s") // set persistent subnet mask
              {
                rcg::setIPV4Address(nodemap, "GevPersistentSubnetMask", argv[i++], true);
              }
              else if (p == "-g") // set persistent gateway
              {
                rcg::setIPV4Address(nodemap, "GevPersistentDefaultGateway", argv[i++], true);
              }
              else
              {
                std::cerr << "Unknown parameter: " << p << std::endl;
                exit(1);
              }
            }
            else if (p.find('=') != std::string::npos)
            {
              // split argument in key and value

              size_t k=p.find('=');
              std::string value=p.substr(k+1);
              std::string key=p.substr(0, k);

              // set key=value pair through GenICam

              rcg::setString(nodemap, key.c_str(), value.c_str(), true);
            }
            else
            {
              // call the command
              rcg::callCommand(nodemap, p.c_str(), true);
            }
          }

          // print network configuration of the device

          if (iponly)
          {
            std::cout << rcg::getString(nodemap, "GevCurrentIPAddress") << std::endl;
          }
          else if (showsummary)
          {
            std::cout << "ID:                       " << dev->getParent()->getID() << ":"
                                                      << rcg::getString(nodemap, "DeviceID") << std::endl;
            std::cout << "GenTL ID:                 " << dev->getID() << std::endl;
            std::cout << "Serial number:            " << rcg::getString(nodemap, "DeviceID") << std::endl;
            std::cout << "User defined ID:          " << rcg::getString(nodemap, "DeviceUserID") << std::endl;
            std::cout << "MAC Address:              " << rcg::getString(nodemap, "GevMACAddress") << std::endl;
            std::cout << std::endl;

            std::cout << "Current IP:               " << rcg::getString(nodemap, "GevCurrentIPAddress") << std::endl;
            std::cout << "Current subnet mask:      " << rcg::getString(nodemap, "GevCurrentSubnetMask") << std::endl;
            std::cout << "Current gateway:          " << rcg::getString(nodemap, "GevCurrentDefaultGateway") << std::endl;
            std::cout << std::endl;

            std::cout << "Persistent IP on/off:     " << rcg::getString(nodemap, "GevCurrentIPConfigurationPersistentIP") << std::endl;
            std::cout << "  Persistent IP:          " << rcg::getString(nodemap, "GevPersistentIPAddress") << std::endl;
            std::cout << "  Persistent subnet mask: " << rcg::getString(nodemap, "GevPersistentSubnetMask") << std::endl;
            std::cout << "  Persistent gateway:     " << rcg::getString(nodemap, "GevPersistentDefaultGateway") << std::endl;
            std::cout << "DHCP on/off:              " << rcg::getString(nodemap, "GevCurrentIPConfigurationDHCP") << std::endl;
            std::cout << "Link local on/off:        " << rcg::getString(nodemap, "GevCurrentIPConfigurationLLA") << std::endl;
            std::cout << std::endl;

            std::cout << "PTP:                      " << rcg::getString(nodemap, "PtpEnable") << std::endl;
            std::cout << "PTP status:               " << rcg::getString(nodemap, "PtpStatus") << std::endl;
            std::cout << "PTP offset:               " << rcg::getInteger(nodemap, "PtpOffsetFromMaster") << " ns" << std::endl;
          }

          dev->close();
        }
        else
        {
          std::cerr << "Cannot find device: " << argv[1] << std::endl;
          ret=1;
        }
      }
    }
    else
    {
      std::cout << argv[0] << " -h | -l | ([<interface-id>:]<device-id> <options> ...)" << std::endl;
      std::cout << std::endl;
      std::cout << "Configuration of a GigE Vision device via GenICam." << std::endl;
      std::cout << std::endl;
      std::cout << "-h             Prints help information and exits" << std::endl;
      std::cout << "-l             Lists all available GigE Vision devices" << std::endl;
      std::cout << std::endl;
      std::cout << "Parameters:" << std::endl;
      std::cout << "<interface-id> Optional GenICam ID of interface for connecting to the device" << std::endl;
      std::cout << "<device-id>    GenICam device ID, serial number or user defined name of device" << std::endl;
      std::cout << std::endl;
      std::cout << "Options:" << std::endl;
      std::cout << "-n <id>        Set user defined id" << std::endl;
      std::cout << "-d 1|0         Switch DHCP on or off" << std::endl;
      std::cout << "-p 1|0         Switch persistent IP on or off" << std::endl;
      std::cout << "-t 1|0         Switch precision time protocol (ptp) on or off" << std::endl;
      std::cout << "-i <ip>        Set persistent IP address" << std::endl;
      std::cout << "-s <ip>        Set subnet mask for persistent IP address" << std::endl;
      std::cout << "-g <ip>        Set default gateway for persistent IP address" << std::endl;
      std::cout << "--iponly       Show current IP of device instead of full summary" << std::endl;
      std::cout << "<key>=<value>  Optional GenICam parameters to be changed in the given order" << std::endl;
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
