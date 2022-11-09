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
#ifndef CLPROTOCOL_CLAUTOBUFFER_H
#define CLPROTOCOL_CLAUTOBUFFER_H


namespace CLProtocol
{

    /*! \internal
        \brief Simple helper class to manage automatic deletion of arrays to provide exception safety see std:auto_ptr but works only with arrays mainly used for strings. Do not use directly
    */
    template<class T>
    class CLAutoBuffer
    {
    public:
        explicit CLAutoBuffer(T* p = NULL)
            : m_p(p)
        {
        }
        CLAutoBuffer(CLAutoBuffer<T>& rhs)
            : m_p(rhs.Release())
        {
        }
        ~CLAutoBuffer()
        {
            DeleteSafe();
        }
        CLAutoBuffer<T>& operator= (T* p)
        {
            if (m_p != p)
            {
                DeleteSafe();
                m_p = p;
            }

            return *this;
        }
        CLAutoBuffer<T>& operator= (CLAutoBuffer<T>& rhs)
        {
            if (m_p != rhs.m_p)
            {
                DeleteSafe();
                m_p = rhs.Release();
            }

            return *this;
        }
        T* operator->() const
        {
            return m_p;
        }
        T* Get() const
        {
            return m_p;
        }
        T* Release()
        {
            T* p(m_p);
            m_p = NULL;
            return p;
        }
    private:
        void DeleteSafe()
        {
            if (m_p != NULL)
            {
                delete[] m_p;
                m_p = NULL;
            }
        }
        T* m_p;   // the wrapped object pointer
    };


}
#endif // CLPROTOCOL_CLAUTOBUFFER_H

