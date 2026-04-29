/**
 *
 *  @file Value.hpp
 *  @author Gaspard Kirira
 *
 *  Copyright 2026, Softadastra.
 *  All rights reserved.
 *  https://github.com/softadastra/softadastra
 *
 *  Licensed under the Apache License, Version 2.0.
 *
 *  Softadastra Store
 *
 */

#ifndef SOFTADASTRA_STORE_VALUE_HPP
#define SOFTADASTRA_STORE_VALUE_HPP

#include <cstddef>
#include <cstdint>
#include <span>
#include <string_view>
#include <utility>
#include <vector>

namespace softadastra::store::types
{
  /**
   * @brief Binary-safe value stored by the store.
   *
   * Value wraps raw bytes and is intentionally domain-agnostic.
   *
   * It is used by:
   * - Entry
   * - Operation
   * - StoreEngine
   * - SnapshotStore
   * - serializers
   *
   * Rules:
   * - Values are binary-safe.
   * - Empty values are allowed.
   * - The store does not interpret value bytes.
   * - Higher-level modules decide what the bytes represent.
   */
  class Value
  {
  public:
    /**
     * @brief Internal byte container type.
     */
    using Container = std::vector<std::uint8_t>;

    /**
     * @brief Creates an empty value.
     */
    Value() = default;

    /**
     * @brief Creates a value from bytes.
     *
     * @param data Binary value bytes.
     */
    explicit Value(Container data)
        : data_(std::move(data))
    {
    }

    /**
     * @brief Creates a value from a byte span.
     *
     * @param data Binary value bytes.
     */
    explicit Value(std::span<const std::uint8_t> data)
        : data_(data.begin(), data.end())
    {
    }

    /**
     * @brief Creates a value from a string view.
     *
     * The string is stored as raw bytes.
     *
     * @param data Text or binary-compatible bytes.
     */
    explicit Value(std::string_view data)
        : data_(data.begin(), data.end())
    {
    }

    /**
     * @brief Creates a value from bytes.
     *
     * @param data Binary value bytes.
     * @return Value instance.
     */
    [[nodiscard]] static Value from_bytes(Container data)
    {
      return Value(std::move(data));
    }

    /**
     * @brief Creates a value from a string view.
     *
     * @param data Text payload.
     * @return Value instance.
     */
    [[nodiscard]] static Value from_string(std::string_view data)
    {
      return Value(data);
    }

    /**
     * @brief Returns the stored bytes.
     *
     * @return Read-only byte container.
     */
    [[nodiscard]] const Container &bytes() const noexcept
    {
      return data_;
    }

    /**
     * @brief Returns the stored bytes.
     *
     * Alias for bytes(), useful for generic serializers.
     *
     * @return Read-only byte container.
     */
    [[nodiscard]] const Container &data() const noexcept
    {
      return data_;
    }

    /**
     * @brief Returns a mutable reference to the stored bytes.
     *
     * @return Mutable byte container.
     */
    [[nodiscard]] Container &data() noexcept
    {
      return data_;
    }

    /**
     * @brief Returns true if the value has no bytes.
     *
     * @return true when empty.
     */
    [[nodiscard]] bool empty() const noexcept
    {
      return data_.empty();
    }

    /**
     * @brief Returns the number of stored bytes.
     *
     * @return Value size in bytes.
     */
    [[nodiscard]] std::size_t size() const noexcept
    {
      return data_.size();
    }

    /**
     * @brief Removes all stored bytes.
     */
    void clear() noexcept
    {
      data_.clear();
    }

    /**
     * @brief Converts the value to a string.
     *
     * This treats bytes as characters and does not validate UTF-8.
     *
     * @return String built from the stored bytes.
     */
    [[nodiscard]] std::string to_string() const
    {
      return std::string(data_.begin(), data_.end());
    }

    /**
     * @brief Compares two values for equality.
     */
    [[nodiscard]] friend bool operator==(
        const Value &a,
        const Value &b)
    {
      return a.data_ == b.data_;
    }

    /**
     * @brief Compares two values for inequality.
     */
    [[nodiscard]] friend bool operator!=(
        const Value &a,
        const Value &b)
    {
      return !(a == b);
    }

  private:
    Container data_{};
  };

} // namespace softadastra::store::types

#endif // SOFTADASTRA_STORE_VALUE_HPP
