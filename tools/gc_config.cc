/*
 * Roboception GmbH
 * Munich, Germany
 * www.roboception.com
 *
 * Copyright (c) 2017 Roboception GmbH
 * All rights reserved
 *
 * Author: Heiko Hirschmueller
 */

#include <rcgcapi/system.h>
#include <rcgcapi/interface.h>
#include <rcgcapi/device.h>
#include <rcgcapi/config.h>

#include <iostream>

int main(int argc, char *argv[])
{
  try
  {
    if (argc > 1)
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
                std::cout << "  " << device[j]->getID();

                std::string uname=device[j]->getUserDefinedName();

                if (uname.size() > 0)
                {
                  std::cout << " (" << uname << ")";
                }

                std::cout << std::endl;
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
          dev->open(rcg::Device::EXCLUSIVE);

          // change setting according to given parameters

          std::shared_ptr<GenApi::CNodeMapRef> nodemap=dev->getRemoteNodeMap();

          while (i+1 < argc)
          {
            std::string p=argv[i++];

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
            else if (p == "-i") // set persistent IP address
            {
              rcg::setString(nodemap, "GevPersistentIPAddress", argv[i++], true);
            }
            else if (p == "-s") // set persistent subnet mask
            {
              rcg::setString(nodemap, "GevPersistentSubnetMask", argv[i++], true);
            }
            else if (p == "-g") // set persistent gateway
            {
              rcg::setString(nodemap, "GevPersistentDefaultGateway", argv[i++], true);
            }
          }

          // print network configuration of the device

          std::cout << "Device ID:       " << rcg::getString(nodemap, "DeviceID") << std::endl;
          std::cout << "User defined ID: " << rcg::getString(nodemap, "DeviceUserID") << std::endl;
          std::cout << "MAC Address:     " << rcg::getString(nodemap, "GevMACAddress") << std::endl;
          std::cout << std::endl;

          std::cout << "IP:              " << rcg::getString(nodemap, "GevCurrentIPAddress") << std::endl;
          std::cout << "Subnet mask:     " << rcg::getString(nodemap, "GevCurrentSubnetMask") << std::endl;
          std::cout << "Default gateway: " << rcg::getString(nodemap, "GevCurrentDefaultGateway") << std::endl;
          std::cout << std::endl;

          std::cout << "Persistent IP:   " << rcg::getString(nodemap, "GevCurrentIPConfigurationPersistentIP") << std::endl;
          std::cout << "DHCP:            " << rcg::getString(nodemap, "GevCurrentIPConfigurationDHCP") << std::endl;
          std::cout << "Link local:      " << rcg::getString(nodemap, "GevCurrentIPConfigurationLLA") << std::endl;

          dev->close();
        }
        else
        {
          std::cerr << "Cannot find device: " << argv[1] << std::endl;
        }
      }
    }
    else
    {
      std::cout << argv[0] << " -l|(<device-id> <options>)" << std::endl;
      std::cout << std::endl;
      std::cout << "-n <id>: Set user defined id" << std::endl;
      std::cout << "-d 1|0:  Switch DHCP on or off" << std::endl;
      std::cout << "-p 1|0:  Switch persistent IP on or off" << std::endl;
      std::cout << "-i <ip>: Set persistent IP address" << std::endl;
      std::cout << "-s <ip>: Set subnet mask for persistent IP address" << std::endl;
      std::cout << "-g <ip>: Set default gateway for persistent IP address" << std::endl;
    }
  }
  catch (const std::exception &ex)
  {
    std::cerr << ex.what() << std::endl;
  }

  return 0;
}