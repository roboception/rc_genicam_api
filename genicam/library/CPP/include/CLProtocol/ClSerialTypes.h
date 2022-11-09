//-----------------------------------------------------------------------------
//  (c) 2008 by Basler Vision Technologies
//  Section: Vision Components
//  Project: GenApi
//  Author:  Fritz Dierks
//  $Header$
//
//  License: This file is published under the license of the EMVA GenICam  Standard Group. 
//  A text file describing the legal terms is included in  your installation as 'GenICam_license.pdf'. 
//  If for some reason you are missing  this file please contact the EMVA or visit the website
//  (http://www.genicam.org) for a full copy.
// 
//  THIS SOFTWARE IS PROVIDED BY THE EMVA GENICAM STANDARD GROUP "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,  
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR  
//  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE EMVA GENICAM STANDARD  GROUP 
//  OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,  SPECIAL, 
//  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT  LIMITED TO, 
//  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,  DATA, OR PROFITS; 
//  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY  THEORY OF LIABILITY, 
//  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE  OF THIS SOFTWARE, EVEN IF ADVISED OF THE  
//  POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------
/*!
    \file
    \brief    types and constants as used by the Camera Link's API definition
*/

#ifndef CLPROTOCOL_CLSERIALTYPES_H
#define CLPROTOCOL_CLSERIALTYPES_H

//==============================================================================
//  Macros 
//==============================================================================

//------------------------------------------------------------------------------
//  CL serial DLL versions
//------------------------------------------------------------------------------
#define CL_DLL_VERSION_NO_VERSION               1   // Not a CL dll
#define CL_DLL_VERSION_1_0                      2   // Oct 2000 compliant
#define CL_DLL_VERSION_1_1                      3   // Oct 2001 compliant
#define CL_DLL_VERSION_2_0                      4   // Feb 2012 compliant
#define CL_DLL_VERSION_2_1                      5   // Apr 2018 compliant

//------------------------------------------------------------------------------
//  Baud Rates (CLUINT32)
//------------------------------------------------------------------------------
#define CL_BAUDRATE_9600                        0x00000001
#define CL_BAUDRATE_19200                       0x00000002
#define CL_BAUDRATE_38400                       0x00000004
#define CL_BAUDRATE_57600                       0x00000008
#define CL_BAUDRATE_115200                      0x00000010
#define CL_BAUDRATE_230400                      0x00000020
#define CL_BAUDRATE_460800                      0x00000040
#define CL_BAUDRATE_921600                      0x00000080
#define CL_BAUDRATE_AUTOMAX                     0x40000000

//------------------------------------------------------------------------------
//  Error Codes
//------------------------------------------------------------------------------
#define CL_ERR_NO_ERR                               0
#define CL_ERR_BUFFER_TOO_SMALL                     -10001
#define CL_ERR_MANU_DOES_NOT_EXIST                  -10002
#define CL_ERR_PORT_IN_USE                          -10003
#define CL_ERR_TIMEOUT                              -10004
#define CL_ERR_INVALID_INDEX                        -10005
#define CL_ERR_INVALID_REFERENCE                    -10006
#define CL_ERR_ERROR_NOT_FOUND                      -10007
#define CL_ERR_BAUD_RATE_NOT_SUPPORTED              -10008
#define CL_ERR_OUT_OF_MEMORY                        -10009
#define CL_ERR_REGISTRY_KEY_NOT_FOUND               -10010
#define CL_ERR_INVALID_PTR                          -10011
#define CL_ERR_IN_USE                               -10012
#define CL_ERR_UNABLE_TO_LOAD_DLL                   -10098
#define CL_ERR_FUNCTION_NOT_FOUND                   -10099

#define CL_ERR_PENDING_WRITE                         20101
#define CL_ERR_INVALID_DEVICEID                     -20100
#define CL_ERR_NO_DEVICE_FOUND                      -20101
#define CL_ERR_NO_XMLDESCRIPTION_FOUND              -20102
#define CL_ERR_INVALID_COOKIE                       -20103
#define CL_ERR_GET_LAST_ERROR                       -20104
#define CL_ERR_PARAM_NOT_SUPPORTED                  -20105
#define CL_ERR_PARAM_READ_ONLY                      -20106
#define CL_ERR_PARAM_DATA_SIZE                      -20107
#define CL_ERR_PARAM_DATA_VALUE                     -20108

//------------------------------------------------------------------------------
//  Types
//------------------------------------------------------------------------------

#ifndef _CL_hSerRef_DEFINED_
    #define _CL_hSerRef_DEFINED_
    typedef void*        hSerRef;
#endif

#ifndef _CL_INT32_DEFINED_
    #define _CL_INT32_DEFINED_
    typedef int          CLINT32;
#endif

#ifndef _CL_UINT32_DEFINED_
    #define _CL_UINT32_DEFINED_
    typedef unsigned int CLUINT32;
#endif

#ifndef _CL_INT64_DEFINED_
    #if !defined(_WIN32)
        #include <inttypes.h>
        #if !defined(INTEL_COMPILER) && !defined(INTEL_LLVM_COMPILER) && !defined(int64)
            typedef int64_t __int64;
        #endif
    #endif
    #define _CL_INT64_DEFINED_
    typedef __int64 CLINT64;
#endif

#ifndef _CL_UINT16_DEFINED_
    #define _CL_UINT16_DEFINED_
    typedef unsigned short UINT16;
#endif

#ifndef _CL_INT8_DEFINED_
    #define _CL_INT8_DEFINED_
    typedef char         CLINT8;
#endif

#ifndef _CL_BOOL8_DEFINED_
    #define _CL_BOOL8_DEFINED_
    typedef char         BOOL8;
#endif

#endif // CLPROTOCOL_CLSERIALTYPES_H
