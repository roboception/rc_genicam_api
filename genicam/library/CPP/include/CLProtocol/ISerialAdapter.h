//-----------------------------------------------------------------------------
//  (c) 2009 by DALSA
//  Section: Digital Imaging
//  Project: CLProtocol
//  Author:  Eric Bourbonnais
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
*/

#ifndef CLPROTOCOL_ISERIALADAPTER_H
#define CLPROTOCOL_ISERIALADAPTER_H

#include <CLProtocol/CLProtocol.h>
#include <CLProtocol/ISerial.h>

namespace CLProtocol
{
    struct CLPROTOCOL_DECL ISerialAdapter : public ISerial
    {
        //! Opens the port
        virtual CLINT32 SerialInit( void) = 0;

        //! Closes the port
        virtual void SerialClose( void) = 0;

        //! throws an exception if the ErrorCode describes an error
        virtual void CheckError( 
                CLINT32 ErrorCode       //!< The error code to validate. If at he error is found,
                ) = 0;                  //!< A CLErrException will be thrown.
                
    };

}
#endif // CLPROTOCOL_ISERIALADAPTER_H
