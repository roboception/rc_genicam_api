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

#include <iostream>

namespace
{

/**
  Set a string to a nodemap.

  @param nodemap Nodemap.
  @param key     Key of node in nodemap.
  @param value   New value of node.
*/

void setString(std::shared_ptr<GenApi::CNodeMapRef> &nodemap, const char *key, const char *value)
{
  GenApi::INode *node=nodemap->_GetNode(key);
  GenApi::IString *val=dynamic_cast<GenApi::IString *>(node);

  if (GenApi::IsWritable(val))
  {
    val->SetValue(value);
  }
  else
  {
    std::cerr << "Cannot set '" << key << "'" << std::endl;
  }
}

/**
  Set a formatted IP address, given as string to a nodemap.

  @param nodemap Nodemap.
  @param key     Key of node in nodemap.
  @param value   IP address in the format xxx.xxx.xxx.xxx
*/

void setIP(std::shared_ptr<GenApi::CNodeMapRef> &nodemap, const char *key, const char *value)
{
  GenApi::INode *node=nodemap->_GetNode(key);
  GenApi::IInteger *val=dynamic_cast<GenApi::IInteger *>(node);

  if (GenApi::IsWritable(val))
  {
    int64_t ip=0;

    std::stringstream in(value);
    std::string elem;

    getline(in, elem, '.');
    ip=stoi(elem)&0xff;

    getline(in, elem, '.');
    ip=(ip<<8)|(stoi(elem)&0xff);

    getline(in, elem, '.');
    ip=(ip<<8)|(stoi(elem)&0xff);

    getline(in, elem, '.');
    ip=(ip<<8)|(stoi(elem)&0xff);

    val->SetValue(ip);
  }
  else
  {
    std::cerr << "Cannot set '" << key << "'" << std::endl;
  }
}

/**
  Set 0 or != 0 as boolean value to a nodemap.

  @param nodemap Nodemap.
  @param key     Key of node in nodemap.
  @param value   0 or != 0 as string.
*/

void setBoolean(std::shared_ptr<GenApi::CNodeMapRef> &nodemap, const char *key, const char *value)
{
  GenApi::INode *node=nodemap->_GetNode(key);
  GenApi::IBoolean *val=dynamic_cast<GenApi::IBoolean *>(node);

  if (GenApi::IsWritable(val))
  {
    val->SetValue(std::stoi(std::string(value)));
  }
  else
  {
    std::cerr << "Cannot set '" << key << "'" << std::endl;
  }
}

/**
  Retrieves a value from a string node.

  @param nodemap Nodemap.
  @param key     Key of node.
  @return        Value of node.
*/

std::string getString(std::shared_ptr<GenApi::CNodeMapRef> &nodemap, const char *key)
{
  std::string ret;

  GenApi::INode *node=nodemap->_GetNode(key);
  GenApi::IString *val=dynamic_cast<GenApi::IString *>(node);

  if (GenApi::IsReadable(val))
  {
    ret=val->GetValue();
  }

  return ret;
}

/**
  Retrieves a MAC address as string from an integer node.

  @param nodemap Nodemap.
  @param key     Key of node.
  @return        Value of node.
*/

std::string getMac(std::shared_ptr<GenApi::CNodeMapRef> &nodemap, const char *key)
{
  std::ostringstream ret;

  GenApi::INode *node=nodemap->_GetNode(key);
  GenApi::IInteger *val=dynamic_cast<GenApi::IInteger *>(node);

  if (GenApi::IsReadable(val))
  {
    int64_t v=val->GetValue();

    ret << std::hex << ((v>>32)&0xff) << ':' << ((v>>30)&0xff) << ':'
                    << ((v>>24)&0xff) << ':' << ((v>>16)&0xff) << ':'
                    << ((v>>8)&0xff) << ':' << (v&0xff);
  }

  return ret.str();
}

/**
  Retrieves an IP address as string from an integer node.

  @param nodemap Nodemap.
  @param key     Key of node.
  @return        Value of node.
*/

std::string getIP(std::shared_ptr<GenApi::CNodeMapRef> &nodemap, const char *key)
{
  std::ostringstream ret;

  GenApi::INode *node=nodemap->_GetNode(key);
  GenApi::IInteger *val=dynamic_cast<GenApi::IInteger *>(node);

  if (GenApi::IsReadable(val))
  {
    int64_t v=val->GetValue();

    ret << ((v>>24)&0xff) << '.' << ((v>>16)&0xff) << '.'
        << ((v>>8)&0xff) << '.' << (v&0xff);
  }

  return ret.str();
}

/**
  Get a boolean value as string from a boolean node.

  @param nodemap Nodemap.
  @param key     Key of node.
  @return        Value of node.
*/

std::string getBoolean(std::shared_ptr<GenApi::CNodeMapRef> &nodemap, const char *key)
{
  std::ostringstream ret;

  GenApi::INode *node=nodemap->_GetNode(key);
  GenApi::IBoolean *val=dynamic_cast<GenApi::IBoolean *>(node);

  if (GenApi::IsReadable(val))
  {
    ret << val->GetValue();
  }

  return ret.str();
}

}

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
        // discover and open device

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
              setString(nodemap, "DeviceUserID", argv[i++]);
            }
            else if (p == "-d") // switch dhcp on or off
            {
              setBoolean(nodemap, "GevCurrentIPConfigurationDHCP", argv[i++]);
            }
            else if (p == "-p") // switch persistent IP on or off
            {
              setBoolean(nodemap, "GevCurrentIPConfigurationPersistentIP", argv[i++]);
            }
            else if (p == "-i") // set persistent IP address
            {
              setIP(nodemap, "GevPersistentIPAddress", argv[i++]);
            }
            else if (p == "-s") // set persistent subnet mask
            {
              setIP(nodemap, "GevPersistentSubnetMask", argv[i++]);
            }
            else if (p == "-g") // set persistent gateway
            {
              setIP(nodemap, "GevPersistentDefaultGateway", argv[i++]);
            }
          }

          // print network configuration of the device

          std::cout << "Device ID:       " << getString(nodemap, "DeviceID") << std::endl;
          std::cout << "User defined ID: " << getString(nodemap, "DeviceUserID") << std::endl;
          std::cout << "MAC Address:     " << getMac(nodemap, "GevMACAddress") << std::endl;
          std::cout << std::endl;

          std::cout << "IP:              " << getIP(nodemap, "GevCurrentIPAddress") << std::endl;
          std::cout << "Subnet mask:     " << getIP(nodemap, "GevCurrentSubnetMask") << std::endl;
          std::cout << "Default gateway: " << getIP(nodemap, "GevCurrentDefaultGateway") << std::endl;
          std::cout << std::endl;

          std::cout << "Persistent IP:   " << getBoolean(nodemap, "GevCurrentIPConfigurationPersistentIP") << std::endl;
          std::cout << "DHCP:            " << getBoolean(nodemap, "GevCurrentIPConfigurationDHCP") << std::endl;
          std::cout << "Link local:      " << getBoolean(nodemap, "GevCurrentIPConfigurationLLA") << std::endl;

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