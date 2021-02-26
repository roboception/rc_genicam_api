Roboception GenICam Convenience Layer
=====================================

This package combines the Roboception convenience layer for images with the
GenICam reference implementation and a GigE Vision transport layer. It is a
self contained package that permits configuration and image streaming of
GenICam / GigE Vision 2.0 compatible cameras like the Roboception rc_visard.
The API is based on C++ 11 and can be compiled under Linux and Windows.

This package also provides some tools that can be called from the command line
for discovering cameras, changing their configuration and streaming images.

**Prebuilt binaries can be downloaded on the [releases](https://github.com/roboception/rc_genicam_api/releases) page.**

Contents
--------

- [Minimum Requirements](#minimum-requirements)
- [Compiling and Installing](#compiling-and-installing)
  - [Linux](#linux)
  - [Windows](#windows-and-visual-studio)
- [Description of Tools](#description-of-tools)
  - [gc_info](#gc_info)
  - [gc_config](#gc_config)
  - [gc_stream](#gc_stream)
  - [gc_pointcloud](#gc_pointcloud)
  - [gc_file](#gc_file)
- [Definition of Device ID](#definition-of-device-id)
- [Finding the Transport Layer](#finding-the-transport-layer)
- [Network Optimization under Linux](#network-optimization-under-linux)

Minimum Requirements
--------------------

- Linux x64 / i86: gcc >= 4.8
- ARMhf: gcc >= 4.9.4
- Linux AArch64: gcc >= 5.4
- Windows 10: Visual Studio >= VC140

Compiling and Installing
------------------------

### Linux

Building follows the standard cmake build flow. Please make sure to set the
install path *before* compiling. Otherwise it can happen that the transport
layer is not found when calling the tools.

    cd <main-directory>
    mkdir build
    cd build
    cmake -DCMAKE_INSTALL_PREFIX=<install-directory> ..
    make
    make install

#### Bash completion

To install bash completion, configure cmake with `-DINSTALL_COMPLETION=ON`

#### Debian package

A Debian package can be built with e.g.

    cd <main-directory>
    mkdir build
    cd build
    cmake -DCMAKE_INSTALL_PREFIX=/usr ..
    make
    make package

### Windows and Visual Studio

Building is based on cmake. Therefore, cmake must be downloaded and installed
according to the operating system from https://cmake.org/download/ After
starting the cmake-gui, the path to the rc_genicam_api source code directory
as well as the build directory must be specified. It is common to choose
a sub-directory of the source code directory and name it 'build' for the
the temporary files that are created during the build process. After setting
both paths, the 'Configure' button must be pressed. In the up-coming dialog,
it can be chosen for which version of Visual Studio and which platform (e.g.
Win64) the project files should be generated. The dialog is closed by pressing
'Finish'.

After configuration, the value of the key with the name `CMAKE_INSTALL_PREFIX`
may be changed to an install directory. By default, the install directory is
set to a path like `C:/Program Files/rc_genicam_api`. The 'Generate' button
leads to creating the project file. Visual Studio can be opened with this
project by pressing the 'Open Project' button.

By default, a 'Debug' version will be compiled. This can be changed to 'Release'
for compiling an optimized version. The package can then be created, e.g. by
pressing 'F7'. For installing the compiled package, the 'INSTALL' target can be
*created* in the project explorer.

After installation, the install directory will contain three sub-directories.
The 'bin' directory contains the tools, DLLs and the default transport layer
including configuration. The 'include' and 'lib' sub-directories contain the
headers and libraries for using the API in own programs.

NOTE: For using the libraries in own projects, define the symbol
`GENICAM_NO_AUTO_IMPLIB` in your project file to avoid linker problems with the
GenICam libraries.

Description of Tools
--------------------

The tools do not offer a graphical user interface. They are meant to be called
from a shell (e.g. Power Shell under Windows) or script and controlled by
command line parameters. Calling the tools without any parameters prints a help
text on the standard output.

NOTE: If any tool returns the error `No transport layers found in path ...`,
then read the section 'Transport Layer' below.

### gc_info

Lists all available systems (i.e. transport layers), interfaces and devices
with some information. If a device ID is given on the command line, then the
complete GenICam nodemap with all parameters and their current values are
listed.

```
gc_info -h | -l | ([-o <xml-output-file>] [<interface-id>:]<device-id>[?<node>] [<key>=<value>] ...)

Provides information about GenICam transport layers, interfaces and devices.

Options:
-h   Prints help information and exits
-l   List all all available devices on all interfaces
-o   Filename to store XML description from specified device

Parameters:
<interface-id> Optional GenICam ID of interface for connecting to the device
<device-id>    GenICam device ID, serial number or user defined name of device
<node>         Optional name of category or parameter to be reported
<key>=<value>  Optional GenICam parameters to be changed in the given order before reporting
```

### gc_config

Can be used to list network specific information of GenICam compatible GigE
Vision 2 cameras. The network settings as well as all other parameters
provided via GenICam can be changed.

```
gc_config -h | -l | ([<interface-id>:]<device-id> <options> ...)

Configuration of a GigE Vision device via GenICam.

-h             Prints help information and exits
-l             Lists all available GigE Vision devices

Parameters:
<interface-id> Optional GenICam ID of interface for connecting to the device
<device-id>    GenICam device ID, serial number or user defined name of device

Options:
-n <id>        Set user defined id
-d 1|0         Switch DHCP on or off
-p 1|0         Switch persistent IP on or off
-t 1|0         Switch precision time protocol (ptp) on or off
-i <ip>        Set persistent IP address
-s <ip>        Set subnet mask for persistent IP address
-g <ip>        Set default gateway for persistent IP address
--iponly       Show current IP of device instead of full summary
<key>=<value>  Optional GenICam parameters to be changed in the given order
```

### gc_stream

This tool shows how to configure and stream images from a camera. GenICam
features can be configured directly from the command line. Images will be
stored in PGM or PPM format, depending on the image format.

Streams of the Roboception rc_visard can be enabled or disabled directly on
the command line by setting the appropriate GenICam parameters. The following
command enables intensity images, disables disparity images and stores 10
images:

```
gc_stream <ID> ComponentSelector=Intensity ComponentEnable=1 ComponentSelector=Disparity ComponentEnable=0 n=10
```

NOTE: Many image viewers can display PGM and PPM format. The sv tool of
[cvkit](https://github.com/roboception/cvkit) can also be used.

```
gc_stream -h | [-f <fmt>] [-t] [<interface-id>:]<device-id> [n=<n>] [<key>=<value>] ...

Stores images from the specified device after applying the given optional GenICam parameters.

Options:
-h         Prints help information and exits
-t         Testmode, which does not store images and provides extended statistics
-f pnm|png Format for storing images. Default is pnm

Parameters:
<interface-id> Optional GenICam ID of interface for connecting to the device
<device-id>    GenICam device ID, serial number or user defined name of device
n=<n>          Optional number of images to be received (default is 1)
<key>=<value>  Optional GenICam parameters to be changed in the given order
```

### gc_pointcloud

This tool streams the left image, disparity, confidence and error from a
Roboception rc_visard sensor. It takes the first set of time synchronous
images, computes a colored point cloud and stores it in PLY ASCII format.
This tool demonstrates how to synchronize different images according to
their timestamps.

NOTE: PLY is a standard format for scanned 3D data that can be read by many
programs. The plyv tool of [cvkit](https://github.com/roboception/cvkit) can
also be used for visualization.

```
gc_pointcloud -h | [-o <output-filename>] [<interface-id>:]<device-id>

Gets the first synchronized image set of the Roboception rc_visard, consisting
of left, disparity, confidence and error image, creates a point cloud and
stores it in ply ascii format.

Options:
-h        Prints help information and exits
-o <file> Set name of output file (default is 'rc_visard_<timestamp>.ply')

Parameters:
<interface-id> Optional GenICam ID of interface for connecting to the device
<device-id>    GenICam device ID, serial number or user defined name of device
```

### gc_file

This tool can be used to upload and download a file into the persistent user
space of an industrial camera.

```
tools/gc_file -h | [<interface-id>:]<device-id> -f | (<device-file> [-w|-r <file>])

Downloading or uploading a file via GenICam.

-h            Prints help information and exits
-f            Lists names of files on the device
-w <file>     Writes the given local file into the selected file on the device
-r <file>     Reads the selected file on the device and stores it as local file

The selected file is printed on std out if none of -f, -w and -r are given.
```

Definition of Device ID
-----------------------

There are multiple ways of specifying an ID to identify a device.

1. The serial number of the device serves as ID.
   Example: `02911931`

2. The given ID can also be a user defined name. The user defined name is set
   to `rc_visard` by default and can be changed with:

       gc_config <ID> -n <user-defined-name>

   This way of identifying a device can fail if there is more than one device
   with the same name. No device is returned in this case.

   If the user defined name contains one or more colons, it must be preceded by
   a colon (e.g. `:my:name`) or an interface ID (see below).

3. The device ID of the GenTL producer (see `Transport Layer` section below)
   may also be used. This ID is unique, but not persistent as it depends on the
   implementation of the GenTL producer. Thus, it can change after software
   updates. It often encodes the MAC address of the sensor in some way.

   Example: `00_14_2d_2c_6e_bb`

All three options can be seen in the output of `gc_config -l`.

### Optional Interface ID prefix

If the given ID contains a colon (i.e. `:`), the part before the (first)
colon is interpreted as interface ID and the part after the first colon is
treated as device ID. This is the format that `gc_config -l` shows. A device
with the given ID is only sought on the specified interface. This can be
useful if there are several ways to reach a device from a host computer,
e.g. via wireless and wired network connection, but a certain connection
type (e.g. wired) is preferred due to higher bandwidth and lower latency.

Examples: `eth0:00_14_2d_2c_6e_bb`, `eth1:02911931` or `wlan0:rc_visard`

A colon at the beginning of the ID effectively defines an empty interface ID
which triggers looking on all interfaces.

If the given ID does not contain a colon, the ID is interpreted as the
device ID itself and is sought throughout all interfaces as well.

Finding the Transport Layer
---------------------------

The communication to the device is done through a so called transport layer
(i.e. GenTL producer version 1.5 or higher). This package provides and installs
a default transport layer that implements the GigE Vision protocol for
connecting to the Roboception rc_visard. According to the GenICam
specification, the transport layer has the suffix '.cti'. The environment
variable `GENICAM_GENTL32_PATH` (for 32 bit applications) or `GENICAM_GENTL64_PATH`
(for 64 bit applications) must contain a list of paths that contain transport
layers. All transport layers are provided as systems to the application.

For convenience, if the environment variable is not defined or empty, it is
internally defined with the install path of the provided transport layer (as
known at compile time!). If the package is not installed, the install path is
changed after compilation or the package is moved to another location after
installation, then the transport layer may not be found. In this case, the
tools shows an error like e.g.:

    'No transport layers found in path /usr/lib/rc_genicam_api'

In this case, the corresponding environment variable (see above) must be set to
the directory in which the transport layer (i.e. file with suffix '.cti')
resides.

Under Windows, as second fall back additionally to the install path, the
directory of the executable is also added to the environment variable. Thus,
the install directory can be moved, as long as the cti file stays in the same
directory as the executable.

Network Optimization under Linux
--------------------------------

When images are received at a lower rate than set/exepected the most
likely problem is that this (user space) library cannot read the many UDP
packets fast enough resulting in incomplete image buffers.

### Test Script

The `net_perf_check.sh` script performs some simple checks and should be run
while or after streaming images via GigE Vision.

    ./net_perf_check.sh --help

### Jumbo Frames

First of all increasing the UDP packet size (using jubo frames) is strongly
recommended! Increase the MTU of your network interface to 9000, e.g.

    sudo ifconfig eth0 mtu 9000

Also make sure that all network devices/switches between your host and the
sensor support this.

### sysctl settings

There are several Linux sysctl options that can be modified to increase
performance for the GigE Vision usecase.

These values can be changed during runtime with `sysctl` or written to
`/etc/sysctl.conf` for persistence across reboots.

#### rmem_max

If the number of UDP RcvbufErrors increases while streaming, increasing the
socket receive buffer size usually fixes the problem.

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
