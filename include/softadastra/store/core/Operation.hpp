/**
 *
 *  @file Operation.hpp
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

#ifndef SOFTADASTRA_STORE_OPERATION_HPP
#define SOFTADASTRA_STORE_OPERATION_HPP

#include <utility>

#include <softadastra/core/Core.hpp>
#include <softadastra/store/types/Key.hpp>
#include <softadastra/store/types/OperationType.hpp>
#include <softadastra/store/types/Value.hpp>

namespace softadastra::store::core
{
  namespace types = softadastra::store::types;
  namespace core_time = softadastra::core::time;

  /**
   * @brief Logical mutation applied to the store.
   *
   * Operation describes an action that can be applied to StoreEngine and
   * encoded into a WAL payload.
   *
   * It contains:
   * - operation type
   * - target key
   * - optional binary value
   * - operation timestamp
   *
   * Rules:
   * - Put requires a valid key.
   * - Put may store an empty value.
   * - Delete requires a valid key.
   * - Delete ignores value.
   * - Unknown is invalid.
   */
  struct Operation
  {
    /**
     * @brief Logical operation type.
     */
    types::OperationType type{types::OperationType::Unknown};

    /**
     * @brief Target store key.
     */
    types::Key key{};

    /**
     * @brief Binary value used by Put operations.
     *
     * Delete operations ignore this field.
     */
    types::Value value{};

    /**
     * @brief Operation timestamp.
     */
    core_time::Timestamp timestamp{};

    /**
     * @brief Creates an empty invalid operation.
     */
    Operation() = default;

    /**
     * @brief Creates an operation with explicit fields.
     *
     * @param operation_type Operation type.
     * @param operation_key Target key.
     * @param operation_value Operation value.
     * @param operation_timestamp Operation timestamp.
     */
    Operation(
        types::OperationType operation_type,
        types::Key operation_key,
        types::Value operation_value,
        core_time::Timestamp operation_timestamp)
        : type(operation_type),
          key(std::move(operation_key)),
          value(std::move(operation_value)),
          timestamp(operation_timestamp)
    {
    }

    /**
     * @brief Creates a Put operation.
     *
     * @param key Target key.
     * @param value Value to store.
     * @return Put operation.
     */
    [[nodiscard]] static Operation put(
        types::Key key,
        types::Value value)
    {
      return Operation(
          types::OperationType::Put,
          std::move(key),
          std::move(value),
          core_time::Timestamp::now());
    }

    /**
     * @brief Creates a Delete operation.
     *
     * @param key Target key.
     * @return Delete operation.
     */
    [[nodiscard]] static Operation remove(types::Key key)
    {
      return Operation(
          types::OperationType::Delete,
          std::move(key),
          types::Value{},
          core_time::Timestamp::now());
    }

    /**
     * @brief Returns true if this operation is a Put.
     *
     * @return true for Put operations.
     */
    [[nodiscard]] bool is_put() const noexcept
    {
      return type == types::OperationType::Put;
    }

    /**
     * @brief Returns true if this operation is a Delete.
     *
     * @return true for Delete operations.
     */
    [[nodiscard]] bool is_delete() const noexcept
    {
      return type == types::OperationType::Delete;
    }

    /**
     * @brief Returns true if this operation has a valid key.
     *
     * @return true when key is valid.
     */
    [[nodiscard]] bool has_key() const noexcept
    {
      return key.is_valid();
    }

    /**
     * @brief Returns true if this operation is structurally valid.
     *
     * Empty values are allowed for Put.
     * Delete ignores the value field.
     *
     * @return true when type, key, and timestamp are valid.
     */
    [[nodiscard]] bool is_valid() const noexcept
    {
      return types::is_valid(type) &&
             key.is_valid() &&
             timestamp.is_valid();
    }

    /**
     * @brief Clears the operation and resets it to the default state.
     */
    void clear() noexcept
    {
      type = types::OperationType::Unknown;
      key.clear();
      value.clear();
      timestamp = core_time::Timestamp{};
    }
  };

} // namespace softadastra::store::core

#endif // SOFTADASTRA_STORE_OPERATION_HPP
