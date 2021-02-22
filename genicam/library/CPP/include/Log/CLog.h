/****************************************************************************

(c) 2017 by STEMMER IMAGING GmbH
Project: Modular Logging
Author:  Quang Nguyen

License: This file is published under the license of the EMVA GenICam  Standard Group.
A text file describing the legal terms is included in  your installation as 'GenICam_license.pdf'.
If for some reason you are missing  this file please contact the EMVA or visit the website
(http://www.genicam.org) for a full copy.

THIS SOFTWARE IS PROVIDED BY THE EMVA GENICAM STANDARD GROUP "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE EMVA GENICAM STANDARD  GROUP
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,  SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT  LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,  DATA, OR PROFITS;
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY  THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE  OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

****************************************************************************/




#ifndef LOG_CLOG_H_
#define LOG_CLOG_H_

#ifdef _MSC_VER
#   pragma warning (push, 3)
#   pragma warning(disable:4706)  // assignment within conditional expression
#endif
//#pragma warning (push, 3)
//#pragma warning(disable:4706)  // assignment within conditional expression

#include <stdio.h>
#include <map>
#include <Log/LogDll.h>

#include <Base/GCBase.h>
#include <Log/ILogger.h>
#include <Log/ILoggerFactory.h>


namespace GENICAM_NAMESPACE
{
    /**
    \brief This logging class initializes the logger. By default that would be Log4cpp
    \ingroup Log_PublicUtilities
    */
    class LOG_DECL CLog
    {
    public:
        //! Checks if a category/logger exists
        //! First checks the local logger map
        //! Finally checks via ILoggerFactory::Exist() in the ILoggerFactory implementation
        static bool Exist(const gcstring &LoggerName);
        static bool Exist(const char* LoggerName);


        //! Retrieves (and if necessary create) a category by name
        //! Stores logger which have been requested before in a map
        static ILogger& GetLogger(const gcstring &LoggerName);
        static ILogger& GetLogger(const char* LoggerName);

        static void PushIndent();
        static void PopIndent();

        // No logger is being used
        static void OmitLogger();
        
        // Will take ownership of the ILoggerFactory. ShutDown will delete that object
        static void SetLoggerFactory(ILoggerFactory&);
        static ILoggerFactory* GetLoggerFactory();

        //! de-initializes the logging framework
        static void ShutDown(void);

    private:

      //! A type corresponds to a library handle which can be platform specific.
#if defined (_WIN32)
      typedef  HMODULE lib_handle_t;
#else
      typedef void * lib_handle_t;
#endif

      // Default initializer
      static void DefaultInitializer();

      //! Opens a library.
      static lib_handle_t OpenLibrary(const gcstring Name);

      //! Finds a symbol and return its pointer.
      static void *FindSymbol(lib_handle_t Handle, const gcstring Name);

      //! Makes sure log4cpp has been found.
      static void MakeSureLoggerHasBeenFound();

      static bool ExistInMap(gcstring loggerName);
      // Delete all available ILoggers
      static void DeleteLoggerMap();

      // Contains the logger factory unless it has not been loaded
      static ILoggerFactory* m_LogFactory;


        
      //! A handle to an associated logger library.
      static lib_handle_t g_pLibHandle;
        
      // True if logger has been found
      static bool m_LoggerHasBeenFound;
      static bool m_OmitLogger;

    };

}

#if defined(__GNUC__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wvariadic-macros"
#endif

// Logging macros
#define GCLOGINFO( cat, ... )       if(cat && GENICAM_NAMESPACE::CLog::Exist("")) { (cat)->Log(GENICAM_NAMESPACE::ILogger::INFO, ##__VA_ARGS__ ); }
#define GCLOGINFOPUSH( cat, ... )   if(cat && GENICAM_NAMESPACE::CLog::Exist("")) { (cat)->Log(GENICAM_NAMESPACE::ILogger::INFO, ##__VA_ARGS__ ); GENICAM_NAMESPACE::CLog::PushIndent();}
#define GCLOGWARN( cat, ... )       if(cat && GENICAM_NAMESPACE::CLog::Exist("")) { (cat)->Log(GENICAM_NAMESPACE::ILogger::WARN, ##__VA_ARGS__ ); }
#define GCLOGERROR( cat, ... )      if(cat && GENICAM_NAMESPACE::CLog::Exist("")) { (cat)->Log(GENICAM_NAMESPACE::ILogger::ERR, ##__VA_ARGS__ ); }
#define GCLOGDEBUG( cat, ... )      if(cat && GENICAM_NAMESPACE::CLog::Exist("")) { (cat)->Log(GENICAM_NAMESPACE::ILogger::DEBUG, ##__VA_ARGS__ ); }
#define GCLOGINFOPOP( cat, ... )    if(cat && GENICAM_NAMESPACE::CLog::Exist("")) { (cat)->Log(GENICAM_NAMESPACE::ILogger::INFO, ##__VA_ARGS__ ); GENICAM_NAMESPACE::CLog::PopIndent();}


#if defined(__GNUC__)
#   pragma GCC diagnostic pop
#endif

#endif // LOG_CLOG_H_
