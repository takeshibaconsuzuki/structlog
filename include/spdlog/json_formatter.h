#pragma once

#include <spdlog/details/os.h>
#include <spdlog/formatter.h>
#include <spdlog/populators.h>

#include <memory>
#include <string>

namespace spdlog {

class json_formatter : public formatter
{
private:
    const std::string kEOL;

    populators::populator_set populators_;

    static populators::populator_set make_default_populators_();

public:
    json_formatter(std::string eol = spdlog::details::os::default_eol);

    json_formatter(populators::populator_set &&populators, std::string eol = spdlog::details::os::default_eol);

    virtual void format(const details::log_msg &msg, memory_buf_t &dest) override;

    virtual std::unique_ptr<formatter> clone() const override;
};

} // namespace spdlog

#ifdef SPDLOG_HEADER_ONLY
#    include "json_formatter-inl.h"
#endif
