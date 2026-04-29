/**
 *
 *  @file OperationType.hpp
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

#ifndef SOFTADASTRA_STORE_OPERATION_TYPE_HPP
#define SOFTADASTRA_STORE_OPERATION_TYPE_HPP

#include <cstdint>
#include <string_view>

namespace softadastra::store::types
{

  /**
   * @brief Type of mutation applied to the store.
   *
   * OperationType describes the logical action represented by a store
   * operation.
   *
   * It is used by:
   * - Operation
   * - StoreEngine
   * - OperationEncoder
   * - OperationDecoder
   * - WAL integration
   *
   * Rules:
   * - Values must remain stable over time.
   * - Do not reorder existing values.
   * - Do not remove existing values once released.
   * - Add new values only at the end.
   */
  enum class OperationType : std::uint8_t
  {
    /**
     * @brief Unknown or invalid operation type.
     */
    Unknown = 0,

    /**
     * @brief Insert or replace a value for a key.
     */
    Put,

    /**
     * @brief Remove a key from the store.
     */
    Delete
  };

  /**
   * @brief Returns a stable string representation of an operation type.
   *
   * This is intended for logs, diagnostics, and text serialization.
   *
   * @param type Operation type.
   * @return Stable string representation.
   */
  [[nodiscard]] constexpr std::string_view
  to_string(OperationType type) noexcept
  {
    switch (type)
    {
    case OperationType::Unknown:
      return "unknown";

    case OperationType::Put:
      return "put";

    case OperationType::Delete:
      return "delete";

    default:
      return "invalid";
    }
  }

  /**
   * @brief Returns true if the operation type is known and usable.
   *
   * Unknown is intentionally treated as invalid.
   *
   * @param type Operation type.
   * @return true for Put and Delete.
   */
  [[nodiscard]] constexpr bool is_valid(OperationType type) noexcept
  {
    return type == OperationType::Put ||
           type == OperationType::Delete;
  }

  /**
   * @brief Returns true if the operation writes or replaces a value.
   *
   * @param type Operation type.
   * @return true for Put.
   */
  [[nodiscard]] constexpr bool writes_value(OperationType type) noexcept
  {
    return type == OperationType::Put;
  }

} // namespace softadastra::store::types

#endif // SOFTADASTRA_STORE_OPERATION_TYPE_HPP
