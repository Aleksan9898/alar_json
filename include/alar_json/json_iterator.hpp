#pragma once

#include "alar_json/json_error.hpp"
#include "alar_json/json_types.hpp"

#include <string_view>
#include <variant>

namespace alar
{
    /**
     * @brief Iterator for traversing JSON objects and arrays.
     *
     * BasicJsonIterator provides a common iterator implementation for both
     * mutable and const JSON iterators.
     *
     * @tparam ObjectIterator Iterator type used to traverse JSON objects.
     * @tparam ArrayIterator Iterator type used to traverse JSON arrays.
     */
    template <typename ObjectIterator, typename ArrayIterator>
    class BasicJsonIterator
    {
      public:
        /**
         * @brief Stores either an object iterator or an array iterator.
         */
        using Iterator = std::variant<ObjectIterator, ArrayIterator>;

        /**
         * @brief Constructs an iterator from an object iterator.
         *
         * @param it Object iterator.
         */
        explicit BasicJsonIterator(ObjectIterator it)
            : m_iterator{it}
        {
        }

        /**
         * @brief Constructs an iterator from an array iterator.
         *
         * @param it Array iterator.
         */
        explicit BasicJsonIterator(ArrayIterator it)
            : m_iterator{it}
        {
        }

        /**
         * @brief Returns the key of the current JSON object element.
         *
         * This function is only available when the iterator refers to a
         * JSON object.
         *
         * @return String view containing the current object's key.
         *
         * @throws JsonTypeError If the iterator refers to a JSON array.
         */
        std::string_view key() const
        {
            if (std::holds_alternative<ArrayIterator>(m_iterator))
            {
                throw JsonTypeError("JsonIterator::key() is only available for JSON objects");
            }

            return std::get<ObjectIterator>(m_iterator)->first;
        }

        /**
         * @brief Returns the current JSON value.
         *
         * @return Reference to the current JSON value.
         */
        decltype(auto) value() const
        {
            if (std::holds_alternative<ArrayIterator>(m_iterator))
            {
                return (*std::get<ArrayIterator>(m_iterator));
            }

            return (std::get<ObjectIterator>(m_iterator)->second);
        }

        /**
         * @brief Dereferences the iterator.
         *
         * @return Reference to the current JSON value.
         */
        decltype(auto) operator*() const
        {
            return value();
        }

        /**
         * @brief Accesses the current JSON value through the iterator.
         *
         * @return Pointer to the current JSON value.
         */
        decltype(auto) operator->() const
        {
            return &value();
        }

        /**
         * @brief Advances the iterator to the next element.
         *
         * @return Reference to this iterator.
         */
        BasicJsonIterator& operator++()
        {
            if (std::holds_alternative<ArrayIterator>(m_iterator))
            {
                ++std::get<ArrayIterator>(m_iterator);
            }
            else
            {
                ++std::get<ObjectIterator>(m_iterator);
            }

            return *this;
        }

        /**
         * @brief Advances the iterator to the next element.
         *
         * The returned iterator refers to the position before the increment.
         *
         * @return Copy of the iterator before incrementing.
         */
        BasicJsonIterator operator++(int)
        {
            BasicJsonIterator temp{*this};
            ++(*this);

            return temp;
        }

        /**
         * @brief Compares two iterators for equality.
         *
         * Iterators containing different underlying iterator types are
         * considered unequal.
         *
         * @param other Iterator to compare with.
         *
         * @return true if both iterators refer to the same position,
         *         otherwise false.
         */
        bool operator==(const BasicJsonIterator& other) const
        {
            if (m_iterator.index() != other.m_iterator.index())
            {
                return false;
            }

            if (std::holds_alternative<ObjectIterator>(m_iterator))
            {
                return std::get<ObjectIterator>(m_iterator) == std::get<ObjectIterator>(other.m_iterator);
            }

            return std::get<ArrayIterator>(m_iterator) == std::get<ArrayIterator>(other.m_iterator);
        }

        /**
         * @brief Compares two iterators for inequality.
         *
         * @param other Iterator to compare with.
         *
         * @return true if the iterators are not equal, otherwise false.
         */
        bool operator!=(const BasicJsonIterator& other) const
        {
            return !(*this == other);
        }

      private:
        Iterator m_iterator;
    };

    /**
     * @brief Mutable iterator for JSON objects and arrays.
     */
    using JsonIterator = BasicJsonIterator<JsonObject::iterator, JsonArray::iterator>;

    /**
     * @brief Const iterator for JSON objects and arrays.
     */
    using JsonConstIterator = BasicJsonIterator<JsonObject::const_iterator, JsonArray::const_iterator>;

} // namespace alar
