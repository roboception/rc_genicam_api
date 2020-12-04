//-----------------------------------------------------------------------------
//  (c) 2019 by Matrox
//  Project:  GenApi
//  Author:  Stephane Maurice
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
//  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY  THEORY OF LIABILITY,
//  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------
/**
\file
\brief    Declaration of the CChunkAdapterGenDC class.
*/

#ifndef GENAPI_CHUNKADAPTERGENDC_H
#define GENAPI_CHUNKADAPTERGENDC_H

#include <GenApi/ChunkAdapter.h>
#include <GenApi/ChunkAdapterU3V.h>

namespace GENAPI_NAMESPACE
{

    /* ------------------------------------------- */
    // Declaration of the GenDC Chunk trailing tag.

    // Make sure everything is properly packed
#   pragma pack(push, 1)

    //! Header of a GenDC chunk trailing tag.
    typedef struct GENDC_CHUNK_TRAILER
    {
        uint32_t ChunkID;
        uint32_t ChunkLength;
    } GENDC_CHUNK_TRAILER;

    // Restore the previous packing
#   pragma pack(pop)
    /* ------------------------------------------- */
    class CChunkAdapterU3V;

    //! Connects a GenDC Metadata GenICam Chunk Part buffer to a node map.
    class GENAPI_DECL CChunkAdapterGenDC : public  CChunkAdapterU3V
    {
    public:
        //! Constructor
        CChunkAdapterGenDC( INodeMap* pNodeMap = NULL, int64_t MaxChunkCacheSize = -1 );

        //! Destructor
        virtual ~CChunkAdapterGenDC();
    private:
        CChunkAdapterGenDC& operator=( const CChunkAdapterGenDC & rhs );
        CChunkAdapterGenDC( const CChunkAdapterGenDC &other );
    };
}

#endif // GENAPI_CHUNKADAPTERGENDC_H
