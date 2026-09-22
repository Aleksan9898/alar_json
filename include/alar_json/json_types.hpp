#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace alar
{
    class JsonValue;

    using JsonArray = std::vector<JsonValue>;
    using JsonObject = std::unordered_map<std::string, JsonValue>;

} // namespace alar
