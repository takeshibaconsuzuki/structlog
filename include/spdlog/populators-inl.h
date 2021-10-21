#pragma once

#include <spdlog/common.h>

#ifdef SPDLOG_JSON_LOGGER

#ifndef SPDLOG_HEADER_ONLY
#    include <spdlog/populators.h>
#endif

namespace spdlog {

namespace populators {

SPDLOG_INLINE pattern_populator::pattern_populator(const std::string &key, const std::string &pattern)
    : kKey(key)
    , pf_(details::make_unique<spdlog::pattern_formatter>(pattern, pattern_time_type::local, ""))
{}

SPDLOG_INLINE pattern_populator::pattern_populator(const pattern_populator &other)
    : kKey(other.kKey)
    , pf_(other.pf_->clone())
{}

SPDLOG_INLINE void pattern_populator::populate(const details::log_msg &msg, nlohmann::json &dest)
{
    memory_buf_t tmp;
    pf_->format(msg, tmp);
    dest[kKey] = std::string(tmp.data(), tmp.size());
}

SPDLOG_INLINE std::unique_ptr<populator> pattern_populator::clone() const
{
    return details::make_unique<pattern_populator>(*this);
}

SPDLOG_INLINE date_time_populator::date_time_populator()
    : pattern_populator("date_time", "%Y-%m-%d %H:%M:%S.%e%z")
{}

SPDLOG_INLINE level_populator::level_populator()
    : pattern_populator("level", "%l")
{}

SPDLOG_INLINE logger_name_populator::logger_name_populator()
    : pattern_populator("logger_name", "%n")
{}

SPDLOG_INLINE void logger_name_populator::populate(const details::log_msg &msg, nlohmann::json &dest)
{
    memory_buf_t tmp;
    pf_->format(msg, tmp);
    if (tmp.size() > 0) {
        dest[kKey] = std::string(tmp.data(), tmp.size());
    }
}

SPDLOG_INLINE std::unique_ptr<populator> logger_name_populator::clone() const
{
    return details::make_unique<logger_name_populator>(*this);
}

SPDLOG_INLINE message_populator::message_populator()
    : pattern_populator("message", "%v")
{}

SPDLOG_INLINE void pid_populator::populate(const details::log_msg &, nlohmann::json &dest)
{
    dest["pid"] = details::os::pid();
}

SPDLOG_INLINE std::unique_ptr<populator> pid_populator::clone() const
{
    return details::make_unique<pid_populator>();
}

SPDLOG_INLINE src_loc_populator::src_loc_populator()
    : pattern_populator("src_loc", "%@")
{}

SPDLOG_INLINE void thread_id_populator::populate(const details::log_msg &msg, nlohmann::json &dest)
{
    dest["thread_id"] = msg.thread_id;
}

SPDLOG_INLINE std::unique_ptr<populator> thread_id_populator::clone() const
{
    return details::make_unique<thread_id_populator>();
}

SPDLOG_INLINE void timestamp_populator::populate(const details::log_msg &msg, nlohmann::json &dest)
{
    const auto dur = msg.time.time_since_epoch();
    dest["timestamp"] = std::chrono::duration_cast<std::chrono::seconds>(dur).count();
}

SPDLOG_INLINE std::unique_ptr<populator> timestamp_populator::clone() const
{
    return details::make_unique<timestamp_populator>();
}

} // namespace populators

} // namespace spdlog

#endif
