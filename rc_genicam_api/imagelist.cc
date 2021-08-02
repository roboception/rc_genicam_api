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

#include <algorithm>

#ifdef _WIN32
#undef min
#undef max
#endif

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

void ImageList::add(const Buffer *buffer, uint32_t part)
{
  list.push_back(std::shared_ptr<const Image>(new Image(buffer, part)));

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
      list.erase(list.begin()+static_cast<int>(i));
    }
    else
    {
      i++;
    }
  }
}

uint64_t ImageList::getOldestTime() const
{
  uint64_t ret=0;

  if (list.size() > 0)
  {
    ret=list[0]->getTimestampNS();
  }

  return ret;
}

std::shared_ptr<const Image> ImageList::find(uint64_t timestamp) const
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

namespace
{

inline uint64_t absDiff(uint64_t a, uint64_t b)
{
  if (a < b)
  {
    return b-a;
  }

  return a-b;
}

}

std::shared_ptr<const Image> ImageList::find(uint64_t timestamp, uint64_t tolerance) const
{
  if (list.size() > 0)
  {
    if (tolerance > 0)
    {
      size_t min_i=0;
      uint64_t min_ad=absDiff(list[0]->getTimestampNS(), timestamp);

      for (size_t i=1; i<list.size(); i++)
      {
        uint64_t ad=absDiff(list[i]->getTimestampNS(), timestamp);

        if (ad < min_ad)
        {
          min_i=i;
          min_ad=ad;
        }
      }

      if (min_ad < tolerance)
      {
        return list[min_i];
      }
    }
    else
    {
      return find(timestamp);
    }
  }

  return std::shared_ptr<const Image>();
}

}
