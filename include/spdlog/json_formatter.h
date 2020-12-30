#pragma once

#include <spdlog/details/os.h>
#include <spdlog/formatter.h>
#include <spdlog/populator.h>

namespace spdlog {

class JSONFormatter : public formatter
{
private:
    const std::string kEOL;

    populators::PopulatorSet populators_;

public:
    static populators::PopulatorSet make_default_populator_set()
    {
        return populators::make_populator_set(details::make_unique<populators::DateTimePopulator>(),
            details::make_unique<populators::LevelPopulator>(), details::make_unique<populators::LoggerNamePopulator>(),
            details::make_unique<populators::MessagePopulator>());
    }

    JSONFormatter(std::string eol = spdlog::details::os::default_eol)
        : JSONFormatter(make_default_populator_set(), eol)
    {}

    JSONFormatter(populators::PopulatorSet &&populators, std::string eol = spdlog::details::os::default_eol)
        : kEOL(std::move(eol))
        , populators_(std::move(populators))
    {}

    void format(const details::log_msg &msg, memory_buf_t &dest) override
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

    std::unique_ptr<formatter> clone() const override
    {
        populators::PopulatorSet populators;
        for (const auto &populator : populators_)
        {
            populators.insert(populator->clone());
        }
        return details::make_unique<JSONFormatter>(std::move(populators), kEOL);
    }
};

} // namespace spdlog
