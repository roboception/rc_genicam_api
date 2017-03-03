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