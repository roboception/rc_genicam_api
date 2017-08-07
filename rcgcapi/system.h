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

#ifndef RCGCAPI_SYSTEM
#define RCGCAPI_SYSTEM

#include <GenApi/GenApi.h>

#include <memory>
#include <vector>

namespace rcg
{

class GenTLWrapper;
class Interface;
class CPort;

/**
  The system class encapsulates a Genicam transport layer.

  NOTE: A GenTLException is thrown in case of a severe error.
*/

class System : public std::enable_shared_from_this<System>
{
  public:

    ~System();

    /**
      Returns a list of systems (i.e. transport layers) that is currently
      available. For discovering available transport layers, the environment
      variable GENICAM_GENTL32_PATH or GENICAM_GENTL64_PATH (depending on the
      compilation with 32 or 64 bit) is used.

      NOTE: This is the only method that can instantiate System objects.

      @return List of all available transport layers.
    */

    static std::vector<std::shared_ptr<System> > getSystems();

    /**
      Clears the internal list of systems. This may be called before exit so
      that all resources are cleaned before leaving the main function.
    */

    static void clearSystems();

    /**
      Get file name from which this system was created.

      @return File name.
    */

    const std::string &getFilename() const;

    /**
      Opens the system for working with it. The system may be opened multiple
      times. However, for each open(), the close() method must be called as
      well.
    */

    void open();

    /**
      Closes the system. Each call of open() must be followed by a call to
      close() at some point in time.
    */

    void close();

    /**
      Returns the currently available interfaces.

      NOTE: open() must be called before calling this method.

      @return List of interfaces.
    */

    std::vector<std::shared_ptr<Interface> > getInterfaces();

    /**
      Returns the ID of the GenTL provider.

      @return ID.
    */

    std::string getID() const;

    /**
      Returns the vendor name of the GenTL provider.

      @return Vendor name.
    */

    std::string getVendor() const;

    /**
      Returns the model of the GenTL provider.

      @return Model.
    */

    std::string getModel() const;

    /**
      Returns the version of the GenTL provider.

      @return Version.
    */

    std::string getVersion() const;

    /**
      Returns the transport layer type of the GenTL provider.

      @return Transport layer type.
    */

    std::string getTLType() const;

    /**
      Returns the path name of the GenTL provider.

      @return Path name.
    */

    std::string getPathname() const;

    /**
      Returns the character encoding.

      @return True for ASCII, false for UTF8.
    */

    bool isCharEncodingASCII() const;

    /**
      Returns the major version number.

      @return Major version number.
    */

    int getMajorVersion() const;

    /**
      Returns the minor version number.

      @return Minor version number.
    */

    int getMinorVersion() const;

    /**
      Returns the node map of this object.

      NOTE: open() must be called before calling this method. The returned
      pointer remains valid until close() of this object is called.

      @return Node map of this object.
    */

    std::shared_ptr<GenApi::CNodeMapRef> getNodeMap();

    /**
      Get internal handle of open transport layer.

      @return Internal handle.
    */

    void *getHandle() const;

  private:

    System(const std::string &_filename);
    System(class System &); // forbidden
    System &operator=(const System &); // forbidden

    std::string filename;
    std::shared_ptr<const GenTLWrapper> gentl;

    int n_open;
    void *tl;

    std::shared_ptr<CPort> cport;
    std::shared_ptr<GenApi::CNodeMapRef> nodemap;

    std::vector<std::weak_ptr<Interface> > ilist;
};

}

#endif