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

#include "interface.h"
#include "device.h"

#include "gentl_wrapper.h"
#include "exception.h"
#include "cport.h"

#include <iostream>

namespace rcg
{

Interface::Interface(const std::shared_ptr<System> &_parent,
                     const std::shared_ptr<const GenTLWrapper> &_gentl, const char *_id)
{
  parent=_parent;
  gentl=_gentl;
  id=_id;

  n_open=0;
  ifh=0;
}

Interface::~Interface()
{
  if (n_open > 0)
  {
    gentl->IFClose(ifh);
    parent->close();
  }
}

std::shared_ptr<System> Interface::getParent() const
{
  return parent;
}

const std::string &Interface::getID() const
{
  return id;
}

void Interface::open()
{
  if (n_open == 0)
  {
    parent->open();

    // updating before opening is apparently necessary for some TLs

    gentl->TLUpdateInterfaceList(parent->getHandle(), 0, 10);

    if (gentl->TLOpenInterface(parent->getHandle(), id.c_str(), &ifh) != GenTL::GC_ERR_SUCCESS)
    {
      parent->close();
      throw GenTLException("Interface::open()", gentl);
    }
  }

  n_open++;
}

void Interface::close()
{
  if (n_open > 0)
  {
    n_open--;
  }

  if (n_open == 0)
  {
    gentl->IFClose(ifh);
    ifh=0;

    nodemap=0;
    cport=0;

    parent->close();
  }
}

namespace
{

int find(const std::vector<std::shared_ptr<Device> > &list, const std::string &id)
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

std::vector<std::shared_ptr<Device> > Interface::getDevices()
{
  std::vector<std::shared_ptr<Device> > ret;

  if (ifh != 0)
  {
    // get list of previously requested devices that are still in use

    std::vector<std::shared_ptr<Device> > current;

    for (size_t i=0; i<dlist.size(); i++)
    {
      std::shared_ptr<Device> p=dlist[i].lock();
      if (p)
      {
        current.push_back(p);
      }
    }

    // update available interfaces

    if (gentl->IFUpdateDeviceList(ifh, 0, 10) != GenTL::GC_ERR_SUCCESS)
    {
      throw GenTLException("Interface::getDevices()", gentl);
    }

    // create list of interfaces, using either existing interfaces or
    // instantiating new ones

    uint32_t n=0;
    if (gentl->IFGetNumDevices(ifh, &n) != GenTL::GC_ERR_SUCCESS)
    {
      throw GenTLException("Interface::getDevices()", gentl);
    }

    for (uint32_t i=0; i<n; i++)
    {
      char tmp[256]="";
      size_t size=sizeof(tmp);

      if (gentl->IFGetDeviceID(ifh, i, tmp, &size) != GenTL::GC_ERR_SUCCESS)
      {
        throw GenTLException("Interface::getDevices()", gentl);
      }

      int k=find(current, tmp);

      if (k >= 0)
      {
        ret.push_back(current[k]);
      }
      else
      {
        ret.push_back(std::shared_ptr<Device>(new Device(shared_from_this(), gentl, tmp)));
      }
    }

    // update internal list of devices for reusage on next call

    dlist.clear();
    for (size_t i=0; i<ret.size(); i++)
    {
      dlist.push_back(ret[i]);
    }
  }

  return ret;
}

std::shared_ptr<Device> Interface::getDevice(const char *devid)
{
  std::shared_ptr<Device> ret;

  // check if the device is already available

  for (size_t i=0; i<dlist.size(); i++)
  {
    std::shared_ptr<Device> p=dlist[i].lock();

    if (p && p->getID() == devid)
    {
      ret=p;
    }
  }

  // find new device by opening it

  if (!ret && ifh != 0)
  {
    GenTL::DEV_HANDLE dev=0;
    if (gentl->IFOpenDevice(ifh, devid, GenTL::DEVICE_ACCESS_READONLY, &dev) ==
        GenTL::GC_ERR_SUCCESS)
    {
      gentl->DevClose(dev);

      ret=std::shared_ptr<Device>(new Device(shared_from_this(), gentl, devid));

      dlist.push_back(ret);
    }
  }

  return ret;
}

namespace
{

std::string cIFGetInfo(const Interface *obj,
                       const std::shared_ptr<const GenTLWrapper> &gentl,
                       GenTL::INTERFACE_INFO_CMD info)
{
  std::string ret;

  GenTL::INFO_DATATYPE type;
  char tmp[1024]="";
  size_t tmp_size=sizeof(tmp);
  GenTL::GC_ERROR err=GenTL::GC_ERR_ERROR;

  if (obj->getHandle() != 0)
  {
    err=gentl->IFGetInfo(obj->getHandle(), info, &type, tmp, &tmp_size);
  }
  else if (obj->getParent()->getHandle() != 0)
  {
    err=gentl->TLGetInterfaceInfo(obj->getParent()->getHandle(), obj->getID().c_str(), info,
                                  &type, tmp, &tmp_size);
  }

  if (err == GenTL::GC_ERR_SUCCESS && type == GenTL::INFO_DATATYPE_STRING)
  {
    ret=tmp;
  }

  return ret;
}

}

std::string Interface::getDisplayName() const
{
  return cIFGetInfo(this, gentl, GenTL::INTERFACE_INFO_DISPLAYNAME);
}

std::string Interface::getTLType() const
{
  return cIFGetInfo(this, gentl, GenTL::INTERFACE_INFO_TLTYPE);
}

std::shared_ptr<GenApi::CNodeMapRef> Interface::getNodeMap()
{
  if (ifh != 0 && !nodemap)
  {
    cport=std::shared_ptr<CPort>(new CPort(gentl, &ifh));
    nodemap=allocNodeMap(gentl, ifh, cport.get());
  }

  return nodemap;
}

void *Interface::getHandle() const
{
  return ifh;
}

}