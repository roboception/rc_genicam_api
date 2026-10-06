//-----------------------------------------------------------------------------
//  (c) 2015 by Basler
//  Author:  Fritz Dierks
//
//  License: This file is published under the license of the EMVA GenICam  Standard Group.
//  A text file describing the legal terms is included in  your installation as 'GenICam_license.pdf'.
//  If for some reason you are missing  this file please contact the EMVA or visit the website
//  (http://www.genicam.org) for a full copy.
//
//  THIS SOFTWARE IS PROVIDED BY THE EMVA GENICAM STANDARD GROUP "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE EMVA GENICAM STANDARD GROUP
//  OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
//  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
//  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE)
//  ARISING IN ANY WAY OUT OF THE USE  OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//  
//-----------------------------------------------------------------------------
/*!
\file   IUserData.h
\brief  Definition of interface IUserData
*/

#ifndef GENAPI_IUSERDATA_H
#define GENAPI_IUSERDATA_H

#include <GenApi/GenApiDll.h>

#ifdef _MSC_VER
#   pragma warning ( push )
#   pragma warning ( disable : 4251 ) // XXX needs to have dll-interface to be used by clients of class YYY
#endif

namespace GENAPI_NAMESPACE
{
    typedef void* UserData_t;
    //*************************************************************
    // IUserData
    //*************************************************************
    /**
    * \ingroup internal_itf
    * \brief Interface to manage user data stored in nodes or nodemaps.
    */

    GENICAM_INTERFACE GENAPI_DECL_ABSTRACT IUserData
    {
        //! Returns the stored user defined data.
        virtual UserData_t GetUserData() const = 0;
        //! Stores user defined data without taking ownership. Returns the previously stored value.
        virtual UserData_t SetUserData( UserData_t userdata ) = 0;

    };

}

#ifdef _MSC_VER
#   pragma warning ( pop )
#endif

#endif // GENAPI_IUSERDATA_H
