//-----------------------------------------------------------------------------
//  (c) 2006 by Basler Vision Technologies
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
\brief    Definition of the interface IInteger.
\ingroup GenApi_PublicInterface
*/

#ifndef GENAPI_IIINTEGER_H
#define GENAPI_IIINTEGER_H

#include <Base/GCUtilities.h>
#include <GenApi/GenApiDll.h>
#include <GenApi/Types.h>
#include <GenApi/IValue.h>

#ifdef _MSC_VER
#   pragma warning ( push )
#   pragma warning ( disable : 4251 ) // XXX needs to have dll-interface to be used by clients of class YYY
#endif

namespace GENAPI_NAMESPACE
{
    //*************************************************************
    // IInteger interface
    //*************************************************************

    /**
    \brief Interface for integer properties
    \ingroup GenApi_PublicInterface
    */
    GENICAM_INTERFACE GENAPI_DECL_ABSTRACT IInteger : virtual public IValue
    {
        //! Set node value
        /*!
        \param Value The value to set
        \param Verify Enables AccessMode and Range verification (default = true)
        */
        virtual void SetValue(int64_t Value, bool Verify = true) = 0;

        //! Set node value
        virtual IInteger& operator=(int64_t Value) = 0;

        //! Get node value
        /*!
        \param Verify Enables Range verification (default = false). The AccessMode is always checked
        \param IgnoreCache If true the value is read ignoring any caches (default = false)
        \return The value read
        */
        virtual int64_t GetValue(bool Verify = false, bool IgnoreCache = false ) = 0;

        //! Get node value
        virtual int64_t operator()() = 0;

        //! Get node value
        virtual int64_t operator*() = 0;

        //! Get minimum value allowed
        virtual int64_t GetMin() = 0;

        //! Get maximum value allowed
        virtual int64_t GetMax() = 0;

        //! Get increment mode
        virtual EIncMode GetIncMode() = 0;

        //! Get increment
        virtual int64_t GetInc() = 0;

        //! Get list of valid value
        virtual int64_autovector_t GetListOfValidValues(bool bounded = true) = 0;

        //! Get recommended representation
        virtual  ERepresentation GetRepresentation() = 0;

        //! Get the physical unit name
        virtual GENICAM_NAMESPACE::gcstring GetUnit() = 0;

        //! Restrict minimum value
        virtual void ImposeMin(int64_t Value) = 0;

        //! Restrict maximum value
        virtual void ImposeMax(int64_t Value) = 0;
    };


    //*************************************************************
    // CIntegerRef class
    //*************************************************************

#ifndef DOXYGEN_IGNORE

    GENICAM_INTERFACE IFloat;

    /**
    \internal
    \brief Reference to an IInteger pointer
    \ingroup GenApi_PublicImpl
    */
    template <class T, class I=T>
    class CIntegerRefT : public CValueRefT<T, I>
    {
        typedef CValueRefT<T, I> ref;

    public:
        /*--------------------------------------------------------*/
        // IInteger
        /*--------------------------------------------------------*/

        //! Set node value
        virtual void SetValue(int64_t Value, bool Verify = true)
        {
            if(!ref::m_Ptr)
                throw ACCESS_EXCEPTION("Feature not present (reference not valid)");
            ref::m_Ptr->SetValue(Value, Verify);
        }

        //! Set node value
        virtual IInteger& operator=(int64_t Value)
        {
            if(!ref::m_Ptr)
                throw ACCESS_EXCEPTION("Feature not present (reference not valid)");
            ref::m_Ptr->SetValue(Value);
            return *this;
        }

        //! Get node value
        virtual int64_t GetValue( bool Verify = false, bool IgnoreCache = false )
        {
            if(!ref::m_Ptr)
                throw ACCESS_EXCEPTION("Feature not present (reference not valid)");
            return ref::m_Ptr->GetValue( Verify, IgnoreCache );
        }

        //! Get node value
        virtual int64_t operator()()
        {
            if(!ref::m_Ptr)
                throw ACCESS_EXCEPTION("Feature not present (reference not valid)");
            return ref::m_Ptr->operator()();
        }

        //! Get node value
        virtual int64_t operator*()
        {
            if(!ref::m_Ptr)
                throw ACCESS_EXCEPTION("Feature not present (reference not valid)");
            return ref::m_Ptr->operator*();
        }

        //! Get minimum value allowed
        virtual int64_t GetMin()
        {
            if(!ref::m_Ptr)
                throw ACCESS_EXCEPTION("Feature not present (reference not valid)");
            return ref::m_Ptr->GetMin();
        }

        //! Get maximum value allowed
        virtual int64_t GetMax()
        {
            if(!ref::m_Ptr)
                throw ACCESS_EXCEPTION("Feature not present (reference not valid)");
            return ref::m_Ptr->GetMax();
        }

        //! Get increment
        virtual EIncMode GetIncMode()
        {
            if(!ref::m_Ptr)
                throw ACCESS_EXCEPTION("Feature not present (reference not valid)");
            return ref::m_Ptr->GetIncMode();
        }

        //! Get increment
        virtual int64_t GetInc()
        {
            if(!ref::m_Ptr)
                throw ACCESS_EXCEPTION("Feature not present (reference not valid)");
            return ref::m_Ptr->GetInc();
        }

        //! Implementation of IInteger::GetListOfValidValues
        virtual int64_autovector_t GetListOfValidValues(bool bounded = true)
        {
            if(!ref::m_Ptr)
                throw ACCESS_EXCEPTION("Feature not present (reference not valid)");
            return ref::m_Ptr->GetListOfValidValues(bounded);
        }

        //! Get recommended representation
        virtual  ERepresentation GetRepresentation()
        {
            if(!ref::m_Ptr)
                throw ACCESS_EXCEPTION("Feature not present (reference not valid)");
            return ref::m_Ptr->GetRepresentation();
        }

        //! Get the physical unit name
        virtual GENICAM_NAMESPACE::gcstring GetUnit()
        {
            if(!ref::m_Ptr)
                throw ACCESS_EXCEPTION("Feature not present (reference not valid)");
            return ref::m_Ptr->GetUnit();
        }

        //! gets the interface of an alias node.
        IFloat *GetFloatAlias()
        {
            if(!ref::m_Ptr)
                throw ACCESS_EXCEPTION("Feature not present (reference not valid)");
            return dynamic_cast<IFloat*>(ref::m_Ptr->GetNode()->GetCastAlias());
        }

        //! Restrict minimum value
        virtual void ImposeMin(int64_t Value)
        {
            if(!ref::m_Ptr)
                throw ACCESS_EXCEPTION("Feature not present (reference not valid)");
            ref::m_Ptr->ImposeMin(Value);
        }

        //! Restrict maximum value
        virtual void ImposeMax(int64_t Value)
        {
            if(!ref::m_Ptr)
                throw ACCESS_EXCEPTION("Feature not present (reference not valid)");
            ref::m_Ptr->ImposeMax(Value);
        }

    };

    //! Reference to an IInteger pointer
    //! \ingroup GenApi_PublicImpl
    typedef CIntegerRefT<IInteger> CIntegerRef;

#endif

}

#ifdef _MSC_VER
#   pragma warning ( pop )
#endif

#endif // ifndef GENAPI_IIINTEGER_H
