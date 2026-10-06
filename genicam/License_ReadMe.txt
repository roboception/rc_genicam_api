==============
== Licenses ==
==============

GenICam comes in two versions
~ a runtime version
  GenICam runtime and SDK files for developing and/or running GenICam based software.
~ a development version.
  GenICam complete source code for developing and building the GenICam SDK.

The runtime version comes under the following license:

  Copyright (c) EMVA and contributors (see source files)
  All rights reserved

  Redistribution and use in source and binary forms, without modification,
  are permitted provided that the following conditions are met:

  ~ Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.

  ~ Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

  ~ Neither the name of the GenICam standard group nor the names of its contributors
    may be used to endorse or promote products derived from this software without
    specific prior written permission.


  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
  SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.

When distributing GenICam runtime libraries, this file and the license for the development version which
also describes GenICam compliancy and rules (GenICam_License.pdf) must be distributed for the
sake of completeness of the license information.

GenICam runtime packages use the following 3rd party software packages (*):

Package     Version              License   File                                                                                                 Internet                                        Used by
=======================================================================================================================================================================================================================
CLSerAll    1.1.0                NI        CLSerAll_LICENSE.txt (http://sourceforge.net/projects/clallserial)                                   http://sourceforge.net/projects/clallserial     CLAllSerial, CLProtocol
Expat       2.8.1                MIT       expat-COPYING.txt (https://github.com/libexpat/libexpat/blob/master/expat/COPYING)                   https://libexpat.github.io/                     FirmwareUpdate, XmlParser
GenX        cs-1                 MIT       genx-LICENSE.txt  (http://www.tbray.org/ongoing/genx/COPYING)                                        http://www.tbray.org/ongoing/genx               FirmwareUpdate, XmlParser
Mathparser  0.03b (modified)     MIT       MIT_License.txt                                                                                      http://kirya.narod.ru/mathparser.html           GenApi
miniz       3.0.2                MIT       miniz_LICENSE.txt (https://github.com/richgel999/miniz/blob/master/LICENSE)                          https://github.com/richgel999/miniz             FirmwareUpdate, GenTL, XmlParser
PCRE2       10.44                BSD       pcre2_License.txt (http://www.pcre.org/licence.txt)                                                  http://www.pcre.org/                            FirmwareUpdate
regexp      3.2.0  (modified)    MIT       regexp-LICENSE.txt (https://git.codesynthesis.com/cgit/xsde/xsde/tree/libxsde/xsde/c/regexp/LICENSE) https://git.codesynthesis.com/cgit/xsde/        FirmwareUpdate
xxhash      0.8.2                Free BSD  xxhash_License.txt (http://opensource.org/licenses/BSD-2-Clause)                                     https://code.google.com/p/xxhash/               GenApi
zlib        1.3.1                Free                                                                                                           https://www.zlib.net/                           GenTL

(*) When distributing subset of GenICam libraries, any unused 3rd party software packages and their
license information may be omitted.

All license texts come as part of the GenICam distribution in the licenses
subdirectory. If not, you can download them from the Internet.

Last but not least GenICam redistributes the C/C++ runtime DLLs of the
Microsoft Visual C++ compiler.
