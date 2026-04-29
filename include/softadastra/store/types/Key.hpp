/**
 *
 *  @file Key.hpp
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

#ifndef SOFTADASTRA_STORE_KEY_HPP
#define SOFTADASTRA_STORE_KEY_HPP

#include <string>
#include <string_view>
#include <utility>

namespace softadastra::store::types
{

  /**
   * @brief Strong key used to identify entries in the store.
   *
   * Key is a small value object wrapping a string identifier.
   *
   * It is used by:
   * - StoreEngine
   * - Operation
   * - Entry
   * - Index
   * - SnapshotStore
   *
   * Rules:
   * - A valid key must not be empty.
   * - Keys are ordered lexicographically.
   * - Keys are stable and safe to serialize.
   *
   * The store intentionally keeps keys string-based for now.
   * Binary or hash-based keys can be added later without changing the
   * high-level store API.
   */
  class Key
  {
  public:
    /**
     * @brief Creates an empty key.
     */
    Key() = default;

    /**
     * @brief Creates a key from a string.
     *
     * @param value Key value.
     */
    explicit Key(std::string value)
        : value_(std::move(value))
    {
    }

    /**
     * @brief Creates a key from a string view.
     *
     * @param value Key value.
     */
    explicit Key(std::string_view value)
        : value_(value)
    {
    }

    /**
     * @brief Creates a key from a C string.
     *
     * @param value Key value.
     */
    explicit Key(const char *value)
        : value_(value == nullptr ? "" : value)
    {
    }

    /**
     * @brief Creates a key from a string.
     *
     * This helper improves readability at call sites.
     *
     * @param value Key value.
     * @return Key instance.
     */
    [[nodiscard]] static Key from(std::string value)
    {
      return Key(std::move(value));
    }

    /**
     * @brief Returns the underlying string value.
     *
     * @return Key string.
     */
    [[nodiscard]] const std::string &str() const noexcept
    {
      return value_;
    }

    /**
     * @brief Returns the underlying string value.
     *
     * Alias for str(), useful for generic serializers.
     *
     * @return Key string.
     */
    [[nodiscard]] const std::string &value() const noexcept
    {
      return value_;
    }

    /**
     * @brief Returns true if the key is empty.
     *
     * @return true when the key has no value.
     */
    [[nodiscard]] bool empty() const noexcept
    {
      return value_.empty();
    }

    /**
     * @brief Returns true if the key is usable by the store.
     *
     * @return true when the key is not empty.
     */
    [[nodiscard]] bool is_valid() const noexcept
    {
      return !value_.empty();
    }

    /**
     * @brief Clears the key value.
     */
    void clear() noexcept
    {
      value_.clear();
    }

    /**
     * @brief Compares two keys for equality.
     */
    [[nodiscard]] friend bool operator==(
        const Key &a,
        const Key &b) noexcept
    {
      return a.value_ == b.value_;
    }

    /**
     * @brief Compares two keys for inequality.
     */
    [[nodiscard]] friend bool operator!=(
        const Key &a,
        const Key &b) noexcept
    {
      return !(a == b);
    }

    /**
     * @brief Orders keys lexicographically.
     */
    [[nodiscard]] friend bool operator<(
        const Key &a,
        const Key &b) noexcept
    {
      return a.value_ < b.value_;
    }

  private:
    std::string value_{};
  };

} // namespace softadastra::store::types

#endif // SOFTADASTRA_STORE_KEY_HPP
