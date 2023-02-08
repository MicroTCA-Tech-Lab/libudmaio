#include "udmaio/Logging.hpp"

namespace udmaio {

namespace bl = boost::log;
namespace expr = boost::log::expressions;

void Logger::set_level(bls lvl) {
    bl::core::get()->set_filter(bl::trivial::severity >= lvl);
}

BOOST_LOG_ATTRIBUTE_KEYWORD(channel, "Channel", std::string)

void Logger::init(const int channel_padding) {
    bl::add_common_attributes();
    bl::add_console_log(
        std::cout,
        bl::keywords::format = (expr::stream << expr::format_date_time<boost::posix_time::ptime>(
                                                    "TimeStamp",
                                                    "%Y-%m-%d %H:%M:%S.%f")

                                             << " [" << std::left << std::setw(5)
                                             << std::setfill(' ') << bl::trivial::severity << "] "

                                             << std::right << std::setw(channel_padding)
                                             << std::setfill(' ') << channel

                                             << ": " << expr::smessage));
}

}  // namespace udmaio
