/**
 *
 *  @file ApplyResult.hpp
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

#ifndef SOFTADASTRA_STORE_APPLY_RESULT_HPP
#define SOFTADASTRA_STORE_APPLY_RESULT_HPP

#include <cstdint>

namespace softadastra::store::engine
{
  /**
   * @brief Result metadata produced after applying a store operation.
   *
   * ApplyResult describes what happened when an operation was applied to the
   * materialized store state.
   *
   * It is used by:
   * - StoreEngine
   * - replay logic
   * - tests
   * - diagnostics
   *
   * The result does not carry errors.
   * Operation failures should be represented with Result<ApplyResult, Error>
   * at the StoreEngine API level.
   */
  struct ApplyResult
  {
    /**
     * @brief True when the operation was applied successfully.
     */
    bool success{false};

    /**
     * @brief Version assigned to the applied operation.
     *
     * Usually mapped to the WAL sequence number.
     */
    std::uint64_t version{0};

    /**
     * @brief True when a new entry was created.
     */
    bool created{false};

    /**
     * @brief True when an existing entry was updated.
     */
    bool updated{false};

    /**
     * @brief True when an entry was deleted.
     */
    bool deleted{false};

    /**
     * @brief Creates a successful creation result.
     *
     * @param applied_version Applied version.
     * @return ApplyResult.
     */
    [[nodiscard]] static constexpr ApplyResult created_entry(
        std::uint64_t applied_version) noexcept
    {
      return ApplyResult{
          true,
          applied_version,
          true,
          false,
          false};
    }

    /**
     * @brief Creates a successful update result.
     *
     * @param applied_version Applied version.
     * @return ApplyResult.
     */
    [[nodiscard]] static constexpr ApplyResult updated_entry(
        std::uint64_t applied_version) noexcept
    {
      return ApplyResult{
          true,
          applied_version,
          false,
          true,
          false};
    }

    /**
     * @brief Creates a successful deletion result.
     *
     * @param applied_version Applied version.
     * @return ApplyResult.
     */
    [[nodiscard]] static constexpr ApplyResult deleted_entry(
        std::uint64_t applied_version) noexcept
    {
      return ApplyResult{
          true,
          applied_version,
          false,
          false,
          true};
    }

    /**
     * @brief Creates a no-op successful result.
     *
     * Useful when deleting a missing key is treated as idempotent.
     *
     * @param applied_version Applied version.
     * @return ApplyResult.
     */
    [[nodiscard]] static constexpr ApplyResult noop(
        std::uint64_t applied_version = 0) noexcept
    {
      return ApplyResult{
          true,
          applied_version,
          false,
          false,
          false};
    }

    /**
     * @brief Creates a failed apply result.
     *
     * @return ApplyResult.
     */
    [[nodiscard]] static constexpr ApplyResult failed() noexcept
    {
      return ApplyResult{};
    }

    /**
     * @brief Returns true if no state mutation happened.
     *
     * @return true when created, updated, and deleted are all false.
     */
    [[nodiscard]] constexpr bool is_noop() const noexcept
    {
      return success && !created && !updated && !deleted;
    }

    /**
     * @brief Returns true if the result has an applied version.
     *
     * @return true when version is greater than zero.
     */
    [[nodiscard]] constexpr bool has_version() const noexcept
    {
      return version > 0;
    }

    /**
     * @brief Returns true if exactly one mutation flag is set.
     *
     * @return true when the mutation flags are consistent.
     */
    [[nodiscard]] constexpr bool has_consistent_flags() const noexcept
    {
      const int count =
          (created ? 1 : 0) +
          (updated ? 1 : 0) +
          (deleted ? 1 : 0);

      return count <= 1;
    }

    /**
     * @brief Returns true if the result is structurally valid.
     *
     * A failed result is valid only when it has no mutation flag and no version.
     *
     * @return true if the result is internally consistent.
     */
    [[nodiscard]] constexpr bool is_valid() const noexcept
    {
      if (!has_consistent_flags())
      {
        return false;
      }

      if (!success)
      {
        return version == 0 && !created && !updated && !deleted;
      }

      return true;
    }
  };

} // namespace softadastra::store::engine

#endif // SOFTADASTRA_STORE_APPLY_RESULT_HPP
