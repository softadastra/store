/*
 * OperationType.hpp
 */

#ifndef SOFTADASTRA_STORE_OPERATION_TYPE_HPP
#define SOFTADASTRA_STORE_OPERATION_TYPE_HPP

#include <cstdint>

namespace softadastra::store::types
{
  /**
   * @brief Store operation type
   */
  enum class OperationType : std::uint8_t
  {
    Unknown = 0,

    Put,   // insert or update
    Delete // remove key
  };

} // namespace softadastra::store::types

#endif
