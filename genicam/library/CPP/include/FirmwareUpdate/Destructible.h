//-----------------------------------------------------------------------------
//  (c) 2016-2019 by Basler AG
//  Project: GenApi
//
//  License: This file is published under the license of the EMVA GenICam Standard Group.
//  A text file describing the legal terms is included in your installation as 'GenICam_license.pdf'.
//  If for some reason you are missing this file please contact the EMVA or visit the website
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
//  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//  ARISING IN ANY WAY OUT OF THE USE  OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------
/*!
\file
\brief    Declaration of  of IDestructible base class
*/

#ifndef GENFW_DESTRUCTIBLE_H
#define GENFW_DESTRUCTIBLE_H

namespace GENAPI_NAMESPACE
{

    /**
    \brief Destructible object to ensure destruction via interface.
    This is used by all implementers of IFirmwareUpdateInfoCollector, IFirmwareUpdatableDevice and CFirmwareUpdateDefaultInfoCollector.
    */
    class GENFW_DECL_ABSTRACT IDestructible
    {
    public:
        /**
        \brief Default implementation of destructor

        Intentionally empty implementation.
         */
        virtual ~IDestructible()
        {
        }
    };
}
#endif // GENFW_DESTRUCTIBLE_H


