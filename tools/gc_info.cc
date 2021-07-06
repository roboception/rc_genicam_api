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
#include <rc_genicam_api/config.h>

#include <iostream>

namespace
{

/**
  Returns the access mode of a node as string.

  @param node Node.
  @return     String with access mode.
*/

const char *getAccessMode(const GenApi::INode *node)
{
  switch (node->GetAccessMode())
  {
    case GenApi::NI:
      return "(NI)";

    case GenApi::NA:
      return "(NA)";

    case GenApi::WO:
      return "(WO)";

    case GenApi::RO:
      return "(RO)";

    case GenApi::RW:
      return "(RW)";

    case GenApi::_UndefinedAccesMode:
      return "(undefined access mode)";

    case GenApi::_CycleDetectAccesMode:
      return "(cycle detection)";

    default:
      return "(unknown)";
  }
}

/**
  Takes an integer value and formats it according to the specification in the
  node.

  @param node  Node.
  @param value Integer value.
  @return      Formated value.
*/

std::string formatValue(GenApi::IInteger *node, int64_t value)
{
  std::ostringstream out;

  switch (node->GetRepresentation())
  {
    case GenApi::HexNumber:
      out << "0x" << std::hex << value;
      break;

    case GenApi::IPV4Address:
       out << ((value>>24)&0xff) << '.' << ((value>>16)&0xff) << '.'
           << ((value>>8)&0xff) << '.' << (value&0xff);
       break;

    case GenApi::MACAddress:
       out << std::hex << ((value>>40)&0xff) << ':' << ((value>>32)&0xff) << ':'
                       << ((value>>24)&0xff) << ':' << ((value>>16)&0xff) << ':'
                       << ((value>>8)&0xff) << ':' << (value&0xff);
       break;

    default:
      out << value;
      break;
  }

  return out.str();
}

/**
  Recursive printing of nodes to standard out.

  @param prefix Prefix that will be prepended to every line.
  @param node   Node to be printed.
  @param depth  This value is reduced when calling printNode() recursively on
                category nodes. If the value is <= 0, then no recursion is
                done.
*/

void printNode(const std::string &prefix, GenApi::INode *node, int depth)
{
  if (node != 0 && node->GetAccessMode() != GenApi::NI)
  {
    switch (node->GetPrincipalInterfaceType())
    {
      case GenApi::intfIValue:
        std::cout << prefix << "Value: " << node->GetName() << " " << getAccessMode(node)
                  << std::endl;
        break;

      case GenApi::intfIBase:
        std::cout << prefix << "Base: " << node->GetName() << " " << getAccessMode(node)
                  << std::endl;
        break;

      case GenApi::intfIInteger:
        {
          std::cout << prefix << "Integer: " << node->GetName() << " "
                    << getAccessMode(node) << " ";

          GenApi::IInteger *p=dynamic_cast<GenApi::IInteger *>(node);

          if (GenApi::IsReadable(p))
          {
            std::cout << "[" << formatValue(p, p->GetMin()) << ", "
                      << formatValue(p, p->GetMax()) << "]: ";
            std::cout << formatValue(p, p->GetValue()) << " " << p->GetUnit();
          }

          std::cout << std::endl;
        }
        break;

      case GenApi::intfIBoolean:
        {
          std::cout << prefix << "Boolean: " << node->GetName() << " " << getAccessMode(node);

          GenApi::IBoolean *p=dynamic_cast<GenApi::IBoolean *>(node);

          if (GenApi::IsReadable(p))
          {
            std::cout << ": " << p->GetValue();
          }

          std::cout << std::endl;
        }
        break;

      case GenApi::intfICommand:
        std::cout << prefix << "Command: " << node->GetName() << " " << getAccessMode(node)
                  << std::endl;
        break;

      case GenApi::intfIFloat:
        {
          std::cout << prefix << "Float: " << node->GetName() << " " << getAccessMode(node)
                    << " ";

          GenApi::IFloat *p=dynamic_cast<GenApi::IFloat *>(node);

          if (GenApi::IsReadable(p))
          {
            std::cout << "[" << p->GetMin() << ", "
                      << p->GetMax() << "]: "
                      << p->GetValue() << " " << p->GetUnit();
          }

          std::cout << std::endl;
        }
        break;

      case GenApi::intfIString:
        {
          std::cout << prefix << "String: " << node->GetName() << " " << getAccessMode(node)
                    << ": ";

          GenApi::IString *p=dynamic_cast<GenApi::IString *>(node);

          if (GenApi::IsReadable(p))
          {
            std::cout << p->GetValue();
          }

          std::cout << std::endl;
        }
        break;

      case GenApi::intfIRegister:
        std::cout << prefix << "Register: " << node->GetName() << " " << getAccessMode(node)
                  << std::endl;
        break;

      case GenApi::intfICategory:
        {
          std::cout << prefix << "Category: " << node->GetName() << " "
                    << getAccessMode(node) << std::endl;

          if (depth > 0)
          {
            GenApi::ICategory *root=dynamic_cast<GenApi::ICategory *>(node);

            if (root != 0)
            {
              GenApi::FeatureList_t feature;
              root->GetFeatures(feature);

              for (size_t i=0; i<feature.size(); i++)
              {
                printNode(prefix+"  ", feature[i]->GetNode(), depth-1);
              }
            }
          }
        }
        break;

      case GenApi::intfIEnumeration:
        {
          std::cout << prefix << "Enumeration: " << node->GetName() << " " << getAccessMode(node)
                    << ' ';

          GenApi::IEnumeration *p=dynamic_cast<GenApi::IEnumeration *>(node);

          if (GenApi::IsReadable(p))
          {
            std::cout << '[';

            GenApi::StringList_t list;
            p->GetSymbolics(list);

            for (size_t i=0; i<list.size(); i++)
            {
              if (i > 0)
              {
                std::cout << ' ';
              }

              std::cout << list[i];
            }

            std::cout << "]: ";

            if (p->GetCurrentEntry() != 0)
            {
              std::cout << p->GetCurrentEntry()->GetSymbolic();
            }
          }

          std::cout << std::endl;
        }
        break;

      case GenApi::intfIEnumEntry:
        std::cout << prefix << "EnumEntry: " << node->GetName() << " " << getAccessMode(node)
                  << std::endl;
        break;

      case GenApi::intfIPort:
        std::cout << prefix << "Port: " << node->GetName() << " " << getAccessMode(node)
                  << std::endl;
        break;

    }
  }
}

}

int main(int argc, char *argv[])
{
  int ret=0;

  try
  {
    if (argc >= 2 && std::string(argv[1]) != "-h")
    {
      if (std::string(argv[1]) == "-l")
      {
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

          for (size_t k=0; k<interf.size(); k++)
          {
            interf[k]->open();

            std::cout << "    Interface     " << interf[k]->getID() << std::endl;
            std::cout << "    Display name: " << interf[k]->getDisplayName() << std::endl;
            std::cout << "    TL type:      " << interf[k]->getTLType() << std::endl;
            std::cout << std::endl;

            std::vector<std::shared_ptr<rcg::Device> > device=interf[k]->getDevices();

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
      else
      {
        int k=1;

        // get parameters, if any

        const char *xml=0;
        if (k+1 < argc && std::string(argv[k]) == "-o")
        {
          k++;
          xml=argv[k++];

          if (std::string(xml) == ".")
          {
            xml="";
          }
        }

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
              node=devid.substr(j+1);
              devid=devid.substr(0, j);
              depth=1;

              if (node.size() == 0)
              {
                node="Root";
              }
            }
          }

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
            else
            {
              dev->open(rcg::Device::READONLY);
            }

            std::shared_ptr<GenApi::CNodeMapRef> nodemap=dev->getRemoteNodeMap(xml);

            if (nodemap)
            {
              while (k < argc)
              {
                std::string p=argv[k++];

                if (p.find('=') != std::string::npos)
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

                std::cout << "Available features:" << std::endl;
                printNode(std::string("  "), nodemap->_GetNode(node.c_str()), depth);
              }
              else
              {
                // report requested node only

                GenApi::INode *p=nodemap->_GetNode(node.c_str());

                if (p)
                {
                  printNode(std::string(), p, depth);
                }
                else
                {
                  std::cerr << "Unknown node: " << node << std::endl;
                  ret=1;
                }
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
    else
    {
      std::cout << argv[0] << " -h | -l | ([-o <xml-output-file>|.] [<interface-id>:]<device-id>[?<node>] [<key>=<value>] ...)" << std::endl;
      std::cout << std::endl;
      std::cout << "Provides information about GenICam transport layers, interfaces and devices." << std::endl;
      std::cout << std::endl;
      std::cout << "Options: " << std::endl;
      std::cout << "-h   Prints help information and exits" << std::endl;
      std::cout << "-l   List all all available devices on all interfaces" << std::endl;
      std::cout << "-o   Store XML description from specified device" << std::endl;
      std::cout << std::endl;
      std::cout << "Parameters:" << std::endl;
      std::cout << "<interface-id> Optional GenICam ID of interface for connecting to the device" << std::endl;
      std::cout << "<device-id>    GenICam device ID, serial number or user defined name of device" << std::endl;
      std::cout << "<node>         Optional name of category or parameter to be reported" << std::endl;
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
