/****************************************************************************
(c) 2004 by Basler Vision Technologies
(c) 2005 STEMMER IMAGING

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


****************************************************************************/

/// \file GenICamFwd.h
/// \brief Forward declarations for GenICam types.
/// \ingroup Base_PublicUtilities

#ifndef GENICAM_FWD_H
#define GENICAM_FWD_H

#ifndef GENICAM_INTERFACE
#   define GENICAM_INTERFACE struct
#endif
#include <Base/GCNamespace.h>
#include <GenApi/GenApiNamespace.h>

// GCBase
namespace GENICAM_NAMESPACE
{
    class gcstring;
    class gcstring_vector;
}

// GenApi
namespace GENAPI_NAMESPACE
{
    GENICAM_INTERFACE IBase;
    GENICAM_INTERFACE IBoolean;
    GENICAM_INTERFACE ICategory;
    GENICAM_INTERFACE IChunkPort;
    GENICAM_INTERFACE ICommand;
    GENICAM_INTERFACE IDeviceInfo;
    GENICAM_INTERFACE IEnumEntry;
    GENICAM_INTERFACE IEnumeration;
    GENICAM_INTERFACE IFloat;
    GENICAM_INTERFACE IInteger;
    GENICAM_INTERFACE INode;
    GENICAM_INTERFACE INodeMap;
    GENICAM_INTERFACE IPort;
    GENICAM_INTERFACE IPortConstruct;
    GENICAM_INTERFACE IPortStackedConstruct;
    GENICAM_INTERFACE IPortRecorder;
    GENICAM_INTERFACE IPortReplay;
    GENICAM_INTERFACE IPortWriteList;
    GENICAM_INTERFACE IRegister;
    GENICAM_INTERFACE ISelector;
    GENICAM_INTERFACE IString;
    GENICAM_INTERFACE IUserData;
    GENICAM_INTERFACE IValue;

    class CLock;
}

#endif // GENICAM_FWD_H
