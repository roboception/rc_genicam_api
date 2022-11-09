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
\brief    Declaration of CFirmwareUpdater
*/
#ifndef GENFW_FIRMWAREUPDATER_H
#define GENFW_FIRMWAREUPDATER_H

#include <GenICamFwd.h>
#include <FirmwareUpdate/FirmwareUpdateDll.h>
#include <FirmwareUpdate/FirmwareUpdateLinkage.h>
#include <FirmwareUpdate/FirmwareUpdateInfoCollector.h>
#include <FirmwareUpdate/FirmwareUpdateProgressObserver.h>
#include <FirmwareUpdate/FirmwareUpdatableDevice.h>

namespace GENAPI_NAMESPACE
{
    /**
    \brief The CFirmwareUpdater is responsible for running firmware updates.

    The CFirmwareUpdater class is not thread-safe.
    */
    class GENFW_DECL CFirmwareUpdater
    {
    public:
        /**
        \brief Creates a CFirmwareUpdater object.

        Does not throw C++ exceptions, except when memory allocation fails.
        */
        CFirmwareUpdater();


        /**
        \brief Destroys a CFirmwareUpdater object.

        Does not throw C++ exceptions.
        */
        ~CFirmwareUpdater();


        /**
        \brief This method provides information required for running a signature check for a guf file.
        \param[in]  Filename  The path and name of the guf file.
        \param[out]  OffsetPackageZipFile  The offset of the package.zip file in the guf file container.
        \param[out]  SizePackageZipFile  The size of the package.zip file in the guf file container.
        \param[out]  OffsetSignatureFile  The offset of the package.p7s file in the guf file container.
                                          Can be 0 if there is no signature available.
        \param[out]  SizeSignatureFile  The size of the package.p7s file in the guf file container.
                                        Can be 0 if there is no signature available.

        Throws a FileNotFoundException if the file with name \c Filename cannot be found.
        Throws a FileIOException if reading the file fails.
        Throws a FormatException if the file is not of GUF format, e.g. is compressed, mandatory info entries are missing etc. 
        Throws a BadAllocException if memory allocation fails.
        */
        void GetSignatureCheckInfo(const GENICAM_NAMESPACE::gcstring& Filename,
            size_t& OffsetPackageZipFile,
            size_t& SizePackageZipFile,
            size_t& OffsetSignatureFile,
            size_t& SizeSignatureFile
            ) const;


        /**
        \brief This method provides information required for running a signature check for a guf file.
        \param[in]  Filename  The path and name of the guf file.
        \param[in]  FirmwareUpdateInfoCollector  An object receiving a callback each time an update is found in a guf file.
        \param[in]  pPreferredLanguageCode  The preferred ISO 639-1 language code. 
                                            Falls back, if not available, to: first "en", then no language code, then the first found entry.

        Throws a FileNotFoundException if the file with name \c Filename cannot be found.
        Throws a FileIOException if reading the file fails.
        Throws a FormatException if the file is not of GUF format, e.g. is compressed, mandatory info entries are missing etc.
        Throws a BadAllocException if memory allocation fails.
        */
        void ReadFirmwareUpdateInfos(
            const GENICAM_NAMESPACE::gcstring& Filename,
            IFirmwareUpdateInfoCollector& FirmwareUpdateInfoCollector,
            const char* pPreferredLanguageCode = NULL) const;


        /**
        \brief Checks if a firmware update represented by \c FirmwareUpdateInfo can be applied.
        \param[in]  pNodeMapOfOpenDevice  The node map of an open device.
        \param[in]  FirmwareUpdateInfo  A firmware update info object representing a firmware update.
        \return Returns true if the firmware update can be applied. Returns false if the firmware update object is not valid.

        Throws an InvalidArgumentException if \c pNodeMapOfOpenDevice is NULL.
        */
        bool FirmwareUpdateMatchesDevice(
            INodeMap *pNodeMapOfOpenDevice,
            const CFirmwareUpdateInfo& FirmwareUpdateInfo) const;


        /**
        \brief Runs the firmware update process for a device.
        \param[in]  pDeviceToUpdate  The device to update wrapped by an object implementing IFirmwareUpdatableDevice.
        \param[in]  FirmwareUpdateInfo  The firmware update info object representing a firmware update to apply.
        \param[in]  pProgressObserver  A progress listener implementing pProgressObserver.
        \param[in]  pPreferredLanguageCode  The preferred ISO 639-1 language code.
                                            Falls back, if not available, to: first "en", then no language code, then the first found entry.

        Double checks if the device can be updated by firmware update info.
        Can update another device after failure.

        Throws a FileNotFoundException if the guf file with specified by \c FirmwareUpdateInfo cannot be found.
        Throws a FileIOException if reading the guf file fails.
        Throws a RuntimeException if the guf file has been tampered with since reading UpdateInfos or if the loading of a user set backup to the node map fails.
        Throws an InvalidArgumentException if \c pDeviceToUpdate is NULL or the \c FirmwareUpdateInfo does not match the device.
        Throws C++ exceptions if accessing the device fails.
        */
        void ApplyUpdate(
            IFirmwareUpdatableDevice* pDeviceToUpdate,
            const CFirmwareUpdateInfo& FirmwareUpdateInfo,
            IFirmwareUpdateProgressObserver* pProgressObserver = NULL,
            const char* pPreferredLanguageCode = NULL);


        /**
        \brief Validates a guf file.
        \param[in]  Filename  The path and name of the guf file.
        
        \note Unused files ore procedures that are part of the guf file are currently tolerated.

        Throws a FileNotFoundException if a required file cannot be found.
        Throws a FileIOException when there was an error during extraction.
        Throws a FormatException when mandatory information is missing.
        Throws a RuntimeException when the XML data is malformed.
        */
        void VerifyGufFile(const GENICAM_NAMESPACE::gcstring& Filename);

    private:
        // Not copyable
        CFirmwareUpdater(const CFirmwareUpdater&);
        CFirmwareUpdater& operator =(const CFirmwareUpdater&);

        // Hide the details:
        class CFirmwareUpdaterImpl;
        CFirmwareUpdaterImpl* m_pImpl;
    };
}

#endif // GENFW_FIRMWAREUPDATER_H
