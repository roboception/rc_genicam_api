
#ifndef LOG4CPP_LOG4CPP_ADAPTER_HPP
#define LOG4CPP_LOG4CPP_ADAPTER_HPP


#include <log4cpp/Portability.hh>
#include <Log/ILoggerFactory.h>
#include <Log/ILogger.h>


LOG4CPP_NS_BEGIN

class Log4cppAdapter : public GenICam::ILoggerFactory
{
public:
  Log4cppAdapter();
  ~Log4cppAdapter();
  GenICam::gcstring GetLoggerFactoryName();
  void ConfigureFromString(GenICam::gcstring configString);
  void ConfigureDefault();


  GenICam::ILogger* GetLogger(GenICam::gcstring name);
  bool Exist(GenICam::gcstring name);
  void PushIndent();
  void PopIndent();
private:
  GenICam::gcstring mLoggerFactoryName;
  bool mHasBeenConfigured;


};


// Wrapper Function

extern "C" LOG4CPP_EXPORT const void* GetILoggerFactory()
{
  GenICam::ILoggerFactory* wrapperPtr = new LOG4CPP_NS::Log4cppAdapter();
  return static_cast<void*>(wrapperPtr);
}

LOG4CPP_NS_END


#endif 
