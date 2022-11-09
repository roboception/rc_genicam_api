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
#ifndef CLPROTOCOL_CLPROTOCOL_H
#define CLPROTOCOL_CLPROTOCOL_H

#include <CLProtocol/ClSerialTypes.h>

#include <stdarg.h>

//==============================================================================
//  Macros 
//==============================================================================

#if (defined(__cplusplus) || defined(__cplusplus__))
    #define USER_EXTERN_C extern "C"
    #define USER_EXTERN_C_START extern "C" {
    #define USER_EXTERN_C_END }
#else
    #define USER_EXTERN_C
    #define USER_EXTERN_C_START
    #define USER_EXTERN_C_END
#endif

#ifdef CLPROTOCOL_EXPORTS
    #if defined(_WIN32)
        #define CLPROTOCOLEXPORT USER_EXTERN_C __declspec(dllexport)
        #define CLPROTOCOL_DECL __declspec(dllexport)
    #else
        #define CLPROTOCOLEXPORT USER_EXTERN_C __attribute__((visibility("default")))
        #define CLPROTOCOL_DECL __attribute__((visibility("default")))
    #endif
#else
    #if defined(_WIN32)
        #define CLPROTOCOLEXPORT USER_EXTERN_C __declspec(dllimport)
        #define CLPROTOCOL_DECL __declspec(dllimport)
    #else
        #define CLPROTOCOLEXPORT USER_EXTERN_C __attribute__((visibility("default")))
        #define CLPROTOCOL_DECL __attribute__((visibility("default")))
    #endif
#endif

#ifndef CLPROTOCOL
    #if defined(_WIN32)
        #define CLPROTOCOL __cdecl
    #else
        #if !defined(__x86_64) && !defined(__arm__) && !defined(__aarch64__) && !defined(VXWORKS)
            #define  CLPROTOCOL __attribute__((cdecl))
        #else
            #define  CLPROTOCOL  /* use default calling convention */
        #endif
    #endif
#endif

//==============================================================================
//  Types and Enums
//==============================================================================

typedef void (CLPROTOCOL *clp_logger_t)( CLINT32 level, const char* stringFormat, va_list argptr);
USER_EXTERN_C_START
    typedef struct clpLogCallback_t
    {
        CLUINT32 id;           //! Must be set to 0xeb6e1ca9
        clp_logger_t logger;   //! The address of the logging function
    }clpLogCallback_t;
USER_EXTERN_C_END

//! Types of parameters to be accessed by clpGetParam/clpSetParam
enum CLP_PARAMS
{
// Global parameters that do not use the Cookie when called.
    CLP_LOG_LEVEL            = 1,   //! A 32-bit value that contains the current log level.
                                    //! This parameter expects a value represented by the 
                                    //! CLP_LOG_LEVEL_VALUE constants.

    CLP_LOG_CALLBACK         = 2,   //! A 32-bit value that contains a clpLogCallback_t pointer to the logger function.

    CLP_STOP_PROBE_DEVICE    = 3,   //! A 32-bit value that contains a signal to stop all running ProbeDevice.

    // Device related parameters that require a valid cookie to execute properly.
    CLP_DEVICE_BAUDERATE     = -1,  //! A 32-bit value that contains the baudrate used to communicate with the device. 
                                    //! This parameter expects a value represented by the 
                                    //! CL_BAUDRATE_XXX constants.
                                    
    CLP_DEVICE_SUPPORTED_BAUDERATES = -2  //! A 32-bit value that contains the baudrates
                                    //! supported by the device and the host. 
                                    //! This read only parameter return one or more value(s) represented by the 
                                    //! CL_BAUDRATE_XXX constants.
};

enum CLP_LOG_LEVEL_VALUE
{ 
    CLP_LOG_FATAL  = 0,
    CLP_LOG_ALERT  = 100,
    CLP_LOG_CRIT   = 200,
    CLP_LOG_ERROR  = 300, 
    CLP_LOG_WARN   = 400,
    CLP_LOG_NOTICE = 500,
    CLP_LOG_INFO   = 600,
    CLP_LOG_DEBUG  = 700,
    CLP_LOG_NOTSET = 800
};

//==============================================================================
//  Exported Functions
//==============================================================================

class ISerial;

/*! \ingroup PublicInterfaces */
/*@{*/

/*! \defgroup CLProtocolInterface CLProtocol C-Function Interface */
/*@{*/

/*!
\brief This function is call to initialize the library after it is loaded.

\param[in] logger A function pointer that contains a logging function.
\param[in] logLevel The log level default value. This argument expects a value represented 
               by the CLP_LOG_LEVEL_VALUE constants.

\return On completion, this function returns one of the following status codes:
\retval CL_ERR_NO_ERR No error occurred
\return CL_ERR_INVALID_PTR The initialisation parameters are invalid. 
\return CL_ERR_IN_USE The library is already open
*/
CLPROTOCOLEXPORT CLINT32 CLPROTOCOL 
clpInitLib( clp_logger_t logger, 
            CLP_LOG_LEVEL_VALUE logLevel);

/*!
\brief This function is call to close the library before unloading.

\return On completion, this function returns one of the following status codes:
\retval CL_ERR_NO_ERR No error occurred
*/
CLPROTOCOLEXPORT CLINT32 CLPROTOCOL 
clpCloseLib( void );

/*!
\brief This function returns a list of short device templates coded into a single string

       Short device templates can have the following form:
       - <CODE>"CameraManufacturer"</CODE>
       - <CODE>"CameraManufacturer#CameraFamily"</CODE>
       - <CODE>"CameraManufacturer#CameraFamily#CameraModel"</CODE>
       
       As compared to the long version the protocol driver dll's directory and file name is missing at the beginning.

       The device templates are separated by tabulators.
       The whole list is contained in a NULL terminated string.

\param[out] pShortDeviceTemplates A pointer to a user-allocated buffer into which the function 
       copies the list of device template names. 

\param[in,out] pBufferSize As an input, the value contains the size of the buffer that 
       is passed through the pShortDeviceTemplates parameter. 
       On successful return, this parameter contains the number of bytes written into the buffer, 
       including the NULL termination character. 
       On CL_ERR_BUFFER_TOO_SMALL, this parameter contains the size of the buffer needed to write 
       the data text.

\return At completion, this function returns one of the following status codes:
\retval CL_ERR_NO_ERR No error occurred
\retval CL_ERR_BUFFER_TOO_SMALL The pShortDeviceTemplates is too small
*/
CLPROTOCOLEXPORT CLINT32 CLPROTOCOL clpGetShortDeviceIDTemplates ( CLINT8* pShortDeviceTemplates, CLUINT32* pBufferSize );

/*!
\brief Connects and identifies the device and returns a DeviceID

    This function connects to a device attached to the serial port and attempts to identify it using the DeviceID template as hint. 
    If the function is successful it returns a DeviceID as well as a Cookie. The DeviceID has the form:

        <CODE>"CameraManufacturer#CameraFamily#CameraModel#Version.2.3.4#SerialNumber"</CODE>

    The connection is made with 9600 baud which is the power up baud rate as defined by the CameraLink standard.

    This function attempts to re-connect the device if a DeviceID is passed instead of a template as pDeviceIDTemplate.


\param[in] pSerial Pointer to the interface giving access to the serial port

\param[in] pDeviceIDTemplate A pointer to NULL-terminated string containing a device ID template 
    (in the probe use case) or a device ID (in the connect use case)

\param[in, out] pDeviceID A pointer to a user-allocated buffer. If DeviceIDTemplate is valid
    and a device is found and identified the DLL copies a device ID to the buffer. 

\param[in,out] pBufferSize As an input, the value contains the size of the buffer that 
    is passed through the pDeviceID parameter. 
    On successful return, this parameter contains the number of bytes written into the buffer, 
    including the NULL termination character. 
    On CL_ERR_BUFFER_TOO_SMALL, this parameter contains the size of the buffer needed to write 
    the data text.

\param[out] pCookie A token supplied by the driver dll which must be handed in by all functions
    called later to access the device. The dll's implementation can use this token to 
    cache information about the connected device thus avoiding to have to parse the deviceID again.
    Note that the cookie value must not be 0.

\param[in] TimeOut A timeout value in [ms] used during accessing the device. 
    Note that the execution of the function itself can take longer than TimeOut because more than one
    access might be necessary. 

\return At completion, this function returns one of the following status codes:
\retval CL_ERR_NO_ERR No error occurred
\retval CL_BUFFER_TOO_SMALL The pDeviceID buffer is too small
\retval CL_ERR_INVALID_DEVICEID The DeviceID is not recognized as valid by this dll
\retval CL_ERR_NO_DEVICE_FOUND There was no device found matching the DeviceID
*/
CLPROTOCOLEXPORT CLINT32 CLPROTOCOL clpProbeDevice( ISerial *pSerial, const CLINT8* pDeviceIDTemplate, CLINT8* pDeviceID, CLUINT32 *pBufferSize, CLUINT32 *pCookie, const CLUINT32 TimeOut );


/*!
\brief This function returns a list of XML IDs coded into a single string

XML IDs have the following form: 

<CODE>"SchemaVersion.1.1@CameraManufacturer#CameraFamily#CameraModel@XMLVersion.2.3.4"</CODE>

The first part of the XML ID denotes the major and minor version number of the XML schema. 
The last part of the XML ID denotes the major, minor, and subminor version number of the camera. 
Both version numbers mut be the same as given in the attributes of the RegisterDescription node 
of the corrensponding XML file.
The middle part of the XML ID must be a complete DeviceID.

The device templates are separated by tabulators.
The whole list is contained in a NULL terminated string.

\param[in] pSerial Pointer to the interface giving access to the serial port

\param[in] Cookie Token supplied by the driver dll via the clpProbeDevice function. 

\param[out] pXMLIDs A pointer to a user-allocated buffer into which the function 
    copies the list of XML IDs. The list may be empty in which case is contains of a '0' only.

\param[in,out] pBufferSize As an input, the value contains the size of the buffer that 
    is passed through the pXMLIDs parameter. 
    On successful return, this parameter contains the number of bytes written into the buffer, 
    including the NULL termination character. 
    On CL_ERR_BUFFER_TOO_SMALL, this parameter contains the size of the buffer needed to write 
    the data text.

\param[in] TimeOut A timeout value in [ms] used during accessing the device. 
    Note that the execution of the function itself can take longer than TimeOut because more than one
    access might be necessary. 

\return At completion, this function returns one of the following status codes:
\retval CL_ERR_NO_ERR No error occurred
\retval CL_ERR_BUFFER_TOO_SMALL The pXMLIDs buffer is too small
\retval CL_ERR_TIMEOUT A timeout occurred
\retval CL_ERR_INVALID_COOKIE The cookie supplied is invalid
*/
CLPROTOCOLEXPORT CLINT32 CLPROTOCOL clpGetXMLIDs ( ISerial *pSerial, const CLUINT32 Cookie, CLINT8* pXMLIDs, CLUINT32* pBufferSize, const CLUINT32 TimeOut );

/*!
\brief This function retrieves an XML file from the camera if possible

The DLL may ask the camera for the XML code, or may have the code as a resource compiled in 
itself or even may have it's own private registry mechanism installed.
In order to support the latter use case the long DeviceID is supplied which contains
the driver dll's directory.

\param[in] pSerial Pointer to the interface giving access to the serial port

\param[in] Cookie Token supplied by the driver dll via the clpProbeDevice function. 

\param[in] pXMLID A pointer to a NULL terminated string containing the connected device's XML ID 

\param[out] pXMLBuffer A pointer to a user-allocated buffer. Can be NULL if only the size of the 
    buffer is requested (see pBufferSize)

\param[in,out] pBufferSize pBufferSize As an input, the value contains the size of the buffer that 
    is passed through the pDeviceID parameter. 
    On successful return, this parameter contains the number of bytes written into the buffer, 
    including the NULL termination character. 
    On CL_ERR_BUFFER_TOO_SMALL, this parameter contains the size of the buffer needed to write 
    the data text.

\param[in] TimeOut A timeout value in [ms] used during accessing the device. 
    Note that the execution of the function itself can take longer than TimeOut because more than one
    access might be necessary. 

\return At completion, this function returns one of the following status codes:
\retval CL_ERR_NO_ERR No error occurred
\retval CL_ERR_BUFFER_TOO_SMALL The pDeviceID buffer is too small
\retval CL_ERR_NO_XMLDESCRIPTION_FOUND No XML description could be retrieved for the given XML ID
\retval CL_ERR_TIMEOUT A timeout occurred
\retval CL_ERR_INVALID_COOKIE The cookie supplied is invalid
*/
CLPROTOCOLEXPORT CLINT32 CLPROTOCOL 
clpGetXMLDescription( ISerial *pSerial, const CLUINT32 Cookie, const CLINT8* pXMLID, CLINT8* pXMLBuffer, CLUINT32* pBufferSize, const CLUINT32 TimeOut );

/*!
\brief This function reads numBytes from the serial device referred to by serialRef.
       clSerialRead will return when numBytes are available at the serial port or 
       when the serialTimeout period has passed.  
       
       Upon success, numBytes are copied into buffer. 
       In the case of any error, including CL_ERR_TIMEOUT, no data is copied into buffer.

\param[in] pSerial The interface giving access to the serial port

\param[in] Cookie Token supplied by the driver dll via the clpProbeDevice function. 

\param[in] Address Address the data is to be read from 

\param[out] pBuffer Points to a user-allocated buffer. Upon a successful call, 
    buffer contains the data read from the serial device.  
    Caller should ensure that buffer is at least numBytes in size.

\param[in] BufferSize This is the number of bytes requested by the caller.

\param[in] TimeOut A timeout value in [ms] used during accessing the device.
    Note that there may be specific commands which may take longer than TimeOut to be 
    accessed. Since clpReadRegister is missing a CL_ERR_PENDING_WRITE mechanism the
    CLProcotol DLL may be forced for these commands to delay the return for more than TimeOut.

\return On completion, this function returns one of the following status codes:
\retval CL_ERR_NO_ERR No error occurred
\retval CL_ERR_TIMEOUT A timeout occurred
\retval CL_ERR_INVALID_COOKIE The cookie supplied is invalid
*/     
CLPROTOCOLEXPORT CLINT32 CLPROTOCOL
clpReadRegister(ISerial *pSerial, const CLUINT32 Cookie, const CLINT64 Address, CLINT8* pBuffer, const CLINT64 BufferSize, const CLUINT32  TimeOut );

/*!
\brief This function writes numBytes to the serial device referred to by serialRef.
clSerialRead will return when numBytes are available at the serial port or 
when the serialTimeout period has passed.  

Upon success, numBytes are copied into buffer. 
In the case of any error, including CL_ERR_TIMEOUT, no data is copied into buffer.

\param[in] pSerial The interface giving access to the serial port

\param[in] Cookie ManToken supplied by the driver dll via the clpProbeDevice function. 

\param[in] Address Address the data is to be read from 

\param[in] pBuffer Points to a containing the data to be written
    Caller should ensure that buffer is at least BufferSize in size.

\param[in] BufferSize This is the number of bytes to be written.

\param[in] TimeOut A timeout value in [ms] used during accessing the device. 
    If the CLProtocol DLL knowns for specific registers that accessing them will take longer
    than TiemOut for a good reason, e.g., because some re-calibration is triggered, it can 
    use the \ref CL_ERR_PENDING_WRITE mechanism to inform the client abou this and prolong
    the waiting time.

\return On completion, this function returns one of the following status codes:
\retval CL_ERR_NO_ERR No error occurred
\retval CL_ERR_TIMEOUT A timeout occurred
\retval CL_ERR_INVALID_COOKIE The cookie supplied is invalid
\retval CL_ERR_PENDING_WRITE The timeout has expired but the write is still pending for a good reason. 
    The next call must be \ref clpContinueWriteRegister
*/     
CLPROTOCOLEXPORT CLINT32 CLPROTOCOL
clpWriteRegister(ISerial *pSerial, const CLUINT32 Cookie, const CLINT64 Address, const CLINT8* pBuffer, const CLINT64 BufferSize, const CLUINT32  TimeOut );

/*!
\brief Calling This function is called after a write function returned CL_ERR_PENDING_WRITE. It either issues another wait for write cycle or cancels the write action 

This function must be called if clpWriteRegister or clpContinueWriteRegister returns CL_ERR_PENDING_WRITE.

Note, that the CLProtocol.dll is responsible for preventing an endless number of calls to this
function by tracking the total time elapsed since the original call and returning CL_ERR_TIMEOUT
if this time is larger than the write operation should normally take place.

\param[in] pSerial The interface giving access to the serial port

\param[in] Cookie ManToken supplied by the driver dll via the clpProbeDevice function. 

\param[in] ContinueWaiting If true the function will block waiting for the previous call to clpWriteRegister to finish. If false the call will be cancelled.

\param[in] TimeOut A timeout value in [ms] used during accessing the device. 

\return On completion, this function returns one of the following status codes:
\retval CL_ERR_NO_ERR No error occurred
\retval CL_ERR_TIMEOUT A timeout occurred
\retval CL_ERR_INVALID_COOKIE The cookie supplied is invalid
\retval CL_ERR_PENDING_WRITE The timeout has expired but the write is still pending for a good reason. The next call must be \ref clpContinueWriteRegister
*/     
CLPROTOCOLEXPORT CLINT32 CLPROTOCOL
clpContinueWriteRegister(ISerial *pSerial, const CLUINT32 Cookie, const BOOL8 ContinueWaiting, const CLUINT32  TimeOut );

/*!
\brief This function converts an error code to error text for display in a 
       dialog box or in a standard I/O window. 
    
    Note:   clGetErrorText first looks for the error code in CCTserial.dll. 
    If the error code is not found in CCTserial.dll, it is not a standard Camera Link error. 
    clGetErrorText then passes the error code to the manufacturer-specific .dll, 
    which returns the manufacturer-specific error text.

\param[in] errorCode The error code used to find the appropriate error text. 
    An error code is returned by every function in this library.

\param[out] errorText A caller-allocated buffer which contains the 
    NULL-terminated error text on function return.

\param[in,out] errorTextSize On success, contains the number of bytes written 
    into the buffer, including the NULL-termination character. 
    This value should be the size in bytes of the error text 
    buffer passed in. On CL_ERR__BUFFER_TOO_SMALL, 
    contains the size of the buffer needed to write the error text.

\param[in] Cookie Used only for CL_ERR_GET_LAST_ERROR. Returns the last error
    fired on the connection identified by the Cookie

\return On completion, this function returns one of the following status codes:
\retval CL_ERR_NO_ERR No error occurred
\retval CL_ERR_BUFFER_TOO_SMALL The pDeviceID buffer is too small
\retval CL_ERR_ERROR_NOT_FOUND No error string could be retrieved for the given error number 
*/
CLPROTOCOLEXPORT CLINT32 CLPROTOCOL 
clpGetErrorText( CLINT32 errorCode, CLINT8* errorText, CLUINT32* errorTextSize, const CLUINT32 Cookie = 0);

/*!
\brief This function disconnects the DLL from the device

When this function is called all resources allocated in the driver DLL and associated 
with the Cookie are deleted and the Cookie becomes invalid.

\param[in] Cookie ManToken supplied by the driver dll via the clpProbeDevice function. 

\return On completion, this function returns one of the following status codes:
\retval CL_ERR_NO_ERR No error occurred
\retval CL_ERR_INVALID_COOKIE The cookie supplied is invalid
*/
CLPROTOCOLEXPORT CLINT32 CLPROTOCOL 
clpDisconnect( const CLUINT32 Cookie );

/*!
\brief This function returns the version of the CLProtocol interface version

The function is required to ensure this interface can be extended.

\param[out] pVersionMajor If this parameter changes backward compatibility is broken.
\param[out] pVersionMinor If this parameter changes backward compatibility is maintained.

\return On completion, this function returns one of the following status codes:
\retval CL_ERR_NO_ERR No error occurred
*/
CLPROTOCOLEXPORT CLINT32 CLPROTOCOL 
clpGetCLProtocolVersion( CLUINT32 *pVersionMajor, CLUINT32 *pVersionMinor );

/*!
\brief This function is call to read a CLProtocol parameter value.

\param[in] param The parameter to read.
\param[in] Cookie Token supplied by the driver dll via the clpProbeDevice function. 
    Some parameters ignore the Cookie.
\param[in] pBuffer Points to a user-allocated buffer. Upon a successful call, 
    buffer contains the value of the parameter.

\param[in] BufferSize This is the number of bytes to be written.
         Caller must ensure that buffer size match the parameters size.

\param[in] TimeOut A timeout value in [ms] used during accessing the device. 

\return On completion, this function returns one of the following status codes:
\retval CL_ERR_NO_ERR No error occurred
\retval CL_ERR_TIMEOUT A timeout occurred
\retval CL_ERR_PARAM_NOT_SUPPORTED The parameters is not supported
*/
CLPROTOCOLEXPORT CLINT32 CLPROTOCOL 
clpGetParam( ISerial *pSerial, CLP_PARAMS param, const CLUINT32 Cookie, CLINT8* pBuffer, const CLINT64 BufferSize, const CLUINT32  TimeOut);

/*!
\brief This function is call to change a CLProtocol parameter value.

\param[in] param The parameter to be changed.
\param[in] Cookie Token supplied by the driver dll via the clpProbeDevice function. 
   Some parameters ignore the Cookie.
\param[in] pBuffer Points to a buffer containing the data to be written

\param[in] BufferSize This is the number of bytes in the buffer.
    Caller must ensure that buffer size match the parameters size.

\param[in] TimeOut A timeout value in [ms] used during accessing the device. 

\return On completion, this function returns one of the following status codes:
\retval CL_ERR_NO_ERR No error occurred
\retval CL_ERR_TIMEOUT A timeout occurred
\retval CL_ERR_PARAM_NOT_SUPPORTED The parameters is not supported
*/
CLPROTOCOLEXPORT CLINT32 CLPROTOCOL 
clpSetParam( ISerial *pSerial, CLP_PARAMS param, const CLUINT32 Cookie, const CLINT8* pBuffer, const CLINT64 BufferSize, const CLUINT32  TimeOut);

/*!
\brief This function is call know if a parameter is supported by a CLProtocol.

\param[in] param The parameter to probe.

\return On completion, this function returns one of the following status codes:
\retval CL_ERR_NO_ERR The parameters is supported
\retval CL_ERR_PARAM_NOT_SUPPORTED The parameters is not supported
*/
CLPROTOCOLEXPORT CLINT32 CLPROTOCOL 
clpIsParamSupported( CLP_PARAMS param);

/*!
\brief This function is call to get a possible event by a CLProtocol.

\param[in] pSerial Pointer to the interface giving access to the serial port

\param[in] Cookie Token supplied by the driver dll via the clpProbeDevice function.

\param[out] pBuffer A pointer to a user-allocated buffer into which the function
copies the event data. 

\param[in,out] pBufferSize As an input, the value contains the size of the buffer that
is passed through the pBuffer parameter.
On successful return, this parameter contains the number of bytes written into the buffer,
including the NULL termination character.
On CL_ERR_BUFFER_TOO_SMALL, this parameter contains the size of the buffer needed to write
the data text.

\return On completion, this function returns one of the following status codes:
\retval CL_ERR_NO_ERR The next event is copied to pBuffer
\retval CL_ERR_TIMEOUT There was no new event received
\retval CL_ERR_BUFFER_TOO_SMALL The pBuffer is too small
\retval CL_ERR_INVALID_COOKIE The cookie supplied is invalid
*/
CLPROTOCOLEXPORT CLINT32 CLPROTOCOL
clpGetEventData(const CLUINT32 Cookie, CLINT8* pBuffer, CLUINT32* pBufferSize);

/*@}*/ // CLProtocolInterface
/*@}*/ // PublicInterfaces



#endif // CLPROTOCOL_CLPROTOCOL_H
