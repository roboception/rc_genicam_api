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
    \brief    Exported C functions as defined by the Camera Link standard v1.1
*/
#ifndef CLPROTOCOL_CLSERIAL_H
#define CLPROTOCOL_CLSERIAL_H

#include <CLProtocol/ClSerialTypes.h>

//==============================================================================
//  Macros 
//==============================================================================

#if (defined(__cplusplus) || defined(__cplusplus__))
    #define USER_EXTERN_C extern "C"
#else
    #define USER_EXTERN_C
#endif

#ifdef CLSERIAL_EXPORTS
    #if defined(_WIN32)
        #define CLSERIALEXPORT USER_EXTERN_C __declspec(dllexport)
    #else
        #define CLSERIALEXPORT USER_EXTERN_C __attribute__((visibility("default")))
    #endif
#else
    #if defined(_WIN32)
        #define CLSERIALEXPORT USER_EXTERN_C __declspec(dllimport)
    #else
        #define CLSERIALEXPORT USER_EXTERN_C __attribute__((visibility("default")))
    #endif
#endif

#ifndef CLSERIALCC
    // Note that the CLAllSerial.dll uses __stdcall instead of __cdecl
    #if defined(_WIN32)
        #define CLSERIALCC __cdecl
    #else
        #if !defined(__x86_64) && !defined(__arm__) && !defined(__aarch64__) && !defined(VXWORKS)
            #define  CLSERIALCC __attribute__((cdecl))
        #else
            #define  CLSERIALCC  /* use default calling convention */
        #endif
    #endif
#endif

//==============================================================================
//  exported functions
//==============================================================================

/*! \ingroup PublicInterfaces */
/*@{*/

/*! \defgroup CLSerialInterface CLSerial C-Function Interface */
/*@{*/


/*!
\brief This function returns the name of the manufacturer represented by the index parameter 
       and the version of the Camera Link specifications with which the specified .dll complies.

\return At completion, this function returns one of the following status codes: 
        CL_ERR_NO_ERR
        CL_ERR_BUFFER_TOO_SMALL
        CL_ERR_FUNCTION_NOT_FOUND
        CL_ERR_INVALID_INDEX
*/
CLSERIALEXPORT CLINT32 CLSERIALCC
clGetManufacturerInfo(
    CLINT8* ManufacturerName,           /*!< A pointer to a user-allocated buffer into which the function
                                         copies the manufacturer name. The returned name is 
                                         NULL-terminated. In the case that the .dll conforms to the October 2000 spec, 
                                         this parameter will contain the file name of the .dll rather than 
                                         the manufacturer name. */
    CLUINT32* bufferSize,               /*!< As an input, this value should be the size of the buffer that 
                                         is passed. On successful return, this parameter contains the 
                                         number of bytes written into the buffer, including the NULL 
                                         termination character. On CL_ERR_BUFFER_TOO_SMALL, 
                                         this parameter contains the size of the buffer needed to write 
                                         the data text. */
    CLUINT32* version                 /*!< The version of the Camera Link Specifications with which this 
                                         .dll complies. */
    );

/*!
\brief This function initializes the device referred to by serialIndex and returns 
       a pointer to an internal serial reference structure.
\return 
    On completion, this function returns one of the following status codes:
    CL_ERR_NO_ERR
    CL_ERR_PORT_IN_USE
    CL_ERR_INVALID_INDEX
*/
CLSERIALEXPORT CLINT32 CLSERIALCC 
clSerialInit(
    CLUINT32 serialIndex,   //!< A zero-based index value. For n serial devices 
                            //!< in your system supported by this library, 
                            //!< serialIndex  has a range of 0 to(n-1). 
                            //!< The order of the serial devices is vendor specific.
    hSerRef* serialRefPtr   //!< Points to a value that contains, on a successful call, 
    );                      //!< a pointer to the vendor-specific reference to the current session.

/*!
\brief This function reads numBytes from the serial device referred to by serialRef.
       clSerialRead will return when numBytes are available at the serial port or 
       when the serialTimeout period has passed.  
       
       Upon success, numBytes are copied into buffer. 
       In the case of any error, including CL_ERR_TIMEOUT, no data is copied into buffer.

\return On completion, this function returns one of the following status codes:
    CL_ERR_NO_ERR
    CL_ERR_TIMEOUT
    CL_ERR_INVALID_REFERENCE
*/     
CLSERIALEXPORT CLINT32 CLSERIALCC
clSerialRead(
    hSerRef serialRef,  //!< The value obtained from the clSerialInit function.
    CLINT8* buffer,     //!< Points to a user-allocated buffer. Upon a successful call, 
                        //!< buffer contains the data read from the serial device.  
                        //!< Upon failure, this buffer is not affected.  
                        //!< Caller should ensure that buffer is at least numBytes in size.
    CLUINT32* bufferSize,   //!< This is the number of bytes requested by the caller.
    CLUINT32  serialTimeOut //!< Indicates the timeout in milliseconds.
    );

/*!
\brief  This function writes the data in the buffer to the serial device referenced by serialRef.
\return On completion, this function returns one of the following status codes:
    CL_ERR_NO_ERR
    CL_ERR_INVALID_REFERENCE
    CL_ERR_TIMEOUT
*/
CLSERIALEXPORT CLINT32 CLSERIALCC
clSerialWrite(
    hSerRef serialRef,  //!< The value obtained from the clSerialInit function.
    CLINT8* buffer,     //!< Contains data to write to the serial device.
    CLUINT32* bufferSize,  //!< Contains the buffer size indicating 
                         //!< the number of bytes to be written. 
                         //!< Upon a successful call, bufferSize contains the number of 
                         //!< bytes written to the serial device. 
    CLUINT32 serialTimeOut //!< Indicates the timeout in milliseconds.
    );

/*!
\brief This function closes the serial device and cleans up the resources associated with serialRef. 
       Upon return, serialRef is no longer usable.
*/
CLSERIALEXPORT void CLSERIALCC 
clSerialClose(
    hSerRef serialRef //!< The value obtained from the clSerialInit function for clean up.
    );


/*!
\brief This function returns the number of serial ports in your system from a specified manufacturer.
       Use this number to determine the number of times you should loop through the 
       clGetSerialPortIdentifier function. 
\return At completion, this function returns one of the following status codes:
    CL_ERR_NO_ERR
    CL_ERR_FUNCTION_NOT_FOUND
    CL_ERR_MANU_DOES_NOT_EXIST
*/
CLSERIALEXPORT CLINT32 CLSERIALCC 
clGetNumSerialPorts(
    CLUINT32* numSerialPorts    //!< The number of serial ports in your system 
    );                          //!< that you can access with the current .dll.

/*
\brief This function returns a manufacturer-specific identifier for each serial port in your system.
\return At completion, this function returns one of the following status codes:
    CL_ERR_NO_ERR
    CL_BUFFER_TOO_SMALL
    CL_ERR_FUNCTION_NOT_FOUND
    CL_ERR_INVALID_INDEX
*/
CLSERIALEXPORT CLINT32 CLSERIALCC 
clGetSerialPortIdentifier(
    CLUINT32 serialIndex,   //!< A zero-based index value. The valid range for serialIndex 
                            //!< is 0 to n-1, where n is the value of numSerialPorts, 
                            //!< as returned by clGetNumManuSerialPorts.
    CLINT8* PortID,     //!< Manufacturer-specific identifier for the serial port.  
                        //!< In the case that the manufacturer .dll conforms to the 
                        //!< October 2000 specification, on return this parameter will 
                        //!< be "Port #n" where n is a unique index for the port.
    CLUINT32 *bufferSize  //!< As an input, this value should be the size of the buffer that 
    );                    //!< is passed. On successful return, this parameter contains 
                        //!< the number of bytes written into the buffer, 
                        //!< including the NULL termination character. 
                        //!< On CL_ERR_BUFFER_TOO_SMALL, this parameter contains the size 
                        //!< of the buffer needed to write the data text.

/*!
\brief This function will output how many bytes have are available at the port specified by serialRef.
\return At completion, this function returns one of the following status codes:
    CL_ERR_NO_ERR
    CL_ERR_INVALID_REFERENCE
    CL_ERR_FUNCTION_NOT_FOUND
*/
CLSERIALEXPORT CLINT32 CLSERIALCC 
clGetNumBytesAvail( 
    hSerRef serialRef,  //!< The value obtained by the clSerialInit function.
    CLUINT32 *numBytes    //!< The number of bytes currently available to be read from the port. 
    );

/*!
\brief This function will discard any bytes that are available in the input buffer.
\return At completion, this function returns one of the following status codes:
    CL_ERR_NO_ERR
    CL_ERR_INVALID_REFERENCE
*/
CLSERIALEXPORT CLINT32 CLSERIALCC 
clFlushPort( 
    hSerRef serialRef //!< The value obtained by the clSerialInit function that describes the port to be flushed.
    );

/*!
\brief This function returns the valid baud rates of the current interface.
\return At completion, this function returns one of the following status codes:
    CL_ERR_NO_ERR
    CL_ERR_INVALID_REFERENCE
    CL_ERR_FUNCTION_NOT_FOUND
*/
CLSERIALEXPORT CLINT32 CLSERIALCC
clGetSupportedBaudRates(
    hSerRef serialRef,  //!< Points to a value that contains, on a successful call, 
                        //!< a pointer to the vendor-specific reference to the current session.
    CLUINT32 *baudRates //!< Bit-field that describes all supported baud rates of the serial port 
    );                  //!< as described by serialRefPtr. See Table B-2 of the Camera Link 
                        //!< documentation for more information on constants.

/*!
\brief This function sets the baud rate for the serial port of the selected device. 
       Use clGetSupportedBaudRate to determine supported baud rates.
\return On completion, this function returns one of the following status codes:
    CL_ERR_NO_ERR
    CL_ERR_INVALID_REFERENCE
    CL_ERR_BAUD_RATE_NOT_SUPPORTED
*/
CLSERIALEXPORT CLINT32 CLSERIALCC 
clSetBaudRate( 
    hSerRef serialRef,  //!< The value obtained from the clSerialInit function.
    CLUINT32 baudRate   //!< The baud rate you want to use. This parameter expects the 
    );                  //!< values represented by the CL_BAUDRATE constants in table B-2. 
                        //!< By default, the baud rate for serial communication is 9600.

/*!
\brief This function converts an error code to error text for display in a 
       dialog box or in a standard I/O window. 
    
    Note:   clGetErrorText first looks for the error code in CCTserial.dll. 
    If the error code is not found in CCTserial.dll, it is not a standard Camera Link error. 
    clGetErrorText then passes the error code to the manufacturer-specific .dll, 
    which returns the manufacturer-specific error text.
\return On completion, this function returns one of the following status codes:
    CL_ERR_NO_ERR
    CL_ERR_BUFFER_TOO_SMALL
    CL_ERR_ERROR_NOT_FOUND
*/
CLSERIALEXPORT CLINT32 CLSERIALCC 
clGetErrorText( 
    CLINT32 errorCode,  //!< The error code used to find the appropriate error text. 
                        //!< An error code is returned by every function in this library.
    CLINT8* errorText,  //!< A caller-allocated buffer which contains the 
                        //!< NULL-terminated error text on function return.
    CLUINT32* errorTextSize //!< On success, contains the number of bytes written 
    );                      //!< into the buffer, including the NULL-termination character. 
                            //!< This value should be the size in bytes of the error text 
                            //!< buffer passed in. On CL_ERR__BUFFER_TOO_SMALL, 
                            //!< contains the size of the buffer needed to write the error text.


/*@}*/ // CLSerialInterface
/*@}*/ // PublicInterfaces


//==============================================================================
//  helper functions
//==============================================================================


#endif // CLPROTOCOL_CLSERIAL_H

