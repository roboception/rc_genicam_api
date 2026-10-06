//-----------------------------------------------------------------------------
//  (c) 2006 by Basler Vision Technologies
//  Author:  Fritz Dierks
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
/*!
\file
\brief    declspec's to be used for Log Windows dll
\ingroup GenApi_PublicImpl
*/

#ifndef LOGDLL_H
#define LOGDLL_H

// Log import/export and static/shared
#if defined(GENAPI_STATIC)
#   define LOG_DECL
#elif defined(_MSC_VER)
#   ifdef LOG_EXPORTS
#       define LOG_DECL __declspec(dllexport)
#   else
#       define LOG_DECL __declspec(dllimport)
#   endif
#elif defined (__GNUC__)
#   ifdef LOG_EXPORTS
#       define LOG_DECL __attribute__((visibility("default")))
#   else
#       define LOG_DECL
#   endif
#else
#   error Unsupported compiler or platform!
#endif

#endif /* LOGDLL_H */
