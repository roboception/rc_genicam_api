//-----------------------------------------------------------------------------
//  (c) 2017 by Teledyne DALSA
//  Section: Digital Imaging
//  Project: GenAPI
//  Author:  Eric Bourbonnais
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
//
/*!
\file
\brief   This file contains the public definition of the node write concatenator classes.
\ingroup GenApi_PublicInterface
*/

#ifndef GENAPI_NODE_WRITE_CONCATENATOR_H
#define GENAPI_NODE_WRITE_CONCATENATOR_H

namespace GENAPI_NAMESPACE
{
 
    class GENAPI_DECL CNodeWriteConcatenator
    {
    protected:
        //! Destructor (Prevent freeing the object, need to call Destroy)
        virtual  ~CNodeWriteConcatenator(void) = 0;
    public:
        //! Add a write with value of type gcstring.
        virtual  void Add(const GENICAM_NAMESPACE::gcstring &nodeName, const GENICAM_NAMESPACE::gcstring &nodeValue) = 0;
        //! Add a write with value of type char *.
        virtual  void Add(const GENICAM_NAMESPACE::gcstring &nodeName, const char *) = 0;
        //! Add a write with value of type int64_t.
        virtual  void Add(const GENICAM_NAMESPACE::gcstring &nodeName, const int64_t nodeValue) = 0;
        //! Add a write with value of type double.
        virtual  void Add(const GENICAM_NAMESPACE::gcstring &nodeName, const double nodeValue) = 0;
        //! Add a write with value of type bool.
        virtual  void Add(const GENICAM_NAMESPACE::gcstring &nodeName, const bool nodeValue) = 0;
        //! Remove all write from the container.
        virtual  void Clear() = 0;
        //! //! Destroys this container
        virtual  void Destroy() = 0;
    };

    /**
    \brief Reference object for CNodeWriteConcatenator create with NewNodeWriteConcatenator function of nodemap objects.
    */

    class GENAPI_DECL CNodeWriteConcatenatorRef
    {
    public:
        //! Constructor
        CNodeWriteConcatenatorRef() : m_pConcatenator(NULL) 
        {
        };
        //! Constructor
        CNodeWriteConcatenatorRef(CNodeWriteConcatenator *pConcatenator) : m_pConcatenator(pConcatenator) 
        {
        };
        //! Destructor
        ~CNodeWriteConcatenatorRef()
        {
            if (m_pConcatenator)
                m_pConcatenator->Destroy();
            m_pConcatenator = NULL;

        }

        //! Add a write with value of type gcstring.
        void _Add(const GENICAM_NAMESPACE::gcstring &nodeName, const GENICAM_NAMESPACE::gcstring &nodeValue)
        {
            if (m_pConcatenator)
                m_pConcatenator->Add(nodeName, nodeValue);
            else
                throw ACCESS_EXCEPTION("Concatenator not present (reference not valid)");
        }
        //! Add a write with value of type char *.
        void _Add(const GENICAM_NAMESPACE::gcstring &nodeName, const char *nodeValue)
        {
            if (m_pConcatenator)
                m_pConcatenator->Add(nodeName, nodeValue);
            else
                throw ACCESS_EXCEPTION("Concatenator not present (reference not valid)");
        }
        //! Add a write with value of type int64_t.
        void _Add(const GENICAM_NAMESPACE::gcstring &nodeName, const int64_t nodeValue)
        {
            if (m_pConcatenator)
                m_pConcatenator->Add(nodeName, nodeValue);
            else
                throw ACCESS_EXCEPTION("Concatenator not present (reference not valid)");
        }
        //! Add a write with value of type double.
        void _Add(const GENICAM_NAMESPACE::gcstring &nodeName, const double nodeValue)
        {
            if (m_pConcatenator)
                m_pConcatenator->Add(nodeName, nodeValue);
            else
                throw ACCESS_EXCEPTION("Concatenator not present (reference not valid)");
        }
        //! Add a write with value of type bool.
        void _Add(const GENICAM_NAMESPACE::gcstring &nodeName, const bool nodeValue)
        {
            if (m_pConcatenator)
                m_pConcatenator->Add(nodeName, nodeValue);
            else
                throw ACCESS_EXCEPTION("Concatenator not present (reference not valid)");
        }
        //! Remove all write from the container.
        void _Clear()
        {
            if (m_pConcatenator)
                m_pConcatenator->Clear();
            else
                throw ACCESS_EXCEPTION("Concatenator not present (reference not valid)");
        }
        //! Release owenership of the write concatenator object.
        CNodeWriteConcatenator *Release()
        {
            CNodeWriteConcatenator *tmp = m_pConcatenator;
            m_pConcatenator = NULL;
            return tmp;
        }
        //! Provide access to the object owned by *this.
        operator CNodeWriteConcatenator * () const
        {
            if (m_pConcatenator)
                return m_pConcatenator;
            else
                throw ACCESS_EXCEPTION("Concatenator not present (reference not valid)");
        }
        //! Returns a pointer tthe object owned by *this or nullptr if no object is owned.
        CNodeWriteConcatenator *get() const
        {
            return m_pConcatenator;
        }

        //! Take ownership of a new object. Release the currently owned object if any.
        CNodeWriteConcatenatorRef& operator = (CNodeWriteConcatenator *pConcatenator)
        {

            if (m_pConcatenator)
                m_pConcatenator->Destroy();
            // Take ownership of the object
            m_pConcatenator = pConcatenator;
            return *this;
        }

    private:
        CNodeWriteConcatenator *m_pConcatenator;
    };

}

#endif // GENAPI_NODE_WRITE_CONCATENATOR_H
