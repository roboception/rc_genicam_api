////////////////////////////////////////////////////////////////////////////////
//
//  Title     : clallserial.h
//  Created   : 12/03/2003 @ 14:27:00
//
//  Copyright (c) 2004. National Instruments Corporation
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted, provided that each of the following 
//  conditions are met. By using the software in any manner, you agree
//  to each of the following:
//    - All redistributions of the software must be accompanied with the
//      above copyright notice (provided however that for redistributions 
//      in binary form, the copyright notice may be omitted), the above 
//      preamble, this list of conditions, and the disclaimer set forth below.
//    - Except for the copyright notice required above, neither the name 
//      or trademarks of National Instruments Corporation (NI) nor the names
//      of its contributors may be used in any manner (including, but not
//      limited to, using the same to endorse or promote products derived from 
//      this software) without the specific prior written permission of NI.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" WITHOUT WARRANTY OF ANY KIND. NO WARRANTIES, EITHER EXPRESS
//  OR IMPLIED, ARE MADE WITH RESPECT TO THE SOFTWARE, INCLUDING, BUT 
//  NOT LIMITED TO, ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS 
//  FOR A PARTICULAR PURPOSE, TITLE OR NON-INFRINGEMENT, OR ANY OTHER 
//  WARRANTIES THAT MAY ARISE FROM USAGE OF TRADE OR COURSE OF DEALING. 
//  THE COPYRIGHT HOLDERS AND CONTRIBUTORS DO NOT WARRANT, GUARANTEE, OR 
//  MAKE ANY REPRESENTATIONS REGARDING THE USE OF OR THE RESULTS OF THE 
//  USE OF THE SOFTWARE IN TERMS OF CORRECTNESS, ACCURACY, RELIABILITY, 
//  OR OTHERWISE AND DO NOT WARRANT THAT THE OPERATION OF THE SOFTWARE 
//  WILL BE UNINTERRUPTED OR ERROR FREE.  THE ENTIRE RISK AS TO THE 
//  PERFORMANCE OF THE SOFTWARE IS WITH YOU. IN NO EVENT SHALL THE 
//  COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
//  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
//  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS 
//  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
//  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
//  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF 
//  THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF ADVISED OF THE 
//  POSSIBILITY OF SUCH DAMAGE.
//
//////////////////////////////////////////////////////////////////////////////
#ifndef CLPROTOCOL_CLALLSERIAL_H
#define CLPROTOCOL_CLALLSERIAL_H

#include <CLProtocol/ClSerialTypes.h>

//==============================================================================
//  Macros 
//==============================================================================

#if (defined(__cplusplus) || defined(__cplusplus__))
    #define USER_EXTERN_C extern "C"
#else
    #define USER_EXTERN_C
#endif

#ifdef CLALLSERIAL_EXPORTS
    #if defined(_WIN32)
        #define CLALLSERIALEXPORT USER_EXTERN_C __declspec(dllexport)
    #else
        #define CLALLSERIALEXPORT USER_EXTERN_C __attribute__((visibility("default")))
    #endif
#else
    #if defined(_WIN32)
        #define CLALLSERIALEXPORT USER_EXTERN_C __declspec(dllimport)
    #else
        #define CLALLSERIALEXPORT USER_EXTERN_C __attribute__((visibility("default")))
    #endif
#endif

#ifndef CLALLSERIALCC
    #if defined(_WIN32)
        // Note that using __stdcall changes the name decoration as compared with __cdecl
        // For details see http://www.willus.com/mingw/yongweiwu_stdcall.html
        #define CLALLSERIALCC __stdcall
    #else
        #if !defined(__x86_64) && !defined(__arm__) && !defined(__aarch64__) && !defined(VXWORKS)
            #define  CLALLSERIALCC __attribute__((stdcall))
        #else
            #define  CLALLSERIALCC  /* use default calling convention */
        #endif
    #endif
#endif

//==============================================================================
// Exported functions 
//==============================================================================
////////////////////////////////////////////////////////////////////////////////
//
//  clFlushPort
//
//  Description: Discards any bytes that are available in the input 
//  buffer.
//      
//  Parameters:
//      serialRef       input parameter - The value obtained by the clSerialInit function 
//                      that describes the port to be flushed.
//
//  Return Value:       CL_ERR_NO_ERR
//                      CL_ERR_INVALID_REFERENCE
//
////////////////////////////////////////////////////////////////////////////////
CLALLSERIALEXPORT CLINT32 CLALLSERIALCC clFlushPort(hSerRef serialRef);


////////////////////////////////////////////////////////////////////////////////
//
//  clGetErrorText
//
//  Description:
//  Converts an error code to error text which you can
//  display in a dialog box or in the standard I/O window.  
//  Note: This function first looks up the error code in clserial.dll to determine
//  whether it is a standard Camera Link error. If it is a non-standard error, this function 
//  passes the error code to the manufacturer-specific DLL, which returns the
//  manufacturer-specific error code.
//
//  Parameters:
//      manuName            input parameter - The manufacturer name in a NULL-terminated
//                          buffer.  Manufacturer name is returned from clGetPortInfo.
//      errorCode           input parameter - The error code used to look up the 
//                          appropriate error text.  This code can be returned 
//                          from any function in this library.
//      errorText           output parameter - A caller-allocated buffer which will contain
//                          a NULL-terminated error description on return.
//      errorTextSize       input/output parameter - As an input, this value is the size,
//                          in bytes, of the errorText buffer that is passed in. On
//                          success, this value is the number of bytes 
//                          that have been written into the buffer, including the NULL
//                          termination character. On CL_ERR_BUFFER_TOO_SMALL error,
//                          this value is the size of the buffer required to write
//                          the data text.
//
//  Return Value:           CL_ERR_NO_ERR
//                          CL_ERR_MANU_DOES_NOT_EXIST
//                          CL_ERR_BUFFER_TOO_SMALL
//                          CL_ERR_ERROR_NOT_FOUND
//      
/////////////////////////////////////////////////////////////////////////////////
CLALLSERIALEXPORT CLINT32 CLALLSERIALCC clGetErrorText(const CLINT8* manuName, CLINT32 errorCode, CLINT8* errorText, CLUINT32* errorTextSize);


////////////////////////////////////////////////////////////////////////////////
//
//  clGetNumPorts 
//
//  Description: Returns the number of Camera Link serial ports in 
//  this machine that are supported by clallserial.dll. 
//      
//  Parameters:
//      numPorts        output parameter - The number of Camera Link serial ports 
//                      in this machine supported by this DLL.
//
//  Return Value:       CL_ERR_NO_ERR
//
////////////////////////////////////////////////////////////////////////////////
CLALLSERIALEXPORT CLINT32 CLALLSERIALCC clGetNumPorts(CLUINT32* Ports); 


////////////////////////////////////////////////////////////////////////////////
//
//  clGetNumBytesAvail 
//
//  Description: Outputs the number of bytes that are received, but not yet read out, at 
//  the port specified by serialRef.
//      
//  Parameters:
//      serialRef       input - The value obtained by the clSerialInit function.
//      numBytes        output - The number of bytes currently available to be 
//                      read from the port.
//
//  Return Value:       CL_ERR_NO_ERR
//                      CL_ERR_INVALID_REFERENCE
//
////////////////////////////////////////////////////////////////////////////////
CLALLSERIALEXPORT CLINT32 CLALLSERIALCC clGetNumBytesAvail(hSerRef serialRef, CLUINT32* numBytes);


////////////////////////////////////////////////////////////////////////////////
//
//  clGetPortInfo
//
//  Description: This function provides information about the port specified by 
//  the index.      
//
//  Parameters:
//      serialIndex       input parameter - Zero-based index of the serial port you are   
//                        finding the name for. Use clGetNumSerialPorts to determine
//                        the valid range of serialIndex. This range will be 0 to 
//                        numSerialPorts-1. 
//      manufacturerName  output parameter - Pointer to a user-allocated buffer into 
//                        which the function copies the manufacturer name.  The returned
//                        name is NULL-terminated.  In the case that the DLL conforms to the 
//                        version 1.0 Camera Link Specification, this 
//                        parameter will contain the file name of the DLL rather than the 
//                        manufacturer name.
//      nameBytes         input/output parameter - As an input parameter, 
//                        this value is the size of the name buffer, including
//                        the NULL termination. As an output parameter, this value
//                        is the number of bytes written into the name buffer.
//                        If the provided name buffer is not large enough, this value
//                        is the number of required bytes.
//      portID            output parameter - The identifier for the port. For now, this 
//                        value is the interface name. In the future, there may be an 
//                        extension for multiple serial ports on one board.
//      IDBytes           input/output parameter. As an input parameter, this value is
//                        the size of the portID buffer, including
//                        the NULL termination. As an output parameter, this value is
//                        the number of bytes written into the portID buffer.
//                        If the provided portID buffer is not large enough, this value is
//                        the number of required bytes.
//      Version           The version of the Camera Link specifications with which this frame
//                        grabber software complies.
//
//  Return Value:         CL_ERR_NO_ERR
//                        CL_ERR_BUFFER_TOO_SMALL
//                        CL_ERR_INVALID_INDEX//
////////////////////////////////////////////////////////////////////////////////
CLALLSERIALEXPORT CLINT32 CLALLSERIALCC clGetPortInfo(CLUINT32 serialIndex, CLINT8* manufacturerName, CLUINT32* nameBytes, CLINT8* portID, CLUINT32* IDBytes, CLUINT32* version);


////////////////////////////////////////////////////////////////////////////////
//
//  clGetSupportedBaudRates
//
//  Description:    Returns the valid baud rates of the interface.
//
//  Parameters:
//      serialRefPtr    input parameter - The value obtained from 
//                      clSerialInit function.
//      baudRates       output parameter -  Indicates which baud
//                      rates are supported by this serial interface. This 
//                      is represented as a bit-field. Refer to the CL_BAUDRATE_
//                      constants above for the supported baud rates.
//
//  Return Value:       CL_ERR_NO_ERR
//                      CL_ERR_INVALID_REFERENCE
//                      CL_ERR_FUNCTION_NOT_FOUND   
//      
/////////////////////////////////////////////////////////////////////////////////
CLALLSERIALEXPORT CLINT32 CLALLSERIALCC clGetSupportedBaudRates(hSerRef serialRef, CLUINT32* baudRates);


//////////////////////////////////////////////////////////////////////////////////
//
//  clSerialClose
//
//  Description:    Closes the serial device and cleans up resources associated
//  with serialRef. Upon return, serialRef is no longer usable.
//
//  Parameters:
//    serialRef        input parameter - The value obtained from clSerialInit 
//                     function to clean up.
//                   
///////////////////////////////////////////////////////////////////////////////////
CLALLSERIALEXPORT void CLALLSERIALCC clSerialClose(hSerRef serialRef);


////////////////////////////////////////////////////////////////////////////////
//
//  clSerialInit
//
//  Description:
//  Initializes the device referred to by serialIndex, and
//  returns a pointer to an internal serial reference structure.    
//
//  Parameters:
//      serialIndex     input parameter - Zero-based index value. If there are n
//                      serial devices in the system that is supported by this
//                      library, the range of serialIndex is 0 to (n-1).
//                      The order of the serial devices is vendor-specific. The 
//                      number of serial ports supported by this DLL is
//                      output by the clGetNumSerialPorts function.
//      serialRefPtr    output parameter - Upon a successful call, a pointer to the 
//                      vendor-specific reference to the current serial session 
//                      will be put into the value pointed to by serialRefPtr.
//
//  Return Value:           CL_ERR_NO_ERR
//                          CL_ERR_PORT_IN_USE
//                          CL_ERR_INVALID_INDEX
//                          CL_ERR_UNABLE_TO_OPEN_PORT
//      
////////////////////////////////////////////////////////////////////////////////
CLALLSERIALEXPORT CLINT32 CLALLSERIALCC clSerialInit(CLUINT32 serialIndex, hSerRef* serialRefPtr);


////////////////////////////////////////////////////////////////////////////////
//
//  clSerialRead
//
//  Description: Reads from the serial device referenced by serialRef.
//
//               Note: this function has been deprecated and will not be available
//               in Camera Link v3.0. Use the new function clSerialReadEx instead
//
//  Parameters:
//    serialRef      input parameter - The value obtained from clSerialInit function
//    buffer         output parameter - Points to a user-allocated buffer.
//                   Upon a successful call, contains the data
//                   read from the serial device.
//    bufferSize     input-output parameter - As an input parameter, bufferSize contains the 
//                   buffer size to indicate the maximum number of bytes that
//                   the buffer can accommodate.
//                   Upon a successful call, bufferSize is the
//                   number of bytes that were read successfully from the
//                   serial device.
//    serialTimeout  input parameter - Indicates the timeout, in milliseconds
//
//  Return value     CL_ERR_NO_ERR
//                   CL_ERR_TIMEOUT
//                   CL_ERR_INVALID_REFERENCE
//   
////////////////////////////////////////////////////////////////////////////////
CLALLSERIALEXPORT CLINT32 CLALLSERIALCC clSerialRead(hSerRef serialRef, CLINT8* buffer, CLUINT32* bufferSize, CLUINT32 serialTimeout);


////////////////////////////////////////////////////////////////////////////////
//
//  clSerialReadEx
//
//  Description: Reads from the serial device referenced by serialRef.
//
//  Parameters:
//    serialRef      input parameter - The value obtained from clSerialInit function
//    buffer         output parameter - Points to a user-allocated buffer.
//                   Upon a successful call, contains the data
//                   read from the serial device.
//    numBytes       input-output parameter - As an input parameter, numBytes contains the 
//                   buffer size, to indicate the maximum number of bytes that
//                   the buffer can accommodate.
//                   Upon a successful call (CL_ERR_NO_ERR or CL_ERR_TIMEOUT), numBytes
//                   is the number of bytes that were read successfully from the
//                   serial device.
//    serialTimeout  input parameter - Indicates the timeout in milliseconds. This
//                   parameter may be set to 0.
//
//  Return value     CL_ERR_NO_ERR
//                   CL_ERR_TIMEOUT
//                   CL_ERR_INVALID_REFERENCE
//   
////////////////////////////////////////////////////////////////////////////////
CLALLSERIALEXPORT CLINT32 CLALLSERIALCC clSerialReadEx(hSerRef serialRef, CLINT8* buffer, CLUINT32* numBytes, CLUINT32 serialTimeout);


////////////////////////////////////////////////////////////////////////////////
//
//  clSerialWrite
//
//  Description: This function writes data in buffer to the serial device 
//  referenced by serialRef.
//
//  Parameters:
//    serialRef      input parameter - The value obtained from clSerialInit function
//    buffer         input parameter - Contains data to write to the serial port
//    bufferSize     input/output parameter - As an input parameter, bufferSize contains the 
//                   number of bytes of data in the buffer to write to the
//                   serial device. Upon a successful call, bufferSize contains the
//                   number of bytes that was written successfully to the
//                   serial device.
//    serialTimeout  input parameter - Indicates the timeout, in milliseconds
//
//  Return value:    CL_ERR_NO_ERR
//                   CL_ERR_INVALID_REFERENCE
//                   CL_ERR_TIMEOUT
/////////////////////////////////////////////////////////////////////////////////
CLALLSERIALEXPORT CLINT32 CLALLSERIALCC clSerialWrite(hSerRef serialRef, CLINT8* buffer, CLUINT32* bufferSize, CLUINT32 serialTimeout);


////////////////////////////////////////////////////////////////////////////////
//
//  clSetBaudRate
//
//  Description:     Sets the Baud Rate for the serial port of the
//                   selected port. Use clGetSupportedBaudRates to determine
//                   supported baud rates.
//      
//  Parameters:
//      serialRef    input parameter - The value obtained from 
//                   clSerialInit function.
//      baudRate     input parameter - The baud rate you want to use. This
//                   input expects the values represented by the CL_BAUDRATE
//                   constants.
//
//  Return Value:    CL_ERR_NO_ERR
//                   CL_ERR_INVALID_REFERENCE
//                   CL_ERR_BAUD_RATE_NOT_SUPPORTED
//
////////////////////////////////////////////////////////////////////////////////
CLALLSERIALEXPORT CLINT32 CLALLSERIALCC clSetBaudRate(hSerRef serialRef, CLUINT32 baudRate);

#endif // CLPROTOCOL_CLALLSERIAL_H
