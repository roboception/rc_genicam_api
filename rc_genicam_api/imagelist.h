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

#ifndef RC_GENICAM_API_IMAGELIST
#define RC_GENICAM_API_IMAGELIST

#include "image.h"

#include <memory>
#include <vector>

namespace rcg
{

/**
  An object of this class manages a limited number of images. It is intended as
  a helper class for time synchronization of different images that can be
  associated by timestamp.
*/

class ImageList
{
  public:

    /**
      Create an image list.

      @param maxsize Maximum number of elements that the list can hold. The
                     default is 25, which is 1s at 25 Hz.
    */

    ImageList(size_t maxsize=25);

    /**
      Adds the given image to the internal list. If the maximum number of
      elements is exceeded, then the oldes image will be dropped.

      @param image Image to be added.
    */

    void add(const std::shared_ptr<const Image> &image);

    /**
      Creates an image from the given buffer and adds it to the internal list.
      If the maximum number of elements is exceeded, then the oldes image will
      be dropped.

      @param buffer Buffer from which an image will be created.
    */

    void add(const Buffer *buffer);

    /**
      Removes all images that have a timestamp that is older or equal than the
      given timestamp.

      @param timestamp Timestamp.
    */

    void removeOld(uint64_t timestamp);

    /**
      Returns the image that has the given timestamp. If the image cannot be
      found, then a nullptr is returned.

      @param timestamp Timestamp.
      @return Pointer to image or 0.
    */

    std::shared_ptr<const Image> find(uint64_t timestamp);

  private:

    size_t maxsize;
    std::vector<std::shared_ptr<const Image> > list;
};

}

#endif