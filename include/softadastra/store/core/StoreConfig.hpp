/**
 *
 *  @file StoreConfig.hpp
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

#ifndef SOFTADASTRA_STORE_CONFIG_HPP
#define SOFTADASTRA_STORE_CONFIG_HPP

#include <cstddef>
#include <string>
#include <utility>

namespace softadastra::store::core
{
  /**
   * @brief Configuration for the Softadastra Store.
   *
   * StoreConfig keeps the store setup small and predictable.
   *
   * It controls:
   * - WAL persistence
   * - WAL file path
   * - initial index capacity
   * - automatic WAL flush
   *
   * The store is designed to work as an in-memory materialized state backed
   * by WAL durability.
   */
  struct StoreConfig
  {
    /**
     * @brief Path to the WAL file used by the store.
     */
    std::string wal_path{"data/store.wal"};

    /**
     * @brief Enable WAL persistence for store operations.
     *
     * When enabled, operations should be persisted before being applied.
     */
    bool enable_wal{true};

    /**
     * @brief Initial capacity hint for the in-memory index.
     *
     * This is only a performance hint.
     */
    std::size_t initial_capacity{1024};

    /**
     * @brief Flush WAL after each accepted operation.
     *
     * This is the safest production default.
     */
    bool auto_flush{true};

    /**
     * @brief Creates a default store configuration.
     */
    StoreConfig() = default;

    /**
     * @brief Creates a store configuration with a custom WAL path.
     *
     * @param path WAL file path.
     */
    explicit StoreConfig(std::string path)
        : wal_path(std::move(path))
    {
    }

    /**
     * @brief Returns a production-oriented durable configuration.
     *
     * @param path WAL file path.
     * @return Store configuration with WAL and auto flush enabled.
     */
    [[nodiscard]] static StoreConfig durable(std::string path)
    {
      StoreConfig config(std::move(path));
      config.enable_wal = true;
      config.auto_flush = true;
      config.initial_capacity = 1024;
      return config;
    }

    /**
     * @brief Returns a fast configuration for tests or benchmarks.
     *
     * @param path WAL file path.
     * @return Store configuration with auto flush disabled.
     */
    [[nodiscard]] static StoreConfig fast(std::string path)
    {
      StoreConfig config(std::move(path));
      config.enable_wal = true;
      config.auto_flush = false;
      config.initial_capacity = 1024;
      return config;
    }

    /**
     * @brief Returns an in-memory configuration without WAL persistence.
     *
     * This is useful for tests, temporary stores, or purely ephemeral state.
     *
     * @return Store configuration with WAL disabled.
     */
    [[nodiscard]] static StoreConfig memory_only()
    {
      StoreConfig config;
      config.enable_wal = false;
      config.auto_flush = false;
      return config;
    }

    /**
     * @brief Returns true if the configuration is usable.
     *
     * If WAL is enabled, wal_path must not be empty.
     *
     * @return true when configuration is valid.
     */
    [[nodiscard]] bool is_valid() const noexcept
    {
      if (!enable_wal)
      {
        return initial_capacity > 0;
      }

      return !wal_path.empty() && initial_capacity > 0;
    }
  };

} // namespace softadastra::store::core

#endif // SOFTADASTRA_STORE_CONFIG_HPP
