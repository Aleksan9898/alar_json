#include "alar_json/json.hpp"

#include <cstddef>
#include <cstdint>
#include <string_view>

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size)
{
    const std::string_view input{
        reinterpret_cast<const char*>(data),
        size
    };

    try
    {
        (void)alar::parse(input);
    }
    catch (const alar::JsonParseException&)
    {
        // Invalid JSON is an expected result.
    }

    return 0;
}