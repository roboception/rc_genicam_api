//-----------------------------------------------------------------------------
//  (c) 2008 by Basler Vision Technologies
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
    \brief    C++ class wrapping CLSerialAll and CLProtocolDriver 
*/
#ifndef CLPROTOCOL_CVERSION_H
#define CLPROTOCOL_CVERSION_H

#include <Base/GCBase.h>
#include <CLProtocol/CLProtocol.h>

namespace CLProtocol
{

    //! Encapsulates a version number
    /*! \ingroup PublicInterfaces */
    class CLPROTOCOL_DECL CVersion
    {
    public:
        //! Constructor
        CVersion();

        //! Copy Constructor
        CVersion(const CVersion& Version);

        //! Copy Assignment
        CVersion& operator=(const CVersion& Version);

        //! Parse a Version string
        /*!
        Returns true if the parsing was successful.
        The following two forms of strings are allowed
        "SchemaVersion.1.2"
        "XMLVersion.1.2.3"
        */
        bool FromString(const GENICAM_NAMESPACE::gcstring strVersion);

        GENICAM_NAMESPACE::gcstring ToString();

        //! Comparison (for sorting)
        bool operator>(const CVersion& Version) const;
        bool operator<(const CVersion& Version) const;
        bool operator==(const CVersion& Version) const;


    protected:
        //! Major version number; if changed backward compatiblitity is broken
        int32_t m_Major;

        //! Minor version number; if changed backward compatiblitity is maintained
        int32_t m_Minor;

        //! SubMinor version number; bugfixes only - no new features
        int32_t m_SubMinor;

    };

}
#endif // CLPROTOCOL_CVERSION_H

