#include "logger.h"

/**
  @breif : 로그객체 초기화
**/

namespace logging = boost::log;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;

using namespace AprLogger;

Logger* Logger::m_pThis = nullptr;


Logger::Logger() {
  init_logger();
}

Logger::Logger(Logger const& rhs) {
  ;
}

Logger& Logger::operator=(Logger const& rhs) {
  m_logger = rhs.m_logger;
  m_pThis = rhs.m_pThis;

  return *this;
}


/**
  @brief: 로그 Sink 초기화 및 전체 초기화

**/
void Logger::init_logger() {

  // 로깅코어 get
  boost::shared_ptr<logging::core> core = logging::core::get();
 
  // backend 객체 생성 
  boost::shared_ptr<sinks::text_file_backend> backend =
      boost::make_shared<sinks::text_file_backend>(
          keywords::file_name = "T5Calibration_Log_%Y%m%d_%N.log",  // 로그파일
          /*
           *  1 KiB = 1024 Bit
           *  1 MiB = 1024 KiB
           *  1 MiB = 1.04858 MB
           * */
          keywords::rotation_size =
              1 * 1024 * 1024,  // 파일 사이즈 ( N * 1024 Bit * 1024 Bit )
          keywords::open_mode = (std::ios::out | std::ios::app),
          keywords::time_based_rotation =
              sinks::file::rotation_at_time_point(12, 0, 0));
  backend->auto_flush(true);

  // front end : synchronous_sink 
  // back  end : text file backend
  typedef sinks::synchronous_sink<sinks::text_file_backend> text_sink;

  // 새로운 text file base 객체 할당
  boost::shared_ptr<text_sink> sink(new text_sink(backend));
  core->add_sink(sink);

  sink->set_formatter(&init_format);

  // 로그 레벨 설정
  core->set_filter(logging::trivial::severity >=
                                   logging::trivial::trace);

  // 로깅 속성값 적용
  logging::add_common_attributes();
}

/**
  @breif: 로그 포맷팅 형식 초기화

**/
void Logger::init_format(logging::record_view const& rec,
                         logging::formatting_ostream& strm) {

  /*
		[LineID]  [Timestamp]	        [func]  [Line]  [severity]   [message]

	  ex) [55] 20200802-10:13:52.24123 [main.cpp : 13] <debug> : log test insert
  */

  typedef boost::log::formatter formatter;
  formatter f = expr::stream << 
	expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y%m%d-%H:%M:%S");

  // custom value 
  logging::value_ref<std::string> fullpath =
      logging::extract<std::string>("File", rec);
  logging::value_ref<unsigned int> line = logging::extract<unsigned int>("Line", rec);
 
  // format 
  //strm << "[" << boost::format("%04d") % logging::extract<unsigned int>("LineID", rec) <<"] ";
  f(rec,strm);
  strm << " [" << boost::filesystem::path(fullpath.get()).filename().string() <<" : ";
  strm << boost::format("%03d") % logging::extract<int>("Line", rec) <<"]";
  strm << " <" << rec[logging::trivial::severity] << ">";
  strm << " : " << rec[expr::smessage];

}
