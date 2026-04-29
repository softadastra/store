/**
 *
 *  @file Serializer.hpp
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

#ifndef SOFTADASTRA_STORE_SERIALIZER_HPP
#define SOFTADASTRA_STORE_SERIALIZER_HPP

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace softadastra::store::utils
{
  /**
   * @brief Small deterministic serialization helpers.
   *
   * Serializer provides basic byte conversion helpers used by the store module.
   *
   * It is intentionally simple and dependency-free.
   *
   * Rules:
   * - Strings are stored as raw bytes.
   * - Integer helpers use little-endian order.
   * - POD-style helpers are restricted to trivially copyable types.
   * - from_bytes() returns std::nullopt when bytes are insufficient.
   *
   * Prefer explicit integer helpers for stable persisted formats.
   */
  class Serializer
  {
  public:
    /**
     * @brief Converts a string view to bytes.
     *
     * @param value String or binary-compatible text.
     * @return Byte vector.
     */
    [[nodiscard]] static std::vector<std::uint8_t>
    to_bytes(std::string_view value)
    {
      return std::vector<std::uint8_t>(value.begin(), value.end());
    }

    /**
     * @brief Converts bytes to a string.
     *
     * This does not validate UTF-8.
     *
     * @param data Bytes to convert.
     * @return String built from bytes.
     */
    [[nodiscard]] static std::string
    to_string(std::span<const std::uint8_t> data)
    {
      return std::string(data.begin(), data.end());
    }

    /**
     * @brief Converts a byte vector to a string.
     *
     * @param data Bytes to convert.
     * @return String built from bytes.
     */
    [[nodiscard]] static std::string
    to_string(const std::vector<std::uint8_t> &data)
    {
      return to_string(std::span<const std::uint8_t>(data.data(), data.size()));
    }

    /**
     * @brief Serializes a trivially copyable value as raw bytes.
     *
     * This helper is intended for temporary in-memory conversions.
     * For persisted formats, prefer explicit little-endian helpers.
     *
     * @tparam T Trivially copyable type.
     * @param value Value to serialize.
     * @return Byte vector.
     */
    template <typename T>
    [[nodiscard]] static std::vector<std::uint8_t>
    to_raw_bytes(const T &value)
      requires std::is_trivially_copyable_v<T>
    {
      const auto *ptr =
          reinterpret_cast<const std::uint8_t *>(&value);

      return std::vector<std::uint8_t>(ptr, ptr + sizeof(T));
    }

    /**
     * @brief Deserializes a trivially copyable value from raw bytes.
     *
     * Returns std::nullopt if there are not enough bytes.
     *
     * @tparam T Trivially copyable type.
     * @param data Byte vector.
     * @return Decoded value or std::nullopt.
     */
    template <typename T>
    [[nodiscard]] static std::optional<T>
    from_raw_bytes(const std::vector<std::uint8_t> &data)
      requires std::is_trivially_copyable_v<T>
    {
      return from_raw_bytes<T>(
          std::span<const std::uint8_t>(data.data(), data.size()));
    }

    /**
     * @brief Deserializes a trivially copyable value from raw bytes.
     *
     * Returns std::nullopt if there are not enough bytes.
     *
     * @tparam T Trivially copyable type.
     * @param data Byte span.
     * @return Decoded value or std::nullopt.
     */
    template <typename T>
    [[nodiscard]] static std::optional<T>
    from_raw_bytes(std::span<const std::uint8_t> data)
      requires std::is_trivially_copyable_v<T>
    {
      if (data.size() < sizeof(T))
      {
        return std::nullopt;
      }

      T value{};

      auto *out = reinterpret_cast<std::uint8_t *>(&value);

      for (std::size_t i = 0; i < sizeof(T); ++i)
      {
        out[i] = data[i];
      }

      return value;
    }

    /**
     * @brief Appends an unsigned 8-bit integer.
     */
    static void append_u8(
        std::vector<std::uint8_t> &out,
        std::uint8_t value)
    {
      out.push_back(value);
    }

    /**
     * @brief Appends an unsigned 32-bit integer in little-endian order.
     */
    static void append_u32(
        std::vector<std::uint8_t> &out,
        std::uint32_t value)
    {
      for (std::uint8_t i = 0; i < 4; ++i)
      {
        out.push_back(static_cast<std::uint8_t>((value >> (i * 8)) & 0xFFU));
      }
    }

    /**
     * @brief Appends an unsigned 64-bit integer in little-endian order.
     */
    static void append_u64(
        std::vector<std::uint8_t> &out,
        std::uint64_t value)
    {
      for (std::uint8_t i = 0; i < 8; ++i)
      {
        out.push_back(static_cast<std::uint8_t>((value >> (i * 8)) & 0xFFU));
      }
    }

    /**
     * @brief Appends a signed 64-bit integer in little-endian order.
     */
    static void append_i64(
        std::vector<std::uint8_t> &out,
        std::int64_t value)
    {
      append_u64(out, static_cast<std::uint64_t>(value));
    }

    /**
     * @brief Reads an unsigned 8-bit integer from a buffer.
     *
     * @param data Input bytes.
     * @param offset Current offset, updated on success.
     * @param value Output value.
     * @return true on success.
     */
    [[nodiscard]] static bool read_u8(
        std::span<const std::uint8_t> data,
        std::size_t &offset,
        std::uint8_t &value) noexcept
    {
      if (!can_read(data, offset, 1))
      {
        return false;
      }

      value = data[offset];
      ++offset;
      return true;
    }

    /**
     * @brief Reads an unsigned 32-bit little-endian integer.
     *
     * @param data Input bytes.
     * @param offset Current offset, updated on success.
     * @param value Output value.
     * @return true on success.
     */
    [[nodiscard]] static bool read_u32(
        std::span<const std::uint8_t> data,
        std::size_t &offset,
        std::uint32_t &value) noexcept
    {
      if (!can_read(data, offset, 4))
      {
        return false;
      }

      value = 0;

      for (std::uint8_t i = 0; i < 4; ++i)
      {
        value |= static_cast<std::uint32_t>(data[offset + i]) << (i * 8);
      }

      offset += 4;
      return true;
    }

    /**
     * @brief Reads an unsigned 64-bit little-endian integer.
     *
     * @param data Input bytes.
     * @param offset Current offset, updated on success.
     * @param value Output value.
     * @return true on success.
     */
    [[nodiscard]] static bool read_u64(
        std::span<const std::uint8_t> data,
        std::size_t &offset,
        std::uint64_t &value) noexcept
    {
      if (!can_read(data, offset, 8))
      {
        return false;
      }

      value = 0;

      for (std::uint8_t i = 0; i < 8; ++i)
      {
        value |= static_cast<std::uint64_t>(data[offset + i]) << (i * 8);
      }

      offset += 8;
      return true;
    }

    /**
     * @brief Reads a signed 64-bit little-endian integer.
     *
     * @param data Input bytes.
     * @param offset Current offset, updated on success.
     * @param value Output value.
     * @return true on success.
     */
    [[nodiscard]] static bool read_i64(
        std::span<const std::uint8_t> data,
        std::size_t &offset,
        std::int64_t &value) noexcept
    {
      std::uint64_t raw = 0;

      if (!read_u64(data, offset, raw))
      {
        return false;
      }

      value = static_cast<std::int64_t>(raw);
      return true;
    }

    /**
     * @brief Returns true if count bytes can be read safely.
     */
    [[nodiscard]] static bool can_read(
        std::span<const std::uint8_t> data,
        std::size_t offset,
        std::size_t count) noexcept
    {
      return offset <= data.size() && count <= data.size() - offset;
    }
  };

} // namespace softadastra::store::utils

#endif // SOFTADASTRA_STORE_SERIALIZER_HPP
