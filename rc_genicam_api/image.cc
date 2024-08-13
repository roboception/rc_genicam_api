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

#include "image.h"

#include "exception.h"
#include "pixel_formats.h"

#include <cstring>

#ifdef _WIN32
#undef min
#undef max
#endif

namespace rcg
{

Image::Image(const Buffer *buffer, uint32_t part)
{
  if (buffer->getImagePresent(part))
  {
    timestamp=buffer->getTimestampNS();

    width=buffer->getWidth(part);
    height=buffer->getHeight(part);
    xoffset=buffer->getXOffset(part);
    yoffset=buffer->getYOffset(part);
    xpadding=buffer->getXPadding(part);
    ypadding=buffer->getYPadding();
    frameid=buffer->getFrameID();
    pixelformat=buffer->getPixelFormat(part);
    bigendian=buffer->isBigEndian();

    const size_t size=std::min(buffer->getSize(part), buffer->getSizeFilled());

    if (buffer->getSizeFilled() == 0)
    {
      throw GenTLException("Image without data");
    }

    pixel.reset(new uint8_t [size]);

    memcpy(pixel.get(), reinterpret_cast<uint8_t *>(buffer->getBase(part)), size);
  }
  else
  {
    throw GenTLException("Image::Image(): Now image available.");
  }
}

namespace
{

/**
  Clamp the given value to the range of 0 to 255 and cast to byte.
*/

inline unsigned char clamp8(int v)
{
  const int v2=v<0 ? 0:v;
  return static_cast<unsigned char>(v2>255 ? 255:v2);
}

}

void convYCbCr411toRGB(uint8_t rgb[3], const uint8_t *row, int i)
{
  const uint32_t j=static_cast<uint32_t>((i>>2)*6);
  const uint32_t js=static_cast<uint32_t>(i&0x3);

  int Y=row[j+js];
  if (js > 1)
  {
    Y=row[j+js+1];
  }

  const int Cb=static_cast<int>(row[j+2])-128;
  const int Cr=static_cast<int>(row[j+5])-128;

  // conversion of YCbCr into RGB with correct rounding
  const int rc=((90*Cr+16384+32)>>6)-256;
  const int gc=((-22*Cb-46*Cr+16384+32)>>6)-256;
  const int bc=((113*Cb+16384+32)>>6)-256;

  rgb[0]=clamp8(Y+rc);
  rgb[1]=clamp8(Y+gc);
  rgb[2]=clamp8(Y+bc);
}

void convYCbCr411toQuadRGB(uint8_t rgb[12], const uint8_t *row, int i)
{
  i=(i>>2)*6;

  const int Y[4]={row[i], row[i+1], row[i+3], row[i+4]};
  const int Cb=static_cast<int>(row[i+2])-128;
  const int Cr=static_cast<int>(row[i+5])-128;

  // conversion of YCbCr into RGB with correct rounding
  const int rc=((90*Cr+16384+32)>>6)-256;
  const int gc=((-22*Cb-46*Cr+16384+32)>>6)-256;
  const int bc=((113*Cb+16384+32)>>6)-256;

  for (int j=0; j<4; j++)
  {
    *rgb++=clamp8(Y[j]+rc);
    *rgb++=clamp8(Y[j]+gc);
    *rgb++=clamp8(Y[j]+bc);
  }
}

void convYCbCr422toRGB(uint8_t rgb[3], const uint8_t *row, int i)
{
  const uint32_t j=static_cast<uint32_t>((i>>1)*4);
  const uint32_t js=static_cast<uint32_t>(i&0x1)*2;

  const int Y=row[j+js];
  const int Cb=static_cast<int>(row[j+1])-128;
  const int Cr=static_cast<int>(row[j+3])-128;

  // conversion of YCbCr into RGB with correct rounding
  const int rc=((90*Cr+16384+32)>>6)-256;
  const int gc=((-22*Cb-46*Cr+16384+32)>>6)-256;
  const int bc=((113*Cb+16384+32)>>6)-256;

  rgb[0]=clamp8(Y+rc);
  rgb[1]=clamp8(Y+gc);
  rgb[2]=clamp8(Y+bc);
}

void convYCbCr422toQuadRGB(uint8_t rgb[12], const uint8_t *row, int i)
{
  i=(i>>2)*8;

  for (int k=0; k<8; k+=4)
  {
    const int Y[2]={row[i+k], row[i+2+k]};
    const int Cb=static_cast<int>(row[i+1+k])-128;
    const int Cr=static_cast<int>(row[i+3+k])-128;

    // conversion of YCbCr into RGB with correct rounding
    const int rc=((90*Cr+16384+32)>>6)-256;
    const int gc=((-22*Cb-46*Cr+16384+32)>>6)-256;
    const int bc=((113*Cb+16384+32)>>6)-256;

    for (int j=0; j<2; j++)
    {
      *rgb++=clamp8(Y[j]+rc);
      *rgb++=clamp8(Y[j]+gc);
      *rgb++=clamp8(Y[j]+bc);
    }
  }
}

void getColor(uint8_t rgb[3], const std::shared_ptr<const Image> &img,
              uint32_t ds, uint32_t i, uint32_t k)
{
  if (ds < 1)
    ds = 1;

  i*=ds;
  k*=ds;

  if (img->getPixelFormat() == Mono8) // convert from monochrome
  {
    size_t lstep=img->getWidth()+img->getXPadding();
    const uint8_t *p=img->getPixels()+k*lstep+i;

    uint32_t g=0, n=0;

    for (uint32_t kk=0; kk<ds; kk++)
    {
      for (uint32_t ii=0; ii<ds; ii++)
      {
        g+=p[ii];
        n++;
      }

      p+=lstep;
    }

    rgb[2]=rgb[1]=rgb[0]=static_cast<uint8_t>(g/n);
  }
  else if (img->getPixelFormat() == RGB8) // convert from RGB8
  {
    size_t lstep=3*img->getWidth()+img->getXPadding();
    const uint8_t *p=img->getPixels()+k*lstep+3*i;

    uint32_t r=0;
    uint32_t g=0;
    uint32_t b=0;
    uint32_t n=0;

    for (uint32_t kk=0; kk<ds; kk++)
    {
      const uint8_t *pp=p;
      for (uint32_t ii=0; ii<ds; ii++)
      {
        r+=*pp++;
        g+=*pp++;
        b+=*pp++;
        n++;
      }

      p+=lstep;
    }

    rgb[0]=static_cast<uint8_t>(r/n);
    rgb[1]=static_cast<uint8_t>(g/n);
    rgb[2]=static_cast<uint8_t>(b/n);
  }
  else if (img->getPixelFormat() == YCbCr411_8) // convert from YUV411
  {
    size_t lstep=(img->getWidth()>>2)*6+img->getXPadding();
    const uint8_t *p=img->getPixels()+k*lstep;

    uint32_t r=0;
    uint32_t g=0;
    uint32_t b=0;
    uint32_t n=0;

    for (uint32_t kk=0; kk<ds; kk++)
    {
      for (uint32_t ii=0; ii<ds; ii++)
      {
        uint8_t v[3];
        convYCbCr411toRGB(v, p, static_cast<int>(i+ii));

        r+=v[0];
        g+=v[1];
        b+=v[2];
        n++;
      }

      p+=lstep;
    }

    rgb[0]=static_cast<uint8_t>(r/n);
    rgb[1]=static_cast<uint8_t>(g/n);
    rgb[2]=static_cast<uint8_t>(b/n);
  }
  else if (img->getPixelFormat() == YCbCr422_8 || img->getPixelFormat() == YUV422_8) // convert from YUV422
  {
    size_t lstep=(img->getWidth()>>2)*8+img->getXPadding();
    const uint8_t *p=img->getPixels()+k*lstep;

    uint32_t r=0;
    uint32_t g=0;
    uint32_t b=0;
    uint32_t n=0;

    for (uint32_t kk=0; kk<ds; kk++)
    {
      for (uint32_t ii=0; ii<ds; ii++)
      {
        uint8_t v[3];
        convYCbCr422toRGB(v, p, static_cast<int>(i+ii));

        r+=v[0];
        g+=v[1];
        b+=v[2];
        n++;
      }

      p+=lstep;
    }

    rgb[0]=static_cast<uint8_t>(r/n);
    rgb[1]=static_cast<uint8_t>(g/n);
    rgb[2]=static_cast<uint8_t>(b/n);
  }
}

namespace
{

/*
  Convert at green pixel in green-red row.
*/

inline void convertGreenGR(uint8_t &r, uint8_t &g, uint8_t &b,
  const uint8_t *&row0, const uint8_t *&row1, const uint8_t *&row2)
{
  r=static_cast<uint8_t>((static_cast<int>(*row1)+row1[2]+1)>>1);
  g=row1[1];
  b=static_cast<uint8_t>((static_cast<int>(row0[1])+row2[1]+1)>>1);

  row0++; row1++; row2++;
}

/*
  Convert at green pixel in green-blue row.
*/

inline void convertGreenGB(uint8_t &r, uint8_t &g, uint8_t &b,
  const uint8_t *&row0, const uint8_t *&row1, const uint8_t *&row2)
{
  r=static_cast<uint8_t>((static_cast<int>(row0[1])+row2[1]+1)>>1);
  g=row1[1];
  b=static_cast<uint8_t>((static_cast<int>(*row1)+row1[2]+1)>>1);

  row0++; row1++; row2++;
}

/*
  Convert at red pixel.
*/

inline void convertRed(uint8_t &r, uint8_t &g, uint8_t &b,
  const uint8_t *&row0, const uint8_t *&row1, const uint8_t *&row2)
{
  r=row1[1];
  g=static_cast<uint8_t>((static_cast<int>(row0[1])+row2[1]+*row1+row1[2]+2)>>2);
  b=static_cast<uint8_t>((static_cast<int>(*row0)+row0[2]+*row2+row2[2]+2)>>2);

  row0++; row1++; row2++;
}

/*
  Convert at blue pixel.
*/

inline void convertBlue(uint8_t &r, uint8_t &g, uint8_t &b,
  const uint8_t *&row0, const uint8_t *&row1, const uint8_t *&row2)
{
  r=static_cast<uint8_t>((static_cast<int>(*row0)+row0[2]+*row2+row2[2]+2)>>2);
  g=static_cast<uint8_t>((static_cast<int>(row0[1])+row2[1]+*row1+row1[2]+2)>>2);
  b=row1[1];

  row0++; row1++; row2++;
}

/*
  Convert RGB to monochrome.
*/

inline uint8_t rgb2Grey(uint8_t r, uint8_t g, uint8_t b)
{
  return static_cast<uint8_t>((9798*static_cast<uint32_t>(r)+
                               19234*static_cast<uint32_t>(g)+
                               3736*static_cast<uint32_t>(b)+16384)>>15);
}

inline void storeRGBMono(uint8_t *&rgb_out, uint8_t *&mono_out, uint8_t red, uint8_t green,
  uint8_t blue)
{
  if (rgb_out)
  {
    *rgb_out++ = red;
    *rgb_out++ = green;
    *rgb_out++ = blue;
  }

  if (mono_out)
  {
    *mono_out++ = rgb2Grey(red, green, blue);
  }
}

/*
  Convert green-red image row.
*/

void convertBayerGR(uint8_t *rgb_out, uint8_t *mono_out,
  const uint8_t *row0, const uint8_t *row1, const uint8_t *row2,
  bool greenfirst, size_t width)
{
  uint8_t red, green, blue;

  // convert if first pixel is green

  size_t i=0;

  if (greenfirst)
  {
    convertGreenGR(red, green, blue, row0, row1, row2);
    storeRGBMono(rgb_out, mono_out, red, green, blue);

    i++;
  }

  while (i+1 < width)
  {
    // convert at red pixel

    convertRed(red, green, blue, row0, row1, row2);
    storeRGBMono(rgb_out, mono_out, red, green, blue);

    // convert at green pixel

    convertGreenGR(red, green, blue, row0, row1, row2);
    storeRGBMono(rgb_out, mono_out, red, green, blue);

    i+=2;
  }

  // convert at red pixel

  if (i < width)
  {
    convertRed(red, green, blue, row0, row1, row2);
    storeRGBMono(rgb_out, mono_out, red, green, blue);
  }
}

/*
  Convert green-blue image row.
*/

void convertBayerGB(uint8_t *rgb_out, uint8_t *mono_out,
  const uint8_t *row0, const uint8_t *row1, const uint8_t *row2,
  bool greenfirst, size_t width)
{
  uint8_t red, green, blue;

  // convert if first pixel is green

  size_t i=0;

  if (greenfirst)
  {
    convertGreenGB(red, green, blue, row0, row1, row2);
    storeRGBMono(rgb_out, mono_out, red, green, blue);

    i++;
  }

  while (i+1 < width)
  {
    // convert at red pixel

    convertBlue(red, green, blue, row0, row1, row2);
    storeRGBMono(rgb_out, mono_out, red, green, blue);

    // convert at green pixel

    convertGreenGB(red, green, blue, row0, row1, row2);
    storeRGBMono(rgb_out, mono_out, red, green, blue);

    i+=2;
  }

  // convert at red pixel

  if (i < width)
  {
    convertBlue(red, green, blue, row0, row1, row2);
    storeRGBMono(rgb_out, mono_out, red, green, blue);
  }
}

}

bool convertImage(uint8_t *rgb_out, uint8_t *mono_out, const uint8_t *raw, uint64_t pixelformat,
  size_t width, size_t height, size_t xpadding)
{
  bool ret=true;

  switch (pixelformat)
  {
    case Mono8:
    case Confidence8:
    case Error8:
      {
        for (size_t k=0; k<height; k++)
        {
          if (rgb_out)
          {
            for (size_t i=0; i<width; i++)
            {
              uint8_t v=raw[i];
              *rgb_out++ = v;
              *rgb_out++ = v;
              *rgb_out++ = v;
            }
          }

          if (mono_out)
          {
            std::memcpy(mono_out, raw, width*sizeof(uint8_t));
            mono_out+=width;
          }

          raw+=width+xpadding;
        }
      }
      break;

    case YCbCr411_8:
      {
        size_t pstep=(width>>2)*6+xpadding;
        for (size_t k=0; k<height; k++)
        {
          for (size_t i=0; i<width; i+=4)
          {
            if (rgb_out)
            {
              uint8_t rgb[12];
              convYCbCr411toQuadRGB(rgb, raw, static_cast<int>(i));

              for (int j=0; j<12; j++)
              {
                *rgb_out++ = rgb[j];
              }
            }

            if (mono_out)
            {
              size_t j=(i>>2)*6;
              *mono_out++ = raw[j];
              *mono_out++ = raw[j+1];
              *mono_out++ = raw[j+3];
              *mono_out++ = raw[j+4];
            }
          }

          raw+=pstep;
        }
      }
      break;

    case YCbCr422_8:
    case YUV422_8:
      {
        size_t pstep=(width>>2)*8+xpadding;
        for (size_t k=0; k<height; k++)
        {
          for (size_t i=0; i<width; i+=4)
          {
            if (rgb_out)
            {
              uint8_t rgb[12];
              convYCbCr422toQuadRGB(rgb, raw, static_cast<int>(i));

              for (int j=0; j<12; j++)
              {
                *rgb_out++ = rgb[j];
              }
            }

            if (mono_out)
            {
              size_t j=(i>>2)*8;
              *mono_out++ = raw[j];
              *mono_out++ = raw[j+2];
              *mono_out++ = raw[j+4];
              *mono_out++ = raw[j+6];
            }
          }

          raw+=pstep;
        }
      }
      break;

    case RGB8:
      {
        for (size_t k=0; k<height; k++)
        {
          if (rgb_out)
          {
            std::memcpy(rgb_out, raw, 3*width*sizeof(uint8_t));
            rgb_out+=3*width;
          }

          if (mono_out)
          {
            size_t j=0;
            for (size_t i=0; i<width; i++)
            {
              *mono_out++ = rgb2Grey(raw[j], raw[j+1], raw[j+2]);
              j+=3;
            }
          }

          raw+=3*width+xpadding;
        }
      }
      break;

    case BayerRG8:
    case BayerBG8:
    case BayerGR8:
    case BayerGB8:
      {
        // In every row, every second pixel is green and every other pixel is
        // either red or blue. This flag specifies if the current row is red or
        // blue.

        bool greenfirst=(pixelformat == BayerGR8 || pixelformat == BayerGB8);
        bool red=(pixelformat == BayerRG8 || pixelformat == BayerGR8);

        // setup temporary buffer that is 1 pixel larger than the image

        std::unique_ptr<uint8_t []> buffer(new uint8_t [(width+2)*3]);
        uint8_t *row[3];

        row[0]=buffer.get();
        row[1]=row[0]+width+2;
        row[2]=row[1]+width+2;

        // initialize buffer with 2 rows, extended by two pixel to avoid a special
        // treatment for the image border

        memcpy(row[1]+1, raw+width+xpadding, width*sizeof(uint8_t));
        memcpy(row[2]+1, raw, width*sizeof(uint8_t));

        row[1][0]=row[1][2]; row[1][width+1]=row[1][width-1];
        row[2][0]=row[2][2]; row[2][width+1]=row[2][width-1];

        // for all rows

        for (size_t k=0; k<height; k++)
        {
          // store next extended row in buffer

          if (k+1 < height)
          {
            uint8_t *p=row[0];
            row[0]=row[1];
            row[1]=row[2];
            row[2]=p;

            memcpy(row[2]+1, raw+(k+1)*(width+xpadding), width*sizeof(uint8_t));

            row[2][0]=row[2][2]; row[2][width+1]=row[2][width-1];
          }

          if (red)
          {
            convertBayerGR(rgb_out, mono_out, row[0], row[1], row[2], greenfirst, width);

            if (rgb_out) rgb_out+=3*width;
            if (mono_out) mono_out+=width;
          }
          else
          {
            convertBayerGB(rgb_out, mono_out, row[0], row[1], row[2], greenfirst, width);

            if (rgb_out) rgb_out+=3*width;
            if (mono_out) mono_out+=width;
          }

          greenfirst=!greenfirst;
          red=!red;
        }
      }
      break;

    default:
      ret=false;
      break;
  }

  return ret;
}

bool isFormatSupported(uint64_t pixelformat, bool only_color)
{
  if (pixelformat == YCbCr411_8 || pixelformat == YCbCr422_8 || pixelformat == YUV422_8 ||
    pixelformat == RGB8 || pixelformat == BayerRG8 || pixelformat == BayerBG8 ||
    pixelformat == BayerGR8 || pixelformat == BayerGB8)
  {
    return true;
  }

  if (!only_color && (pixelformat == Mono8 || pixelformat == Confidence8 ||
    pixelformat == Error8))
  {
    return true;
  }

  return false;
}

}
