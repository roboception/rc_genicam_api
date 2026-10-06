//-----------------------------------------------------------------------------
//  (c) 2019-2022 by MVTec Software GmbH
//  Author:  Jan Becvar
//
//  License: This file is published under the license of the EMVA GenICam  Standard Group.
//  A text file describing the legal terms is included in  your installation as 'GenICam_license.pdf'.
//  If for some reason you are missing  this file please contact the EMVA or visit the website
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
//  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE)
//  ARISING IN ANY WAY OUT OF THE USE  OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//  
//-----------------------------------------------------------------------------
/**
\file
\brief    Declaration of the CChunkAdapterUniversal class.
*/

#ifndef GENAPI_CHUNKADAPTERUNIVERSAL_H
#define GENAPI_CHUNKADAPTERUNIVERSAL_H

#include <GenApi/ChunkAdapter.h>
#include <GenApi/ChunkAdapterGeneric.h>


namespace GENAPI_NAMESPACE
{
    //! Connects a universal chunked buffer to a node map
    class GENAPI_DECL CChunkAdapterUniversal : public CChunkAdapter
    {

    public:
        //! Constructor
        CChunkAdapterUniversal(INodeMap* pNodeMap = NULL, int64_t MaxChunkCacheSize = -1);

        //! Destructor
        virtual ~CChunkAdapterUniversal();

        // Does not have implementation, use one of the specific versions if needed
        virtual bool CheckBufferLayout(uint8_t *pBuffer, int64_t BufferLength);

        // Buffer layout checkers for big/little-endian standard chunk formats
        bool CheckBufferLayoutBE(const uint8_t *pBuffer, int64_t BufferLength);
        bool CheckBufferLayoutLE(const uint8_t *pBuffer, int64_t BufferLength);

        // Does not have implementation, use one of the specific versions
        virtual void AttachBuffer(uint8_t *pBuffer, int64_t BufferLength, AttachStatistics_t *pAttachStatistics = NULL);

        // Attach buffer containing standard big/little-endian chunk formats
        void AttachBufferBE(const uint8_t *pBuffer, int64_t BufferLength, AttachStatistics_t *pAttachStatistics = NULL);
        void AttachBufferLE(const uint8_t *pBuffer, int64_t BufferLength, AttachStatistics_t *pAttachStatistics = NULL);

        // Named versions referring to particular standards defining the chunk formats
        void AttachBufferGEV(const uint8_t *pBuffer, int64_t BufferLength, AttachStatistics_t *pAttachStatistics = NULL);
        void AttachBufferU3V(const uint8_t *pBuffer, int64_t BufferLength, AttachStatistics_t *pAttachStatistics = NULL);
        void AttachBufferGenDC(const uint8_t *pBuffer, int64_t BufferLength, AttachStatistics_t *pAttachStatistics = NULL);

        // Generic version for use with GenTL-based chunk parsing functions for buffer with unknown format
        void AttachBufferGeneric(const uint8_t *pBuffer, SingleChunkData_t *ChunkData, int64_t NumChunks, AttachStatistics_t *pAttachStatistics = NULL);
        void AttachBufferGeneric(const uint8_t *pBuffer, SingleChunkDataStr_t *ChunkData, int64_t NumChunks, AttachStatistics_t *pAttachStatistics = NULL);
    };

}

#endif // GENAPI_CHUNKADAPTERUNIVERSAL_H
