/**
 *
 *  @file InMemoryIndex.hpp
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

#ifndef SOFTADASTRA_STORE_IN_MEMORY_INDEX_HPP
#define SOFTADASTRA_STORE_IN_MEMORY_INDEX_HPP

#include <cstddef>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>

#include <softadastra/store/index/IndexEntry.hpp>
#include <softadastra/store/types/Key.hpp>

namespace softadastra::store::index
{
  namespace types = softadastra::store::types;

  /**
   * @brief In-memory key-value index for materialized store entries.
   *
   * InMemoryIndex stores the current materialized state of the store.
   *
   * It is used by:
   * - StoreEngine
   * - SnapshotBuilderStore
   * - SnapshotStore
   * - tests and diagnostics
   *
   * The index maps each Key to its latest IndexEntry.
   *
   * Rules:
   * - invalid keys are ignored by put().
   * - get() returns std::nullopt when the key does not exist.
   * - remove() returns true only when an entry was removed.
   * - the index does not persist data by itself.
   */
  class InMemoryIndex
  {
  public:
    /**
     * @brief Internal map type.
     */
    using Map = std::unordered_map<std::string, IndexEntry>;

    /**
     * @brief Creates an empty index.
     */
    InMemoryIndex() = default;

    /**
     * @brief Creates an index with an initial capacity hint.
     *
     * @param capacity Initial bucket reservation.
     */
    explicit InMemoryIndex(std::size_t capacity)
    {
      reserve(capacity);
    }

    /**
     * @brief Inserts or updates an index entry.
     *
     * Invalid keys are ignored.
     *
     * @param key Store key.
     * @param entry Index entry to store.
     */
    void put(const types::Key &key, const IndexEntry &entry)
    {
      if (!key.is_valid())
      {
        return;
      }

      index_[key.str()] = entry;
    }

    /**
     * @brief Inserts or updates an index entry by move.
     *
     * Invalid keys are ignored.
     *
     * @param key Store key.
     * @param entry Index entry to store.
     */
    void put(const types::Key &key, IndexEntry &&entry)
    {
      if (!key.is_valid())
      {
        return;
      }

      index_[key.str()] = std::move(entry);
    }

    /**
     * @brief Inserts or updates using the key inside the entry.
     *
     * Invalid entries are ignored.
     *
     * @param entry Index entry to store.
     */
    void put(IndexEntry entry)
    {
      if (!entry.has_key())
      {
        return;
      }

      index_[entry.entry.key.str()] = std::move(entry);
    }

    /**
     * @brief Returns an entry by key.
     *
     * @param key Store key.
     * @return IndexEntry if found, std::nullopt otherwise.
     */
    [[nodiscard]] std::optional<IndexEntry>
    get(const types::Key &key) const
    {
      const auto it = index_.find(key.str());

      if (it == index_.end())
      {
        return std::nullopt;
      }

      return it->second;
    }

    /**
     * @brief Returns a pointer to an entry by key.
     *
     * This avoids copying the entry.
     *
     * @param key Store key.
     * @return Pointer to entry, or nullptr if missing.
     */
    [[nodiscard]] const IndexEntry *
    find(const types::Key &key) const noexcept
    {
      const auto it = index_.find(key.str());

      if (it == index_.end())
      {
        return nullptr;
      }

      return &it->second;
    }

    /**
     * @brief Returns a mutable pointer to an entry by key.
     *
     * @param key Store key.
     * @return Pointer to entry, or nullptr if missing.
     */
    [[nodiscard]] IndexEntry *
    find(const types::Key &key) noexcept
    {
      const auto it = index_.find(key.str());

      if (it == index_.end())
      {
        return nullptr;
      }

      return &it->second;
    }

    /**
     * @brief Removes an entry by key.
     *
     * @param key Store key.
     * @return true if an entry was removed.
     */
    bool remove(const types::Key &key)
    {
      if (!key.is_valid())
      {
        return false;
      }

      return index_.erase(key.str()) > 0;
    }

    /**
     * @brief Returns true if the key exists in the index.
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

      return index_.find(key.str()) != index_.end();
    }

    /**
     * @brief Reserves storage for at least capacity entries.
     *
     * @param capacity Number of entries to reserve.
     */
    void reserve(std::size_t capacity)
    {
      index_.reserve(capacity);
    }

    /**
     * @brief Removes all entries.
     */
    void clear() noexcept
    {
      index_.clear();
    }

    /**
     * @brief Returns the number of indexed entries.
     *
     * @return Entry count.
     */
    [[nodiscard]] std::size_t size() const noexcept
    {
      return index_.size();
    }

    /**
     * @brief Returns true if the index is empty.
     *
     * @return true when no entry is stored.
     */
    [[nodiscard]] bool empty() const noexcept
    {
      return index_.empty();
    }

    /**
     * @brief Returns all indexed entries.
     *
     * @return Read-only reference to the internal map.
     */
    [[nodiscard]] const Map &all() const noexcept
    {
      return index_;
    }

  private:
    Map index_{};
  };

} // namespace softadastra::store::index

#endif // SOFTADASTRA_STORE_IN_MEMORY_INDEX_HPP
