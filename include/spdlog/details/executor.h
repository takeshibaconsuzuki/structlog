#pragma once

#include <spdlog/details/log_msg_buffer.h>

#include <nlohmann/json.hpp>

namespace spdlog {

class logger;

namespace details {

class SPDLOG_API Executor
{
private:
    struct Context
    {
        logger *lgr;
        log_msg_buffer msg;
        bool log_enabled;
        bool traceback_enabled;

        Context(logger *lgr, const log_msg &msg, bool log_enabled, bool traceback_enabled);
        Context(Context &&other);
    };

    uint8_t buf_[sizeof(Context)];

    Context *ctx_;

public:
    Executor();
    Executor(logger *lgr, const log_msg &msg, bool log_enabled, bool traceback_enabled);
    Executor(const Executor &other) = delete;
    Executor(Executor &&other);

    ~Executor() noexcept(false);

    Executor &operator=(const Executor &other) = delete;
    Executor &operator=(Executor &&other) = delete;

    Executor &operator()(const nlohmann::json &params);
};

} // namespace details

} // namespace spdlog

// executor-inl.h is included in logger.h because it needs to know the logger
// interface
