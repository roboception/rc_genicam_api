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

#include "imagelist.h"

namespace rcg
{

ImageList::ImageList(size_t _maxsize)
{
  maxsize=std::max(static_cast<size_t>(1), _maxsize);
}

void ImageList::add(const std::shared_ptr<const Image> &image)
{
  list.push_back(image);

  while (list.size() > maxsize)
  {
    list.erase(list.begin());
  }
}

void ImageList::add(const Buffer *buffer)
{
  list.push_back(std::shared_ptr<const Image>(new Image(buffer)));

  while (list.size() > maxsize)
  {
    list.erase(list.begin());
  }
}

void ImageList::removeOld(uint64_t timestamp)
{
  size_t i=0;

  while (i < list.size())
  {
    if (list[i]->getTimestampNS() <= timestamp)
    {
      list.erase(list.begin()+i);
    }
    else
    {
      i++;
    }
  }
}

std::shared_ptr<const Image> ImageList::find(uint64_t timestamp)
{
  for (size_t i=0; i<list.size(); i++)
  {
    if (list[i]->getTimestampNS() == timestamp)
    {
      return list[i];
    }
  }

  return std::shared_ptr<const Image>();
}

}