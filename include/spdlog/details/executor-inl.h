#pragma once

#ifndef SPDLOG_HEADER_ONLY
#include <spdlog/details/executor.h>
#endif

namespace spdlog {

namespace details {

SPDLOG_INLINE Executor::Context::Context(logger *lgr, const log_msg &msg, bool log_enabled, bool traceback_enabled)
    : lgr(lgr)
    , msg(msg)
    , log_enabled(log_enabled)
    , traceback_enabled(traceback_enabled)
{}

SPDLOG_INLINE Executor::Context::Context(Context &&other)
    : lgr(other.lgr)
    , msg(std::move(other.msg))
    , log_enabled(other.log_enabled)
    , traceback_enabled(other.traceback_enabled)
{}

SPDLOG_INLINE Executor::Executor()
    : ctx_(nullptr)
{}

SPDLOG_INLINE Executor::Executor(logger *lgr, const log_msg &msg, bool log_enabled, bool traceback_enabled)
    : ctx_(new (buf_) Context(lgr, msg, log_enabled, traceback_enabled))
{}

SPDLOG_INLINE Executor::Executor(Executor &&other)
    : ctx_(other.ctx_ ? new (buf_) Context(std::move(*other.ctx_)) : nullptr)
{
    other.ctx_ = nullptr;
}

SPDLOG_INLINE Executor::~Executor() noexcept(false)
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
            ctx_->~Context();
            throw;
        }
        ctx_->~Context();
    }
}

SPDLOG_INLINE Executor &Executor::operator()(const nlohmann::json &params)
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

} // namespace details

} // namespace spdlog
