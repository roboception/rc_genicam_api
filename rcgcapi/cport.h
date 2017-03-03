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

#ifndef RCGCAPI_CPORT
#define RCGCAPI_CPORT

#include "gentl_wrapper.h"

#include <GenApi/GenApi.h>

namespace rcg
{

/**
  This is the port definition that connects GenAPI to GenTL. It is implemented
  such that it works with a pointer to a handle. The methods do nothing if the
  handle is 0.
*/

class CPort : public GenApi::IPort
{
  public:

    CPort(std::shared_ptr<const GenTLWrapper> gentl, void **port);

    void Read(void *buffer, int64_t addr, int64_t length);
    void Write(const void *buffer, int64_t addr, int64_t length);
    GenApi::EAccessMode GetAccessMode() const;

  private:

    std::shared_ptr<const GenTLWrapper> gentl;
    void **port;
};

/**
  Convenience function that returns a GenICam node map from the given port.

  @param gentl Pointer to GenTL Wrapper.
  @param port  Pointer to module or remote port.
  @param cport Pointer to CPort Wrapper.
  @param xml   Path and name for storing the received XML file or 0 if xml file
               should not be stored.
  @return      Allocated node map object or 0 if it cannot be allocated. The
               pointer must be freed by the calling function with delete.
*/

std::shared_ptr<GenApi::CNodeMapRef> allocNodeMap(std::shared_ptr<const GenTLWrapper> gentl,
                                                  void *port, CPort *cport, const char *xml=0);

}

#endif