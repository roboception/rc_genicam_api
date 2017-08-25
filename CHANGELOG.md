Version 1.2.0 (2017-08-15)
--------------------------

- rename rcgcapi to rc_genicam_api
- getDevice() now accepts specification if interface as prefix
- Added possibility to clean all resources before exit to avoid crashes of GenTL
- add all GenICam files/libs instead of depending on external package
- rename pfnc.h to pixel_formats.h
- bundle GenTL lib from Baumer as fallback if GENICAM_GENTL64_PATH is not set

Version 1.1.5 (2017-05-11)
--------------------------

- Enforced using IPv4 format for setting persistent IPs and improved output of gc_config

Version 1.1.4 (2017-04-13)
--------------------------

- Check if device name is not empty before looking for the device
- Fixed formating of MAC addresses as string

Version 1.1.3 (2017-04-11)
--------------------------

- Catching exceptions in destructors of Device and Stream
- Added changing of GenICam parameters through gc_config

Version 1.1.2 (2017-04-10)
--------------------------

- Streaming with at least 8 buffers by default
- Additionally show display name when listing devices with gc_config -l

Version 1.1.1 (2017-03-06)
--------------------------

- Fixed bug in YCbCr411 to RGB conversion

Version 1.1.0 (2017-03-05)
--------------------------

- Check in gc_stream if buffer is incomplete
- Added optional storing of XML file via gc_info tool
- Added switching PTP on and off via gc_config tool
- Convertions GenApi exceptions to standard exceptions when requesting the node map
- Added helper functions for conversion from YCbCr411 to RGB
- Catching GenApi exception in gc_info tool
- gc_stream tool chooses name of saved images according to pixel format
- Added pfnc.h with custom image format Error8
- Added image and image list helper classes for time synchronization of images
- Open transport layer libraries with deep bind option to prefer local symbol resolution
- Wrapping GenApi Exceptions in get/set feature value helper functions
- Ensure that Buffer::getTimestampNS() always returns a valid value
- Added helper functions for setting and getting features via GenICam
- Fixed crash of gc_stream if device is unknown
- Implemented image streaming and an example for streaming images to file

Version 1.0.1 (2017-02-16)
--------------------------

- Initial release
