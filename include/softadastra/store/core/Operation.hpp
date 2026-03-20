/*
 * Operation.hpp
 */

#ifndef SOFTADASTRA_STORE_OPERATION_HPP
#define SOFTADASTRA_STORE_OPERATION_HPP

#include <cstdint>

#include <softadastra/store/types/Key.hpp>
#include <softadastra/store/types/Value.hpp>
#include <softadastra/store/types/OperationType.hpp>

namespace softadastra::store::core
{
  namespace types = softadastra::store::types;

  /**
   * @brief Store operation (logical action)
   *
   * This is what gets encoded into WAL payload.
   */
  struct Operation
  {
    types::OperationType type{types::OperationType::Unknown};

    types::Key key;
    types::Value value;

    std::uint64_t timestamp{0};
  };

} // namespace softadastra::store::core

#endif
