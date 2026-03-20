/*
 * Key.hpp
 */

#ifndef SOFTADASTRA_STORE_KEY_HPP
#define SOFTADASTRA_STORE_KEY_HPP

#include <string>

namespace softadastra::store::types
{
  /**
   * @brief Key used in the store
   *
   * Simple string-based key.
   * Can be extended later (hash, binary key, etc.)
   */
  struct Key
  {
    std::string value;

    bool operator==(const Key &other) const noexcept
    {
      return value == other.value;
    }

    bool operator!=(const Key &other) const noexcept
    {
      return !(*this == other);
    }

    bool operator<(const Key &other) const noexcept
    {
      return value < other.value;
    }
  };

} // namespace softadastra::store::types

#endif
