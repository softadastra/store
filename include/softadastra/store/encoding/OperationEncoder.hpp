/**
 *
 *  @file OperationEncoder.hpp
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

#ifndef SOFTADASTRA_STORE_OPERATION_ENCODER_HPP
#define SOFTADASTRA_STORE_OPERATION_ENCODER_HPP

#include <cstdint>
#include <string_view>
#include <vector>

#include <softadastra/store/core/Operation.hpp>
#include <softadastra/store/types/OperationType.hpp>

namespace softadastra::store::encoding
{
  namespace core = softadastra::store::core;
  namespace types = softadastra::store::types;

  /**
   * @brief Encodes store operations into stable binary bytes.
   *
   * OperationEncoder converts a store Operation into a deterministic binary
   * payload that can be stored inside the WAL.
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
   * Integer values are encoded in little-endian order.
   *
   * Notes:
   * - Put operations may contain an empty value.
   * - Delete operations encode an empty value.
   * - Invalid operations return an empty buffer.
   */
  class OperationEncoder
  {
  public:
    /**
     * @brief Current operation payload format version.
     */
    static constexpr std::uint8_t format_version = 1;

    /**
     * @brief Maximum key size accepted by the encoder.
     */
    static constexpr std::uint32_t max_key_size = 1024u * 1024u;

    /**
     * @brief Maximum value size accepted by the encoder.
     */
    static constexpr std::uint32_t max_value_size = 64u * 1024u * 1024u;

    /**
     * @brief Encodes a store operation into bytes.
     *
     * @param operation Store operation to encode.
     * @return Encoded binary payload, or an empty vector if invalid.
     */
    [[nodiscard]] static std::vector<std::uint8_t>
    encode(const core::Operation &operation)
    {
      if (!can_encode(operation))
      {
        return {};
      }

      const auto key_size =
          static_cast<std::uint32_t>(operation.key.str().size());

      const auto value_size =
          static_cast<std::uint32_t>(operation.value.size());

      std::vector<std::uint8_t> out;
      out.reserve(
          1 +
          1 +
          4 + key_size +
          4 + value_size +
          8);

      append_u8(out, format_version);
      append_u8(out, static_cast<std::uint8_t>(operation.type));

      append_string(out, operation.key.str());

      append_u32(out, value_size);
      out.insert(
          out.end(),
          operation.value.bytes().begin(),
          operation.value.bytes().end());

      append_i64(out, operation.timestamp.millis());

      return out;
    }

    /**
     * @brief Returns true if an operation can be encoded safely.
     *
     * @param operation Store operation to validate.
     * @return true if the operation is valid and within size limits.
     */
    [[nodiscard]] static bool can_encode(
        const core::Operation &operation) noexcept
    {
      if (!operation.is_valid())
      {
        return false;
      }

      if (operation.key.str().size() > max_key_size)
      {
        return false;
      }

      if (operation.value.size() > max_value_size)
      {
        return false;
      }

      return true;
    }

  private:
    /**
     * @brief Appends one unsigned byte.
     */
    static void append_u8(
        std::vector<std::uint8_t> &out,
        std::uint8_t value)
    {
      out.push_back(value);
    }

    /**
     * @brief Appends a 32-bit unsigned integer in little-endian order.
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
     * @brief Appends a 64-bit unsigned integer in little-endian order.
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
     * @brief Appends a 64-bit signed integer in little-endian order.
     */
    static void append_i64(
        std::vector<std::uint8_t> &out,
        std::int64_t value)
    {
      append_u64(out, static_cast<std::uint64_t>(value));
    }

    /**
     * @brief Appends a length-prefixed string.
     */
    static void append_string(
        std::vector<std::uint8_t> &out,
        std::string_view value)
    {
      append_u32(out, static_cast<std::uint32_t>(value.size()));

      out.insert(
          out.end(),
          value.begin(),
          value.end());
    }
  };

} // namespace softadastra::store::encoding

#endif // SOFTADASTRA_STORE_OPERATION_ENCODER_HPP
