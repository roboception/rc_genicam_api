//-----------------------------------------------------------------------------
//  (c) 2016-2017 by Basler AG
//  Project: GenApi
//  Author:  hn
//
//  License: This file is published under the license of the EMVA GenICam Standard Group.
//  A text file describing the legal terms is included in your installation as 'GenICam_license.pdf'.
//  If for some reason you are missing this file please contact the EMVA or visit the website
//  (http://www.genicam.org) for a full copy.
//
//  THIS SOFTWARE IS PROVIDED BY THE EMVA GENICAM STANDARD GROUP "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE EMVA GENICAM STANDARD GROUP
//  OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
//  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
//  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//  ARISING IN ANY WAY OUT OF THE USE  OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------
/*!
 \file
 \brief    helpers for pragma linkage
 */

#ifndef GENFW_FIRMWAREUPDATE_LINKAGE_H
#define GENFW_FIRMWAREUPDATE_LINKAGE_H

#if defined (_MSC_VER)
#    include <Base/GCLinkage.h>
#    include <GenApi/GenApiLinkage.h>
     
#    if defined(GENICAM_FORCE_AUTO_IMPLIB) || (!defined(GENICAM_NO_AUTO_IMPLIB) && !defined(GENFW_EXPORTS))
#       if defined (_WIN32) && defined (_MT )
#           pragma comment(lib, LIB_NAME( "FirmwareUpdate" ))
#       else
#           error Invalid configuration
#       endif
#    endif
#endif // _MSC_VER

#endif // GENFW_FIRMWAREUPDATE_LINKAGE_H
