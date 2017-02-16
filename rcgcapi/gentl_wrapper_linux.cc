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

#include "gentl_wrapper.h"

#include <string>
#include <sstream>
#include <stdexcept>
#include <cstdlib>
#include <iostream>

#include <dlfcn.h>
#include <dirent.h>

namespace rcg
{

std::vector<std::string> getAvailableGenTLs(const char *paths)
{
  std::vector<std::string> ret;

  if (paths != 0)
  {
    // split path list into individual paths

    std::stringstream in(paths);
    std::string path;

    while (getline(in, path, ':'))
    {
      if (path.size() > 0)
      {
        if (path.size() > 4 && path.compare(path.size()-4, 4, ".cti") == 0)
        {
          // the given path points to one file ending with .cti

          ret.push_back(path);
        }
        else
        {
          // try to instantiate GenTLWrapper for all files in the given path that
          // end with .cti

          DIR *p=opendir(path.c_str());

          if (p != 0)
          {
            struct dirent *entry=readdir(p);

            while (entry != 0)
            {
              std::string name=entry->d_name;

              if (name.size() >= 4 && name.compare(name.size()-4, 4, ".cti") == 0)
              {
                ret.push_back(path+"/"+name);
              }

              entry=readdir(p);
            }

            closedir(p);
          }
        }
      }
    }
  }

  return ret;
}

GenTLWrapper::GenTLWrapper(const std::string &filename)
{
  // open library

  lib=dlopen(filename.c_str(), RTLD_NOW);

  if (lib == 0)
  {
    throw std::invalid_argument(std::string("Cannot open GenTL library: ")+dlerror());
  }

  dlerror(); // clear possible existing error

  // resolve function calls that will only be used privately

  *reinterpret_cast<void**>(&GCInitLib)=dlsym(lib, "GCInitLib");
  *reinterpret_cast<void**>(&GCCloseLib)=dlsym(lib, "GCCloseLib");

  // resolve public symbols

  *reinterpret_cast<void**>(&GCGetInfo)=dlsym(lib, "GCGetInfo");
  *reinterpret_cast<void**>(&GCGetLastError)=dlsym(lib, "GCGetLastError");
  *reinterpret_cast<void**>(&GCReadPort)=dlsym(lib, "GCReadPort");
  *reinterpret_cast<void**>(&GCWritePort)=dlsym(lib, "GCWritePort");
  *reinterpret_cast<void**>(&GCGetPortURL)=dlsym(lib, "GCGetPortURL");
  *reinterpret_cast<void**>(&GCGetPortInfo)=dlsym(lib, "GCGetPortInfo");

  *reinterpret_cast<void**>(&GCRegisterEvent)=dlsym(lib, "GCRegisterEvent");
  *reinterpret_cast<void**>(&GCUnregisterEvent)=dlsym(lib, "GCUnregisterEvent");
  *reinterpret_cast<void**>(&EventGetData)=dlsym(lib, "EventGetData");
  *reinterpret_cast<void**>(&EventGetDataInfo)=dlsym(lib, "EventGetDataInfo");
  *reinterpret_cast<void**>(&EventGetInfo)=dlsym(lib, "EventGetInfo");
  *reinterpret_cast<void**>(&EventFlush)=dlsym(lib, "EventFlush");
  *reinterpret_cast<void**>(&EventKill)=dlsym(lib, "EventKill");
  *reinterpret_cast<void**>(&TLOpen)=dlsym(lib, "TLOpen");
  *reinterpret_cast<void**>(&TLClose)=dlsym(lib, "TLClose");
  *reinterpret_cast<void**>(&TLGetInfo)=dlsym(lib, "TLGetInfo");
  *reinterpret_cast<void**>(&TLGetNumInterfaces)=dlsym(lib, "TLGetNumInterfaces");
  *reinterpret_cast<void**>(&TLGetInterfaceID)=dlsym(lib, "TLGetInterfaceID");
  *reinterpret_cast<void**>(&TLGetInterfaceInfo)=dlsym(lib, "TLGetInterfaceInfo");
  *reinterpret_cast<void**>(&TLOpenInterface)=dlsym(lib, "TLOpenInterface");
  *reinterpret_cast<void**>(&TLUpdateInterfaceList)=dlsym(lib, "TLUpdateInterfaceList");
  *reinterpret_cast<void**>(&IFClose)=dlsym(lib, "IFClose");
  *reinterpret_cast<void**>(&IFGetInfo)=dlsym(lib, "IFGetInfo");
  *reinterpret_cast<void**>(&IFGetNumDevices)=dlsym(lib, "IFGetNumDevices");
  *reinterpret_cast<void**>(&IFGetDeviceID)=dlsym(lib, "IFGetDeviceID");
  *reinterpret_cast<void**>(&IFUpdateDeviceList)=dlsym(lib, "IFUpdateDeviceList");
  *reinterpret_cast<void**>(&IFGetDeviceInfo)=dlsym(lib, "IFGetDeviceInfo");
  *reinterpret_cast<void**>(&IFOpenDevice)=dlsym(lib, "IFOpenDevice");

  *reinterpret_cast<void**>(&DevGetPort)=dlsym(lib, "DevGetPort");
  *reinterpret_cast<void**>(&DevGetNumDataStreams)=dlsym(lib, "DevGetNumDataStreams");
  *reinterpret_cast<void**>(&DevGetDataStreamID)=dlsym(lib, "DevGetDataStreamID");
  *reinterpret_cast<void**>(&DevOpenDataStream)=dlsym(lib, "DevOpenDataStream");
  *reinterpret_cast<void**>(&DevGetInfo)=dlsym(lib, "DevGetInfo");
  *reinterpret_cast<void**>(&DevClose)=dlsym(lib, "DevClose");

  *reinterpret_cast<void**>(&DSAnnounceBuffer)=dlsym(lib, "DSAnnounceBuffer");
  *reinterpret_cast<void**>(&DSAllocAndAnnounceBuffer)=dlsym(lib, "DSAllocAndAnnounceBuffer");
  *reinterpret_cast<void**>(&DSFlushQueue)=dlsym(lib, "DSFlushQueue");
  *reinterpret_cast<void**>(&DSStartAcquisition)=dlsym(lib, "DSStartAcquisition");
  *reinterpret_cast<void**>(&DSStopAcquisition)=dlsym(lib, "DSStopAcquisition");
  *reinterpret_cast<void**>(&DSGetInfo)=dlsym(lib, "DSGetInfo");
  *reinterpret_cast<void**>(&DSGetBufferID)=dlsym(lib, "DSGetBufferID");
  *reinterpret_cast<void**>(&DSClose)=dlsym(lib, "DSClose");
  *reinterpret_cast<void**>(&DSRevokeBuffer)=dlsym(lib, "DSRevokeBuffer");
  *reinterpret_cast<void**>(&DSQueueBuffer)=dlsym(lib, "DSQueueBuffer");
  *reinterpret_cast<void**>(&DSGetBufferInfo)=dlsym(lib, "DSGetBufferInfo");

  *reinterpret_cast<void**>(&GCGetNumPortURLs)=dlsym(lib, "GCGetNumPortURLs");
  *reinterpret_cast<void**>(&GCGetPortURLInfo)=dlsym(lib, "GCGetPortURLInfo");
  *reinterpret_cast<void**>(&GCReadPortStacked)=dlsym(lib, "GCReadPortStacked");
  *reinterpret_cast<void**>(&GCWritePortStacked)=dlsym(lib, "GCWritePortStacked");

  *reinterpret_cast<void**>(&DSGetBufferChunkData)=dlsym(lib, "DSGetBufferChunkData");

  *reinterpret_cast<void**>(&IFGetParentTL)=dlsym(lib, "IFGetParentTL");
  *reinterpret_cast<void**>(&DevGetParentIF)=dlsym(lib, "DevGetParentIF");
  *reinterpret_cast<void**>(&DSGetParentDev)=dlsym(lib, "DSGetParentDev");

  *reinterpret_cast<void**>(&DSGetNumBufferParts)=dlsym(lib, "DSGetNumBufferParts");
  *reinterpret_cast<void**>(&DSGetBufferPartInfo)=dlsym(lib, "DSGetBufferPartInfo");

  const char *err=dlerror();

  if (err != 0)
  {
    dlclose(lib);
    throw std::invalid_argument(std::string("Cannot resolve GenTL symbol: ")+err);
  }
}

GenTLWrapper::~GenTLWrapper()
{
  dlclose(lib);
}

}