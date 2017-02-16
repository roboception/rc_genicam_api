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

#include "stream.h"

#include "gentl_wrapper.h"
#include "exception.h"
#include "cport.h"

#include <iostream>

namespace rcg
{

Stream::Stream(const std::shared_ptr<Device> &_parent,
               const std::shared_ptr<const GenTLWrapper> &_gentl, const char *_id)
{
  parent=_parent;
  gentl=_gentl;
  id=_id;

  n_open=0;
  stream=0;
}

Stream::~Stream()
{
  if (n_open > 0)
  {
    gentl->DSClose(stream);
  }
}

std::shared_ptr<Device> Stream::getParent() const
{
  return parent;
}

const std::string &Stream::getID() const
{
  return id;
}

void Stream::open()
{
  if (n_open == 0)
  {
    if (parent->getHandle() != 0)
    {
      if (gentl->DevOpenDataStream(parent->getHandle(), id.c_str(), &stream) !=
          GenTL::GC_ERR_SUCCESS)
      {
        throw GenTLException("Stream::open()", gentl);
      }
    }
    else
    {
      throw GenTLException("Stream::open(): Device must be opened before open before opening a stream");
    }
  }

  n_open++;
}

void Stream::close()
{
  if (n_open > 0)
  {
    n_open--;
  }

  if (n_open == 0)
  {
    gentl->DSClose(stream);
    stream=0;

    nodemap=0;
    cport=0;
  }
}

uint64_t Stream::getNumDelivered() const
{
  uint64_t ret=0;

  GenTL::INFO_DATATYPE type;
  size_t size=sizeof(ret);

  if (stream != 0)
  {
    gentl->DSGetInfo(stream, GenTL::STREAM_INFO_NUM_DELIVERED, &type, &ret, &size);
  }

  return ret;
}

uint64_t Stream::getNumUnderrun() const
{
  uint64_t ret=0;

  GenTL::INFO_DATATYPE type;
  size_t size=sizeof(ret);

  if (stream != 0)
  {
    gentl->DSGetInfo(stream, GenTL::STREAM_INFO_NUM_UNDERRUN, &type, &ret, &size);
  }

  return ret;
}

size_t Stream::getNumAnnounced() const
{
  size_t ret=0;

  GenTL::INFO_DATATYPE type;
  size_t size=sizeof(ret);

  if (stream != 0)
  {
    gentl->DSGetInfo(stream, GenTL::STREAM_INFO_NUM_ANNOUNCED, &type, &ret, &size);
  }

  return ret;
}

size_t Stream::getNumQueued() const
{
  size_t ret=0;

  GenTL::INFO_DATATYPE type;
  size_t size=sizeof(ret);

  if (stream != 0)
  {
    gentl->DSGetInfo(stream, GenTL::STREAM_INFO_NUM_QUEUED, &type, &ret, &size);
  }

  return ret;
}

size_t Stream::getNumAwaitDelivery() const
{
  size_t ret=0;

  GenTL::INFO_DATATYPE type;
  size_t size=sizeof(ret);

  if (stream != 0)
  {
    gentl->DSGetInfo(stream, GenTL::STREAM_INFO_NUM_AWAIT_DELIVERY, &type, &ret, &size);
  }

  return ret;
}

uint64_t Stream::getNumStarted() const
{
  uint64_t ret=0;

  GenTL::INFO_DATATYPE type;
  size_t size=sizeof(ret);

  if (stream != 0)
  {
    gentl->DSGetInfo(stream, GenTL::STREAM_INFO_NUM_STARTED, &type, &ret, &size);
  }

  return ret;
}

size_t Stream::getPayloadSize() const
{
  size_t ret=0;

  GenTL::INFO_DATATYPE type;
  size_t size=sizeof(ret);

  if (stream != 0)
  {
    gentl->DSGetInfo(stream, GenTL::STREAM_INFO_PAYLOAD_SIZE, &type, &ret, &size);
  }

  return ret;
}

bool Stream::getIsGrabbing() const
{
  uint8_t ret=0;

  GenTL::INFO_DATATYPE type;
  size_t size=sizeof(ret);

  if (stream != 0)
  {
    gentl->DSGetInfo(stream, GenTL::STREAM_INFO_IS_GRABBING, &type, &ret, &size);
  }

  return ret != 0;
}

bool Stream::getDefinesPayloadsize() const
{
  uint8_t ret=0;

  GenTL::INFO_DATATYPE type;
  size_t size=sizeof(ret);

  if (stream != 0)
  {
    gentl->DSGetInfo(stream, GenTL::STREAM_INFO_DEFINES_PAYLOADSIZE, &type, &ret, &size);
  }

  return ret != 0;
}

std::string Stream::getTLType() const
{
  std::string ret;

  GenTL::INFO_DATATYPE type;
  char tmp[1024]="";
  size_t size=sizeof(tmp);

  if (stream != 0)
  {
    if (gentl->DSGetInfo(stream, GenTL::STREAM_INFO_TLTYPE, &type, &ret, &size) ==
        GenTL::GC_ERR_SUCCESS)
    {
      if (type == GenTL::INFO_DATATYPE_STRING)
      {
        ret=tmp;
      }
    }
  }

  return ret;
}

size_t Stream::getNumChunksMax() const
{
  size_t ret=0;

  GenTL::INFO_DATATYPE type;
  size_t size=sizeof(ret);

  if (stream != 0)
  {
    gentl->DSGetInfo(stream, GenTL::STREAM_INFO_NUM_CHUNKS_MAX, &type, &ret, &size);
  }

  return ret;
}

size_t Stream::getBufAnnounceMin() const
{
  size_t ret=0;

  GenTL::INFO_DATATYPE type;
  size_t size=sizeof(ret);

  if (stream != 0)
  {
    gentl->DSGetInfo(stream, GenTL::STREAM_INFO_BUF_ANNOUNCE_MIN, &type, &ret, &size);
  }

  return ret;
}

size_t Stream::getBufAlignment() const
{
  size_t ret=0;

  GenTL::INFO_DATATYPE type;
  size_t size=sizeof(ret);

  if (stream != 0)
  {
    gentl->DSGetInfo(stream, GenTL::STREAM_INFO_BUF_ALIGNMENT, &type, &ret, &size);
  }

  return ret;
}

std::shared_ptr<GenApi::CNodeMapRef> Stream::getNodeMap()
{
  if (stream != 0 && !nodemap)
  {
    cport=std::shared_ptr<CPort>(new CPort(gentl, &stream));
    nodemap=allocNodeMap(gentl, stream, cport.get());
  }

  return nodemap;
}

void *Stream::getHandle() const
{
  return stream;
}

}