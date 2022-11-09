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
    \brief    C++ class wrapping CLSerialAll and CLProtocolDriver 
*/
#ifndef CLPROTOCOL_CLPORT_H
#define CLPROTOCOL_CLPORT_H

#include <Base/GCBase.h>
#include <GenApi/PortImpl.h>
#include <CLProtocol/CLException.h>
#include <CLProtocol/CLAutoBuffer.h>
#include <CLProtocol/CLProtocol.h>
#include <CLProtocol/ISerial.h>
#include <CLProtocol/ISerialAdapter.h>
#include <CLProtocol/CLAllAdapter.h>
#include <CLProtocol/DeviceID.h>
#include <CLProtocol/CVersion.h>
#include <CLProtocol/XMLID.h>
#include <iostream>
#include <fstream>
#include <list>
#include <algorithm>


namespace CLProtocol
{

    //! the magic GUID which indicates that the file is a CLProtocol registry file. Must be the first entry
#   define CLPORT_PERSISTENCE_MAGIC   "{3DFD485D-0ADB-4d84-9956-C3766504A2DD}"


#ifdef _MSC_VER
#   pragma warning (push )
#   pragma warning( disable:4275)  // warning C4275: non dll-interface class 'GenApi::CPortImpl' used as base for dll-interface class 'CLProtocol::CCLPort'
#endif
   
    /*! \brief Wraps a CameraLink protocol DLL and implements the IPort and the ISerial interface
        \ingroup PublicInterfaces

        The class dynamically loads a protocol driver DLL and binds it to a given port.
        It implements the IPort and the ISerial interface.
        
        The class can enumerate
        - the PortIDs of all frame grabbers installed in the system
        - the DeviceID templates of all CLProtocol drivers installed in the system
        The class can probe a port and retrieve the DeviceID of the attached camera.
        The class can retrieve the GenApi XML file for a camera.
    */
    class CLPROTOCOL_DECL CCLPort : public GENAPI_NAMESPACE::CPortImpl, public ISerial
    {
    public:

        //! \name Device discovery and binding
        //!@{

        /*! \brief Retrieves a list of PortIDs which can be used to create a corresponding port object
            \param[out] PortIDs A list of PortIDs which each has the form 'ManufacturerName#PortName'
        */
        static void GetPortIDs( GENAPI_NAMESPACE::StringList_t &PortIDs );
 
        /*! \brief Gets a SerialAdapter port
            \param[in] PortID PortID of the serial port
            \return ISerialAdapter interface belonging to the PortID; NULL if PortID is not valid
        */
        static ISerialAdapter *GetPort( const GENICAM_NAMESPACE::gcstring &PortID );

        /*! \brief Retrieves a list of \ref DeviceID templates 

            Retrieves all DLLs found in the subdirectories of $(GENICAM_CLPROTOCOL),
            loads the dlls, asks them for their supported DeviceIDTemplates, and returns a 
            list of the combined DeviceIDTemplate strings of all dlls.

            \param[out] DeviceIDTemplates A list of DeviceID templates 
        */
        static void GetDeviceIDTemplates( GENICAM_NAMESPACE::gcstring_vector &DeviceIDTemplates );

        /*! \brief Connects a camera and retrieves all information required for the DeviceID

            Connects the camera with 9600 Baud (= CL standard baud rate after power up) and
            identifies the camera attached to the port given by PortID. The DeviceIDTemplate
            helps with the identification whose result is returned as DeviceID.

            \param[in] PortID Identifies the port the camera to be probed is connected to
            \param[in] strDeviceIDTemplate The \ref DeviceID templates helping the protocol driver dll to identify the camera
            \param[in] SerialTimeout The timeout in milliseconds when probing the port (see \ref SetSerialTimeOut).
            \return A \ref DeviceID containing a unique description of the camera found. If the DeviceID is an empty string no device was found.
        */
        static GENICAM_NAMESPACE::gcstring ProbeDevice(const GENICAM_NAMESPACE::gcstring &PortID, const GENICAM_NAMESPACE::gcstring &strDeviceIDTemplate, const CLUINT32 SerialTimeout = 500);

        //! \brief Stop all running ProbeDevice.
        static void StopProbing(CLUINT32 toStop);

        /*! \brief Register a custom serial port
            \param[in] PortName Identifies the port to use for connection.
            \param[in] pSerialAdapter An interface that allow CCLPort to access the serial port.
            \return    PortID a portId of the form "Local#<PortName>"
        */
        static GENICAM_NAMESPACE::gcstring RegisterSerial( const GENICAM_NAMESPACE::gcstring &PortName, ISerialAdapter *pSerialAdapter);

        //! Unregister all resource related to serial port
        static void UnRegisterSerials( );

        //! Cleans up static variables
        static void Terminate( );

        /*! \brief Unregister serial  serial port
            \param[in] PortID Identifies the port that need to be disconnected
        */
        static void UnRegisterSerial( const GENICAM_NAMESPACE::gcstring &PortID );

        /*! \brief Creates a port object for a given PortID and DeviceID
            \param[in] PortID Identifies the port to use for connection.
            \param[in] strDeviceID Identifies the device to connect. Can be optionally a DeviceID template.
        */
        void Connect( const GENICAM_NAMESPACE::gcstring &PortID, const GENICAM_NAMESPACE::gcstring &strDeviceID );

        /*! \brief Creates a port object for a given PortID taking the DeviceID from a internal cache
            \param[in] PortID Identifies the port to use for connection.
            \return false, if the cache doesn't contain a DeviceID for the PortID; 
                    true, if the cache contains a corresponding DeviceID and if a matching device is connected
                    Throws an exception if there is a cache entry but no matching device is connected
        */
        bool Connect( const GENICAM_NAMESPACE::gcstring &PortID );

        /*! \brief Retrieves a list of \ref XMLID strings

            Retrieves a list of XML IDs from the file system and the CLProtocol.dll 
            which in turn may ask the camera, e.g. by reading a Manifest register

            The list contains only XML ID which would match the DeviceID and 
            whose schema version can be interpreted by the current GenICam version.

            The XML ID are order best matching first.

            \param[out] XMLIDs A list of XML IDs templates 
        */
        void GetXMLIDs( GENICAM_NAMESPACE::gcstring_vector &XMLIDs );

        /*! \brief Retrieves an GenApi XML description either from the DLL or the file system 
            \param[in] XMLID The XML ID identifying the XML file whose data is to be received
            \param[in] UseMaxBaudrate if true the baud rate is set to maximum while downloading the XML file and reset afterwards
            \return GenApi compliant camera XML description code ready to instantiate a node map.
        */
        GENICAM_NAMESPACE::gcstring GetXML( GENICAM_NAMESPACE::gcstring XMLID, const bool UseMaxBaudrate = true );

        /*! \brief Delete the device xml file from the download folder if the file was downloaded for the camera.
            
            This can be use to remove a corrupted file.
            File can easily be corrupted if the download is aborted.
            
            \param[in] XMLID The XML ID identifying the XML file whose data is to be received
        */
        void DeleteXMLDownload( GENICAM_NAMESPACE::gcstring XMLID );

        /*! \brief Retrieves the best matching GenApi XML description either from the DLL or the file system
            \param[in] UseMaxBaudrate if true the baud rate is set to maximum while downloading the XML file and reset afterwards
            \return GenApi compliant camera description code ready to instantiate a node map.
        */
        GENICAM_NAMESPACE::gcstring GetXML( const bool UseMaxBaudrate = true );

        /*! \brief Creates a file URL for a given XML ID
            
            The URL has the following format: "file:///filepath.xml"

            \return file URL to a XML file as described in the GenTL module
        */
        GENICAM_NAMESPACE::gcstring MakeURLfromXMLID( GENICAM_NAMESPACE::gcstring &XMLID );

        //!@}

        //! \name Creation and parameterizing
        //!@{

        //! Public constructor
        CCLPort();

        //! Public destructor
        virtual ~CCLPort();

        //! Sets the timeout for operations on the serial port
        /*!
            \param[in] Timeout Timeout in [ms] for operations on the serial port.
        */
        void SetSerialTimeOut(const CLUINT32 Timeout);

        //! Gets the timeout for operations on the serial port
        CLUINT32 GetSerialTimeOut() const;

        //! Returns the Device ID the object has been created with
        GENICAM_NAMESPACE::gcstring GetDeviceID() const;
        
        //! Set device protocol log lever
        void SetProtocolLogLevel( const CLP_LOG_LEVEL_VALUE logLever);

        //! Set device bauderate using one of the CL_BAUDRATE_XXXX value.
        void SetDeviceBauderate( const CLUINT32 baudrate);

        //! Returns the current device bauderate, using one of the CL_BAUDRATE_XXXX value.
        CLUINT32 GetDeviceBauderate( );

        //! Sed supported bauderates of the combined device and host interface.
        CLUINT32 SupportedBaudrates();

        //! Set log level that is assigned to the device protocol when the dll is loaded.
        static void SetDefaultLogLevel( const CLP_LOG_LEVEL_VALUE logLever);

        //! Stores a PortID/DeviceID pair
        static void StorePortIDDeviceIDPairs( GENICAM_NAMESPACE::gcstring_vector &PortIDs, GENICAM_NAMESPACE::gcstring_vector &DeviceIDs );

        //! Retrieves a PortID/DeviceID pair
        static void RetrievePortIDDeviceIDPairs( GENICAM_NAMESPACE::gcstring_vector &PortIDs, GENICAM_NAMESPACE::gcstring_vector &DeviceIDs );

        //!@}

        //! \name IPort interface
        //@{ 

        //! Get the access mode of the node
        virtual GENAPI_NAMESPACE::EAccessMode GetAccessMode() const;

        //! Reads a chunk of bytes from the port
        virtual void Read(void * pBuffer, int64_t Address, int64_t Length);

        //! Writes a chunk of bytes to the port
        virtual void Write(const void * pBuffer, int64_t Address, int64_t Length);

        //! We are an IPort interface
        virtual GENAPI_NAMESPACE::EInterfaceType GetPrinicipalInterfaceType() const;
        //@} 

        //! We can get event data
        CLINT32 GetEvent(
            void * buffer,      //!< Points to a user-allocated buffer. Upon a successful call, 
                                //!< buffer contains the data read from the serial device.  
                                //!< Upon failure, this buffer is not affected.  
                                //!< if buffersize is too small result is CL_ERR_BUFFER_TOO_SMALL.
            CLUINT32 * bufferSize //!< input is the size of the buffer, output is the size of 
                                  //!< event data in buffer.
            );

        //! \name ISerial interface
        //@{ 

        //! calls clSerialRead as defined by the camera link standard
        virtual CLINT32 CLPROTOCOL clSerialRead(
            CLINT8* buffer,     //!< Points to a user-allocated buffer. Upon a successful call, 
                                //!< buffer contains the data read from the serial device.  
                                //!< Upon failure, this buffer is not affected.  
                                //!< Caller should ensure that buffer is at least numBytes in size.
            CLUINT32* bufferSize,   //!< This is the number of bytes requested by the caller.
            CLUINT32  serialTimeOut //!< Indicates the timeout in milliseconds.
            );

        //! calls clSerialWrite as defined by the camera link standard
        virtual CLINT32 CLPROTOCOL clSerialWrite(
            CLINT8* buffer, //!< Contains data to write to the serial device.
            CLUINT32* bufferSize,  //!< Contains the buffer size indicating 
                                 //!< the number of bytes to be written. 
                                 //!< Upon a successful call, bufferSize contains the number of 
                                 //!< bytes written to the serial device. 
            CLUINT32 serialTimeOut //!< Indicates the timeout in milliseconds.
            );

        //! returns the valid baud rates of the current interface.
        virtual CLINT32 CLPROTOCOL clGetSupportedBaudRates(
            CLUINT32 *baudRates //!< Bitfield that describes all supported baud rates of the serial port 
                                //!< as described by serialRefPtr. See Table B-2 of the Camera Link 
                                //!< documentation for more information on constants. 
                                );
        //@} 

    private:
        //! Initialize the PortID list
        static void InitPortIDs(void);

        //! This function sets the baud rate for the serial port of the selected device. 
        virtual CLINT32 CLPROTOCOL clSetBaudRate( 
            CLUINT32 baudRate   //!< The baud rate you want to use. This parameter expects the 
                                //!< values represented by the CL_BAUDRATE constants in table B-2. 
                                //!< By default, the baud rate for serial communication is 9600.;
                                );

        //! Helper retrieving error messages
        void CheckError( CLINT32 ErrorCode ) const;

        //! helper retrieving error messages
        static void CheckLastError();

        //! loads the protocol driver DLL
        void LoadProtocolDriver( const GENICAM_NAMESPACE::gcstring &PortDriverDLLPath );

        //! unloads the driver DLL
        void UnLoadProtocolDriver( );

        //! Creates a CLProtocol driver and connects it a a port identified by PortID
        void CreatePort( const GENICAM_NAMESPACE::gcstring &PortID, const GENICAM_NAMESPACE::gcstring &PortDriverDLLPath );

        //! updates the DeviceID cache
        static void UpdateCache( const GENICAM_NAMESPACE::gcstring &PortID, const GENICAM_NAMESPACE::gcstring &DeviceID );

        //! A proxy to the serial port used by the CCLPort object
        ISerialAdapter *m_pSerialAdapter;

        //! A list of all available proxies in the system
        static SerialPortMap *s_pSerialList;

        //! A list of all available proxies registers manually
        static SerialPortMap *s_pPrivateSerialList;

        //! indicates if s_pSerialList is initialized
        static bool s_IsSerialListInitialized;


        //! handle to the port driver DLL loaded
        lib_handle_t m_hPortDriverDLL;

        //! Timeout in [ms]
        CLUINT32 m_SerialTimeOut;
        
        //! the \ref DeviceID of the connected device
        GENICAM_NAMESPACE::gcstring m_DeviceID;

        //! the \ref PortID of the connected device
        GENICAM_NAMESPACE::gcstring m_PortID;

        //! Cookie
        CLUINT32 m_Cookie;

        /*------------------------------------------------*/
        // functions of the CL protocol DLL
        /*------------------------------------------------*/
        //! Function pointer to clpInitLib
        CLINT32 (__cdecl *m_clpInitLib)( clp_logger_t logger, CLP_LOG_LEVEL_VALUE logLevel);

        //! Function pointer to clpCloseLib
        CLINT32 (__cdecl *m_clpCloseLib)( void );

        //! Function pointer to clpGetShortDeviceIDTemplates
        CLINT32 (__cdecl *m_clpGetShortDeviceIDTemplates) (CLINT8* pDeviceTemplates, CLUINT32* pBufferSize );

        //! Function pointer to clpProbeDevice
        CLINT32 (__cdecl *m_clpProbeDevice) (ISerial *pSerial, const CLINT8* pDeviceIDTemplate, CLINT8* pDeviceID, CLUINT32 *pBufferSize, CLUINT32 *pCookie, const CLUINT32 TimeOut );

        //! Function pointer to clpGetXMLIDs
        CLINT32 (__cdecl *m_clpGetXMLIDs) ( ISerial *pSerial, const CLUINT32 Cookie ,CLINT8* pXMLIDs, CLUINT32* pBufferSize, const CLUINT32 TimeOut );

        //! Function pointer to m_clpGetXMLDescription
        CLINT32 (__cdecl *m_clpGetXMLDescription) (ISerial *pSerial, const CLUINT32 Cookie, const CLINT8* pDeviceID, CLINT8* pXMLBuffer, CLUINT32* pBufferSize, const CLUINT32 TimeOut );

        //! Function pointer to clpReadRegister
        CLINT32 (__cdecl *m_clpReadRegister) (ISerial *pSerial, const CLUINT32 Cookie, const CLINT64 Address, CLINT8* pBuffer, const CLINT64 BufferSize, const CLUINT32  TimeOut );

        //! Function pointer to clpWriteRegister
        CLINT32 (__cdecl *m_clpWriteRegister) (ISerial *pSerial, const CLUINT32 Cookie, const CLINT64 Address, const CLINT8* pBuffer, const CLINT64 BufferSize, const CLUINT32  TimeOut );

        //! Function pointer to clpContinueWriteRegister
        CLINT32 (__cdecl *m_clpContinueWriteRegister) (ISerial *pSerial, const CLUINT32 Cookie, const BOOL8 ContinueWaiting, const CLUINT32  TimeOut );

        //! Function pointer to clpGetErrorText
        CLINT32 (__cdecl *m_clpGetErrorText) (const CLINT32 errorCode, CLINT8* errorText, CLUINT32* errorTextSize, const CLUINT32 Cookie );

        //! Function pointer to clpDisconnect
        CLINT32 (__cdecl *m_clpDisconnect) (const CLUINT32 Cookie );

        //! Function pointer to clpGetCLProtocolVersion
        CLINT32 (__cdecl *m_clpGetCLProtocolVersion) (CLUINT32 *pVersionMajor, CLUINT32 *pVersionMinor );

        //! Function pointer to clpGetParam
        CLINT32 (__cdecl *m_clpGetParam) (ISerial *pSerial, CLP_PARAMS param, const CLUINT32 Cookie, const CLINT8* pBuffer, const CLINT64 BufferSize, const CLUINT32  TimeOut);

        //! Function pointer to clpSetParam
        CLINT32 (__cdecl *m_clpSetParam) (ISerial *pSerial, CLP_PARAMS param, const CLUINT32 Cookie, const CLINT8* pBuffer, const CLINT64 BufferSize, const CLUINT32  TimeOut);

        //! Function pointer to clpIsParamSupported
        CLINT32 (__cdecl *m_clpIsParamSupported) (CLP_PARAMS param);

        //! Function pointer to clpgetEventData
        CLINT32 (__cdecl *m_clpGetEventData) (const CLUINT32 Cookie, CLINT8* pBuffer, CLUINT32* pBufferSize);

        // What 1.1 feature are supported by the device protocol?
        //! true if you can set baud rate via the CLProtocol's function interface 
        bool m_protocolHasBaudratePrms;

        //! true if you can set the log level
        bool m_protocolHasLogLevelPrm;

        //! true if you can set the logger
        bool m_protocolHasLoggerPrm;

        //! true if you can set stop probing
        bool m_protocolHasStopProbeDevice;

        //! Default log level
        static CLP_LOG_LEVEL_VALUE m_defaultloglevel;

        //! probe of device is stopped
        static CLUINT32 m_stopProbing;
    private:
        //! must not be used
        CCLPort( CCLPort& );

        //! must not be used
        CCLPort& operator=( CCLPort& );
    };

#ifdef _MSC_VER
#   pragma warning(pop)
#endif

}
#endif // CLPROTOCOL_CLPORT_H
