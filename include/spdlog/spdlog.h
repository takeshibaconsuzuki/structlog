// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

// spdlog main header file.
// see example.cpp for usage example

#ifndef SPDLOG_H
#define SPDLOG_H

#pragma once

#include <spdlog/common.h>
#include <spdlog/details/registry.h>
#include <spdlog/logger.h>
#include <spdlog/version.h>
#include <spdlog/details/synchronous_factory.h>
#include <spdlog/json_formatter.h>

#include <chrono>
#include <functional>
#include <memory>
#include <string>

namespace spdlog {

using default_factory = synchronous_factory;

// Create and register a logger with a templated sink type
// The logger's level, formatter and flush level will be set according the
// global settings.
//
// Example:
//   spdlog::create<daily_file_sink_st>("logger_name", "dailylog_filename", 11, 59);
template<typename Sink, typename... SinkArgs>
inline std::shared_ptr<spdlog::logger> create(std::string logger_name, SinkArgs &&...sink_args)
{
    return default_factory::create<Sink>(std::move(logger_name), std::forward<SinkArgs>(sink_args)...);
}

// Initialize and register a logger,
// formatter and flush level will be set according the global settings.
//
// Useful for initializing manually created loggers with the global settings.
//
// Example:
//   auto mylogger = std::make_shared<spdlog::logger>("mylogger", ...);
//   spdlog::initialize_logger(mylogger);
SPDLOG_API void initialize_logger(std::shared_ptr<logger> logger);

// Return an existing logger or nullptr if a logger with such name doesn't
// exist.
// example: spdlog::get("my_logger")->info("hello {}", "world");
SPDLOG_API std::shared_ptr<logger> get(const std::string &name);

// Set global formatter. Each sink in each logger will get a clone of this object
SPDLOG_API void set_formatter(std::unique_ptr<spdlog::formatter> formatter);

// Set global format string.
// example: spdlog::set_pattern("%Y-%m-%d %H:%M:%S.%e %l : %v");
SPDLOG_API void set_pattern(std::string pattern, pattern_time_type time_type = pattern_time_type::local);

template<class... Args>
SPDLOG_API void set_populators(Args &&... args)
{
    set_formatter(details::make_unique<json_formatter>(populators::make_populator_set(std::forward<Args>(args)...)));
}

// enable global backtrace support
SPDLOG_API void enable_backtrace(size_t n_messages);

// disable global backtrace support
SPDLOG_API void disable_backtrace();

// call dump backtrace on default logger
SPDLOG_API void dump_backtrace();

// Get global logging level
SPDLOG_API level::level_enum get_level();

// Set global logging level
SPDLOG_API void set_level(level::level_enum log_level);

// Determine whether the default logger should log messages with a certain level
SPDLOG_API bool should_log(level::level_enum lvl);

// Set global flush level
SPDLOG_API void flush_on(level::level_enum log_level);

// Start/Restart a periodic flusher thread
// Warning: Use only if all your loggers are thread safe!
SPDLOG_API void flush_every(std::chrono::seconds interval);

// Set global error handler
SPDLOG_API void set_error_handler(void (*handler)(const std::string &msg));

// Register the given logger with the given name
SPDLOG_API void register_logger(std::shared_ptr<logger> logger);

// Apply a user defined function on all registered loggers
// Example:
// spdlog::apply_all([&](std::shared_ptr<spdlog::logger> l) {l->flush();});
SPDLOG_API void apply_all(const std::function<void(std::shared_ptr<logger>)> &fun);

// Drop the reference to the given logger
SPDLOG_API void drop(const std::string &name);

// Drop all references from the registry
SPDLOG_API void drop_all();

// stop any running threads started by spdlog and clean registry loggers
SPDLOG_API void shutdown();

// Automatic registration of loggers when using spdlog::create() or spdlog::create_async
SPDLOG_API void set_automatic_registration(bool automatic_registration);

// API for using default logger (stdout_color_mt),
// e.g: spdlog::info("Message {}", 1);
//
// The default logger object can be accessed using the spdlog::default_logger():
// For example, to add another sink to it:
// spdlog::default_logger()->sinks().push_back(some_sink);
//
// The default logger can replaced using spdlog::set_default_logger(new_logger).
// For example, to replace it with a file logger.
//
// IMPORTANT:
// The default API is thread safe (for _mt loggers), but:
// set_default_logger() *should not* be used concurrently with the default API.
// e.g do not call set_default_logger() from one thread while calling spdlog::info() from another.

SPDLOG_API std::shared_ptr<spdlog::logger> default_logger();

SPDLOG_API spdlog::logger *default_logger_raw();

SPDLOG_API void set_default_logger(std::shared_ptr<spdlog::logger> default_logger);

template<typename... Args>
inline SPDLOG_EXECUTOR_T log(source_loc source, level::level_enum lvl, fmt::format_string<Args...> fmt, Args &&...args)
{
    SPDLOG_RETURN_EXECUTOR default_logger_raw()->log(source, lvl, fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline SPDLOG_EXECUTOR_T log(level::level_enum lvl, fmt::format_string<Args...> fmt, Args &&...args)
{
    SPDLOG_RETURN_EXECUTOR default_logger_raw()->log(source_loc{}, lvl, fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline SPDLOG_EXECUTOR_T trace(fmt::format_string<Args...> fmt, Args &&...args)
{
    SPDLOG_RETURN_EXECUTOR default_logger_raw()->trace(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline SPDLOG_EXECUTOR_T debug(fmt::format_string<Args...> fmt, Args &&...args)
{
    SPDLOG_RETURN_EXECUTOR default_logger_raw()->debug(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline SPDLOG_EXECUTOR_T info(fmt::format_string<Args...> fmt, Args &&...args)
{
    SPDLOG_RETURN_EXECUTOR default_logger_raw()->info(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline SPDLOG_EXECUTOR_T warn(fmt::format_string<Args...> fmt, Args &&...args)
{
    SPDLOG_RETURN_EXECUTOR default_logger_raw()->warn(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline SPDLOG_EXECUTOR_T error(fmt::format_string<Args...> fmt, Args &&...args)
{
    SPDLOG_RETURN_EXECUTOR default_logger_raw()->error(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline SPDLOG_EXECUTOR_T critical(fmt::format_string<Args...> fmt, Args &&...args)
{
    SPDLOG_RETURN_EXECUTOR default_logger_raw()->critical(fmt, std::forward<Args>(args)...);
}

template<typename T>
inline SPDLOG_EXECUTOR_T log(source_loc source, level::level_enum lvl, const T &msg)
{
    SPDLOG_RETURN_EXECUTOR default_logger_raw()->log(source, lvl, msg);
}

template<typename T>
inline SPDLOG_EXECUTOR_T log(level::level_enum lvl, const T &msg)
{
    SPDLOG_RETURN_EXECUTOR default_logger_raw()->log(lvl, msg);
}

#ifdef SPDLOG_WCHAR_TO_UTF8_SUPPORT
template<typename... Args>
inline SPDLOG_EXECUTOR_T log(source_loc source, level::level_enum lvl, fmt::wformat_string<Args...> fmt, Args &&...args)
{
    SPDLOG_RETURN_EXECUTOR default_logger_raw()->log(source, lvl, fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline SPDLOG_EXECUTOR_T log(level::level_enum lvl, fmt::wformat_string<Args...> fmt, Args &&...args)
{
    SPDLOG_RETURN_EXECUTOR default_logger_raw()->log(source_loc{}, lvl, fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline SPDLOG_EXECUTOR_T trace(fmt::wformat_string<Args...> fmt, Args &&...args)
{
    SPDLOG_RETURN_EXECUTOR default_logger_raw()->trace(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline SPDLOG_EXECUTOR_T debug(fmt::wformat_string<Args...> fmt, Args &&...args)
{
    SPDLOG_RETURN_EXECUTOR default_logger_raw()->debug(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline SPDLOG_EXECUTOR_T info(fmt::wformat_string<Args...> fmt, Args &&...args)
{
    SPDLOG_RETURN_EXECUTOR default_logger_raw()->info(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline SPDLOG_EXECUTOR_T warn(fmt::wformat_string<Args...> fmt, Args &&...args)
{
    SPDLOG_RETURN_EXECUTOR default_logger_raw()->warn(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline SPDLOG_EXECUTOR_T error(fmt::wformat_string<Args...> fmt, Args &&...args)
{
    SPDLOG_RETURN_EXECUTOR default_logger_raw()->error(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline SPDLOG_EXECUTOR_T critical(fmt::wformat_string<Args...> fmt, Args &&...args)
{
    SPDLOG_RETURN_EXECUTOR default_logger_raw()->critical(fmt, std::forward<Args>(args)...);
}
#endif

template<typename T>
inline SPDLOG_EXECUTOR_T trace(const T &msg)
{
    SPDLOG_RETURN_EXECUTOR default_logger_raw()->trace(msg);
}

template<typename T>
inline SPDLOG_EXECUTOR_T debug(const T &msg)
{
    SPDLOG_RETURN_EXECUTOR default_logger_raw()->debug(msg);
}

template<typename T>
inline SPDLOG_EXECUTOR_T info(const T &msg)
{
    SPDLOG_RETURN_EXECUTOR default_logger_raw()->info(msg);
}

template<typename T>
inline SPDLOG_EXECUTOR_T warn(const T &msg)
{
    SPDLOG_RETURN_EXECUTOR default_logger_raw()->warn(msg);
}

template<typename T>
inline SPDLOG_EXECUTOR_T error(const T &msg)
{
    SPDLOG_RETURN_EXECUTOR default_logger_raw()->error(msg);
}

template<typename T>
inline SPDLOG_EXECUTOR_T critical(const T &msg)
{
    SPDLOG_RETURN_EXECUTOR default_logger_raw()->critical(msg);
}

} // namespace spdlog

//
// enable/disable log calls at compile time according to global level.
//
// define SPDLOG_ACTIVE_LEVEL to one of those (before including spdlog.h):
// SPDLOG_LEVEL_TRACE,
// SPDLOG_LEVEL_DEBUG,
// SPDLOG_LEVEL_INFO,
// SPDLOG_LEVEL_WARN,
// SPDLOG_LEVEL_ERROR,
// SPDLOG_LEVEL_CRITICAL,
// SPDLOG_LEVEL_OFF
//

#define SPDLOG_LOGGER_CALL(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, level, __VA_ARGS__)

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_TRACE
#    define SPDLOG_LOGGER_TRACE(logger, ...) SPDLOG_LOGGER_CALL(logger, spdlog::level::trace, __VA_ARGS__)
#    define SPDLOG_TRACE(...) SPDLOG_LOGGER_TRACE(spdlog::default_logger_raw(), __VA_ARGS__)
#else
#    define SPDLOG_LOGGER_TRACE(logger, ...) SPDLOG_DUMMY_EXECUTOR
#    define SPDLOG_TRACE(...) SPDLOG_DUMMY_EXECUTOR
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_DEBUG
#    define SPDLOG_LOGGER_DEBUG(logger, ...) SPDLOG_LOGGER_CALL(logger, spdlog::level::debug, __VA_ARGS__)
#    define SPDLOG_DEBUG(...) SPDLOG_LOGGER_DEBUG(spdlog::default_logger_raw(), __VA_ARGS__)
#else
#    define SPDLOG_LOGGER_DEBUG(logger, ...) SPDLOG_DUMMY_EXECUTOR
#    define SPDLOG_DEBUG(...) SPDLOG_DUMMY_EXECUTOR
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_INFO
#    define SPDLOG_LOGGER_INFO(logger, ...) SPDLOG_LOGGER_CALL(logger, spdlog::level::info, __VA_ARGS__)
#    define SPDLOG_INFO(...) SPDLOG_LOGGER_INFO(spdlog::default_logger_raw(), __VA_ARGS__)
#else
#    define SPDLOG_LOGGER_INFO(logger, ...) SPDLOG_DUMMY_EXECUTOR
#    define SPDLOG_INFO(...) SPDLOG_DUMMY_EXECUTOR
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_WARN
#    define SPDLOG_LOGGER_WARN(logger, ...) SPDLOG_LOGGER_CALL(logger, spdlog::level::warn, __VA_ARGS__)
#    define SPDLOG_WARN(...) SPDLOG_LOGGER_WARN(spdlog::default_logger_raw(), __VA_ARGS__)
#else
#    define SPDLOG_LOGGER_WARN(logger, ...) SPDLOG_DUMMY_EXECUTOR
#    define SPDLOG_WARN(...) SPDLOG_DUMMY_EXECUTOR
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_ERROR
#    define SPDLOG_LOGGER_ERROR(logger, ...) SPDLOG_LOGGER_CALL(logger, spdlog::level::err, __VA_ARGS__)
#    define SPDLOG_ERROR(...) SPDLOG_LOGGER_ERROR(spdlog::default_logger_raw(), __VA_ARGS__)
#else
#    define SPDLOG_LOGGER_ERROR(logger, ...) SPDLOG_DUMMY_EXECUTOR
#    define SPDLOG_ERROR(...) SPDLOG_DUMMY_EXECUTOR
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_CRITICAL
#    define SPDLOG_LOGGER_CRITICAL(logger, ...) SPDLOG_LOGGER_CALL(logger, spdlog::level::critical, __VA_ARGS__)
#    define SPDLOG_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(spdlog::default_logger_raw(), __VA_ARGS__)
#else
#    define SPDLOG_LOGGER_CRITICAL(logger, ...) SPDLOG_DUMMY_EXECUTOR
#    define SPDLOG_CRITICAL(...) SPDLOG_DUMMY_EXECUTOR
#endif

#ifdef SPDLOG_HEADER_ONLY
#    include "spdlog-inl.h"
#endif

#endif // SPDLOG_H
