//-----------------------------------------------------------------------------
//  (c) 2008 by Basler Vision Technologies
//  Section: Vision Components
//  Project: GenApi
//  Author:  Fritz Dierks
//  $Header$
//
//  License: This file is published under the license of the EMVA GENICAM_NAMESPACE  Standard Group. 
//  A text file describing the legal terms is included in  your installation as 'GenICam_license.pdf'. 
//  If for some reason you are missing  this file please contact the EMVA or visit the website
//  (http://www.GENICAM_NAMESPACE.org) for a full copy.
// 
//  THIS SOFTWARE IS PROVIDED BY THE EMVA GENICAM_NAMESPACE STANDARD GROUP "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,  
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR  
//  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE EMVA GENICAM_NAMESPACE STANDARD  GROUP 
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
*/
#ifndef CLPROTOCOL_DEVICEID_H
#define CLPROTOCOL_DEVICEID_H

#include <Base/GCBase.h>
#include <Base/GCUtilities.h>
#include <GenApi/PortImpl.h>
#include <CLProtocol/ClAllSerial.h>
#include <CLProtocol/CLProtocol.h>
#include <CLProtocol/ISerial.h>
#include <CLProtocol/CVersion.h>
#include <iostream>
#include <fstream>

namespace CLProtocol
{

    //! Helper class which can split DeviceIDs strings into tokens
    /*! \ingroup PublicInterfaces */
    class CLPROTOCOL_DECL CDeviceID 
    {
    public:
        //! returns the token separator string
        static const char* GetTokenSeparator();

        //! returns the token separator string
        /** \deprecated This function has been declared <b>deprecated</b> because of a typo in the functions name. Use <b>CDeviceID::GetTokenSeparator()</b> instead.
         */
        GENICAM_DEPRECATED( static const char* GetTokenSeperator() );

    public:
        //! Constructor
        CDeviceID();

        //! parse string
        bool FromString(const GENICAM_NAMESPACE::gcstring &DeviceID);

        //! retrieves the directory the CL protocol driver is located in 
        /*!
            \param ReplaceEnvironmentVariables If true environment variables like $(GENICAM_CLPROTOCOL) 
                   are replaced by their value.
        */
        GENICAM_NAMESPACE::gcstring GetDriverDirectory(const bool ReplaceEnvironmentVariables = true) const;

        //! retrieves the file name of the CL protocol driver 
        GENICAM_NAMESPACE::gcstring GetDriverFileName() const;

        //! retrieves the path of the CL protocol driver is located in 
        GENICAM_NAMESPACE::gcstring GetDriverPath() const;

        //! retrieves the manufacturer name of the camera
        GENICAM_NAMESPACE::gcstring GetCameraManufacturer() const;

        //! retrieves the camera family (may return empty string in case of a template)
        GENICAM_NAMESPACE::gcstring GetCameraFamily() const;

        //! retrieves the camera model (may return empty string in case of a template)
        GENICAM_NAMESPACE::gcstring GetCameraModel() const;

        //! retrieves the camera model's version in any format (may return empty string in case of a template)
        GENICAM_NAMESPACE::gcstring GetCameraVersion() const;

        //! retrieves the camera model's serial number in any format (may return empty string in case of a template)
        GENICAM_NAMESPACE::gcstring GetCameraSerialNumber() const;

        //! retrieves the ShortDeviceID/DeviceIDTemplate without the DriverDriectory and the Driver FileName in front
        GENICAM_NAMESPACE::gcstring GetShortDeviceID() const;

        //! retrieves the DeviceID without the DriverDriectory and the Driver FileName in front
        GENICAM_NAMESPACE::gcstring GetDeviceID(const bool ReplaceEnvironmentVariables = true) const;

    private:
        //! The positions of the tokens within the DeviceID string
        typedef enum
        {
            didDriverDirectory = 0,
            didDriverFileName = 1,
            didCameraManufacturer = 2,
            didCameraFamily = 3,
            didCameraModel = 4,
            didCameraVersion = 5,
            didCameraSerialNumber = 6
        } EDeviceIDIndex;

        //! The tokens which form the DeviceID
        GENICAM_NAMESPACE::gcstring_vector m_DeviceIDTokens;
    };
}
#endif // CLPROTOCOL_DEVICEID_H
