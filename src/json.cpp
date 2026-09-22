#include "alar_json/json.hpp"
#include "json_parser.hpp"

#include <sstream>

namespace alar
{
    JsonValue parse(std::string_view input)
    {
        json_parser::Parser parser{input};
        auto result = parser.parse();
        if (!result.has_value())
        {
            throw JsonParseException{result.error().get_error(), result.error().line(), result.error().column()};
        }
        return std::move(result.value());
    }

    JsonValue parse(std::istream& input)
    {
        std::ostringstream buffer;
        buffer << input.rdbuf();

        return parse(buffer.str());
    }
} // namespace alar