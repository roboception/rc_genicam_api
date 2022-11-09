//-----------------------------------------------------------------------------
//  (c) 2016-2017 by Basler AG
//  Project: GenApi
//  Author:  Andreas Gau
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
\brief    Defines to be used for FirmwareUpdate library
*/

#ifndef GENFW_DLL_H
#define GENFW_DLL_H

#include <GenApi/GenApiNamespace.h>

#if defined(_MSC_VER)
#   pragma once
#   define GENFW_DECL_ABSTRACT /*nothing*/
#   ifdef GENFW_EXPORTS
#       define GENFW_DECL __declspec(dllexport)
#   else
#       define GENFW_DECL __declspec(dllimport)
#       define GENFW_EXPIMP_TEMPL extern
#   endif
#elif defined (__GNUC__) && (defined (__linux__) || defined(__APPLE__) || defined(VXWORKS))
#   define GENFW_DECL __attribute__((visibility("default"))) 
#   define GENFW_DECL_ABSTRACT __attribute__((visibility("default"))) 
#   ifdef GENFW_EXPORTS
        // nothing
#   else
#       define GENFW_EXPIMP_TEMPL extern
#   endif
#else
#   #error Define how to export symbols on your platform!
#   define GENFW_DECL
#   ifndef GENFW_EXPORTS
#       define GENFW_EXPIMP_TEMPL extern
#   endif
#endif


#endif /* GENFW_DLL_H */
