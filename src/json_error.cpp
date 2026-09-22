#include "alar_json/json_error.hpp"

namespace alar
{
    JsonParseException::JsonParseException(std::string message, std::size_t line, std::size_t column)
        : JsonError{std::move(message)},
          m_line{line},
          m_column{column}
    {
    }
    std::size_t JsonParseException::line() const noexcept
    {
        return m_line;
    }
    std::size_t JsonParseException::column() const noexcept
    {
        return m_column;
    }

} // namespace alar
