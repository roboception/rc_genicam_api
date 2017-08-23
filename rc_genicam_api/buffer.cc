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

#include "gentl_wrapper.h"
#include "exception.h"

namespace rcg
{

Buffer::Buffer(const std::shared_ptr<const GenTLWrapper> &_gentl, Stream *_parent)
{
  parent=_parent;
  gentl=_gentl;
  buffer=0;
}

void Buffer::setHandle(void *handle)
{
  buffer=handle;
}

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
    gentl->DSGetBufferInfo(stream, buffer, cmd, &type, &ret, &size);
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
    if (gentl->DSGetBufferInfo(stream, buffer, cmd, &type, &ret, &size) == GenTL::GC_ERR_SUCCESS)
    {
      if (type == GenTL::INFO_DATATYPE_STRING)
      {
        ret=tmp;
      }
    }
  }

  return ret;
}

}

void *Buffer::getBase() const
{
  return getBufferValue<void *>(gentl, parent->getHandle(), buffer, GenTL::BUFFER_INFO_BASE);
}

size_t Buffer::getSize() const
{
  return getBufferValue<size_t>(gentl, parent->getHandle(), buffer, GenTL::BUFFER_INFO_SIZE);
}

void *Buffer::getUserPtr() const
{
  return getBufferValue<void *>(gentl, parent->getHandle(), buffer, GenTL::BUFFER_INFO_USER_PTR);
}

uint64_t Buffer::getTimestamp() const
{
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

size_t Buffer::getWidth() const
{
  return getBufferValue<size_t>(gentl, parent->getHandle(), buffer, GenTL::BUFFER_INFO_WIDTH);
}

size_t Buffer::getHeight() const
{
  return getBufferValue<size_t>(gentl, parent->getHandle(), buffer, GenTL::BUFFER_INFO_HEIGHT);
}

size_t Buffer::getXOffset() const
{
  return getBufferValue<size_t>(gentl, parent->getHandle(), buffer, GenTL::BUFFER_INFO_XOFFSET);
}

size_t Buffer::getYOffset() const
{
  return getBufferValue<size_t>(gentl, parent->getHandle(), buffer, GenTL::BUFFER_INFO_YOFFSET);
}

size_t Buffer::getXPadding() const
{
  return getBufferValue<size_t>(gentl, parent->getHandle(), buffer, GenTL::BUFFER_INFO_XPADDING);
}

size_t Buffer::getYPadding() const
{
  return getBufferValue<size_t>(gentl, parent->getHandle(), buffer, GenTL::BUFFER_INFO_YPADDING);
}

uint64_t Buffer::getFrameID() const
{
  return getBufferValue<uint64_t>(gentl, parent->getHandle(), buffer, GenTL::BUFFER_INFO_FRAMEID);
}

bool Buffer::getImagePresent() const
{
  return getBufferBool(gentl, parent->getHandle(), buffer, GenTL::BUFFER_INFO_IMAGEPRESENT);
}

size_t Buffer::getImageOffset() const
{
  return getBufferValue<size_t>(gentl, parent->getHandle(), buffer,
                                GenTL::BUFFER_INFO_IMAGEOFFSET);
}

size_t Buffer::getPayloadType() const
{
  return getBufferValue<size_t>(gentl, parent->getHandle(), buffer,
                                GenTL::BUFFER_INFO_PAYLOADTYPE);
}

uint64_t Buffer::getPixelFormat() const
{
  return getBufferValue<uint64_t>(gentl, parent->getHandle(), buffer,
                                  GenTL::BUFFER_INFO_PIXELFORMAT);
}

uint64_t Buffer::getPixelFormatNamespace() const
{
  return getBufferValue<uint64_t>(gentl, parent->getHandle(), buffer,
                                  GenTL::BUFFER_INFO_PIXELFORMAT_NAMESPACE);
}

size_t Buffer::getDeliveredImageHeight() const
{
  return getBufferValue<size_t>(gentl, parent->getHandle(), buffer,
                                GenTL::BUFFER_INFO_DELIVERED_IMAGEHEIGHT);
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