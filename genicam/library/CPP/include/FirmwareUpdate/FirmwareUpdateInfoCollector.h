//-----------------------------------------------------------------------------
//  (c) 2016-2017 by Basler AG
//  Project: GenApi
//  Author:  Andreas Gau
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
\brief    Declaration of IFirmwareUpdateInfoCollector
*/
#ifndef GENFW_FIRMWAREUPDATEINFOCOLLECTOR_H
#define GENFW_FIRMWAREUPDATEINFOCOLLECTOR_H

#include <GenICamFwd.h>
#include <FirmwareUpdate/FirmwareUpdateDll.h>
#include <FirmwareUpdate/FirmwareUpdateLinkage.h>
#include "Destructible.h"

namespace GENAPI_NAMESPACE
{
    class CFirmwareUpdateInfo;
    

    /**
    \brief Represents a sink collecting all firmware updates contained in a guf file.
    This interface is used by CFirmwareUpdater::ReadFirmwareUpdateInfos().
    */
    class GENFW_DECL_ABSTRACT IFirmwareUpdateInfoCollector : public IDestructible
    {
    public:
        /**
        \brief Provides the currently read\c FirmwareUpdateInfo.
        \param[in]  FirmwareUpdateInfo  A firmware update info object representing a firmware update.
        \return Returns true if to continue reading. Returns false if to stop reading. 

        This method is called by CFirmwareUpdater::ReadFirmwareUpdateInfos().

        Can throw C++ exceptions.
        */
        virtual bool OnUpdateRead(const CFirmwareUpdateInfo& FirmwareUpdateInfo) = 0;
    };
}

#endif // GENFW_FIRMWAREUPDATEINFOCOLLECTOR_H
