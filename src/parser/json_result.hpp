#pragma once
#include <utility>
#include <variant>

namespace alar::detail
{
    template <typename T, typename E>
    class Result
    {
      public:
        // success
        Result(T value)
            : m_result{std::move(value)}
        {
        }

        // error
        Result(E error)
            : m_result{std::move(error)}
        {
        }

        [[nodiscard]]
        bool has_value() const noexcept
        {
            return std::holds_alternative<T>(m_result);
        }

        [[nodiscard]]
        explicit operator bool() const noexcept
        {
            return has_value();
        }

        T& value() &
        {
            return std::get<T>(m_result);
        }

        const T& value() const&
        {
            return std::get<T>(m_result);
        }

        T&& value() &&
        {
            return std::get<T>(std::move(m_result));
        }

        E& error() &
        {
            return std::get<E>(m_result);
        }

        const E& error() const&
        {
            return std::get<E>(m_result);
        }

        E&& error() &&
        {
            return std::get<E>(std::move(m_result));
        }

      private:
        std::variant<T, E> m_result;
    };
} // namespace alar::detail
