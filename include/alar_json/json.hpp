#pragma once
#include "json_error.hpp"
#include "json_value.hpp"

#include <iosfwd>
#include <string_view>

namespace alar
{
    /**
     * @brief Parses JSON from a string.
     *
     * @param input JSON text.
     * @return Parsed JSON value.
     * @throws JsonParseError If the input contains invalid JSON.
     */
    JsonValue parse(std::string_view input);

    /**
     * @brief Parses JSON from an input stream.
     *
     * @param input Input stream containing JSON text.
     * @return Parsed JSON value.
     * @throws JsonParseError If the input contains invalid JSON.
     */
    JsonValue parse(std::istream& input);

} // namespace alar
