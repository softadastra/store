/**
 *
 *  @file SnapshotStore.hpp
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

#ifndef SOFTADASTRA_STORE_SNAPSHOT_STORE_HPP
#define SOFTADASTRA_STORE_SNAPSHOT_STORE_HPP

#include <cstddef>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>

#include <softadastra/store/types/Key.hpp>
#include <softadastra/store/types/Value.hpp>

namespace softadastra::store::snapshot
{
  namespace types = softadastra::store::types;

  /**
   * @brief Immutable-style key-value snapshot container.
   *
   * SnapshotStore stores a point-in-time view of key-value data.
   *
   * It is used by:
   * - SnapshotBuilderStore
   * - tests
   * - diagnostics
   * - compacted state export
   *
   * Unlike StoreEngine, SnapshotStore does not apply operations and does not
   * write to WAL. It only stores materialized key-value pairs.
   *
   * Rules:
   * - invalid keys are ignored by put().
   * - get() returns nullptr when the key does not exist.
   * - values are binary-safe.
   * - the snapshot is in-memory only.
   */
  class SnapshotStore
  {
  public:
    /**
     * @brief Internal snapshot map type.
     */
    using Map = std::unordered_map<std::string, types::Value>;

    /**
     * @brief Creates an empty snapshot store.
     */
    SnapshotStore() = default;

    /**
     * @brief Creates a snapshot store from an existing map.
     *
     * @param data Snapshot data.
     */
    explicit SnapshotStore(Map data)
        : data_(std::move(data))
    {
    }

    /**
     * @brief Inserts or replaces a value by move.
     *
     * Invalid keys are ignored.
     *
     * @param key Store key.
     * @param value Value to store.
     */
    void put(types::Key key, types::Value value)
    {
      if (!key.is_valid())
      {
        return;
      }

      data_[key.str()] = std::move(value);
    }

    /**
     * @brief Removes a value by key.
     *
     * @param key Store key.
     * @return true if a value was removed.
     */
    bool remove(const types::Key &key)
    {
      if (!key.is_valid())
      {
        return false;
      }

      return data_.erase(key.str()) > 0;
    }

    /**
     * @brief Returns a pointer to a value.
     *
     * @param key Store key.
     * @return Pointer to value, or nullptr if missing.
     */
    [[nodiscard]] const types::Value *
    get(const types::Key &key) const noexcept
    {
      const auto it = data_.find(key.str());

      if (it == data_.end())
      {
        return nullptr;
      }

      return &it->second;
    }

    /**
     * @brief Returns an optional copy of a value.
     *
     * @param key Store key.
     * @return Value if found, std::nullopt otherwise.
     */
    [[nodiscard]] std::optional<types::Value>
    value(const types::Key &key) const
    {
      const auto *found = get(key);

      if (!found)
      {
        return std::nullopt;
      }

      return *found;
    }

    /**
     * @brief Returns true if the key exists.
     *
     * @param key Store key.
     * @return true if found.
     */
    [[nodiscard]] bool contains(const types::Key &key) const
    {
      if (!key.is_valid())
      {
        return false;
      }

      return data_.find(key.str()) != data_.end();
    }

    /**
     * @brief Returns all snapshot entries.
     *
     * @return Read-only snapshot map.
     */
    [[nodiscard]] const Map &all() const noexcept
    {
      return data_;
    }

    /**
     * @brief Returns the number of entries.
     *
     * @return Entry count.
     */
    [[nodiscard]] std::size_t size() const noexcept
    {
      return data_.size();
    }

    /**
     * @brief Returns true if the snapshot is empty.
     *
     * @return true when no entry is stored.
     */
    [[nodiscard]] bool empty() const noexcept
    {
      return data_.empty();
    }

    /**
     * @brief Reserves storage for at least capacity entries.
     *
     * @param capacity Number of entries to reserve.
     */
    void reserve(std::size_t capacity)
    {
      data_.reserve(capacity);
    }

    /**
     * @brief Removes all entries.
     */
    void clear() noexcept
    {
      data_.clear();
    }

  private:
    Map data_{};
  };

} // namespace softadastra::store::snapshot

#endif // SOFTADASTRA_STORE_SNAPSHOT_STORE_HPP
