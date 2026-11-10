rc_gev_producer
===============

This directory contains rc_gev_producer 0.1.2, the GenTL producer for
GigE Vision devices of Roboception, in binary form. It is not open source, see
LICENSE.txt for the terms of use and redistribution.

- `linux/x86_64`: Linux, 64 bit x86, glibc 2.17 or newer
- `linux/aarch64`: Linux, 64 bit ARM, glibc 2.17 or newer
- `windows/x64`: Windows, 64 bit x86

Each platform directory contains `buildinfo` with the build environment.
`SHA256SUMS` contains the checksums of all files.

The producer is self contained. It only depends on the C library and the
socket implementation of the operating system, so that it can be loaded into
arbitrary GenTL consumers.

The contents of this directory, including the generated parts of
sbom-components.yaml and copyright_extra of the consumer, must not be edited
manually. They are created by the redistributable bundle of the
rc_gev_producer release, see `update.sh`.
