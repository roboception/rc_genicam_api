
#ifndef LOG_I_LOGGER_HPP
#define LOG_I_LOGGER_HPP

//#include <stdarg.h>
#include <stdio.h>


namespace GENICAM_NAMESPACE
{
  class ILogger
  {
  public:
    enum Priority
    {
      ERR = 300,
      WARN = 400,
      INFO = 600,
      DEBUG = 700,
      NOTSET = 800
    };

    virtual ~ILogger() {};
    virtual void Log(Priority value, const char* stringFormat, ...) = 0;
    virtual void LogVA(Priority value, const char* stringFormat, va_list arg) = 0;
  };

}
#endif

