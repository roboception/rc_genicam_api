//-----------------------------------------------------------------------------
//  (c) 2017 by Teledyne DALSA
//  Section: Digital Imaging
//  Project: GenAPI
//  Author:  Eric Bourbonnais
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
\brief    Definition of interface IPort
\ingroup GenApi_PublicInterface
*/

#ifndef GENAPI_IPORT_STACKED_H
#define GENAPI_IPORT_STACKED_H

#include <Base/GCException.h>
#include <GenApi/GenApiDll.h>
#include <GenApi/Types.h>
#include <GenApi/IBase.h>
#include <GenApi/IPort.h>

#ifdef WIN32
#   pragma warning ( push )
#   pragma warning ( disable : 4251 ) // XXX needs to have dll-interface to be used by clients of class YYY
#endif // #ifdef WIN32

namespace GENAPI_NAMESPACE
{
    //*************************************************************
    // IPort interface
    //*************************************************************
    
    typedef struct S_PORT_REGISTER_STACK_ENTRY
    {
      uint64_t   Address;        /* Address of the register. */
      void*      pBuffer;        /* Pointer to the buffer containing the data. */
      size_t     Size;           /* Number of bytes to read write. */
    } PORT_REGISTER_STACK_ENTRY;

    /**
    \brief Interface for ports
    \ingroup GenApi_PublicInterface
    */
    GENICAM_INTERFACE GENAPI_DECL_ABSTRACT IPortStacked : virtual public IPort
    {
        using IPort::Write; // tell the compiler we want both Write functions (the one from the base class and the next one)
        //! Writes a chunk of bytes to the port
        virtual void Write(PORT_REGISTER_STACK_ENTRY *pEntries,  size_t numEntries) = 0;
    };

    //*************************************************************
    // CPortRef class
    //*************************************************************

#ifndef DOXYGEN_IGNORE

    /**
    \internal
    \brief Reference to an IPort pointer
    \ingroup GenApi_PublicImpl
    */
    template <class T>
    class CPortStackedRefT : public CPortRefT<T>
    {
        typedef CPortRefT<T> ref;

    public:
        /*--------------------------------------------------------*/
        // IPort
        /*--------------------------------------------------------*/

        //! Reads a chunk of bytes from the port
        virtual void Read(PORT_REGISTER_STACK_ENTRY *pEntries,  size_t numEntries)
        {
            if(!ref::m_Ptr)
                throw ACCESS_EXCEPTION("Feature not present (reference not valid)");
            ref::m_Ptr->Read(pEntries, numEntries);
        }

        //! Writes a chunk of bytes to the port
        virtual void Write(PORT_REGISTER_STACK_ENTRY *pEntries,  size_t numEntries)
        {
            if(!ref::m_Ptr)
                throw ACCESS_EXCEPTION("Feature not present (reference not valid)");
            ref::m_Ptr->Write(pEntries, numEntries);
        }

    };

    //! Reference to an IEnumEntry pointer
    //! \ingroup GenApi_PublicImpl
    typedef CPortStackedRefT<IPortStacked> CPortStackedRef;

#endif

}

#ifdef WIN32
#   pragma warning ( pop )
#endif // #ifdef WIN32

#endif // ifndef GENAPI_IPORT_STACKED_H
