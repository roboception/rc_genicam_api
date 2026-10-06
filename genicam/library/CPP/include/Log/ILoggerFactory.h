//-----------------------------------------------------------------------------
//  (c) 20017 by STEMMER IMAGING GmbH
//  Author:  Quang Nguyen
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

#ifndef LOG_I_LOGGER_FACTORY_HPP
#define LOG_I_LOGGER_FACTORY_HPP

#include <Log/ILogger.h>

namespace GENICAM_NAMESPACE
{
  class ILoggerFactory
  {
  public:
    virtual ~ILoggerFactory() {};
    virtual const char* GetLoggerFactoryName() = 0;

    // Configure the logger
    // The existing configuration is not cleared or reset
    // Do not remove Loggers when reconfiguring
    // Appenders can be removed
    virtual void ConfigureFromString(const char* configString) = 0;
    virtual void ConfigureDefault() = 0;
    
    // Logger pointer for a particular logger shall not change during runtime
    virtual ILogger* GetLogger(const char* name) = 0;
    virtual bool Exist(const char* name) = 0;
    virtual void PushIndent() = 0;
    virtual void PopIndent() = 0;
  };
}

#endif
