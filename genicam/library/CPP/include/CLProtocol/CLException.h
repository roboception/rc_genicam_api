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
    \brief    CLProtocol exception type
*/
#ifndef CLPROTOCOL_CLEXCEPTION_H
#define CLPROTOCOL_CLEXCEPTION_H

#include <Base/GCBase.h>
#include <CLProtocol/CLProtocol.h>

namespace CLProtocol
{

    //! CL Serial Exception type to hold the CL_ERR-code
    class CLErrException : public GENICAM_NAMESPACE::GenericException
    {
    public:
        // must have this c'tor signature to use the ExceptionReporter
        CLErrException(const char* description, const char *pSourceFileName, int SourceLine, const char* pExceptionType)
            : GENICAM_NAMESPACE::GenericException(description, pSourceFileName, SourceLine, pExceptionType)
            , m_CLCode(CL_ERR_NO_ERR)
        {
        }

    public:
        CLINT32 GetCLErrorCode() const
        {
            return m_CLCode;
        }
        
        CLErrException& SetCLErrorCode(CLINT32 errorCode)
        {
            m_CLCode = errorCode;
            return *this;
        }
    protected:
        CLINT32 m_CLCode;
    };


#if !defined(CL_ERR_EXCEPTION)
#   define CLERR_EXCEPTION GENICAM_NAMESPACE::ExceptionReporter<CLProtocol::CLErrException>(__FILE__, __LINE__, "CLErrException" ).Report
#endif

}
#endif // CLPROTOCOL_CLEXCEPTION_H
