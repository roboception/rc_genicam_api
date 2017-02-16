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

#ifndef RCGCAPI_INTERFACE
#define RCGCAPI_INTERFACE

#include "system.h"

namespace rcg
{

class Device;

/**
  The interface class encapsulates a Genicam interface.

  NOTE: A GenTLException is thrown in case of a severe error.
*/

class Interface : public std::enable_shared_from_this<Interface>
{
  public:

    /**
      Constructs an interface class. Interfaces must only be created by the
      system class.
    */

    Interface(const std::shared_ptr<System> &parent,
              const std::shared_ptr<const GenTLWrapper> &gentl, const char *id);
    ~Interface();

    /**
      Returns the pointer to the parent system object.

      @return Pointer to parent system object.
    */

    std::shared_ptr<System> getParent() const;

    /**
      Get the internal ID of this interface.

      @return ID.
    */

    const std::string &getID() const;

    /**
      Opens the interface for working with it. The interface may be opened
      multiple times. However, for each open(), the close() method must be
      called as well.
    */

    void open();

    /**
      Closes the interface. Each call of open() must be followed by a call to
      close() at some point in time.
    */

    void close();

    /**
      Returns the currently available devices on this interface.

      NOTE: open() must be called before calling this method.

      @return List of devices.
    */

    std::vector<std::shared_ptr<Device> > getDevices();

    /**
      Returns a device with the given device id.

      NOTE: open() must be called before calling this method.

      @return Pointer to device or std::nullptr.
    */

    std::shared_ptr<Device> getDevice(const char *devid);

    /**
      Returns the display name of the interface.

      NOTE: At least the parent object must have been opened before calling
      this method.

      @return Display name.
    */

    std::string getDisplayName() const;

    /**
      Returns the transport layer type of the interface.

      NOTE: At least the parent object must have been opened before calling
      this method.

      @return Transport layer type.
    */

    std::string getTLType() const;

    /**
      Returns the node map of this object.

      NOTE: open() must be called before calling this method. The returned
      pointer remains valid until close() of this object is called.

      @return Node map of this object.
    */

    std::shared_ptr<GenApi::CNodeMapRef> getNodeMap();

    /**
      Get internal interace handle.

      @return Internal handle.
    */

    void *getHandle() const;

  private:

    Interface(class Interface &); // forbidden
    Interface &operator=(const Interface &); // forbidden

    std::shared_ptr<System> parent;
    std::shared_ptr<const GenTLWrapper> gentl;
    std::string id;

    int n_open;
    void *ifh;

    std::shared_ptr<CPort> cport;
    std::shared_ptr<GenApi::CNodeMapRef> nodemap;

    std::vector<std::weak_ptr<Device> > dlist;
};

}

#endif