#pragma once

#ifndef SPDLOG_HEADER_ONLY
#include <spdlog/details/executor.h>
#endif

#include <spdlog/logger.h>

namespace spdlog {

namespace details {

#ifdef SPDLOG_JSON_LOGGER

SPDLOG_INLINE executor::context::context(logger *lgr, const log_msg &msg, bool log_enabled, bool traceback_enabled)
    : lgr(lgr)
    , msg(msg)
    , log_enabled(log_enabled)
    , traceback_enabled(traceback_enabled)
{}

SPDLOG_INLINE executor::context::context(context &&other)
    : lgr(other.lgr)
    , msg(std::move(other.msg))
    , log_enabled(other.log_enabled)
    , traceback_enabled(other.traceback_enabled)
{}

SPDLOG_INLINE executor::executor()
    : ctx_(nullptr)
{}

SPDLOG_INLINE executor::executor(logger *lgr, const log_msg &msg, bool log_enabled, bool traceback_enabled)
    : ctx_(new (buf_) context(lgr, msg, log_enabled, traceback_enabled))
{}

SPDLOG_INLINE executor::executor(executor &&other)
    : ctx_(other.ctx_ ? new (buf_) context(std::move(*other.ctx_)) : nullptr)
{
    other.ctx_ = nullptr;
}

SPDLOG_INLINE executor::~executor() noexcept(false)
{
    if (ctx_)
    {
        ctx_->msg.params = &ctx_->msg.params_buffer;
        try
        {
            ctx_->lgr->log_it_(ctx_->msg, ctx_->log_enabled, ctx_->traceback_enabled);
        }
        catch (...)
        {
            ctx_->~context();
            throw;
        }
        ctx_->~context();
    }
}

SPDLOG_INLINE executor &executor::operator()(const nlohmann::json &params)
{
    if (ctx_)
    {
        for (const auto &kv : params.items())
        {
            ctx_->msg.params_buffer[kv.key()] = kv.value();
        }
    }
    return *this;
}

#else

SPDLOG_INLINE executor::executor(logger *lgr, const log_msg &msg, bool log_enabled, bool traceback_enabled)
{
    lgr->log_it_(msg, log_enabled, traceback_enabled);
}

SPDLOG_INLINE executor &executor::operator()(const nlohmann::json &params)
{
    (void)params;
    return *this;
}

#endif

} // namespace details

} // namespace spdlog
