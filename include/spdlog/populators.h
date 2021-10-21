#pragma once

#include <spdlog/common.h>

#ifdef SPDLOG_JSON_LOGGER

#include <spdlog/details/log_msg.h>
#include <spdlog/details/os.h>
#include <spdlog/json.h>
#include <spdlog/pattern_formatter.h>

#include <unordered_set>

namespace spdlog {

namespace populators {

class SPDLOG_API populator
{
public:
    virtual ~populator() {}

    virtual void populate(const details::log_msg &msg, nlohmann::json &dest) = 0;

    virtual std::unique_ptr<populator> clone() const = 0;
};

class SPDLOG_API pattern_populator : public populator
{
protected:
    const std::string kKey;

    std::unique_ptr<spdlog::formatter> pf_;

public:
    pattern_populator(const std::string &key, const std::string &pattern);

    pattern_populator(const pattern_populator &other);

    virtual void populate(const details::log_msg &msg, nlohmann::json &dest) override;

    virtual std::unique_ptr<populator> clone() const override;
};

class SPDLOG_API date_time_populator : public pattern_populator
{
public:
    date_time_populator();
};

class SPDLOG_API level_populator : public pattern_populator
{
public:
    level_populator();
};

class SPDLOG_API logger_name_populator : public pattern_populator
{
public:
    logger_name_populator();

    virtual void populate(const details::log_msg &msg, nlohmann::json &dest) override;

    virtual std::unique_ptr<populator> clone() const override;
};

class SPDLOG_API message_populator : public pattern_populator
{
public:
    message_populator();
};

class SPDLOG_API pid_populator : public populator
{
public:
    virtual void populate(const details::log_msg &, nlohmann::json &dest) override;

    virtual std::unique_ptr<populator> clone() const override;
};

class SPDLOG_API src_loc_populator : public pattern_populator
{
public:
    src_loc_populator();
};

class SPDLOG_API thread_id_populator : public populator
{
public:
    virtual void populate(const details::log_msg &msg, nlohmann::json &dest) override;

    virtual std::unique_ptr<populator> clone() const override;
};

class SPDLOG_API timestamp_populator : public populator
{
public:
    virtual void populate(const details::log_msg &msg, nlohmann::json &dest) override;

    virtual std::unique_ptr<populator> clone() const override;
};

typedef std::unordered_set<std::unique_ptr<populator>> populator_set;

template<class... Args>
populator_set make_populator_set(Args &&... args)
{
    populator_set ret;
    int dummy[] = {(ret.insert(std::move(std::forward<Args>(args))), 0)...};
    (void)dummy;
    return ret;
}

} // namespace populators

} // namespace spdlog

#ifdef SPDLOG_HEADER_ONLY
#    include "populators-inl.h"
#endif

#endif
