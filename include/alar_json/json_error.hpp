#pragma once

#include <cstddef>
#include <stdexcept>
#include <string>

namespace alar
{
    /**
     * @brief Base exception for errors produced by the alar_json library.
     */
    class JsonError : public std::runtime_error
    {
      public:
        using std::runtime_error::runtime_error;
    };

    /**
     * @brief Exception thrown when a JSON value has an unexpected type.
     *
     * This exception is used when an operation requires a specific JSON type
     * but the current value contains another type.
     */
    class JsonTypeError : public JsonError
    {
      public:
        using JsonError::JsonError;
    };

    /**
     * @brief Exception thrown when a JSON parsing error occurs.
     *
     * Stores the source location where the parsing error was detected.
     */
    class JsonParseException : public JsonError
    {
      public:
        /**
         * @brief Constructs a JSON parse error.
         *
         * @param message Description of the parsing error.
         * @param line Source line where the error occurred.
         * @param column Source column where the error occurred.
         */
        JsonParseException(std::string message, std::size_t line, std::size_t column);

        /**
         * @brief Returns the source line where the error occurred.
         *
         * @return Source line number.
         */
        std::size_t line() const noexcept;

        /**
         * @brief Returns the source column where the error occurred.
         *
         * @return Source column number.
         */
        std::size_t column() const noexcept;

      private:
        std::size_t m_line;
        std::size_t m_column;
    };

} // namespace alar
