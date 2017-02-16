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

#include "exception.h"

#include "gentl_wrapper.h"

namespace rcg
{

class GenTLWrapper;

GenTLException::GenTLException(const std::string &msg)
{
  s=msg;
}

GenTLException::GenTLException(const std::string &msg,
                               const std::shared_ptr<const GenTLWrapper> &gentl)
{
  GenTL::GC_ERROR err;
  char tmp[1024]="";
  size_t tmp_size=sizeof(tmp);

  gentl->GCGetLastError(&err, tmp, &tmp_size);

  if (msg.size() > 0 && err != GenTL::GC_ERR_SUCCESS)
  {
    s=msg+": "+tmp;
  }
  else if (msg.size() > 0)
  {
    s=msg;
  }
  else
  {
    s=tmp;
  }
}

GenTLException::~GenTLException() throw()
{ }

const char *GenTLException::what() const throw()
{
  return s.c_str();
}

}