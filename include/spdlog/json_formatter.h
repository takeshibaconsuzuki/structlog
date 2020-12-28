#pragma once

#include <spdlog/details/os.h>
#include <spdlog/formatter.h>

namespace spdlog {

class JSONFormatter : public formatter
{
private:
    const std::string kEOL;

public:
    JSONFormatter(std::string eol = spdlog::details::os::default_eol)
        : kEOL(std::move(eol))
    {}

    void format(const details::log_msg &msg, memory_buf_t &dest) override
    {
        nlohmann::json entry{{"message", std::string(msg.payload.data(), msg.payload.size())}};
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
        return details::make_unique<JSONFormatter>();
    }
};

} // namespace spdlog
