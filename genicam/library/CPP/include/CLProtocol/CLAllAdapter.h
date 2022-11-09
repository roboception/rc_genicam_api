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
\brief    C++ class wrapping CLSerialAll
*/

#ifndef CLPROTOCOL_CLALLADAPTER_H
#define CLPROTOCOL_CLALLADAPTER_H

#include <Base/GCBase.h>
#include <CLProtocol/ClAllSerial.h>
#include <CLProtocol/CLException.h>
#include <CLProtocol/CLProtocol.h>
#include <CLProtocol/ISerialAdapter.h>
#include <map>

#if defined(_WIN32)
#include <windows.h>
#else
#include <dlfcn.h>
#include <errno.h>
#endif

#if defined (_WIN32)
typedef HMODULE lib_handle_t;
#else
typedef void * lib_handle_t;
#endif

// manufacturer shared libraries are defined with __cdecl ...; taken from DLLLoad.h
#if defined (_MSC_VER)
// nothing
#else
#  if !defined(__x86_64) && !defined(__arm__) && !defined(__aarch64__) && !defined(VXWORKS)
#   define  __cdecl __attribute__((cdecl))
#  else
#   define  __cdecl /* use default calling convention */
#  endif
#endif

namespace CLProtocol
{
    //! A map holding the pointers to the serial adapters
    typedef std::map<GENICAM_NAMESPACE::gcstring, ISerialAdapter *> SerialPortMap;

    //! Wraps the CLAllSerial.dll
    class CLPROTOCOL_DECL CCLAllAdapter  : public ISerialAdapter
    {
    public:

        //! Destroy the factory and all its objects
        static void Destroy( ISerialAdapter * pAdapter );

        //! \name ISerialAdapter interface
        //@{ 
        virtual CLINT32 CLPROTOCOL clSerialRead( CLINT8*,CLUINT32*, CLUINT32);
        virtual CLINT32 CLPROTOCOL clSerialWrite(CLINT8 *,CLUINT32 *,CLUINT32);
        virtual CLINT32 CLPROTOCOL clGetSupportedBaudRates(CLUINT32 *);
        virtual CLINT32 CLPROTOCOL clSetBaudRate(CLUINT32);
        virtual CLINT32 SerialInit( void);
        virtual void SerialClose( void);
        virtual void CheckError( CLINT32 ErrorCode );
        //@} 

        //! used to copy the content of the SerialPort ma to a std::map
        static void GetPortIDs(GENICAM_NAMESPACE::gcstring_vector &TheVector);
        //! used to copy the content of the SerialPort ma to a std::map
        static ISerialAdapter *GetSerialAdapter(GENICAM_NAMESPACE::gcstring PortID);

    private:
        /*------------------------------------------------*/
        // functions of the CLAllSerial DLL
        /*------------------------------------------------*/
        struct CLAllProxy
        {
            //! Function pointer to clGetErrorText
            CLINT32 (CLALLSERIALCC *m_clGetErrorText)( const CLINT8*, CLINT32, CLINT8*, CLUINT32*);
            //! Function pointer to clGetNumPorts
            CLINT32 (CLALLSERIALCC *m_clGetNumPorts) ( CLUINT32* Ports);
            //! Function pointer to  clGetPortInfo
            CLINT32 (CLALLSERIALCC *m_clGetPortInfo ) ( CLUINT32 serialIndex, CLINT8* manufacturerName, CLUINT32* nameBytes, CLINT8* portID, CLUINT32* IDBytes, CLUINT32* version);
            //! Function pointer to clGetSupportedBaudRates
            CLINT32 (CLALLSERIALCC *m_clGetSupportedBaudRates) ( hSerRef serialRef, CLUINT32* baudRates);
            //! Function pointer to clSerialClose
            void (CLALLSERIALCC *m_clSerialClose) ( hSerRef serialRef);
            //! Function pointer to clSerialInit 
            CLINT32 (CLALLSERIALCC *m_clSerialInit) ( CLUINT32 serialIndex, hSerRef* serialRefPtr);
            //! Function pointer to clSerialRead
            CLINT32 (CLALLSERIALCC *m_clSerialRead) ( hSerRef serialRef, CLINT8* buffer, CLUINT32* bufferSize, CLUINT32 serialTimeout);
            //! Function pointer to clSerialWrite
            CLINT32 (CLALLSERIALCC *m_clSerialWrite) ( hSerRef serialRef, CLINT8* buffer, CLUINT32* bufferSize, CLUINT32 serialTimeout);
            //! Function pointer to clSetBaudRate
            CLINT32 (CLALLSERIALCC *m_clSetBaudRate) ( hSerRef serialRef, CLUINT32 baudRate);
        };

        static void CheckError( CLINT32 ErrorCode, CLUINT32 SerialIndex, CLAllProxy & );

        //! Object constructor
        CCLAllAdapter( CLUINT32, CLAllProxy &);

        //! Object destructor
        virtual ~CCLAllAdapter();

        //! handle of the serial port 
        hSerRef m_SerialRef;

#ifdef _DEBUG // Allow changing serial index to test error situations.
    public:
#endif
        //! Index of the serial port
        CLUINT32 m_SerialIndex;

    private:

        //! retrieves  the list of port mapped by PortID
        static void InternalGetPortMap(SerialPortMap &);

        static SerialPortMap *g_pSerialPortMap;

        //! handle to the loaded ClAllSerial DLL
        lib_handle_t m_ClAlldllHandle;

        //! CLALL functions pointers
        CLAllProxy m_clallproxy;

        //! the previous baud rate as set via SetBaudRate; zero otherwise
        CLUINT32 m_PreviousBaudRate;
    };

    //! retrieves  the list of port mapped by PortID
    inline void GetPortMap(SerialPortMap &StdMap)
    {
        GENICAM_NAMESPACE::gcstring_vector PortIDs;
        CCLAllAdapter::GetPortIDs(PortIDs);
        for (GENICAM_NAMESPACE::gcstring_vector::iterator it = PortIDs.begin(); it != PortIDs.end(); it++)
        {
            StdMap[*it] = CCLAllAdapter::GetSerialAdapter(*it);
        }
    }

    inline void Destroy(SerialPortMap &portList)
    {
        for (SerialPortMap::iterator it = portList.begin(); it != portList.end(); it++)
            CCLAllAdapter::Destroy(it->second);
    }

}
#endif //CLPROTOCOL_CLALLADAPTER_H
