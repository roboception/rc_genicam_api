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
is bundled with this package (except for ARM).

This repository provides a library with wrapper classes for the GenTL producer.
The tools are provided as examples for showing how to use the wrapper classes
and the GenICam reference implementation.

Tools
-----

The tools do not offer a graphical user interface. They are meant to be called
from a shell and controlled by command line parameters. Calling the tools
without any parameters prints a help text on the standard output.

* gc_info

  Lists all available devices with some information. If a device ID is
  provided, all information about the device including current settings are
  listed.

* gc_config

  Can be used to list network specific information of GigE Vision cameras. The
  network settings as well as all other parameters provided via GenICam can be
  changed.

* gc_stream

  This tool shows how to configure and stream images from a camera. GenICam
  features can be configured directly from the command line. Images will be
  stored in PGM or PPM format, dependig on the image format.

  NOTE: Many image viewers can display PGM and PPM format. The sv tool of cvkit
  (https://github.com/roboception/cvkit) can also be used.

* gc_pointcloud

  This tool streams the left image, disparity, confidence and error. It takes
  the first set of time synchroneous images, computes a colored point cloud
  and stores it in PLY ASCII format. This tool demonstrates how to stream
  corresponding images (e.g. left image and disparity with the same time
  stamp).

  NOTE: PLY is a standard format for scanned 3D data. The plyv tool of cvkit
  (https://github.com/roboception/cvkit) can also be used for visualization.

Troubleshooting
---------------

When images are received at a lower rate than set/exepected the most
likely problem is that this (user space) library can't read the many UDP packets
fast enough resulting in incomplete image buffers.

### Test Script

The `net_perf_check.sh` script performs some simple checks and should be run
while or after streaming images via GigE Vision.

    ./net_perf_check.sh --help

### Jumbo Frames

First of all increasing the UDP packet size (using jubo frames) is strongly recommended!
Increase the MTU of your network interface to 9000, e.g.

    sudo ifconfig eth0 mtu 9000

Also make sure that all network devices/switches beween your host and the sensor support this.

### sysctl settings

There are several Linux sysctl options that can be modified to increase
performance for the GigE Vision usecase.

These values can be changed during runtime with `sysctl` or written to
`/etc/sysctl.conf` for persistence across reboots.

#### rmem_max

If the number of UDP RcvbufErrors increases while streaming, increasing the socket receive buffer size usually fixes the problem.

Check the RcvbufErrors with `net_perf_check.sh` or

    netstat -us | grep RcvbufErrors

Increase max receive buffer size:

    sudo sysctl -w net.core.rmem_max=33554432


#### softirq

Changing these values is usually not necessary, but can help if the kernel
is already dropping packets.

Check with `net_perf_check.sh` and increase the values if needed:

    sudo sysctl -w net.core.netdev_max_backlog=2000
    sudo sysctl -w net.core.netdev_budget=600

