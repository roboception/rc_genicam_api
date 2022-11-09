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
\brief    Implementation of firmware update exceptions
*/
#ifndef GENFW_FIRMWAREUPDATEEXCEPTIONS_H
#define GENFW_FIRMWAREUPDATEEXCEPTIONS_H

#include <GenICamFwd.h>
#include <Base/GCException.h>
#include <FirmwareUpdate/FirmwareUpdateDll.h>
#include <FirmwareUpdate/FirmwareUpdateLinkage.h>

//! Creates an exception with the same functionality as the GenericException but being of different type
//! \ingroup Base_PublicUtilities
#define GENFW_DECLARE_EXCEPTION( name ) \
        class GENFW_DECL name : public GENICAM_NAMESPACE::GenericException \
                                                { \
        public: \
            name( const char* pDescription, const char *pSourceFileName, int SourceLine ); \
            name( const char* pDescription, const char *pSourceFileName, int SourceLine, const char* pExceptionType  ); \
            name( const char* pDescription, const char *pSourceFileName, int SourceLine, const char *pEntryPoint, const char *pErrorNodeName, const char* pExceptionType ); \
                                                }

//-------------------------------------------------------------
//
namespace GENICAM_NAMESPACE
{
    //! Exception fired if a file is not found.
    GENFW_DECLARE_EXCEPTION(FileNotFoundException);

    //! Exception fired if a file read or a file write operation fails.
    GENFW_DECLARE_EXCEPTION(FileIOException);

    //! Exception fired if a type parsed by a method is not of the specified format.
    GENFW_DECLARE_EXCEPTION(FormatException);
}



//! Fires a FileNotFoundException
#   define GENFW_FILE_NOT_FOUND_EXCEPTION GENICAM_NAMESPACE::ExceptionReporter<GENICAM_NAMESPACE::FileNotFoundException>(__FILE__, __LINE__).Report

//! Fires a FileIOException
#   define GENFW_FILE_IO_EXCEPTION GENICAM_NAMESPACE::ExceptionReporter<GENICAM_NAMESPACE::FileIOException>(__FILE__, __LINE__).Report

//! Fires a FormatException
#   define GENFW_FORMAT_EXCEPTION GENICAM_NAMESPACE::ExceptionReporter<GENICAM_NAMESPACE::FormatException>(__FILE__, __LINE__).Report

#endif // GENFW_FIRMWAREUPDATEEXCEPTIONS_H
