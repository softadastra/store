/*
 * Value.hpp
 */

#ifndef SOFTADASTRA_STORE_VALUE_HPP
#define SOFTADASTRA_STORE_VALUE_HPP

#include <vector>
#include <cstdint>

namespace softadastra::store::types
{
  /**
   * @brief Value stored in the store
   *
   * Binary-safe container
   */
  struct Value
  {
    std::vector<std::uint8_t> data;

    bool empty() const noexcept
    {
      return data.empty();
    }

    std::size_t size() const noexcept
    {
      return data.size();
    }
  };

} // namespace softadastra::store::types

#endif
