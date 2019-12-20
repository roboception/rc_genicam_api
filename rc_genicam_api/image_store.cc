/*
 * This file is part of the rc_genicam_api package.
 *
 * Copyright (c) 2019 Roboception GmbH
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

#include "image_store.h"
#include "pixel_formats.h"

#include <exception>

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <limits>

namespace rcg
{

std::string ensureNewFileName(std::string name)
{
  // check if given name is already used

  std::ifstream file(name);

  if (file.is_open())
  {
    file.close();

    // split name in prefix and suffix

    std::string suffix;

    size_t i=name.rfind('.');
    if (i != name.npos && name.size()-i <= 4)
    {
      suffix=name.substr(i);
      name=name.substr(0, i);
    }

    // add number for finding name that is nor used

    int n=1;
    while (n < 100)
    {
      std::ostringstream s;
      s << name << "_" << n << suffix;

      file.open(s.str());
      if (!file.is_open())
      {
        name=s.str();
        break;
      }

      file.close();
      n++;
    }
  }

  return name;
}

namespace
{

class IOException : public std::exception
{
  public:

    IOException(const std::string &_msg) { msg=_msg; }
    virtual const char *what() const noexcept { return msg.c_str(); }

  private:

    std::string msg;
};

void storeImagePNM(const std::string &name, const rcg::Image &image, size_t yoffset, size_t height)
{
  size_t width=image.getWidth();
  size_t real_height=image.getHeight();

  if (height == 0) height=real_height;

  yoffset=std::min(yoffset, real_height);
  height=std::min(height, real_height-yoffset);

  const unsigned char *p=static_cast<const unsigned char *>(image.getPixels());

  size_t px=image.getXPadding();

  uint64_t format=image.getPixelFormat();
  switch (format)
  {
    case Mono8: // store 8 bit monochrome image
    case Confidence8:
    case Error8:
      {
        std::ofstream out(ensureNewFileName(name+".pgm"), std::ios::binary);

        out << "P5" << std::endl;
        out << width << " " << height << std::endl;
        out << 255 << "\n";

        std::streambuf *sb=out.rdbuf();

        p+=(width+px)*yoffset;
        for (size_t k=0; k<height && out.good(); k++)
        {
          for (size_t i=0; i<width; i++)
          {
            sb->sputc(static_cast<char>(*p++));
          }

          p+=px;
        }

        out.close();
      }
      break;

    case Mono16:
    case Coord3D_C16: // store 16 bit monochrome image
      {
        std::ofstream out(ensureNewFileName(name+".pgm"), std::ios::binary);

        out << "P5" << std::endl;
        out << width << " " << height << std::endl;
        out << 65535 << "\n";

        std::streambuf *sb=out.rdbuf();

        // copy image data, pgm is always big endian

        p+=(2*width+px)*yoffset;
        if (image.isBigEndian())
        {
          for (size_t k=0; k<height && out.good(); k++)
          {
            for (size_t i=0; i<width; i++)
            {
              sb->sputc(static_cast<char>(*p++));
              sb->sputc(static_cast<char>(*p++));
            }

            p+=px;
          }
        }
        else
        {
          for (size_t k=0; k<height && out.good(); k++)
          {
            for (size_t i=0; i<width; i++)
            {
              sb->sputc(static_cast<char>(p[1]));
              sb->sputc(static_cast<char>(p[0]));
              p+=2;
            }

            p+=px;
          }
        }

        out.close();
      }
      break;

    case YCbCr411_8: // convert and store as color image
      {
        std::ofstream out(ensureNewFileName(name+".ppm"), std::ios::binary);

        out << "P6" << std::endl;
        out << width << " " << height << std::endl;
        out << 255 << "\n";

        std::streambuf *sb=out.rdbuf();

        size_t pstep=(width>>2)*6+px;
        p+=pstep*yoffset;
        for (size_t k=0; k<height && out.good(); k++)
        {
          for (size_t i=0; i<width; i+=4)
          {
            uint8_t rgb[12];
            rcg::convYCbCr411toQuadRGB(rgb, p, static_cast<int>(i));

            for (int j=0; j<12; j++)
            {
              sb->sputc(static_cast<char>(rgb[j]));
            }
          }

          p+=pstep;
        }

        out.close();
      }
      break;

    default:
      throw IOException(std::string("storeImage(): Unknown pixel format: ")+
        GetPixelFormatName(static_cast<PfncFormat>(image.getPixelFormat())));
      break;
  }
}

void storeImageAsDisparityPFM(const std::string &name, const rcg::Image &image, int inv,
  float scale, float offset)
{
  // convert values and store disparity image

  size_t px=image.getXPadding();
  size_t width=image.getWidth();
  size_t height=image.getHeight();
  const unsigned char *p=static_cast<const unsigned char *>(image.getPixels())+
    2*(width+px)*(height+1);

  std::ofstream out(ensureNewFileName(name+".pfm"), std::ios::binary);

  out << "Pf" << std::endl;
  out << width << " " << height << std::endl;
  out << 1 << "\n";

  std::streambuf *sb=out.rdbuf();

  // get 16 bit data, scale and add offset and store as big endian

  bool msbfirst=true;

  {
    int pp=1;
    char *cc=reinterpret_cast<char *>(&pp);
    msbfirst=(cc[0] != 1);
  }

  for (size_t k=0; k<height && out.good(); k++)
  {
    p-=(width+px)<<2;
    for (size_t i=0; i<width; i++)
    {
      int val;
      if (image.isBigEndian())
      {
        val=(static_cast<int>(p[0])<<8)|p[1];
      }
      else
      {
        val=(static_cast<int>(p[1])<<8)|p[0];
      }

      p+=2;

      float d=std::numeric_limits<float>::infinity();
      if (val != inv)
      {
        d=static_cast<float>(val*scale+offset);
      }

      char *c=reinterpret_cast<char *>(&d);

      if (msbfirst)
      {
        sb->sputc(c[0]);
        sb->sputc(c[1]);
        sb->sputc(c[2]);
        sb->sputc(c[3]);
      }
      else
      {
        sb->sputc(c[3]);
        sb->sputc(c[2]);
        sb->sputc(c[1]);
        sb->sputc(c[0]);
      }

      p+=px;
    }
  }

  out.close();
}

}

void storeImage(const std::string &name, ImgFmt fmt, const rcg::Image &image,
  size_t yoffset, size_t height)
{
  if (fmt == PNM)
  {
    storeImagePNM(name, image, yoffset, height);
  }
}

void storeImageAsDisparity(const std::string &name, ImgFmt fmt, const rcg::Image &image,
  int inv, float scale, float offset)
{
  if (fmt == PNM && scale > 0)
  {
    storeImageAsDisparityPFM(name, image, inv, scale, offset);
  }
  else
  {
    storeImage(name, fmt, image, 0, 0);
  }
}

}
