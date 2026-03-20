/*
 * OperationEncoder.hpp
 */

#ifndef SOFTADASTRA_STORE_OPERATION_ENCODER_HPP
#define SOFTADASTRA_STORE_OPERATION_ENCODER_HPP

#include <vector>
#include <cstdint>
#include <cstring>

#include <softadastra/store/core/Operation.hpp>

namespace softadastra::store::encoding
{
  namespace core = softadastra::store::core;
  namespace types = softadastra::store::types;

  class OperationEncoder
  {
  public:
    static std::vector<std::uint8_t> encode(const core::Operation &op)
    {
      const std::uint32_t key_size =
          static_cast<std::uint32_t>(op.key.value.size());

      const std::uint32_t value_size =
          static_cast<std::uint32_t>(op.value.data.size());

      const std::size_t total_size =
          sizeof(std::uint8_t) + // type
          sizeof(std::uint32_t) + key_size +
          sizeof(std::uint32_t) + value_size +
          sizeof(std::uint64_t); // timestamp

      std::vector<std::uint8_t> buffer(total_size);

      std::size_t offset = 0;

      // type
      write(buffer, offset, static_cast<std::uint8_t>(op.type));

      // key size + key
      write(buffer, offset, key_size);
      if (key_size > 0)
      {
        std::memcpy(buffer.data() + offset,
                    op.key.value.data(),
                    key_size);
        offset += key_size;
      }

      // value size + value
      write(buffer, offset, value_size);
      if (value_size > 0)
      {
        std::memcpy(buffer.data() + offset,
                    op.value.data.data(),
                    value_size);
        offset += value_size;
      }

      // timestamp
      write(buffer, offset, op.timestamp);

      return buffer;
    }

  private:
    template <typename T>
    static void write(std::vector<std::uint8_t> &buffer,
                      std::size_t &offset,
                      T value)
    {
      std::memcpy(buffer.data() + offset, &value, sizeof(T));
      offset += sizeof(T);
    }
  };

} // namespace softadastra::store::encoding

#endif
