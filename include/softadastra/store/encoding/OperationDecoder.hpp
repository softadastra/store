/*
 * OperationDecoder.hpp
 */

#ifndef SOFTADASTRA_STORE_OPERATION_DECODER_HPP
#define SOFTADASTRA_STORE_OPERATION_DECODER_HPP

#include <vector>
#include <optional>
#include <cstdint>
#include <cstring>

#include <softadastra/store/core/Operation.hpp>

namespace softadastra::store::encoding
{
  namespace core = softadastra::store::core;
  namespace types = softadastra::store::types;

  class OperationDecoder
  {
  public:
    static std::optional<core::Operation> decode(const std::uint8_t *data,
                                                 std::size_t size)
    {
      if (size < minimum_size())
        return std::nullopt;

      std::size_t offset = 0;

      core::Operation op;

      // type
      std::uint8_t type;
      read(data, offset, type);
      op.type = static_cast<types::OperationType>(type);

      // key
      std::uint32_t key_size;
      read(data, offset, key_size);

      if (offset + key_size > size)
        return std::nullopt;

      if (key_size > 0)
      {
        op.key.value.assign(
            reinterpret_cast<const char *>(data + offset),
            key_size);
        offset += key_size;
      }

      // value
      std::uint32_t value_size;
      read(data, offset, value_size);

      if (offset + value_size > size)
        return std::nullopt;

      if (value_size > 0)
      {
        op.value.data.resize(value_size);
        std::memcpy(op.value.data.data(),
                    data + offset,
                    value_size);
        offset += value_size;
      }

      // timestamp
      if (offset + sizeof(std::uint64_t) > size)
        return std::nullopt;

      read(data, offset, op.timestamp);

      return op;
    }

  private:
    static constexpr std::size_t minimum_size()
    {
      return sizeof(std::uint8_t) +  // type
             sizeof(std::uint32_t) + // key size
             sizeof(std::uint32_t) + // value size
             sizeof(std::uint64_t);  // timestamp
    }

    template <typename T>
    static void read(const std::uint8_t *data,
                     std::size_t &offset,
                     T &value)
    {
      std::memcpy(&value, data + offset, sizeof(T));
      offset += sizeof(T);
    }
  };

} // namespace softadastra::store::encoding

#endif
