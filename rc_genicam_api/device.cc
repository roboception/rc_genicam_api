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

#include "device.h"
#include "stream.h"

#include "gentl_wrapper.h"
#include "exception.h"
#include "cport.h"

#include <iostream>

namespace rcg
{

Device::Device(const std::shared_ptr<Interface> &_parent,
               const std::shared_ptr<const GenTLWrapper> &_gentl, const char *_id)
{
  parent=_parent;
  gentl=_gentl;
  id=_id;

  n_open=0;
  dev=0;
  rp=0;
}

Device::~Device()
{
  if (n_open > 0)
  {
    try
    {
      gentl->DevClose(dev);
    }
    catch (...) // do not throw exceptions in destructor
    { }

    parent->close();
  }
}

std::shared_ptr<Interface> Device::getParent() const
{
  return parent;
}

const std::string &Device::getID() const
{
  return id;
}

void Device::open(ACCESS access)
{
  if (n_open == 0)
  {
    parent->open();

    GenTL::DEVICE_ACCESS_FLAGS mode;

    switch (access)
    {
      case READONLY:
        mode=GenTL::DEVICE_ACCESS_READONLY;
        break;

      case CONTROL:
        mode=GenTL::DEVICE_ACCESS_CONTROL;
        break;

      default:
      case EXCLUSIVE:
        mode=GenTL::DEVICE_ACCESS_EXCLUSIVE;
        break;
    }

    if (gentl->IFOpenDevice(parent->getHandle(), id.c_str(), mode, &dev) != GenTL::GC_ERR_SUCCESS)
    {
      parent->close();
      throw GenTLException("Device::open() failed", gentl);
    }
  }

  n_open++;
}

void Device::close()
{
  if (n_open > 0)
  {
    n_open--;
  }

  if (n_open == 0)
  {
    gentl->DevClose(dev);
    dev=0;
    rp=0;

    nodemap=0;
    rnodemap=0;

    cport=0;
    rport=0;

    parent->close();
  }
}

namespace
{

int find(const std::vector<std::shared_ptr<Stream> > &list, const std::string &id)
{
  for (size_t i=0; i<list.size(); i++)
  {
    if (list[i]->getID() == id)
    {
      return i;
    }
  }

  return -1;
}

}

std::vector<std::shared_ptr<Stream> > Device::getStreams()
{
  std::vector<std::shared_ptr<Stream> > ret;

  if (dev != 0)
  {
    // get list of previously requested devices that are still in use

    std::vector<std::shared_ptr<Stream> > current;

    for (size_t i=0; i<slist.size(); i++)
    {
      std::shared_ptr<Stream> p=slist[i].lock();
      if (p)
      {
        current.push_back(p);
      }
    }

    // create list of interfaces, using either existing interfaces or
    // instantiating new ones

    uint32_t n=0;
    if (gentl->DevGetNumDataStreams(dev, &n) != GenTL::GC_ERR_SUCCESS)
    {
      throw GenTLException("Device::getStreams()", gentl);
    }

    for (uint32_t i=0; i<n; i++)
    {
      char tmp[256]="";
      size_t size=sizeof(tmp);

      if (gentl->DevGetDataStreamID(dev, i, tmp, &size) != GenTL::GC_ERR_SUCCESS)
      {
        throw GenTLException("Device::getStreams()", gentl);
      }

      int k=find(current, tmp);

      if (k >= 0)
      {
        ret.push_back(current[k]);
      }
      else
      {
        ret.push_back(std::shared_ptr<Stream>(new Stream(shared_from_this(), gentl, tmp)));
      }
    }

    // update internal list of devices for reusage on next call

    slist.clear();
    for (size_t i=0; i<ret.size(); i++)
    {
      slist.push_back(ret[i]);
    }
  }

  return ret;

}

namespace
{

std::string cDevGetInfo(const Device *obj, const std::shared_ptr<const GenTLWrapper> &gentl,
                        GenTL::DEVICE_INFO_CMD info)
{
  std::string ret;

  GenTL::INFO_DATATYPE type;
  char tmp[1024]="";
  size_t tmp_size=sizeof(tmp);
  GenTL::GC_ERROR err=GenTL::GC_ERR_ERROR;

  if (obj->getHandle() != 0)
  {
    err=gentl->DevGetInfo(obj->getHandle(), info, &type, tmp, &tmp_size);
  }
  else if (obj->getParent()->getHandle() != 0)
  {
    err=gentl->IFGetDeviceInfo(obj->getParent()->getHandle(), obj->getID().c_str(), info, &type,
                               tmp, &tmp_size);
  }

  if (err == GenTL::GC_ERR_SUCCESS && type == GenTL::INFO_DATATYPE_STRING)
  {
    for (size_t i=0; i<tmp_size && tmp[i] != '\0'; i++)
    {
      ret.push_back(tmp[i]);
    }
  }

  return ret;
}

}

std::string Device::getVendor() const
{
  return cDevGetInfo(this, gentl, GenTL::DEVICE_INFO_VENDOR);
}

std::string Device::getModel() const
{
  return cDevGetInfo(this, gentl, GenTL::DEVICE_INFO_MODEL);
}

std::string Device::getTLType() const
{
  return cDevGetInfo(this, gentl, GenTL::DEVICE_INFO_TLTYPE);
}

std::string Device::getDisplayName() const
{
  return cDevGetInfo(this, gentl, GenTL::DEVICE_INFO_DISPLAYNAME);
}

std::string Device::getAccessStatus() const
{
  std::string ret;

  GenTL::INFO_DATATYPE type;
  int32_t status=-1;
  size_t size=sizeof(ret);
  GenTL::GC_ERROR err=GenTL::GC_ERR_ERROR;

  if (dev != 0)
  {
    err=gentl->DevGetInfo(dev, GenTL::DEVICE_INFO_ACCESS_STATUS, &type, &status, &size);
  }
  else if (parent->getHandle() != 0)
  {
    err=gentl->IFGetDeviceInfo(getParent()->getHandle(), id.c_str(),
                               GenTL::DEVICE_INFO_ACCESS_STATUS, &type, &status, &size);
  }

  if (err == GenTL::GC_ERR_SUCCESS)
  {
    if (type == GenTL::INFO_DATATYPE_INT32)
    {
      switch (status)
      {
        case GenTL::DEVICE_ACCESS_STATUS_READWRITE:
          ret="ReadWrite";
          break;

        case GenTL::DEVICE_ACCESS_STATUS_READONLY:
          ret="ReadOnly";
          break;

        case GenTL::DEVICE_ACCESS_STATUS_NOACCESS:
          ret="NoAccess";
          break;

        case GenTL::DEVICE_ACCESS_STATUS_BUSY:
          ret="Busy";
          break;

        case GenTL::DEVICE_ACCESS_STATUS_OPEN_READWRITE:
          ret="OpenReadWrite";
          break;

        case GenTL::DEVICE_ACCESS_STATUS_OPEN_READONLY:
          ret="OpenReadWrite";
          break;

        default:
          ret="Unknown";
          break;
      }
    }
  }

  return ret;
}

std::string Device::getUserDefinedName() const
{
  return cDevGetInfo(this, gentl, GenTL::DEVICE_INFO_USER_DEFINED_NAME);
}

std::string Device::getSerialNumber() const
{
  return cDevGetInfo(this, gentl, GenTL::DEVICE_INFO_SERIAL_NUMBER);
}

std::string Device::getVersion() const
{
  return cDevGetInfo(this, gentl, GenTL::DEVICE_INFO_VERSION);
}

uint64_t Device::getTimestampFrequency() const
{
  GenTL::INFO_DATATYPE type;
  uint64_t freq=0;
  size_t size=sizeof(freq);

  if (dev != 0)
  {
    gentl->DevGetInfo(dev, GenTL::DEVICE_INFO_TIMESTAMP_FREQUENCY, &type, &freq, &size);
  }
  else if (parent->getHandle() != 0)
  {
    gentl->IFGetDeviceInfo(getParent()->getHandle(), id.c_str(),
                           GenTL::DEVICE_INFO_TIMESTAMP_FREQUENCY, &type, &freq, &size);
  }

  return freq;
}

std::shared_ptr<GenApi::CNodeMapRef> Device::getNodeMap()
{
  if (dev != 0 && !nodemap)
  {
    cport=std::shared_ptr<CPort>(new CPort(gentl, &dev));
    nodemap=allocNodeMap(gentl, dev, cport.get());
  }

  return nodemap;
}

std::shared_ptr<GenApi::CNodeMapRef> Device::getRemoteNodeMap(const char *xml)
{
  if (dev != 0 && !rnodemap)
  {
    if (gentl->DevGetPort(dev, &rp) == GenTL::GC_ERR_SUCCESS)
    {
      rport=std::shared_ptr<CPort>(new CPort(gentl, &rp));
      rnodemap=allocNodeMap(gentl, rp, rport.get(), xml);
    }
  }

  return rnodemap;
}

void *Device::getHandle() const
{
  return dev;
}

std::vector<std::shared_ptr<Device> > getDevices()
{
  std::vector<std::shared_ptr<Device> > ret;

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
        ret.push_back(device[j]);
      }

      interf[k]->close();
    }

    system[i]->close();
  }

  return ret;
}

std::shared_ptr<Device> getDevice(const char *id)
{
  std::shared_ptr<Device> ret;

  if (id != 0 && *id != '\0')
  {
    // split into interface and device id

    std::string interfid;
    std::string devid=id;

    size_t p=devid.find(':');
    if (p != std::string::npos)
    {
      interfid=devid.substr(0, p);
      devid=devid.substr(p+1);
    }

    // go through all systems

    std::vector<std::shared_ptr<rcg::System> > system=rcg::System::getSystems();

    for (size_t i=0; i<system.size() && !ret; i++)
    {
      system[i]->open();

      // get all interfaces

      std::vector<std::shared_ptr<rcg::Interface> > interf=system[i]->getInterfaces();

      if (interfid.size() > 0)
      {
        // if an interface is defined, then only search this interface

        for (size_t k=0; k<interf.size() && !ret; k++)
        {
          if (interf[k]->getID() == interfid)
          {
            interf[k]->open();
            ret=interf[k]->getDevice(devid.c_str());
            interf[k]->close();
          }
        }
      }
      else
      {
        // if interface is not defined, then check all interfaces

        for (size_t k=0; k<interf.size() && !ret; k++)
        {
          interf[k]->open();
          ret=interf[k]->getDevice(devid.c_str());
          interf[k]->close();
        }
      }

      system[i]->close();
    }
  }

  return ret;
}

}
