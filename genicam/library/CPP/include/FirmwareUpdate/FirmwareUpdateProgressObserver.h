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
\brief    Declaration of IFirmwareUpdateProgressObserver and EFirmwareUpdateProgressStepType
*/
#ifndef GENFW_FIRMWAREUPDATEPROGRESSOBSERVER_H
#define GENFW_FIRMWAREUPDATEPROGRESSOBSERVER_H

#include <GenICamFwd.h>
#include <Base/GCException.h>
#include <FirmwareUpdate/FirmwareUpdateDll.h>
#include <FirmwareUpdate/FirmwareUpdateLinkage.h>
#include <FirmwareUpdate/FirmwareUpdateInfo.h>
#include <FirmwareUpdate/Destructible.h>

namespace GENAPI_NAMESPACE
{
    /**
    \brief The steps that can be performed in a firmware update.
    Can be used to display localized information to the user.
    */
    enum EFirmwareUpdateProgressStepType
    {
        ProgressStepType_CheckPreconditions,    ///< Compares all conditions of the update with the device.
        ProgressStepType_AcquireUpdateData,     ///< Checks that everything required for the update is available.
        ProgressStepType_FeatureWrite,          ///< Writes the value to a GenICam feature of the device.
        ProgressStepType_FeatureExecute,        ///< Executes the given GenICam feature.
        ProgressStepType_FeatureAssert,         ///< Asserts a GenICam feature's value matches a regular expression.
        ProgressStepType_FileUpload,            ///< Uploads a file to the device using the GenICam FileAccess feature.
        ProgressStepType_DeviceReset,           ///< A device reset is executed.
        ProgressStepType_Invalid                ///< Invalid value.
    };


    /**
    \brief Represents a firmware update observer.
    This interface is used by CFirmwareUpdater::ApplyUpdate().
    */
    class GENFW_DECL_ABSTRACT IFirmwareUpdateProgressObserver : public IDestructible
    {
    public:
        /**
        \brief Signal the start of a firmware update.
        \param[in]  FirmwareUpdateInfo  The firmware update info object representing a firmware update.
        \param[in]  EstimatedDurationMs The estimated time for the whole update to take. This is provided for informational purposes only.

        Can throw C++ exceptions.
        */
        virtual void OnFirmwareUpdateBegin(const CFirmwareUpdateInfo& FirmwareUpdateInfo, unsigned int EstimatedDurationMs) = 0;


        /**
        \brief Signal the start of a single firmware update step.
        \param[in]  ProgressStepType  The type of the step.
        \param[in]  EstimatedDurationMs The estimated time for the step to take. This provided is for informational purposes only.

        C++ exceptions thrown by this call are ignored to not interrupt the update process and eventually brick the updated device.
        */
        virtual void OnFirmwareUpdateProgressStepBegin(EFirmwareUpdateProgressStepType ProgressStepType, unsigned int EstimatedDurationMs, const char* Description) = 0;


        /**
        \brief Signal the progress of the current firmware update step.
        \param[in]  ProgressStepType  The type of the step. This is the same value as passed in OnFirmwareUpdateProgressStepBegin.
        \param[in]  ProgressPercentOfDone  Progress of the sub step taking longer time to complete, e.g. file upload to the device, data range: 0.0 - 100.0.
        
        This method is not called for all update steps.

        C++ exceptions thrown by this call are ignored to not interrupt the update process and eventually brick the updated device.
        */
        virtual void OnFirmwareUpdateProgressStepPartialProgress(EFirmwareUpdateProgressStepType ProgressStepType, double ProgressPercentOfDone) = 0;


        /**
        \brief Signal the end of a single firmware update step.
        \param[in]  ProgressStepType  The type of the step. This is the same value as passed in OnFirmwareUpdateProgressStepBegin.
        \param[in]  EstimatedDurationMs The estimated time for the step to take. This provided is for informational purposes only.
                                          This is the same value as passed in OnFirmwareUpdateProgressStepBegin.

        C++ exceptions thrown by this call are ignored to not interrupt the update process and eventually brick the updated device.
        */
        virtual void OnFirmwareUpdateProgressStepEnd(EFirmwareUpdateProgressStepType ProgressStepType, unsigned int EstimatedDurationMs) = 0;


        /**
        \brief Signal the end of a firmware update.

        Can throw C++ exceptions.
        */
        virtual void OnFirmwareUpdateEnd() = 0;


        /**
        \brief Signal that an error occurred during firmware update (between OnFirmwareUpdateBegin() and OnFirmwareUpdateEnd()).
        \param[in]  TheError  The exception that has been thrown while performing an update step. Caught unknown exception types are converted to a GenericException.

        After this call the original exception caught is re-thrown

        C++ exceptions thrown by this call are ignored.
        */
        virtual void OnFirmwareUpdateError(const GENICAM_NAMESPACE::GenericException& TheError) = 0;
    };
}

#endif // GENFW_FIRMWAREUPDATEPROGRESSOBSERVER_H
