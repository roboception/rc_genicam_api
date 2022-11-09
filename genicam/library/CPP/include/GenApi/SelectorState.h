//-----------------------------------------------------------------------------
//  (c) 2019 by Teledyne Digital Imaging Inc.
//  Section: DALSA
//  Project: GenApi
//  Author:  Eric Bourbonnais
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
*/

#ifndef GENAPI_SELECTORSTATE_H
#define GENAPI_SELECTORSTATE_H

#include <GenApi/Types.h>
#include <GenApi/IValue.h>

namespace GENAPI_NAMESPACE
{

    class GENAPI_DECL CSelectorState
    {
        GENICAM_NAMESPACE::gcstring_vector m_Names;
        GENICAM_NAMESPACE::gcstring_vector m_Values;

        size_t m_position;

    public:
        CSelectorState();

        /*! \brief Sets the state to the first selector.
        \return true if the resulting value is valid */
        bool SetFirst();

        /*! \brief Sets state to next selector
        \return true if the resulting value is valid */
        bool SetNext();

        /*! \brief Add a node the the selector state*/
        void AddSelector(IValue& item);
        
        /*! \brief Add a node the the selector state*/
        void AddSelector(GENICAM_NAMESPACE::gcstring name, 
                         GENICAM_NAMESPACE::gcstring value);

        /*! \brief Get the name of the current selector */
        GENICAM_NAMESPACE::gcstring GetNodeName();

        /*! \brief Get the value of the current selector */
        GENICAM_NAMESPACE::gcstring GetNodeValue();

        /*! \brief Sets state to next selector
        \return true if the selector states is empty */
        bool IsEmpty();

        /*! \brief Sets the selector of the nodemap to the saved state. 
        \param pNodeMap The node map to load into 
        \param pErrorList If an error occurs during loading the error message is stored in the list and the loading continues */
        bool RestoreState(INodeMap *pNodeMap, GENICAM_NAMESPACE::gcstring_vector *pErrorList = NULL);
    };

}
#endif // GENAPI_SELECTORSTATE_H
