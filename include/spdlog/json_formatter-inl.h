#pragma once

#ifndef SPDLOG_HEADER_ONLY
#    include <spdlog/json_formatter.h>
#endif

namespace spdlog {

SPDLOG_INLINE populators::populator_set json_formatter::make_default_populators_()
{
    return populators::make_populator_set(
        details::make_unique<populators::date_time_populator>(),
        details::make_unique<populators::level_populator>(),
        details::make_unique<populators::logger_name_populator>(),
        details::make_unique<populators::message_populator>());
}

SPDLOG_INLINE json_formatter::json_formatter(std::string eol)
    : kEOL(std::move(eol))
    , populators_(make_default_populators_())
{}

SPDLOG_INLINE json_formatter::json_formatter(populators::populator_set &&populators, std::string eol)
    : kEOL(std::move(eol))
    , populators_(std::move(populators))
{}

SPDLOG_INLINE void json_formatter::format(const details::log_msg &msg, memory_buf_t &dest)
{
    nlohmann::json entry = nlohmann::json::object();
    for (const auto &populator : populators_)
    {
        populator->populate(msg, entry);
    }
    if (msg.params)
    {
        for (const auto &kv : msg.params->items())
        {
            entry[kv.key()] = kv.value();
        }
    }
    dest.append(entry.dump() + kEOL);
}

SPDLOG_INLINE std::unique_ptr<formatter> json_formatter::clone() const
{
    populators::populator_set populators;
    for (const auto &populator : populators_)
    {
        populators.insert(populator->clone());
    }
    return details::make_unique<json_formatter>(std::move(populators), kEOL);
}

} // namespace spdlog
