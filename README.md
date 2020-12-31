# structlog

A structured logging library built off of
[spdlog](https://github.com/gabime/spdlog) and
[nlohmann_json](https://github.com/nlohmann/json).

This project aims to be a drop-in replacement for spdlog. Everything should
work exactly the same with the following exception: the default formatter has
been changed to `JSONFormatter`. This means that if you have 1) not set a
custom formatter and 2) have not set a pattern, then your logs should
automatically start logging in JSON.

## Install

### Header only version

Copy the include
[folder](https://github.com/gabime/spdlog/tree/v1.x/include/spdlog) to your
build tree and use a C++11 compiler.

### Static lib version (recommended - much faster compile times)

```
$ git clone https://github.com/gabime/spdlog.git
$ cd spdlog && mkdir build && cd build
$ cmake .. && make -j
```

## Usage

structlog is a superset of spdlog. All functionality provided by spdlog should
be available in structlog. The following program:

```c++
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_sinks.h>

int main() {
  spdlog::info("lorem");
  SPDLOG_INFO("ipsum");
  auto logger = spdlog::stdout_logger_mt("foo");
  logger->info("dolor");
  spdlog::get("foo")->info("sit");
  SPDLOG_LOGGER_INFO(logger, "amet");

  // fmt still works
  spdlog::error("Some error message with arg: {}", 1);

  spdlog::set_pattern("[%l] %v");
  // This will not log in JSON because the formatter has been changed to pattern_formatter
  spdlog::info("consectetur");

  // set formatter back to JSONFormatter
  spdlog::set_formatter(spdlog::details::make_unique<spdlog::JSONFormatter>());
  spdlog::info("adipiscing");
}
```

Outputs:

```
{"date_time":"2020-12-29 00:28:59.271-06:00","level":"info","message":"lorem"}
{"date_time":"2020-12-29 00:28:59.271-06:00","level":"info","message":"ipsum"}
{"date_time":"2020-12-29 00:28:59.271-06:00","level":"info","logger_name":"foo","message":"dolor"}
{"date_time":"2020-12-29 00:28:59.271-06:00","level":"info","logger_name":"foo","message":"sit"}
{"date_time":"2020-12-29 00:28:59.271-06:00","level":"info","logger_name":"foo","message":"amet"}
{"date_time":"2020-12-29 00:28:59.271-06:00","level":"error","message":"Some error message with arg: 1"}
[info] consectetur
{"date_time":"2020-12-29 00:28:59.271-06:00","level":"info","message":"adipiscing"}
```

The use of fmt is discouraged, however. In structured logging, it is better to
log the argument(s) as a field instead of within the message. structlog
provides an easy way to add extra fields to a log entry by simply chaining
a function call using a JSON as the parameter.

For instance, the following:

```c++
spdlog::info("Welcome to spdlog version {}.{}.{}!", SPDLOG_VER_MAJOR, SPDLOG_VER_MINOR, SPDLOG_VER_PATCH);
```

Can be rewritten as:

```c++
spdlog::info("Welcome to spdlog!")({
  {"version", {
    {"major", SPDLOG_VER_MAJOR},
    {"minor", SPDLOG_VER_MINOR},
    {"patch", SPDLOG_VER_PATCH}}
  }
});
```

Which outputs:

```
{"date_time":"2020-12-29 00:15:41.442-06:00","level":"info","message":"Welcome to spdlog!","version":{"major":1,"minor":8,"patch":2}}
```

As another example, using similar logic to our first program:

```c++
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_sinks.h>

int main() {
  spdlog::info("lorem")({{"ipsum", 0}});
  SPDLOG_INFO("dolor")({{"sit", 1}});
  auto logger = spdlog::stdout_logger_mt("bar");
  logger->info("amet")({{"consectetur", 2}});
  spdlog::get("bar")->info("adipiscing")({{"elit", 3}});
  SPDLOG_LOGGER_INFO(logger, "sed")({{"do", 4}});
}
```

Outputs:

```
{"date_time":"2020-12-29 00:32:34.658-06:00","ipsum":0,"level":"info","message":"lorem"}
{"date_time":"2020-12-29 00:32:34.658-06:00","level":"info","message":"dolor","sit":1}
{"consectetur":2,"date_time":"2020-12-29 00:32:34.658-06:00","level":"info","logger_name":"bar","message":"amet"}
{"date_time":"2020-12-29 00:32:34.658-06:00","elit":3,"level":"info","logger_name":"bar","message":"adipiscing"}
{"date_time":"2020-12-29 00:32:34.658-06:00","do":4,"level":"info","logger_name":"bar","message":"sed"}
```

### Populators

By default, the JSONFormatter adds `date_time`, `level`, `logger_name` (if
available), and `message`. This can by changed through specifying populators.

A populator has the ability to add and remove fields from a log entry before it
is logged. To set the populators, use the `set_populator` function (to set
globally) or the `set_populator` method (on logger or sink instances).
`set_populator` takes a variable number of `std::unique_ptr<Populator>`s:

```c++
spdlog::set_populators(
    spdlog::details::make_unique<spdlog::populators::LevelPopulator>(),
    spdlog::details::make_unique<spdlog::populators::MessagePopulator>());
spdlog::info("abc");
auto logger = spdlog::stdout_logger_mt("baz");
logger->set_populators(
    spdlog::details::make_unique<spdlog::populators::LoggerNamePopulator>(),
    spdlog::details::make_unique<spdlog::populators::MessagePopulator>());
logger->info("def");
auto another_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("baz.txt");
another_sink->set_populators(spdlog::details::make_unique<spdlog::populators::MessagePopulator>());
logger->sinks().push_back(another_sink);
logger->info("ghi");
```

Stdout:

```
{"level":"info","message":"abc"}
{"logger_name":"baz","message":"def"}
{"logger_name":"baz","message":"ghi"}
```

`baz.txt`:

```
{"message":"ghi"}
```

It should be noted that populators are run in an **unspecified** order. Do not
write populators which conflict with each other (e.g. setting the same field),
otherwise you may see inconsistencies.

#### Custom Populators

structlog provides the following populators:
- `DateTimePopulator`. Sets `date_time` in the format
  `YYYY-mm-dd HH:MM:SS.eee[+/-]HH:MM`.
- `LevelPopulator`. Sets `level` to log level.
- `LoggerNamePopulator`. Sets `logger_name` to logger name.
- `MessagePopulator`. Sets `message` to log message.
- `PatternPopulator`. Constructed with a field name and a pattern. Sets the
  field to the format specified by the pattern. See spdlog's pattern
  documentation.
- `PIDPopulator`. Sets `pid` to process ID.
- `SrcLocPopulator`. Sets `src_loc` in the format `file:line`.
- `ThreadIDPopulator`. Sets `thread_id` to thread ID.
- `TimestampPopulator`. Sets `timestamp` to seconds since epoch.

If these are not sufficient, you can extend the `Populator` class. The derived
class must implement:

```c++
void populate(const spdlog::details::log_msg, nlohmann::json &dest);
std::unique_ptr<Populator> clone() const;
```

Example:

```c++
class MyPopulator : public spdlog::Populator {
public:
  void populate(const spdlog::details::log_msg, nlohmann::json &dest) override {
    dest["language"] = "c++";
  }

  std::unique_ptr<Populator> clone() const {
    return spdlog::details::make_unique<MyPopulator>();
  }
};

spdlog::set_populators(
    spdlog::details::make_unique<MyPopulator>(),
    spdlog::details::make_unique<spdlog::populators::MessagePopulator>());
spdlog::info("writing program");
```

Output:

```
{"language":"c++","message":"writing program"}
```

#### PopulatorSet

Populators are a property of JSONFormatter. `set_populator` is a thin wrapper
which passes a new JSONFormatter to `set_formatter`. This may be done manually
on your side instead:

```c++
spdlog::populators::PopulatorSet populators;
populators.insert(spdlog::details::make_unique<...>(...));
spdlog::set_formatter(spdlog::details::make_unique<spdlog::JSONFormatter>(std::move(populators)));
```

## Implementation Details

All log methods on the logger class have return type
`spdlog::details::Executor` instead of void. The Executor class keeps track
of the extra parameters for the entry. The Executor class is callable, and
calling the Executor with a JSON adds the fields to the Executor's JSON and
returns `*this`. As a consequence, Executor calls may be chained multiple
times:

```
spdlog::info(...)(...)(...)(...);
```

The entry is logged when the Executor is destructed. The Executor is not
copyable, only movable, so the entry is only logged once. Executor specifies
the destructor to be `noexcept(false)` so that exceptions may be thrown from
it. Of course, we make sure to catch any exceptions and manually call the
Executor members' destructors before rethrowing.
