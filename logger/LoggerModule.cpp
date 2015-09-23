#include <LoggerModule.h>


void LoggerModule::init()
{
    logging::add_file_log
    (
	keywords::auto_flush = true,
        keywords::file_name = "/var/log/crmproxy_%N.log",                                        /*< file name pattern >*/
        keywords::rotation_size = 100 * 1024 * 1024,                                   /*< rotate files every 10 MiB... >*/
        keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0), /*< ...or at midnight >*/
        keywords::format = "[%TimeStamp%]: %Message%"                                 /*< log record format >*/
    );

    logging::core::get()->set_filter
    (
        logging::trivial::severity >= logging::trivial::info
    );
}

LoggerModule::LoggerModule()
{
    init();
    logging::add_common_attributes();

}

void LoggerModule::makeLog(boost::log::v2s_mt_posix::trivial::severity_level lvl,std::string msg)
{
    BOOST_LOG_SEV(lg, lvl) << msg;
}