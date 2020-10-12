#ifndef LOGGER_H
#define LOGGER_H

// Boost Libraries
#define BOOST_ALL_DYN_LINK 1
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/attributes/mutable_constant.hpp>
#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/attributes/timer.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/support/exception.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/format.hpp>

namespace logging = boost::log;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;

#define TRACE boost::log::trivial::trace
#define INFO logging::trivial::info
#define DEBUG logging::trivial::debug
#define WARNING logging::trivial::warning

#define REG_LOGGER Logger::RegisterLogger()
#define GET_LOGGER Logger::GetLogger()->GetInstance()

#define APR_LOGGER(logger_, sev)                 \
BOOST_LOG_SEV(logger_, sev)                      \
      << boost::log::add_value("Line", __LINE__) \
      << boost::log::add_value("File", __FILE__) \
      << boost::log::add_value("Function", BOOST_CURRENT_FUNCTION)

typedef src::severity_logger<logging::trivial::severity_level> LOGGER;

namespace AprLogger {

// trace, debug, info, warning, error, fatal
class Logger {
 public:
  static void RegisterLogger() {
    if (m_pThis == nullptr) {
      m_pThis = new Logger;
    }
  }

  static Logger* GetLogger() {
    if (m_pThis == nullptr) {
      m_pThis = new Logger;
    } else {
      return m_pThis;
    }
  }

  LOGGER& GetInstance() { return m_logger; }
  void SetLevel() {;}

 private:
  Logger();
  Logger(Logger const&);
  Logger& operator=(Logger const&);

  static void init_logger();
  static void init_format(logging::record_view const& rec,
                          logging::formatting_ostream& strm);

  // member variable
  LOGGER m_logger;
  static Logger* m_pThis;
};

}  // namespace AprLogger
#endif
