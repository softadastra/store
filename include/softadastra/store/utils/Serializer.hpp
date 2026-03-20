/*
 * Serializer.hpp
 */

#ifndef SOFTADASTRA_STORE_SERIALIZER_HPP
#define SOFTADASTRA_STORE_SERIALIZER_HPP

#include <vector>
#include <string>
#include <cstdint>
#include <cstring>

namespace softadastra::store::utils
{
  struct Serializer
  {
    /**
     * @brief Serialize string → bytes
     */
    static std::vector<std::uint8_t> to_bytes(const std::string &str)
    {
      return std::vector<std::uint8_t>(str.begin(), str.end());
    }

    /**
     * @brief Deserialize bytes → string
     */
    static std::string to_string(const std::vector<std::uint8_t> &data)
    {
      return std::string(data.begin(), data.end());
    }

    /**
     * @brief Serialize POD type → bytes
     */
    template <typename T>
    static std::vector<std::uint8_t> to_bytes(const T &value)
    {
      std::vector<std::uint8_t> buffer(sizeof(T));
      std::memcpy(buffer.data(), &value, sizeof(T));
      return buffer;
    }

    /**
     * @brief Deserialize bytes → POD type
     */
    template <typename T>
    static T from_bytes(const std::vector<std::uint8_t> &data)
    {
      T value{};
      if (data.size() >= sizeof(T))
      {
        std::memcpy(&value, data.data(), sizeof(T));
      }
      return value;
    }

    /**
     * @brief Append POD to buffer
     */
    template <typename T>
    static void append(std::vector<std::uint8_t> &buffer, const T &value)
    {
      std::size_t offset = buffer.size();
      buffer.resize(offset + sizeof(T));
      std::memcpy(buffer.data() + offset, &value, sizeof(T));
    }

    /**
     * @brief Read POD from buffer with offset
     */
    template <typename T>
    static bool read(const std::vector<std::uint8_t> &buffer,
                     std::size_t &offset,
                     T &value)
    {
      if (offset + sizeof(T) > buffer.size())
        return false;

      std::memcpy(&value, buffer.data() + offset, sizeof(T));
      offset += sizeof(T);
      return true;
    }
  };

} // namespace softadastra::store::utils

#endif
