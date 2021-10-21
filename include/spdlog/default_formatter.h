#pragma once

#include <spdlog/common.h>

#ifdef SPDLOG_JSON_LOGGER

#include <spdlog/json_formatter.h>

namespace spdlog {

typedef ::spdlog::json_formatter default_formatter;

}  // namespace spdlog

#else

#include <spdlog/pattern_formatter.h>

namespace spdlog {

typedef ::spdlog::pattern_formatter default_formatter;

}  // namespace spdlog

#endif
