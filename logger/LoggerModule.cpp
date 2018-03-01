#include <LoggerModule.h>


void LoggerModule::init(std::string logname)
{
    std::cout<<"CREATE LOGFILE (v2.2)"<<logname<<"\n";
    logging::add_file_log
    (
	keywords::auto_flush = true,
	keywords::max_size = 1000 * 1024 * 1024,
        keywords::file_name = logname,                                        /*< file name pattern >*/
        keywords::rotation_size = 100 * 1024 * 1024,                                   /*< rotate files every 10 MiB... >*/
        keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0), /*< ...or at midnight >*/
        keywords::format = "[%TimeStamp%]: %Message%"                                 /*< log record format >*/
    );

    logging::core::get()->set_filter
    (
        logging::trivial::severity >= logging::trivial::info
    );
}

LoggerModule::LoggerModule(std::string logname)
{
    if(logname.empty())
	init("/var/log/crmproxy_%m%d%Y_%H%M%S_%5N.log");
    else
	init(logname);
    logging::add_common_attributes();

}

void LoggerModule::makeLog(boost::log::v2s_mt_posix::trivial::severity_level lvl,std::string msg)
{
//    if(lvl!=debug)
	BOOST_LOG_SEV(lg, lvl) << msg;
}
