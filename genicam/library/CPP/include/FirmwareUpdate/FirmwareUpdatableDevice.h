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
\brief    Declaration of IFirmwareUpdatableDevice
*/
#ifndef GENFW_FIRMWAREUPDATEABLEDEVICE_H
#define GENFW_FIRMWAREUPDATEABLEDEVICE_H

#include <GenICamFwd.h>
#include <FirmwareUpdate/FirmwareUpdateDll.h>
#include <FirmwareUpdate/FirmwareUpdateLinkage.h>
#include <FirmwareUpdate/Destructible.h>
namespace GENAPI_NAMESPACE
{
    /**
    \brief Represents a device that can be updated.
    This interface is needed because GenICam does not know the actual SDK running an firmware update.
    It is required to write a wrapper class implementing this interface.
    */
    class GENFW_DECL_ABSTRACT IFirmwareUpdatableDevice : public IDestructible
    {
    public:
        /**
        \brief Prepare for an firmware update.
        Store information, e.g. for rediscovering a device after reset, e.g. serial number, MAC address or other information.
        What needs to be stored depends on the actual transport layer used.
        Open the device for running the update if the device is not already open.

        Throws C++ exceptions if accessing the device fails.
        */
        virtual void PrepareForFirmwareUpdate() = 0;


        /**
        \brief Get the node map of the device to update.
        return The node map of the device to update. Never returns NULL.
        
        Does not throw C++ exceptions.
        */
        virtual INodeMap* GetNodeMapOfDeviceToUpdate() = 0;


        /**
        \brief Perform a device reset while updating the device.
        Reset the camera device.
        Rediscover, create and open the device.
        This is also needed if a reset is the last step to assure that the upload succeeded.
        \param EstimatedDurationMs This is the estimated time in milliseconds for this reset. Actual time may vary.
        Note that the Duration information is an optional value in the control XML file, so this can be 0 - Use a reasonable default value in this case.
        \param DelayEnumerationMs This is the time required by the device until it can be enumerated again.
        \return Return the new node map created by creating a new device.

        Throws C++ exceptions if accessing or the device fails.
        */
        virtual INodeMap* DeviceResetWhileUpdatingFirmware(unsigned int EstimatedDurationMs,unsigned int DelayEnumerationMs) = 0;


        /**
        \brief Cleanup after firmware update.
        Close the device if needed.

        Can throw C++ exceptions.
        */
        virtual void CleanupAfterFirmwareUpdate() = 0;


        /**
        \brief Returns a display name for this camera. This is used for display/logging purposes.
        
        Throws C++ exceptions if not available.
        */
        virtual GENICAM_NAMESPACE::gcstring GetDisplayName() = 0;
    };
}

#endif // GENFW_FIRMWAREUPDATEABLEDEVICE_H
