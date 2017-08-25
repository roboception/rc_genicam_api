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

#ifndef RC_GENICAM_API_BUFFER
#define RC_GENICAM_API_BUFFER

#include <memory>

namespace rcg
{

class Stream;
class GenTLWrapper;

/**
  Payload types as taken from GenTL 1.5 definition.
  See Buffer::getPayloadType().
*/

enum PAYLOADTYPE_INFO_IDS
{
  PAYLOAD_TYPE_UNKNOWN         =  0,   /* GenTL v1.2 */
  PAYLOAD_TYPE_IMAGE           =  1,   /* GenTL v1.2 */
  PAYLOAD_TYPE_RAW_DATA        =  2,   /* GenTL v1.2 */
  PAYLOAD_TYPE_FILE            =  3,   /* GenTL v1.2 */
  PAYLOAD_TYPE_CHUNK_DATA      =  4,   /* GenTL v1.2, Deprecated in GenTL 1.5*/
  PAYLOAD_TYPE_JPEG            =  5,   /* GenTL v1.4 */
  PAYLOAD_TYPE_JPEG2000        =  6,   /* GenTL v1.4 */
  PAYLOAD_TYPE_H264            =  7,   /* GenTL v1.4 */
  PAYLOAD_TYPE_CHUNK_ONLY      =  8,   /* GenTL v1.4 */
  PAYLOAD_TYPE_DEVICE_SPECIFIC =  9,   /* GenTL v1.4 */
  PAYLOAD_TYPE_MULTI_PART      =  10,  /* GenTL v1.5 */

  PAYLOAD_TYPE_CUSTOM_ID       = 1000  /* Starting value for GenTL Producer custom IDs. */
};

/**
  Pixelformat namespace IDs as taken from GenTL 1.5 definition.
  See Buffer::getPixelFormatNamespace().
*/

enum PIXELFORMAT_NAMESPACE_IDS
{
  PIXELFORMAT_NAMESPACE_UNKNOWN      = 0,   /* GenTL v1.2 */
  PIXELFORMAT_NAMESPACE_GEV          = 1,   /* GenTL v1.2 */
  PIXELFORMAT_NAMESPACE_IIDC         = 2,   /* GenTL v1.2 */
  PIXELFORMAT_NAMESPACE_PFNC_16BIT   = 3,   /* GenTL v1.4 */
  PIXELFORMAT_NAMESPACE_PFNC_32BIT   = 4,   /* GenTL v1.4 */

  PIXELFORMAT_NAMESPACE_CUSTOM_ID    = 1000 /* Starting value for GenTL Producer custom IDs. */
};

/**
  The buffer class encapsulates a Genicam buffer that is provided by a stream.

  NOTE: A GenTLException is thrown in case of a severe error.
*/

class Buffer
{
  public:

    /**
      Constructs a buffer class as wrapper around a buffer handle. Buffers must
      only be constructed by the stream class.
    */

    Buffer(const std::shared_ptr<const GenTLWrapper> &gentl, Stream *parent);

    /**
      Set the buffer handle that this object should manage. The handle is used
      until a new handle is set.
    */

    void setHandle(void *handle);

    /**
      Returns the pointer to the parent stream object.

      @return Pointer to parent stream object.
    */

    std::shared_ptr<Stream> getParent() const;

    /**
      Returns some information about the buffer.

      @return Base address of the buffer memory.
    */

    void *getBase() const;

    /**
      Returns some information about the buffer.

      @return Size of the buffer in bytes.
    */

    size_t getSize() const;

    /**
      Returns some information about the buffer.

      @return Private data pointer of the GenTL Consumer.
    */

    void *getUserPtr() const;

    /**
      Returns some information about the buffer.

      @return Timestamp the buffer was acquired.
    */

    uint64_t getTimestamp() const;

    /**
      Returns some information about the buffer.

      @return Flag to indicate that the buffer contains new data since the last
              call.
    */

    bool getNewData() const;

    /**
      Returns some information about the buffer.

      @return Flag to indicate if the buffer is in the input pool or output
              queue.
    */

    bool getIsQueued() const;

    /**
      Returns some information about the buffer.

      @return Flag to indicate that the buffer is currently being filled with
              data.
    */

    bool getIsAcquiring() const;

    /**
      Returns some information about the buffer.

      @return Flag to indicate that a buffer was filled but an error occurred
              during that process.
    */

    bool getIsIncomplete() const;

    /**
      Returns some information about the buffer.

      @return Flag to indicate that a buffer was filled but an error occurred
              during that process.
    */

    std::string getTLType() const;

    /**
      Returns some information about the buffer.

      @return Number of bytes written into the buffer last time it has been
              filled. This value is reset to 0 when the buffer is placed into
              the Input Buffer Pool.
    */

    size_t getSizeFilled() const;

    size_t getWidth() const;
    size_t getHeight() const;
    size_t getXOffset() const;
    size_t getYOffset() const;
    size_t getXPadding() const;
    size_t getYPadding() const;
    uint64_t getFrameID() const;
    bool getImagePresent() const;
    size_t getImageOffset() const;
    size_t getPayloadType() const; // see PAYLOADTYPE_INFO_IDS above
    uint64_t getPixelFormat() const; // see pixel_formats.h if namespace is PIXELFORMAT_NAMESPACE_PFNC_32BIT
    uint64_t getPixelFormatNamespace() const; // see PIXELFORMAT_NAMESPACE_IDS above
    size_t getDeliveredImageHeight() const;
    size_t getDeliveredChunkPayloadSize() const;
    uint64_t getChunkLayoutID() const;
    std::string getFilename() const;
    bool isBigEndian() const;
    size_t getDataSize() const;
    uint64_t getTimestampNS() const;
    bool getDataLargerThanBuffer() const;
    bool getContainsChunkdata() const;

    /**
      Get internal stream handle.

      @return Internal handle.
    */

    void *getHandle() const;

  private:

    Buffer(class Buffer &); // forbidden
    Buffer &operator=(const Buffer &); // forbidden

    Stream *parent;
    std::shared_ptr<const GenTLWrapper> gentl;
    void *buffer;
};

bool isHostBigEndian();

}

#endif