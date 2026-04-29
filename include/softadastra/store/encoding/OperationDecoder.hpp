/**
 *
 *  @file OperationDecoder.hpp
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

#ifndef SOFTADASTRA_STORE_OPERATION_DECODER_HPP
#define SOFTADASTRA_STORE_OPERATION_DECODER_HPP

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <vector>

#include <softadastra/core/Core.hpp>
#include <softadastra/store/core/Operation.hpp>
#include <softadastra/store/types/Key.hpp>
#include <softadastra/store/types/OperationType.hpp>
#include <softadastra/store/types/Value.hpp>

namespace softadastra::store::encoding
{
  namespace core = softadastra::store::core;
  namespace types = softadastra::store::types;
  namespace core_time = softadastra::core::time;

  /**
   * @brief Decodes store operations from stable binary bytes.
   *
   * OperationDecoder reads the binary format produced by OperationEncoder
   * and reconstructs a store Operation.
   *
   * Binary format version 1:
   *
   * @code
   * uint8  version
   * uint8  operation_type
   * uint32 key_size
   * bytes  key
   * uint32 value_size
   * bytes  value
   * int64  timestamp_millis
   * @endcode
   *
   * Integer values are decoded in little-endian order.
   *
   * Invalid, incomplete, unsupported, or corrupted payloads return
   * std::nullopt.
   */
  class OperationDecoder
  {
  public:
    /**
     * @brief Supported operation payload format version.
     */
    static constexpr std::uint8_t supported_format_version = 1;

    /**
     * @brief Maximum key size accepted by the decoder.
     */
    static constexpr std::uint32_t max_key_size = 1024u * 1024u;

    /**
     * @brief Maximum value size accepted by the decoder.
     */
    static constexpr std::uint32_t max_value_size = 64u * 1024u * 1024u;

    /**
     * @brief Decodes a store operation from a byte vector.
     *
     * @param data Encoded operation bytes.
     * @return Decoded operation on success, std::nullopt on failure.
     */
    [[nodiscard]] static std::optional<core::Operation>
    decode(const std::vector<std::uint8_t> &data)
    {
      return decode(std::span<const std::uint8_t>(data.data(), data.size()));
    }

    /**
     * @brief Decodes a store operation from raw bytes.
     *
     * @param data Pointer to encoded bytes.
     * @param size Number of bytes.
     * @return Decoded operation on success, std::nullopt on failure.
     */
    [[nodiscard]] static std::optional<core::Operation>
    decode(const std::uint8_t *data, std::size_t size)
    {
      if (data == nullptr && size != 0)
      {
        return std::nullopt;
      }

      return decode(std::span<const std::uint8_t>(data, size));
    }

    /**
     * @brief Decodes a store operation from a byte span.
     *
     * @param data Encoded operation bytes.
     * @return Decoded operation on success, std::nullopt on failure.
     */
    [[nodiscard]] static std::optional<core::Operation>
    decode(std::span<const std::uint8_t> data)
    {
      Reader reader(data);

      std::uint8_t version = 0;
      if (!reader.read_u8(version))
      {
        return std::nullopt;
      }

      if (version != supported_format_version)
      {
        return std::nullopt;
      }

      std::uint8_t raw_type = 0;
      if (!reader.read_u8(raw_type))
      {
        return std::nullopt;
      }

      const auto operation_type =
          static_cast<types::OperationType>(raw_type);

      if (!types::is_valid(operation_type))
      {
        return std::nullopt;
      }

      std::string key_string;
      if (!reader.read_string(key_string, max_key_size))
      {
        return std::nullopt;
      }

      types::Value::Container value_bytes;
      if (!reader.read_bytes(value_bytes, max_value_size))
      {
        return std::nullopt;
      }

      std::int64_t timestamp_millis = 0;
      if (!reader.read_i64(timestamp_millis))
      {
        return std::nullopt;
      }

      if (!reader.done())
      {
        return std::nullopt;
      }

      core::Operation operation{
          operation_type,
          types::Key{std::move(key_string)},
          types::Value::from_bytes(std::move(value_bytes)),
          core_time::Timestamp::from_millis(timestamp_millis)};

      if (!operation.is_valid())
      {
        return std::nullopt;
      }

      return operation;
    }

  private:
    /**
     * @brief Bounds-checked little-endian binary reader.
     */
    class Reader
    {
    public:
      /**
       * @brief Creates a reader over immutable bytes.
       *
       * @param data Encoded byte span.
       */
      explicit Reader(std::span<const std::uint8_t> data) noexcept
          : data_(data)
      {
      }

      /**
       * @brief Reads an unsigned 8-bit integer.
       */
      [[nodiscard]] bool read_u8(std::uint8_t &value) noexcept
      {
        if (!can_read(1))
        {
          return false;
        }

        value = data_[offset_];
        ++offset_;
        return true;
      }

      /**
       * @brief Reads an unsigned 32-bit little-endian integer.
       */
      [[nodiscard]] bool read_u32(std::uint32_t &value) noexcept
      {
        if (!can_read(4))
        {
          return false;
        }

        value = 0;

        for (std::uint8_t i = 0; i < 4; ++i)
        {
          value |= static_cast<std::uint32_t>(data_[offset_ + i]) << (i * 8);
        }

        offset_ += 4;
        return true;
      }

      /**
       * @brief Reads an unsigned 64-bit little-endian integer.
       */
      [[nodiscard]] bool read_u64(std::uint64_t &value) noexcept
      {
        if (!can_read(8))
        {
          return false;
        }

        value = 0;

        for (std::uint8_t i = 0; i < 8; ++i)
        {
          value |= static_cast<std::uint64_t>(data_[offset_ + i]) << (i * 8);
        }

        offset_ += 8;
        return true;
      }

      /**
       * @brief Reads a signed 64-bit little-endian integer.
       */
      [[nodiscard]] bool read_i64(std::int64_t &value) noexcept
      {
        std::uint64_t raw = 0;

        if (!read_u64(raw))
        {
          return false;
        }

        value = static_cast<std::int64_t>(raw);
        return true;
      }

      /**
       * @brief Reads a length-prefixed string.
       */
      [[nodiscard]] bool read_string(
          std::string &value,
          std::uint32_t max_size)
      {
        std::uint32_t size = 0;

        if (!read_u32(size))
        {
          return false;
        }

        if (size > max_size)
        {
          return false;
        }

        if (!can_read(size))
        {
          return false;
        }

        value.assign(
            reinterpret_cast<const char *>(data_.data() + offset_),
            size);

        offset_ += size;
        return true;
      }

      /**
       * @brief Reads a length-prefixed byte vector.
       */
      [[nodiscard]] bool read_bytes(
          std::vector<std::uint8_t> &value,
          std::uint32_t max_size)
      {
        std::uint32_t size = 0;

        if (!read_u32(size))
        {
          return false;
        }

        if (size > max_size)
        {
          return false;
        }

        if (!can_read(size))
        {
          return false;
        }

        value.assign(
            data_.begin() + static_cast<std::ptrdiff_t>(offset_),
            data_.begin() + static_cast<std::ptrdiff_t>(offset_ + size));

        offset_ += size;
        return true;
      }

      /**
       * @brief Returns true when all bytes have been consumed.
       */
      [[nodiscard]] bool done() const noexcept
      {
        return offset_ == data_.size();
      }

    private:
      /**
       * @brief Returns true if count bytes can be read safely.
       */
      [[nodiscard]] bool can_read(std::size_t count) const noexcept
      {
        return count <= data_.size() - offset_;
      }

    private:
      std::span<const std::uint8_t> data_;
      std::size_t offset_{0};
    };
  };

} // namespace softadastra::store::encoding

#endif // SOFTADASTRA_STORE_OPERATION_DECODER_HPP
