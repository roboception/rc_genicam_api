//-----------------------------------------------------------------------------
//  (c) 2007 by National Instruments
//  Project: GenApi
//  Author:  Eric Gross
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
\brief    Definition of interface IPersistScript and class CFeatureBag
\ingroup GenApi_PublicUtilities
*/

#ifndef _GENICAM_PERSISTENCE_H
#define _GENICAM_PERSISTENCE_H

#include <GenApi/Types.h>
#include <GenApi/Pointer.h>
#include <GenApi/GenApiDll.h>
#include <list>
#include <iostream>

using namespace GENICAM_NAMESPACE;

namespace GENAPI_NAMESPACE
{

    //! Basic interface to persist values to
    interface GENAPI_DECL_ABSTRACT IPersistScript
    {
        //! sets information about the node map
        virtual void SetInfo(gcstring &Info) = 0;

        //! Stores a feature
        virtual void PersistFeature(IValue& item) = 0;
    };

    //! Bag holding streamable features of a nodetree
    class GENAPI_DECL CFeatureBag : public IPersistScript
    {
    public:
        //! sets information about the node map
        virtual void SetInfo(gcstring &Info);

        //! Stores a feature
        virtual void PersistFeature(IValue& item);

        //! Loads the features from the bag to the node tree
        //! \param pNodeMap  The node map
        //! \param Verify If true, all streamable features are read back
        //! \param pErrorList If an error occurs during loading the error message is stored in the list and the loading continues
        /*! For Verify=true the list of names in the feature bag is replayed again.
            If a node is a selector it's value is set to the value from the feature bag
            If not the value is read from the camera and compared with the value from the feature bag.
        */
        bool LoadFromBag(INodeMap *pNodeMap, bool Verify = true, gcstring_vector *pErrorList = NULL);

        /*! \brief Stores the streamable nodes to this feature bag
            \param pNodeMap The node map to persist
            \param MaxNumPersistSkriptEntries The max number of entries in the container; -1 means unlimited
            \return number of entries in the bag
        */
        int64_t StoreToBag(INodeMap *pNodeMap, const int MaxNumPersistSkriptEntries = -1 );

        //! compares the content of two feature bags
        bool operator==(const CFeatureBag &FeatureBag) const;

        gcstring ToString();

        //! fills the bag from a stream
        friend std::istream& operator >>(std::istream &is, CFeatureBag &FeatureBag);

        //! puts the bag into a stream
        friend std::ostream& operator <<(std::ostream &os, const CFeatureBag &FeatureBag);

    private:
        //! The features are stored in a list of string pairs = {Name, Value}
        gcstring_vector m_Names;

        //! The features are stored in a list of string pairs = {Name, Value}
        gcstring_vector m_Values;

        //! String describing the node map
        gcstring m_Info;
    };

    //! the magic GUID which indicates that the file is a GenApi stream file Must be the first entry
    #define GENAPI_PERSISTENCE_MAGIC   "{05D8C294-F295-4dfb-9D01-096BD04049F4}"

    //! Helper function ignoring lines starting with comment character '#'
    // note: this method must be inlined because it uses istream in the parameter list
    inline std::istream& EatComments(std::istream &is)
    {
        if( is.eof() )
            return is;

        char FirstCharacter;
        FirstCharacter = (char)is.peek();

        while( FirstCharacter == '#' )
        {
            is.ignore(1024, '\n');
            FirstCharacter = (char)is.peek();
        }
        return is;
    }

    //! reads in persistent data from a stream
    // note: this method must be inlined because it uses istream in the parameter list
    // note: May not be used as inline if called against a library where it is already compiled.
    inline std::istream& operator >>(std::istream &is, CFeatureBag &FeatureBag)
    {
        if( is.eof() )
            throw RUNTIME_EXCEPTION("The stream is eof");

        FeatureBag.m_Names.clear();
        FeatureBag.m_Values.clear();

        const int BufferSize = 1024;
        char Buffer[BufferSize] = {0};
        char Name[BufferSize] = {0};
        gcstring Value("");

        // Check the magic
        is.getline(Buffer, BufferSize, '\n');
        gcstring FirstLine(Buffer);
        gcstring MagicGUID(GENAPI_PERSISTENCE_MAGIC);
        if( gcstring::_npos() == FirstLine.find(MagicGUID) )
            throw RUNTIME_EXCEPTION("The stream is not a GenApi feature stream since it is missing the magic GUID in the first line");

        EatComments( is );
        while( !is.eof() )
        {
            is.getline(Name, BufferSize, '\t');
            if (is.fail())  // if reading from stream failed -> stop reading!
                break;

            GENICAM_NAMESPACE::getline(is, Value);
            if (is.fail())  // if reading from stream failed -> stop reading!
                break;

            FeatureBag.m_Names.push_back(Name);
            FeatureBag.m_Values.push_back(Value);

            Name[0] = '\0';
            Value = "";
            EatComments( is );
        }
        return is;
    }

    //! writes out persistent data to a stream
    // note: this method must be inlined because it uses ostream in the parameter list
    // note: May not be used as inline if called against a library where it is already compiled.
    inline std::ostream& operator <<(std::ostream &os, const CFeatureBag &FeatureBag)
    {
        os << "# " GENAPI_PERSISTENCE_MAGIC "\n";
        os << "# GenApi persistence file (version " << GENAPI_VERSION_MAJOR << "." << GENAPI_VERSION_MINOR << "." << GENAPI_VERSION_SUBMINOR << ")\n";
        os << "# " << FeatureBag.m_Info << "\n";

        gcstring_vector::const_iterator pName = FeatureBag.m_Names.begin();
        gcstring_vector::const_iterator pValue = FeatureBag.m_Values.begin();
        assert(FeatureBag.m_Names.size() == FeatureBag.m_Values.size());
        if (FeatureBag.m_Names.size() == FeatureBag.m_Values.size())
        {
            const gcstring_vector::const_iterator endNames = FeatureBag.m_Names.end();
            for ( /**/; pName != endNames; ++pName, ++pValue)
            {
                gcstring Name(*pName);
                gcstring Value(*pValue);
                os << Name << "\t" << Value << "\n";
            }
        }

        return os;
    }

}

#endif //_GENICAM_PERSISTENCE_H