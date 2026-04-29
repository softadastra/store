/**
 *
 *  @file Entry.hpp
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

#ifndef SOFTADASTRA_STORE_ENTRY_HPP
#define SOFTADASTRA_STORE_ENTRY_HPP

#include <cstdint>
#include <utility>

#include <softadastra/core/Core.hpp>
#include <softadastra/store/types/Key.hpp>
#include <softadastra/store/types/Value.hpp>

namespace softadastra::store::core
{
  namespace types = softadastra::store::types;
  namespace core_time = softadastra::core::time;

  /**
   * @brief Materialized key-value entry stored by the Store.
   *
   * Entry represents the current state of a key after applying store
   * operations.
   *
   * It contains:
   * - the entry key
   * - the current binary value
   * - the version that produced this state
   * - the timestamp of the last update
   *
   * The version is usually the WAL sequence number or another monotonic
   * operation sequence provided by the caller.
   *
   * Rules:
   * - key must be valid.
   * - value may be empty.
   * - version should be greater than zero after persistence.
   * - timestamp should be valid after persistence.
   */
  struct Entry
  {
    /**
     * @brief Store key.
     */
    types::Key key{};

    /**
     * @brief Binary value associated with the key.
     */
    types::Value value{};

    /**
     * @brief Monotonic version of this entry.
     *
     * Usually mapped to the WAL sequence that produced the current state.
     */
    std::uint64_t version{0};

    /**
     * @brief Timestamp of the last update.
     */
    core_time::Timestamp timestamp{};

    /**
     * @brief Creates an empty entry.
     */
    Entry() = default;

    /**
     * @brief Creates an entry with a key and value.
     *
     * Version is set to 0 and timestamp is set to now.
     *
     * @param entry_key Store key.
     * @param entry_value Store value.
     */
    Entry(types::Key entry_key, types::Value entry_value)
        : key(std::move(entry_key)),
          value(std::move(entry_value)),
          timestamp(core_time::Timestamp::now())
    {
    }

    /**
     * @brief Creates an entry with all fields.
     *
     * @param entry_key Store key.
     * @param entry_value Store value.
     * @param entry_version Entry version.
     * @param entry_timestamp Entry timestamp.
     */
    Entry(
        types::Key entry_key,
        types::Value entry_value,
        std::uint64_t entry_version,
        core_time::Timestamp entry_timestamp)
        : key(std::move(entry_key)),
          value(std::move(entry_value)),
          version(entry_version),
          timestamp(entry_timestamp)
    {
    }

    /**
     * @brief Creates an entry using the current timestamp.
     *
     * @param entry_key Store key.
     * @param entry_value Store value.
     * @param entry_version Entry version.
     * @return Entry instance.
     */
    [[nodiscard]] static Entry make(
        types::Key entry_key,
        types::Value entry_value,
        std::uint64_t entry_version)
    {
      return Entry(
          std::move(entry_key),
          std::move(entry_value),
          entry_version,
          core_time::Timestamp::now());
    }

    /**
     * @brief Returns true if this entry has a valid key.
     *
     * @return true when key is valid.
     */
    [[nodiscard]] bool has_key() const noexcept
    {
      return key.is_valid();
    }

    /**
     * @brief Returns true if this entry has a non-zero version.
     *
     * @return true when version is greater than zero.
     */
    [[nodiscard]] bool has_version() const noexcept
    {
      return version > 0;
    }

    /**
     * @brief Returns true if this entry is structurally valid.
     *
     * Empty values are allowed.
     *
     * @return true when key, version, and timestamp are valid.
     */
    [[nodiscard]] bool is_valid() const noexcept
    {
      return key.is_valid() &&
             version > 0 &&
             timestamp.is_valid();
    }

    /**
     * @brief Clears the entry and resets it to the default state.
     */
    void clear() noexcept
    {
      key.clear();
      value.clear();
      version = 0;
      timestamp = core_time::Timestamp{};
    }
  };

} // namespace softadastra::store::core

#endif // SOFTADASTRA_STORE_ENTRY_HPP
