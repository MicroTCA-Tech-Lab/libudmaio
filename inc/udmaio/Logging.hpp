#pragma once

#include <string>

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/console.hpp>

#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

namespace udmaio {

using bls = boost::log::trivial::severity_level;
using boost_logger = boost::log::sources::severity_channel_logger_mt<bls, std::string>;

struct Logger {
    mutable boost_logger _lg;

    Logger() : _lg{} {};
    Logger(std::string name) : _lg{boost::log::keywords::channel = name} {};
    static void set_level(bls lvl);
    static void init(const int channel_padding);
};

};  // namespace udmaio
