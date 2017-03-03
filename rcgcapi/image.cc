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

#include "image.h"

#include "exception.h"

#include <cstring>

namespace rcg
{

Image::Image(const Buffer *buffer)
{
  if (buffer->getImagePresent())
  {
    timestamp=buffer->getTimestampNS();

    width=buffer->getWidth();
    height=buffer->getHeight();
    xoffset=buffer->getXOffset();
    yoffset=buffer->getYOffset();
    xpadding=buffer->getXPadding();
    ypadding=buffer->getYPadding();
    frameid=buffer->getFrameID();
    pixelformat=buffer->getPixelFormat();
    bigendian=buffer->isBigEndian();

    const size_t size=buffer->getSizeFilled();
    const size_t offset=buffer->getImageOffset();

    pixel.reset(new uint8_t [size-offset]);

    memcpy(pixel.get(), reinterpret_cast<uint8_t *>(buffer->getBase())+offset, size-offset);
  }
  else
  {
    throw GenTLException("Image::Image(): Now image available.");
  }
}

}