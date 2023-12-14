/*
 * This file is part of the rc_genicam_api package.
 *
 * Copyright (c) 2023 Roboception GmbH
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

#include "nodemap_out.h"

#include <iostream>

#ifdef USE_NCURSES
#include <ncurses.h>
#endif

#include <iostream>
#include <iomanip>

namespace rcg
{

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

}

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

void printNode(const std::string &prefix, GenApi::INode *node, int depth, bool show_enum_list)
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
        {
          GenApi::IRegister *p=dynamic_cast<GenApi::IRegister *>(node);

          if (p)
          {
            int len=static_cast<int>(p->GetLength());

            std::cout << prefix << "Register: " << node->GetName() << "[" << len << "] "
              << getAccessMode(node) << ": " << std::hex;

            if (GenApi::IsReadable(p))
            {
              uint8_t buffer[32];
              p->Get(buffer, std::min(len, 32));

              for (int i=0; i<len && i<32; i++)
              {
                std::cout << std::setfill('0') << std::setw(2) << static_cast<int>(buffer[i]);
              }

              if (len > 32)
              {
                std::cout << "...";
              }
            }

            std::cout << std::dec << std::endl;
          }
        }

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
                printNode(prefix+"  ", feature[i]->GetNode(), depth-1, show_enum_list);
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

          if (p != nullptr)
          {
            std::cout << '[';

            if (show_enum_list)
            {
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
            }
            else
            {
              std::cout << "...";
            }

            std::cout << "]: ";

            if (GenApi::IsReadable(p->GetAccessMode()) && p->GetCurrentEntry() != 0)
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

      default:
        break;
    }
  }
}

bool printNodemap(const std::shared_ptr<GenApi::CNodeMapRef> &nodemap, const char root[],
  int depth, bool show_enum_list)
{
  GenApi::INode *p=nodemap->_GetNode(root);
  bool ret=false;

  if (p)
  {
    printNode(std::string("  "), p, depth, show_enum_list);
    ret=true;
  }

  return ret;
}

}
