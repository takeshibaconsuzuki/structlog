#pragma once

#include <spdlog/details/log_msg.h>
#include <spdlog/details/os.h>
#include <spdlog/pattern_formatter.h>

#include <nlohmann/json.hpp>

#include <unordered_set>

namespace spdlog {

namespace populators {

class SPDLOG_API Populator
{
public:
    virtual ~Populator() {}

    virtual void populate(const details::log_msg &msg, nlohmann::json &dest) = 0;

    virtual std::unique_ptr<Populator> clone() const = 0;
};

template<bool kShowEmpty>
class SPDLOG_API PatternPopulator : public Populator
{
private:
    const std::string kKey;

    std::unique_ptr<spdlog::formatter> pf_;

public:
    PatternPopulator(const std::string &key, const std::string &pattern)
        : kKey(key)
        , pf_(details::make_unique<spdlog::pattern_formatter>(pattern, pattern_time_type::local, ""))
    {}

    PatternPopulator(const PatternPopulator &other)
        : kKey(other.kKey)
        , pf_(other.pf_->clone())
    {}

    void populate(const details::log_msg &msg, nlohmann::json &dest) override
    {
        memory_buf_t tmp;
        pf_->format(msg, tmp);
        if (tmp.size() || kShowEmpty)
        {
            dest[kKey] = std::string(tmp.data(), tmp.size());
        }
    }

    std::unique_ptr<Populator> clone() const override
    {
        return details::make_unique<PatternPopulator>(*this);
    }
};

class SPDLOG_API DateTimePopulator : public PatternPopulator<true>
{
public:
    DateTimePopulator()
        : PatternPopulator("date_time", "%Y-%m-%d %H:%M:%S.%e%z")
    {}
};

class SPDLOG_API LevelPopulator : public PatternPopulator<true>
{
public:
    LevelPopulator()
        : PatternPopulator("level", "%l")
    {}
};

class SPDLOG_API LoggerNamePopulator : public PatternPopulator<false>
{
public:
    LoggerNamePopulator()
        : PatternPopulator("logger_name", "%n")
    {}
};

class SPDLOG_API MessagePopulator : public PatternPopulator<true>
{
public:
    MessagePopulator()
        : PatternPopulator("message", "%v")
    {}
};

class SPDLOG_API PIDPopulator : public Populator
{
public:
    void populate(const details::log_msg &msg, nlohmann::json &dest) override
    {
        dest["pid"] = details::os::pid();
    }

    std::unique_ptr<Populator> clone() const override
    {
        return details::make_unique<PIDPopulator>();
    }
};

class SPDLOG_API SrcLocPopulator : public PatternPopulator<true>
{
public:
    SrcLocPopulator()
        : PatternPopulator("src_loc", "%@")
    {}
};

class SPDLOG_API ThreadIDPopulator : public Populator
{
public:
    void populate(const details::log_msg &msg, nlohmann::json &dest) override
    {
        dest["thread_id"] = msg.thread_id;
    }

    std::unique_ptr<Populator> clone() const override
    {
        return details::make_unique<ThreadIDPopulator>();
    }
};

class SPDLOG_API TimestampPopulator : public Populator
{
public:
    void populate(const details::log_msg &msg, nlohmann::json &dest) override
    {
        const auto dur = msg.time.time_since_epoch();
        dest["timestamp"] = std::chrono::duration_cast<std::chrono::seconds>(dur).count();
    }

    std::unique_ptr<Populator> clone() const override
    {
        return details::make_unique<TimestampPopulator>();
    }
};

typedef std::unordered_set<std::unique_ptr<Populator>> PopulatorSet;

template<class... Args>
PopulatorSet make_populator_set(Args &&... args)
{
    PopulatorSet ret;
    int dummy[] = {(ret.insert(std::move(std::forward<Args>(args))), 0)...};
    return std::move(ret);
}

} // namespace populators

} // namespace spdlog
