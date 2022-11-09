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
\brief    Declaration of CFirmwareUpdateInfo
*/
#ifndef GENFW_FIRMWAREUPDATEINFO_H
#define GENFW_FIRMWAREUPDATEINFO_H

#include <GenICamFwd.h>
#include <Base/GCString.h>
#include <FirmwareUpdate/FirmwareUpdateDll.h>
#include <FirmwareUpdate/FirmwareUpdateLinkage.h>



namespace GENAPI_NAMESPACE
{
    // Note: Using static const char* here to allow delay loading the library.
    static const char* const FirmwareUpdateInfoKey_Description = "Description"; ///< Key for the description of the update.
    static const char* const FirmwareUpdateInfoKey_Version = "Version";  ///< Key for the version of the update.
    static const char* const FirmwareUpdateInfoKey_VersionStyle = "VersionStyle";  ///< Key for the format name of the version value. This key is optional and may not be present.
    static const char* const FirmwareUpdateInfoValue_VersionStyle_Dotted = "dotted";  ///< A possible value for the version style key.
    static const char* const FirmwareUpdateInfoValue_VersionStyle_SemVer = "semver";  ///< A possible value for the version style key.
    static const char* const FirmwareUpdateInfoKey_VersionExtractionPattern = "VersionExtractionPattern"; ///< Key for the regular expression to extract the device version from the DeviceFirmwareVersion node value of a device.
                                                                                                    ///< This key is optional and may not be present.
    static const char* const FirmwareUpdateInfoKey_ReleaseNotes = "ReleaseNotes"; ///< Key for the release notes of the update. This key is optional and may not be present.
    static const char* const FirmwareUpdateInfoKey_ReleaseNotesURL = "ReleaseNotesURL"; ///< Key for a link to a web page with more release notes. This key is optional and may not be present.
    static const char* const FirmwareUpdateInfoKey_UserSetPersistence = "UserSetPersistence";  ///< Key for the support of user set persistence
    static const char* const FirmwareUpdateInfoKey_SequencerSetPersistence = "SequencerSetPersistence"; ///< Key for the support of sequencer set persistence
    static const char* const FirmwareUpdateInfoKey_PersistenceSupport_None = "none"; ///< A possible value for the persistence support
    static const char* const FirmwareUpdateInfoKey_PersistenceSupport_Full = "full"; ///< A possible value for the persistence support




    /**
    \brief Identifies a single firmware update and holds information about the firmware update.

    <ul>
    <li> Identifies a single firmware update.
    <li> All data held is constant and therefore never changes.
    <li> Holds the preconditions for uploading a FW update package. The actual preconditions for an update are not accessible.
    <li> Holds displayable information.
    <li> Supports internationalization.
    <li> Can be stored in container classes, e.g. std::vector. This helps to keep one instance in different containers to support
         creating different views on update info when handling multiple updates and update files.
    </ul>

    The CFirmwareUpdateInfo class is not thread-safe.
    */
    class GENFW_DECL CFirmwareUpdateInfo
    {
    public:
        /**
        \brief Creates an empty CFirmwareUpdateInfo object.
        CFirmwareUpdateInfo::IsValid() will return false. Valid CFirmwareUpdateInfo() objects are created by calling CFirmwareUpdater::ReadFirmwareUpdateInfos().

        Does not throw C++ exceptions, except when memory allocation fails.
        */
        CFirmwareUpdateInfo();


        /**
        \brief Creates a copy of CFirmwareUpdateInfo object.
        \param[in]  Rhs  The object to copy.
        The CFirmwareUpdateInfo class uses reference counting. It is freely copyable without copying the held data.
        This helps to keep this class in different containers to support creating different views on update info when handling multiple updates and update files.

        Does not throw C++ exceptions.
        */
        CFirmwareUpdateInfo(const CFirmwareUpdateInfo& Rhs);


        /**
        \brief Creates a copy of CFirmwareUpdateInfo object.
        \param[in]  Rhs  The object to assign.
        The CFirmwareUpdateInfo class uses reference counting. It is freely copyable without copying the held data.
        This helps to keep this class in different containers to support creating different views on update info when handling multiple updates and update files.

        Does not throw C++ exceptions.
        */
        CFirmwareUpdateInfo& operator=(const CFirmwareUpdateInfo& Rhs);


        /**
        \brief Destroys the CFirmwareUpdateInfo object.
        
        Does not throw C++ exceptions.
        */
        ~CFirmwareUpdateInfo();


        /**
        \brief Compares two CFirmwareUpdateInfo objects. Returns true if absolute identical information is held.
        \param[in]  Rhs  The object to compare with this object.
        \return Returns true if absolute identical information is held.

        Does not throw C++ exceptions.
        */
        bool operator==(const CFirmwareUpdateInfo& Rhs) const;


        /**
        \brief Checks if valid information is held.
        \return Returns true if the class contains valid information.
        Objects created by calling CFirmwareUpdater::ReadFirmwareUpdateInfos() are valid.
        Objects created only by the constructor CFirmwareUpdateInfo() are not valid.
        
        Does not throw C++ exceptions.
        */
        bool IsValid() const;


        /**
        \brief Returns the source file of an firmware update.
        \return Returns the source file of an firmware update. If the IsValid() is false an empty string is returned.

        Does not throw C++ exceptions.
        */
        const char* GetUpdateFilename() const;


        /**
        \brief Returns the value for an info key name.
        \param[in]  InfoKey  The key name, e.g. FirmwareUpdateInfoKey_Description.
        \return Returns the value for an info key name. Returns an empty string if the key it is not available.

        Does not throw C++ exceptions.
        */
        const char* GetInfo(const char* InfoKey) const;


        /**
        \brief Returns the value for an info key name.
        \param[in]  InfoKey  The key name, e.g. FirmwareUpdateInfoKey_Description.
        \param[in]  pDefaultValueIfNotAvailable  The default to return if the key it is not available.
        \return Returns the value for an info key name. Returns \c pDefaultValueIfNotAvailable if the key it is not available.

        Does not throw C++ exceptions.
        */
        const char* GetInfo(const char* InfoKey, const char* pDefaultValueIfNotAvailable) const;


        /**
        \brief Returns the count of available key value pairs.
        \return Returns the count of available key value pairs. The count can be 0 if the object is empty, see IsValid().

        Does not throw C++ exceptions.
        */
        size_t GetCountOfInfos() const; //can be 0


        /**
        \brief Returns the key name for the given index.
        \param[in]  Index  The key value pair index.
        \return Returns the key name for the given index. Never returns NULL.

        Throws an \c InvalidArgumentException if the given \c Index is out of bounds.
        */
        const char* GetInfoKey(size_t Index) const;


        /**
        \brief Returns the value for the given index.
        \param[in]  Index  The key value pair index.
        \return Returns the value for the given index. Never returns NULL.

        Throws an \c InvalidArgumentException if the given \c Index is out of bounds.
        */
        const char* GetInfo(size_t Index) const;


        /**
        \brief Returns the ISO 639-1 language code for a key name, e.g. "de".
        \param[in]  InfoKey  The key name, e.g. FirmwareUpdateInfoKey_Description.
        \return Returns the language code for the given key name. Returns an empty string if the key cannot be found or if the language code is undefined.

        Does not throw C++ exceptions.
        */
        const char* GetInfoLanguageCode(const char* InfoKey) const;


        /**
        \brief Extracts the firmware version using the device firmware version and the regular expression provided by the key FirmwareUpdateInfoKey_VersionExtractionPattern.
        \param[in]  DeviceFirmwareVersionFromDevice  The device version from the DeviceFirmwareVersion node value of a device.
        \return The extracted firmware version with the style defined by FirmwareUpdateInfoKey_VersionStyle.
        The returned value can be used for comparing firmware versions on a device to the version provided by an firmware update.

        Throws a RuntimeException if the regular expression match fails.
        Throws a LogicalErrorException if the keys FirmwareUpdateInfoKey_VersionStyle and FirmwareUpdateInfoKey_VersionExtractionPattern are not available.
        */
        GENICAM_NAMESPACE::gcstring ExtractVersionFromDeviceFirmwareVersion(const GENICAM_NAMESPACE::gcstring& DeviceFirmwareVersionFromDevice) const;

    protected:
        // Hide the details:
        class CFirmwareUpdateInfoImpl;
        CFirmwareUpdateInfoImpl* m_pImpl;
    };
}

#endif // GENFW_FIRMWAREUPDATEINFO_H
