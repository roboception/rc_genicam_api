//-----------------------------------------------------------------------------
//  (c) 2009 by Basler
//  Section: Digital Imaging
//  Project: CLProtocol
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
*/

#ifndef CLPROTOCOL_ISERIALADAPTERCSTYLE_H
#define CLPROTOCOL_ISERIALADAPTERCSTYLE_H

#include <CLProtocol/ISerialAdapter.h>

#if( defined(__cplusplus) || defined(__cplusplus__) ) 
    extern "C"
#else
    namespace CLProtocol
#endif
    // The following declarations enable you to create an ISerialAdapter using pure C. 
    // For an example see RequirementsAndTests::TestIStructCVersion in CLProtocolTest 
    // The following link gives some technical background http://www.codeproject.com/KB/COM/com_in_c1.aspx
    {

        typedef CLINT32 (CLPROTOCOL *clSerialReadPtr)(
                void * This,
                CLINT8* buffer,
                CLUINT32* bufferSize,
                CLUINT32  serialTimeOut
                );

        typedef CLINT32 (CLPROTOCOL *clSerialWritePtr)(
                void * This,
                CLINT8* buffer,
                CLUINT32* bufferSize,
                CLUINT32 serialTimeOut
                );

        typedef CLINT32 (CLPROTOCOL *clGetSupportedBaudRatesPtr)(
                void * This,
                CLUINT32 *baudRates
                );  

        typedef CLINT32 (CLPROTOCOL *clSetBaudRatePtr)( 
                void * This,
                CLUINT32 baudRate
                );

        typedef CLINT32 (CLPROTOCOL *SerialInitPtr)( 
                void * This
                );

        typedef void (CLPROTOCOL *SerialClosePtr)( 
                void * This
                );

        typedef void (CLPROTOCOL *CheckErrorPtr)( 
                void * This,
                CLUINT32 ErrorCode
                );

        typedef struct ISerialAdapterVtbl_t
        {
            clSerialReadPtr *clSerialRead;
            clSerialWritePtr *clSerialWrite;
            clGetSupportedBaudRatesPtr *clGetSupportedBaudRates;
            clSetBaudRatePtr *clSetBaudRate;
            SerialInitPtr *SerialInit;
            SerialClosePtr *SerialClose;
            CheckErrorPtr *CheckError;
        } ISerialAdapterVtbl_t;

        typedef struct ISerialAdapter_t
        {
            struct ISerialAdapterVtbl_t *lpVtbl;
        } ISerialAdapter_t;

    }

#endif 
