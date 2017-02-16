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

#ifndef RCGCAPI_EXCEPTION
#define RCGCAPI_EXCEPTION

#include <memory>

namespace rcg
{

class GenTLWrapper;

class GenTLException : public std::exception
{
  private:

    std::string s;

  public:

    GenTLException(const std::string &msg);
    GenTLException(const std::string &msg, const std::shared_ptr<const GenTLWrapper> &gentl);

    virtual ~GenTLException() throw();
    virtual const char *what() const throw();
};

}

#endif