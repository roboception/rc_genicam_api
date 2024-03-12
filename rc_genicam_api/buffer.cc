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

#include "buffer.h"
#include "stream.h"
#include "config.h"

#include "gentl_wrapper.h"
#include "exception.h"

#include <GenApi/ChunkAdapterGEV.h>
#include <GenApi/ChunkAdapterU3V.h>
#include <GenApi/ChunkAdapterGeneric.h>

namespace rcg
{

namespace
{

template<class T> inline T getBufferValue(const std::shared_ptr<const GenTLWrapper> &gentl,
                                          void *stream, void *buffer, GenTL::BUFFER_INFO_CMD cmd)
{
  T ret=0;

  GenTL::INFO_DATATYPE type;
  size_t size=sizeof(T);

  if (stream != 0 && buffer != 0)
  {
    if (gentl->DSGetBufferInfo(stream, buffer, cmd, &type, &ret, &size) != GenTL::GC_ERR_SUCCESS)
    {
      ret=0;
    }
  }

  return ret;
}

inline bool getBufferBool(const std::shared_ptr<const GenTLWrapper> &gentl,
                          void *stream, void *buffer, GenTL::BUFFER_INFO_CMD cmd)
{
  bool8_t ret=0;

  GenTL::INFO_DATATYPE type;
  size_t size=sizeof(ret);

  if (stream != 0 && buffer != 0)
  {
    gentl->DSGetBufferInfo(stream, buffer, cmd, &type, &ret, &size);
  }

  return ret != 0;
}

inline std::string getBufferString(const std::shared_ptr<const GenTLWrapper> &gentl,
                                   void *stream, void *buffer, GenTL::BUFFER_INFO_CMD cmd)
{
  std::string ret;

  GenTL::INFO_DATATYPE type;
  char tmp[1024]="";
  size_t size=sizeof(tmp);

  if (stream != 0 && buffer != 0)
  {
    if (gentl->DSGetBufferInfo(stream, buffer, cmd, &type, &tmp, &size) == GenTL::GC_ERR_SUCCESS)
    {
      if (type == GenTL::INFO_DATATYPE_STRING)
      {
        ret=tmp;
      }
    }
  }

  return ret;
}

template<class T> inline T getBufferPartValue(const std::shared_ptr<const GenTLWrapper> &gentl,
                                              void *stream, void *buffer, uint32_t part,
                                              GenTL::BUFFER_PART_INFO_CMD cmd)
{
  T ret=0;

  GenTL::INFO_DATATYPE type;
  size_t size=sizeof(T);

  if (stream != 0 && buffer != 0)
  {
    gentl->DSGetBufferPartInfo(stream, buffer, part, cmd, &type, &ret, &size);
  }

  return ret;
}

}

Buffer::Buffer(const std::shared_ptr<const GenTLWrapper> &_gentl, Stream *_parent)
{
  parent=_parent;
  gentl=_gentl;
  buffer=0;
  multipart=false;
}

Buffer::~Buffer()
{
  if (chunkadapter)
  {
    chunkadapter->DetachBuffer();
  }
}

void Buffer::setNodemap(const std::shared_ptr<GenApi::CNodeMapRef> _nodemap, const std::string &tltype)
{
  nodemap=_nodemap;
  chunkadapter.reset();

  if (nodemap != 0)
  {
    if (getBoolean(nodemap, "ChunkModeActive", false))
    {
      if (tltype == "GEV")
      {
        chunkadapter=std::shared_ptr<GenApi::CChunkAdapter>(new GenApi::CChunkAdapterGEV(nodemap->_Ptr));
      }
      else if (tltype == "U3V")
      {
        chunkadapter=std::shared_ptr<GenApi::CChunkAdapter>(new GenApi::CChunkAdapterU3V(nodemap->_Ptr));
      }
      else
      {
        chunkadapter=std::shared_ptr<GenApi::CChunkAdapter>(new GenApi::CChunkAdapterGeneric(nodemap->_Ptr));
      }
    }
  }
}

void Buffer::setHandle(void *handle)
{
  buffer=handle;

  payload_type=PAYLOAD_TYPE_UNKNOWN;
  multipart=false;

  if (buffer != 0)
  {
    payload_type=getBufferValue<size_t>(gentl, parent->getHandle(), buffer,
                                        GenTL::BUFFER_INFO_PAYLOADTYPE);

    multipart=getBufferValue<size_t>(gentl, parent->getHandle(), buffer,
                                     GenTL::BUFFER_INFO_PAYLOADTYPE) == PAYLOAD_TYPE_MULTI_PART;

    if (chunkadapter && !getBufferBool(gentl, parent->getHandle(), buffer,
      GenTL::BUFFER_INFO_IS_INCOMPLETE))
    {
      chunkadapter->AttachBuffer(reinterpret_cast<uint8_t *>(
        getBufferValue<void *>(gentl, parent->getHandle(), buffer, GenTL::BUFFER_INFO_BASE)),
        static_cast<int64_t>(getBufferValue<size_t>(gentl, parent->getHandle(), buffer, GenTL::BUFFER_INFO_SIZE_FILLED)));
    }
  }
  else
  {
    if (chunkadapter)
    {
      chunkadapter->DetachBuffer();
    }
  }
}

uint32_t Buffer::getNumberOfParts() const
{
  uint32_t ret=0;

  if (multipart)
  {
    gentl->DSGetNumBufferParts(parent->getHandle(), buffer, &ret);
  }
  else
  {
    size_t type=getBufferValue<size_t>(gentl, parent->getHandle(), buffer,
                                       GenTL::BUFFER_INFO_PAYLOADTYPE);

    if (type != PAYLOAD_TYPE_CHUNK_ONLY)
    {
      ret=1;
    }
  }

  return ret;
}

void *Buffer::getGlobalBase() const
{
  return getBufferValue<void *>(gentl, parent->getHandle(), buffer, GenTL::BUFFER_INFO_BASE);
}

size_t Buffer::getGlobalSize() const
{
  return getBufferValue<size_t>(gentl, parent->getHandle(), buffer, GenTL::BUFFER_INFO_SIZE);
}

void *Buffer::getBase(uint32_t part) const
{
  if (multipart)
  {
    return getBufferPartValue<void *>(gentl, parent->getHandle(), buffer, part,
                                      GenTL::BUFFER_PART_INFO_BASE);
  }
  else
  {
    void *ret=getBufferValue<void *>(gentl, parent->getHandle(), buffer, GenTL::BUFFER_INFO_BASE);

    size_t offset=getBufferValue<size_t>(gentl, parent->getHandle(), buffer,
                                         GenTL::BUFFER_INFO_IMAGEOFFSET);

    if (offset > 0)
    {
      ret=reinterpret_cast<char *>(ret)+offset;
    }

    return ret;
  }
}

size_t Buffer::getSize(uint32_t part) const
{
  if (multipart)
  {
    return getBufferPartValue<size_t>(gentl, parent->getHandle(), buffer, part,
                                      GenTL::BUFFER_PART_INFO_DATA_SIZE);
  }
  else
  {
    size_t size=getBufferValue<size_t>(gentl, parent->getHandle(), buffer,
                                       GenTL::BUFFER_INFO_SIZE);

    size_t offset=getBufferValue<size_t>(gentl, parent->getHandle(), buffer,
                                         GenTL::BUFFER_INFO_IMAGEOFFSET);

    return size-offset;
  }
}

void *Buffer::getUserPtr() const
{
  return getBufferValue<void *>(gentl, parent->getHandle(), buffer, GenTL::BUFFER_INFO_USER_PTR);
}

uint64_t Buffer::getTimestamp() const
{
  if (payload_type == PAYLOAD_TYPE_CHUNK_DATA && nodemap)
  {
    try
    {
      return getInteger(nodemap, "ChunkTimestamp", 0, 0, true);
    }
    catch (const std::exception &)
    {
      // ignore error and try getBufferValue()
    }
  }

  return getBufferValue<uint64_t>(gentl, parent->getHandle(), buffer,
                                  GenTL::BUFFER_INFO_TIMESTAMP);
}

bool Buffer::getNewData() const
{
  return getBufferBool(gentl, parent->getHandle(), buffer, GenTL::BUFFER_INFO_NEW_DATA);
}

bool Buffer::getIsQueued() const
{
  return getBufferBool(gentl, parent->getHandle(), buffer, GenTL::BUFFER_INFO_IS_QUEUED);
}

bool Buffer::getIsAcquiring() const
{
  return getBufferBool(gentl, parent->getHandle(), buffer, GenTL::BUFFER_INFO_IS_ACQUIRING);
}

bool Buffer::getIsIncomplete() const
{
  return getBufferBool(gentl, parent->getHandle(), buffer, GenTL::BUFFER_INFO_IS_INCOMPLETE);
}

std::string Buffer::getTLType() const
{
  return getBufferString(gentl, parent->getHandle(), buffer, GenTL::BUFFER_INFO_TLTYPE);
}

size_t Buffer::getSizeFilled() const
{
  return getBufferValue<size_t>(gentl, parent->getHandle(), buffer,
                                GenTL::BUFFER_INFO_SIZE_FILLED);
}

size_t Buffer::getPartDataType(uint32_t part) const
{
  if (multipart)
  {
    return getBufferPartValue<size_t>(gentl, parent->getHandle(), buffer, part,
                                      GenTL::BUFFER_PART_INFO_DATA_TYPE);
  }
  else
  {
    return 0;
  }
}

size_t Buffer::getWidth(uint32_t part) const
{
  if (multipart)
  {
    return getBufferPartValue<size_t>(gentl, parent->getHandle(), buffer, part,
                                      GenTL::BUFFER_PART_INFO_WIDTH);
  }
  else
  {
    if (payload_type == PAYLOAD_TYPE_CHUNK_DATA && nodemap)
    {
      try
      {
        return getInteger(nodemap, "ChunkWidth", 0, 0, true);
      }
      catch (const std::exception &)
      {
        // ignore error and try getBufferValue()
      }
    }

    return getBufferValue<size_t>(gentl, parent->getHandle(), buffer, GenTL::BUFFER_INFO_WIDTH);
  }
}

size_t Buffer::getHeight(uint32_t part) const
{
  if (multipart)
  {
    return getBufferPartValue<size_t>(gentl, parent->getHandle(), buffer, part,
                                      GenTL::BUFFER_PART_INFO_HEIGHT);
  }
  else
  {
    if (payload_type == PAYLOAD_TYPE_CHUNK_DATA && nodemap)
    {
      try
      {
        return getInteger(nodemap, "ChunkHeight", 0, 0, true);
      }
      catch (const std::exception &)
      {
        // ignore error and try getBufferValue()
      }
    }

    return getBufferValue<size_t>(gentl, parent->getHandle(), buffer, GenTL::BUFFER_INFO_HEIGHT);
  }
}

size_t Buffer::getXOffset(uint32_t part) const
{
  if (multipart)
  {
    return getBufferPartValue<size_t>(gentl, parent->getHandle(), buffer, part,
                                      GenTL::BUFFER_PART_INFO_XOFFSET);
  }
  else
  {
    if (payload_type == PAYLOAD_TYPE_CHUNK_DATA && nodemap)
    {
      try
      {
        return getInteger(nodemap, "ChunkOffsetX", 0, 0, true);
      }
      catch (const std::exception &)
      {
        // ignore error and try getBufferValue()
      }
    }

    return getBufferValue<size_t>(gentl, parent->getHandle(), buffer, GenTL::BUFFER_INFO_XOFFSET);
  }
}

size_t Buffer::getYOffset(uint32_t part) const
{
  if (multipart)
  {
    return getBufferPartValue<size_t>(gentl, parent->getHandle(), buffer, part,
                                      GenTL::BUFFER_PART_INFO_YOFFSET);
  }
  else
  {
    if (payload_type == PAYLOAD_TYPE_CHUNK_DATA && nodemap)
    {
      try
      {
        return getInteger(nodemap, "ChunkOffsetY", 0, 0, true);
      }
      catch (const std::exception &)
      {
        // ignore error and try getBufferValue()
      }
    }

    return getBufferValue<size_t>(gentl, parent->getHandle(), buffer, GenTL::BUFFER_INFO_YOFFSET);
  }
}

size_t Buffer::getXPadding(uint32_t part) const
{
  if (multipart)
  {
    return getBufferPartValue<size_t>(gentl, parent->getHandle(), buffer, part,
                                      GenTL::BUFFER_PART_INFO_XPADDING);
  }
  else
  {
    return getBufferValue<size_t>(gentl, parent->getHandle(), buffer, GenTL::BUFFER_INFO_XPADDING);
  }
}

size_t Buffer::getYPadding() const
{
  if (multipart)
  {
    return 0;
  }

  return getBufferValue<size_t>(gentl, parent->getHandle(), buffer, GenTL::BUFFER_INFO_YPADDING);
}

uint64_t Buffer::getFrameID() const
{
  return getBufferValue<uint64_t>(gentl, parent->getHandle(), buffer, GenTL::BUFFER_INFO_FRAMEID);
}

bool Buffer::getImagePresent(uint32_t part) const
{
  if (multipart)
  {
    size_t type=getBufferPartValue<size_t>(gentl, parent->getHandle(), buffer, part,
                                           GenTL::BUFFER_PART_INFO_DATA_TYPE);

    bool ret;

    switch (type)
    {
      case PART_DATATYPE_2D_IMAGE:
      case PART_DATATYPE_2D_PLANE_BIPLANAR:
      case PART_DATATYPE_2D_PLANE_TRIPLANAR:
      case PART_DATATYPE_2D_PLANE_QUADPLANAR:
      case PART_DATATYPE_3D_IMAGE:
      case PART_DATATYPE_3D_PLANE_BIPLANAR:
      case PART_DATATYPE_3D_PLANE_TRIPLANAR:
      case PART_DATATYPE_3D_PLANE_QUADPLANAR:
      case PART_DATATYPE_CONFIDENCE_MAP:
        ret=true;
        break;

      default:
        ret=false;
        break;
    }

    return ret;
  }
  else
  {
    if (payload_type == PAYLOAD_TYPE_CHUNK_DATA && nodemap)
    {
      return true;
    }

    return getBufferBool(gentl, parent->getHandle(), buffer, GenTL::BUFFER_INFO_IMAGEPRESENT);
  }
}

size_t Buffer::getPayloadType() const
{
  return getBufferValue<size_t>(gentl, parent->getHandle(), buffer,
                                GenTL::BUFFER_INFO_PAYLOADTYPE);
}

uint64_t Buffer::getPixelFormat(uint32_t part) const
{
  if (multipart)
  {
    return getBufferPartValue<uint64_t>(gentl, parent->getHandle(), buffer, part,
                                        GenTL::BUFFER_PART_INFO_DATA_FORMAT);
  }
  else
  {
    if (payload_type == PAYLOAD_TYPE_CHUNK_DATA && nodemap)
    {
      try
      {
        return getInteger(nodemap, "ChunkPixelFormat", 0, 0, true);
      }
      catch (const std::exception &)
      {
        // ignore error and try getBufferValue()
      }
    }

    return getBufferValue<uint64_t>(gentl, parent->getHandle(), buffer,
                                    GenTL::BUFFER_INFO_PIXELFORMAT);
  }
}

uint64_t Buffer::getPixelFormatNamespace(uint32_t part) const
{
  if (multipart)
  {
    return getBufferPartValue<uint64_t>(gentl, parent->getHandle(), buffer, part,
                                        GenTL::BUFFER_PART_INFO_DATA_FORMAT_NAMESPACE);
  }
  else
  {
    return getBufferValue<uint64_t>(gentl, parent->getHandle(), buffer,
                                    GenTL::BUFFER_INFO_PIXELFORMAT_NAMESPACE);
  }
}

uint64_t Buffer::getPartSourceID(uint32_t part) const
{
  if (multipart)
  {
    return getBufferPartValue<uint64_t>(gentl, parent->getHandle(), buffer, part,
                                      GenTL::BUFFER_PART_INFO_SOURCE_ID);
  }
  else
  {
    return 0;
  }
}

uint64_t Buffer::getPartRegionID(uint32_t part) const
{
  if (multipart)
  {
    return getBufferPartValue<uint64_t>(gentl, parent->getHandle(), buffer, part,
                                      GenTL::BUFFER_PART_INFO_REGION_ID);
  }
  else
  {
    return 0;
  }
}

uint64_t Buffer::getPartDataPurposeID(uint32_t part) const
{
  if (multipart)
  {
    return getBufferPartValue<uint64_t>(gentl, parent->getHandle(), buffer, part,
                                      GenTL::BUFFER_PART_INFO_DATA_PURPOSE_ID);
  }
  else
  {
    return 0;
  }
}

size_t Buffer::getDeliveredImageHeight(uint32_t part) const
{
  if (multipart)
  {
    return getBufferPartValue<size_t>(gentl, parent->getHandle(), buffer, part,
                                      GenTL::BUFFER_PART_INFO_DELIVERED_IMAGEHEIGHT);
  }
  else
  {
    if (payload_type == PAYLOAD_TYPE_CHUNK_DATA && nodemap)
    {
      try
      {
        return getInteger(nodemap, "ChunkHeight", 0, 0, true);
      }
      catch (const std::exception &)
      {
        // ignore error and try getBufferValue()
      }
    }

    return getBufferValue<size_t>(gentl, parent->getHandle(), buffer,
                                  GenTL::BUFFER_INFO_DELIVERED_IMAGEHEIGHT);
  }
}

size_t Buffer::getDeliveredChunkPayloadSize() const
{
  return getBufferValue<size_t>(gentl, parent->getHandle(), buffer,
                                GenTL::BUFFER_INFO_DELIVERED_CHUNKPAYLOADSIZE);
}

uint64_t Buffer::getChunkLayoutID() const
{
  return getBufferValue<uint64_t>(gentl, parent->getHandle(), buffer,
                                  GenTL::BUFFER_INFO_CHUNKLAYOUTID);
}

std::string Buffer::getFilename() const
{
  return getBufferString(gentl, parent->getHandle(), buffer, GenTL::BUFFER_INFO_FILENAME);
}

bool Buffer::isBigEndian() const
{
  bool ret=false;

  GenTL::INFO_DATATYPE type;
  int32_t v;
  size_t size=sizeof(v);
  GenTL::GC_ERROR err=GenTL::GC_ERR_SUCCESS;

  if (parent->getHandle() != 0 && buffer != 0)
  {
    err=gentl->DSGetBufferInfo(parent->getHandle(), buffer, GenTL::BUFFER_INFO_PIXEL_ENDIANNESS,
                               &type, &v, &size);

    if (err == GenTL::GC_ERR_SUCCESS && type == GenTL::INFO_DATATYPE_INT32 &&
        v == GenTL::PIXELENDIANNESS_BIG)
    {
      ret=true;
    }
  }

  return ret;
}

size_t Buffer::getDataSize() const
{
  return getBufferValue<size_t>(gentl, parent->getHandle(), buffer, GenTL::BUFFER_INFO_DATA_SIZE);
}

uint64_t Buffer::getTimestampNS() const
{
  uint64_t ret=getBufferValue<uint64_t>(gentl, parent->getHandle(), buffer,
                                        GenTL::BUFFER_INFO_TIMESTAMP_NS);

  // if timestamp in nano seconds is not available, then compute it from
  // timestamp and device frequency

  if (ret == 0)
  {
    const uint64_t ns_freq=1000000000ul;
    uint64_t freq=parent->getParent()->getTimestampFrequency();

    if (freq == 0)
    {
      freq=ns_freq;
    }

    ret=getBufferValue<uint64_t>(gentl, parent->getHandle(), buffer,
                                 GenTL::BUFFER_INFO_TIMESTAMP);

    if (freq != ns_freq)
    {
      ret=ret/freq*ns_freq+(ns_freq*(ret%freq))/freq;
    }
  }

  return ret;
}

bool Buffer::getDataLargerThanBuffer() const
{
  return getBufferBool(gentl, parent->getHandle(), buffer,
                       GenTL::BUFFER_INFO_DATA_LARGER_THAN_BUFFER);
}

bool Buffer::getContainsChunkdata() const
{
  if (payload_type == PAYLOAD_TYPE_CHUNK_DATA)
  {
    return true;
  }

  return getBufferBool(gentl, parent->getHandle(), buffer,
                       GenTL::BUFFER_INFO_CONTAINS_CHUNKDATA);
}

void *Buffer::getHandle() const
{
  return buffer;
}

bool isHostBigEndian()
{
  int p=1;
  char *c=reinterpret_cast<char *>(&p);

  if (c[0] == 1)
  {
    return false;
  }

  return true;
}

}
