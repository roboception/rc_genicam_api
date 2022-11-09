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
    \brief    Exported C functions for camera link protocol drivers
*/
#ifndef CLPROTOCOL_ISERIAL_H
#define CLPROTOCOL_ISERIAL_H

#include <CLProtocol/ClSerialTypes.h>

//==============================================================================
//  Exported Functions
//==============================================================================

/*! \ingroup PublicInterfaces */
/*@{*/

//! Abstract interface used by the CLProtocol driver to use a serial port
class ISerial
{
public:
    //! calls clSerialRead as defined by the camera link standard
    virtual CLINT32 CLPROTOCOL clSerialRead(
        CLINT8* buffer,     //!< Points to a user-allocated buffer. Upon a successful call, 
                            //!< buffer contains the data read from the serial device.  
                            //!< Upon failure, this buffer is not affected.  
                            //!< Caller should ensure that buffer is at least numBytes in size.
        CLUINT32* bufferSize,   //!< This is the number of bytes requested by the caller.
        CLUINT32  serialTimeOut //!< Indicates the timeout in milliseconds.
        ) = 0;

    //! calls clSerialWrite as defined by the camera link standard
    virtual CLINT32 CLPROTOCOL clSerialWrite(
        CLINT8* buffer,      //!< Contains data to write to the serial device.
        CLUINT32* bufferSize,//!< Contains the buffer size indicating 
                             //!< the number of bytes to be written. 
                             //!< Upon a successful call, bufferSize contains the number of 
                             //!< bytes written to the serial device. 
        CLUINT32 serialTimeOut //!< Indicates the timeout in milliseconds.
        ) = 0;

    //! returns the valid baud rates of the current interface.
    virtual CLINT32 CLPROTOCOL clGetSupportedBaudRates(
        CLUINT32 *baudRates //!< Bitfield that describes all supported baud rates of the serial port 
        ) = 0;              //!< as described by serialRefPtr. See Table B-2 of the Camera Link 
                            //!< docu for more information on constants.


    //! This function sets the baud rate for the serial port of the selected device. 
    virtual CLINT32 CLPROTOCOL clSetBaudRate( 
        CLUINT32 baudRate   //!< The baud rate you want to use. This parameter expects the 
        ) = 0;              //!< values represented by the CL_BAUDRATE constants in table B-2. 
                            //!< By default, the baud rate for serial communication is 9600.
};

/*@}*/ // PublicInterfaces

#endif // CLPROTOCOL_ISERIAL_H
