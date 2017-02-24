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

#ifndef RCGCAPI_STREAM
#define RCGCAPI_STREAM

#include "device.h"
#include "buffer.h"

namespace rcg
{

class Buffer;

/**
  The stream class encapsulates a Genicam stream.

  NOTE: A GenTLException is thrown in case of a severe error.
*/

class Stream : public std::enable_shared_from_this<Stream>
{
  public:

    /**
      Constructs a stream class. Streams must only be created by the
      device class.
    */

    Stream(const std::shared_ptr<Device> &parent,
           const std::shared_ptr<const GenTLWrapper> &gentl, const char *id);
    ~Stream();

    /**
      Returns the pointer to the parent device object.

      @return Pointer to parent device object.
    */

    std::shared_ptr<Device> getParent() const;

    /**
      Get the internal ID of this stream.

      @return ID.
    */

    const std::string &getID() const;

    /**
      Opens the stream for working with it. The stream may be opened
      multiple times. However, for each open(), the close() method must be
      called as well.
    */

    void open();

    /**
      Closes the stream. Each call of open() must be followed by a call to
      close() at some point in time.
    */

    void close();

    /**
      Allocates buffers and registers internal events if necessary and starts
      streaming.
    */

    void startStreaming();

    /**
      Stops streaming.
    */

    void stopStreaming();

    /**
      Wait for the next image or data and return it in a buffer object. The
      buffer is valid until the next call to grab.

      @param timeout Timeout in ms. A value < 0 sets waiting time to infinite.
      @return        Pointer to received buffer or 0 in case of an error or
                     interrupt.
    */

    const Buffer *grab(int64_t timeout=-1);

    /**
      Returns some information about the stream.

      NOTE: open() must have called before calling this method.

      @return Number of delivered buffers since last acquisition start.
    */

    uint64_t getNumDelivered() const;

    /**
      Returns some information about the stream.

      NOTE: open() must have called before calling this method.

      @return Number of lost buffers due to queue underrun.
    */

    uint64_t getNumUnderrun() const;

    /**
      Returns some information about the stream.

      NOTE: open() must have called before calling this method.

      @return Number of announced buffers.
    */

    size_t getNumAnnounced() const;

    /**
      Returns some information about the stream.

      NOTE: open() must have called before calling this method.

      @return Number of buffers in the input pool.
    */

    size_t getNumQueued() const;

    /**
      Returns some information about the stream.

      NOTE: open() must have called before calling this method.

      @return Number of buffers in the output queue.
    */

    size_t getNumAwaitDelivery() const;

    /**
      Returns some information about the stream.

      NOTE: open() must have called before calling this method.

      @return Number of buffers started in the acquisition engine.
    */

    uint64_t getNumStarted() const;

    /**
      Returns some information about the stream.

      NOTE: open() must have called before calling this method.

      @return Size of the expected data in bytes.
    */

    size_t getPayloadSize() const;

    /**
      Returns some information about the stream.

      NOTE: open() must have called before calling this method.

      @return Flag indicating whether the acquisition engine is started or not.
    */

    bool getIsGrabbing() const;

    /**
      Returns some information about the stream.

      NOTE: open() must have called before calling this method.

      @return Flag that indicated that this data stream defines a payload size
              independent from the remote device.
    */

    bool getDefinesPayloadsize() const;

    /**
      Returns some information about the stream.

      NOTE: open() must have called before calling this method.

      @return Transport layer technology that is supported.
    */

    std::string getTLType() const;

    /**
      Returns some information about the stream.

      NOTE: open() must have called before calling this method.

      @return Max number of chunks in a buffer, if known.
    */

    size_t getNumChunksMax() const;

    /**
      Returns some information about the stream.

      NOTE: open() must have called before calling this method.

      @return Min number of buffers to announce before acq can start, if known.
    */

    size_t getBufAnnounceMin() const;

    /**
      Returns some information about the stream.

      NOTE: open() must have called before calling this method.

      @return Buffer alignment in bytes.
    */

    size_t getBufAlignment() const;

    /**
      Returns the node map of this object.

      NOTE: open() must be called before calling this method. The returned
      pointer remains valid until close() of this object is called.

      @return Node map of this object.
    */

    std::shared_ptr<GenApi::CNodeMapRef> getNodeMap();

    /**
      Get internal stream handle.

      @return Internal handle.
    */

    void *getHandle() const;

  private:

    Stream(class Stream &); // forbidden
    Stream &operator=(const Stream &); // forbidden

    Buffer buffer;

    std::shared_ptr<Device> parent;
    std::shared_ptr<const GenTLWrapper> gentl;
    std::string id;

    int n_open;
    void *stream;
    void *event;
    size_t bn;

    std::shared_ptr<CPort> cport;
    std::shared_ptr<GenApi::CNodeMapRef> nodemap;
};

}

#endif