Roboception GenICam Wrapper API
===============================

This package provides an API for easy camera configuration and image streaming
via GenICam and an underlying GenTL layer. It is based on the GenICam reference
implementation (i.e., http://www.emva.org/standards-technology/genicam/genicam-downloads/),
which is included in this package.

At runtime, a GenTL producer >= version 1.5 is required. A GenTL producer is a
shared library that follows the naming schema '*.cti'. The environment variable
GENICAM_GENTL64_PATH (or GENICAM_GENTL32_PATH on 32 Bit systems) must be set to
path(s) that contain GenTL producers. In case the environment variable is not
set or is empty, a default path is used, which points to the GenTL layer that
is bundled with this package.

This repository provides a library with wrapper classes for the GenTL producer.
The tools are provided as examples for showing how to use the wrapper classes
and the GenICam reference implementation.

Tools
-----

The tools do not offer a graphical user interface. They are meant to be called
from a shell and controlled by command line parameters. Calling the tools
without any parameters prints a help text on the standard output.

* gc_info

  Lists information about all producers and cameras available on the system. If
  a device ID is provided, all information about the device including current
  settings are listed.

* gc_config

  Can be used to list network specific information of GigE Vision cameras. The
  network settings as well as all other parameters provided via GenICam can be
  changed.

* gc_stream

  This tool shows how to configure and stream images from a camera. GenICam
  features can be configured directly from the command line. Images will be
  stored in PGM or PPM format, dependig on the image format.

  NOTE: The sv tool of cvkit can be used to view images in this format.