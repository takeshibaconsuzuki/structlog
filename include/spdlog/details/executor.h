#pragma once

#include <spdlog/common.h>

#ifdef SPDLOG_JSON_LOGGER

#include <spdlog/details/log_msg_buffer.h>
#include <spdlog/json.h>

namespace spdlog {

class logger;

namespace details {

class SPDLOG_API executor
{
private:
    struct context
    {
        logger *lgr;
        log_msg_buffer msg;
        bool log_enabled;
        bool traceback_enabled;

        context(logger *lgr, const log_msg &msg, bool log_enabled, bool traceback_enabled);
        context(context &&other);
    };

    uint8_t buf_[sizeof(context)];

    context *ctx_;

public:
    executor();
    executor(logger *lgr, const log_msg &msg, bool log_enabled, bool traceback_enabled);
    executor(const executor &other) = delete;
    executor(executor &&other);

    ~executor() noexcept(false);

    executor &operator=(const executor &other) = delete;
    executor &operator=(executor &&other) = delete;

    executor &operator()(const nlohmann::json &params);
};

} // namespace details

} // namespace spdlog


#ifdef SPDLOG_HEADER_ONLY
#    include "executor-inl.h"
#endif

#endif
