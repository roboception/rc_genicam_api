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

#include <rcgcapi/system.h>
#include <rcgcapi/interface.h>
#include <rcgcapi/device.h>
#include <rcgcapi/stream.h>
#include <rcgcapi/buffer.h>

#define PFNC_INCLUDE_HELPERS
#include <GenTL/PFNC.h>

#include <iostream>
#include <fstream>
#include <iomanip>

namespace
{

/**
  Set a given value.

  @param nodemap Nodemap.
  @param key     Key of node to be changed.
  @param value   Value to be set.
*/

void setValue(const std::shared_ptr<GenApi::CNodeMapRef> &nodemap, const std::string &key,
              const std::string &value)
{
  GenApi::INode *node=nodemap->_GetNode(key.c_str());

  if (node == 0)
  {
    std::cerr << key << ": Does not exist!" << std::endl;
    return;
  }

  if (!GenApi::IsWritable(node))
  {
    std::cerr << key << ": Is not writable!" << std::endl;
    return;
  }

  switch (node->GetPrincipalInterfaceType())
  {
    case GenApi::intfIBoolean:
      {
        GenApi::IBoolean *val=dynamic_cast<GenApi::IBoolean *>(node);
        val->SetValue(std::stoi(value));
      }
      break;

    case GenApi::intfIString:
      {
        GenApi::IString *val=dynamic_cast<GenApi::IString *>(node);
        val->SetValue(value.c_str());
      }
      break;

    case GenApi::intfIEnumeration:
      {
        GenApi::IEnumeration *val=dynamic_cast<GenApi::IEnumeration *>(node);
        GenApi::IEnumEntry *entry=val->GetEntryByName(value.c_str());

        if (entry != 0)
        {
          val->SetIntValue(entry->GetValue());
        }
        else
        {
          std::cerr << key << ": Cannot find enum: " << value << std::endl;
        }
      }
      break;

    case GenApi::intfIFloat:
      {
        GenApi::IFloat *val=dynamic_cast<GenApi::IFloat *>(node);
        val->SetValue(std::stof(value));
      }
      break;

    case GenApi::intfIInteger:
      {
        GenApi::IInteger *val=dynamic_cast<GenApi::IInteger *>(node);
        val->SetValue(std::stoi(value));
      }
      break;

    default:
      std::cerr << key << ": Cannot set values of that type" << std::endl;
      break;
  }
}

/**
  Clamp the given value to the range of 0 to 255 and cast to byte.
*/

unsigned char clamp8(int v)
{
  if (v < 0) v=0;
  if (v > 255) v=255;

  return static_cast<unsigned char>(v);
}

/**
  Store image given in buffer in PGM or PPM format.
*/

std::string storeBuffer(const rcg::Buffer *buffer, double freq)
{
  // prepare file name

  std::ostringstream name;

  double t=buffer->getTimestamp()/freq;

  name << "image_" << std::setprecision(16) << t;

  // store image (see e.g. the sv tool of cvkit for show images)

  if (buffer->getImagePresent())
  {
    int width=buffer->getWidth();
    int height=buffer->getHeight();
    const unsigned char *p=static_cast<const unsigned char *>(buffer->getBase())+buffer->getImageOffset();

    size_t px=buffer->getXPadding();

    uint64_t format=buffer->getPixelFormat();
    switch (format)
    {
      case Mono8: // store 8 bit monochrome image
        {
          name << ".pgm";
          std::ofstream out(name.str(), std::ios::binary);

          out << "P5" << std::endl;
          out << width << " " << height << std::endl;
          out << 255 << "\n";

          std::streambuf *sb=out.rdbuf();

          for (int k=0; k<height && out.good(); k++)
          {
            for (int i=0; i<width; i++)
            {
              sb->sputc(*p++);
            }

            p+=px;
          }

          out.close();
        }
        break;

      case Mono16: // store 16 bit monochrome image
        {
          name << ".pgm";
          std::ofstream out(name.str(), std::ios::binary);

          out << "P5" << std::endl;
          out << width << " " << height << std::endl;
          out << 65535 << "\n";

          std::streambuf *sb=out.rdbuf();

          // copy image data, pgm is always big endian

          if (buffer->getIsLittleEndian())
          {
            for (int k=0; k<height && out.good(); k++)
            {
              for (int i=0; i<width; i++)
              {
                sb->sputc(p[1]);
                sb->sputc(p[0]);
                p+=2;
              }

              p+=px;
            }
          }
          else
          {
            for (int k=0; k<height && out.good(); k++)
            {
              for (int i=0; i<width; i++)
              {
                sb->sputc(*p++);
                sb->sputc(*p++);
              }

              p+=px;
            }
          }

          out.close();
        }
        break;

      case YCbCr411_8: // convert and store as color image
        {
          name << ".ppm";
          std::ofstream out(name.str(), std::ios::binary);

          out << "P6" << std::endl;
          out << width << " " << height << std::endl;
          out << 255 << "\n";

          std::streambuf *sb=out.rdbuf();

          for (int k=0; k<height && out.good(); k++)
          {
            for (int i=0; i<width; i+=4)
            {
              int Y[4]={p[0], p[1], p[3], p[4]};
              int Cb=static_cast<int>(p[2])-128;
              int Cr=static_cast<int>(p[5])-128;

              int rc=static_cast<int>(1.40200*Cr+0.5);
              int gc=static_cast<int>(-0.34414*Cb-0.71414*Cr+0.5);
              int bc=static_cast<int>(1.77200*Cb+0.5);

              for (int j=0; j<4; j++)
              {
                sb->sputc(clamp8(Y[j]+rc));
                sb->sputc(clamp8(Y[j]+gc));
                sb->sputc(clamp8(Y[j]+bc));
              }

              p+=6;
            }

            p+=px;
          }

          out.close();
        }
        break;

      case YCbCr411_8_CbYYCrYY: // convert and store as color image
        {
          name << ".ppm";
          std::ofstream out(name.str(), std::ios::binary);

          out << "P6" << std::endl;
          out << width << " " << height << std::endl;
          out << 255 << "\n";

          std::streambuf *sb=out.rdbuf();

          for (int k=0; k<height && out.good(); k++)
          {
            for (int i=0; i<width; i+=4)
            {
              int Y[4]={p[1], p[2], p[4], p[5]};
              int Cb=static_cast<int>(p[0])-128;
              int Cr=static_cast<int>(p[3])-128;

              int rc=static_cast<int>(1.40200*Cr+0.5);
              int gc=static_cast<int>(-0.34414*Cb-0.71414*Cr+0.5);
              int bc=static_cast<int>(1.77200*Cb+0.5);

              for (int j=0; j<4; j++)
              {
                sb->sputc(clamp8(Y[j]+rc));
                sb->sputc(clamp8(Y[j]+gc));
                sb->sputc(clamp8(Y[j]+bc));
              }

              p+=6;
            }

            p+=px;
          }

          out.close();
        }
        break;

      default:
        std::cerr << "storeBuffer(): Unknown pixel format: "
                  << GetPixelFormatName(static_cast<PfncFormat>(buffer->getPixelFormat()))
                  << std::endl;
        return std::string();
        break;
    }
  }
  else
  {
    std::cerr << "storeBuffer(): Received buffer without image" << std::endl;
    return std::string();
  }

  return name.str();
}

}

int main(int argc, char *argv[])
{
  try
  {
    if (argc >= 2)
    {
      // find specific device accross all systems and interfaces and open it

      std::shared_ptr<rcg::Device> dev=rcg::getDevice(argv[1]);

      double freq=dev->getTimestampFrequency();

      if (freq == 0)
      {
        freq=1000000000;
      }

      if (dev)
      {
        dev->open(rcg::Device::EXCLUSIVE);
        std::shared_ptr<GenApi::CNodeMapRef> nodemap=dev->getRemoteNodeMap();

        // set values as given on the command line

        int n=1;

        int i=2;
        while (i < argc)
        {
          // split argument in key and value

          std::string key=argv[i++];
          std::string value;

          size_t k=key.find('=');
          if (k != std::string::npos)
          {
            value=key.substr(k+1);
            key=key.substr(0, k);
          }

          if (key == "n") // set number of images
          {
            n=std::stoi(value);
          }
          else // set key=value pair through GenICam
          {
            setValue(nodemap, key, value);
          }
        }

        // open stream and get n images

        std::vector<std::shared_ptr<rcg::Stream> > stream=dev->getStreams();

        if (stream.size() > 0)
        {
          // opening first stream

          stream[0]->open();
          stream[0]->startStreaming();

          for (int k=0; k<n; k++)
          {
            // grab next image with timeout of 3 seconds

            const rcg::Buffer *buffer=stream[0]->grab(3000);

            if (buffer != 0)
            {
              std::string name=storeBuffer(buffer, freq);

              if (name.size() > 0)
              {
                std::cout << "Image '" << name << "' stored" << std::endl;
              }
            }
            else
            {
              std::cerr << "Cannot grab images" << std::endl;
              break;
            }
          }

          stream[0]->stopStreaming();
          stream[0]->close();
        }
        else
        {
          std::cerr << "No streams available" << std::endl;
        }

        dev->close();
      }
      else
      {
        std::cerr << "Device '" << argv[1] << "' not found!" << std::endl;
      }
    }
    else
    {
      // show help

      std::cout << argv[0] << " <device-id> [n=<n>] [<key>=<value>] ..." << std::endl;
      std::cout << std::endl;
      std::cout << "<device-id>   Device from which data will be streamed" << std::endl;
      std::cout << "n=<n>         Number of images to receive. Default is 1" << std::endl;
      std::cout << "<key>=<value> Values set via GenICam before streaming images" << std::endl;
    }
  }
  catch (const std::exception &ex)
  {
    std::cerr << ex.what() << std::endl;
  }

  return 0;
}