#pragma once

#include <spdlog/common.h>

namespace spdlog {

#ifdef SPDLOG_JSON_LOGGER
#    include <spdlog/json_formatter.h>
typedef ::spdlog::JSONFormatter default_formatter;
#else
#    include <spdlog/pattern_formatter.h>
typedef ::spdlog::pattern_formatter default_formatter;
#endif

}  // namespace spdlog
