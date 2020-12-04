
#ifndef LOG4CPP_CATEGORY_ADAPTER_HPP
#define LOG4CPP_CATEGORY_ADAPTER_HPP

#include <log4cpp/Portability.hh>
#include <log4cpp/Category.hh>

#include <Log/ILogger.h>



LOG4CPP_NS_BEGIN

class CategoryAdapter : public GENICAM_NAMESPACE::ILogger
{
public:
  CategoryAdapter(LOG4CPP_NS::Category& categoryRef);
  ~CategoryAdapter();
  void Log(GENICAM_NAMESPACE::ILogger::Priority logPriority, const char*, ...);
  void LogVA(GENICAM_NAMESPACE::ILogger::Priority value, const char* stringFormat, va_list arg);

private:
  // Prevent auto-generated assignment operator by this private one. Otherwise
  // Visual Studio 2013 gives a warning
  void operator=(const CategoryAdapter&);

  LOG4CPP_NS::Category& mCategoryRef;

  LOG4CPP_NS::Priority::Value GetPriority(GENICAM_NAMESPACE::ILogger::Priority gcPriority);

};

LOG4CPP_NS_END



#endif 