#ifndef LOGGER_MODULE
#define LOGGER_MODULE
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>

#include <string>


namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;

using namespace logging::trivial;


class LoggerModule
{
    
    src::severity_logger< logging::trivial::severity_level > lg;
    void init(std::string);
    
    public:
	void makeLog(boost::log::v2s_mt_posix::trivial::severity_level lvl,std::string message);
	LoggerModule(std::string logname = "");
};
#endif