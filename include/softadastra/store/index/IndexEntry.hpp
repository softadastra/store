/**
 *
 *  @file IndexEntry.hpp
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

#ifndef SOFTADASTRA_STORE_INDEX_ENTRY_HPP
#define SOFTADASTRA_STORE_INDEX_ENTRY_HPP

#include <cstdint>
#include <utility>

#include <softadastra/store/core/Entry.hpp>

namespace softadastra::store::index
{
  namespace core = softadastra::store::core;

  /**
   * @brief Entry stored inside the in-memory index.
   *
   * IndexEntry wraps a materialized store Entry with index-specific metadata.
   *
   * It tracks:
   * - the current materialized entry
   * - the last applied version
   * - an optional WAL/storage position
   *
   * The version usually mirrors Entry::version and represents the last
   * operation sequence that produced the indexed state.
   *
   * The position field is reserved for future storage integration:
   * - WAL offset
   * - segment id
   * - snapshot offset
   * - storage pointer
   */
  struct IndexEntry
  {
    /**
     * @brief Materialized store entry.
     */
    core::Entry entry{};

    /**
     * @brief Last operation version applied to this index entry.
     *
     * Usually mapped to the WAL sequence.
     */
    std::uint64_t version{0};

    /**
     * @brief Optional storage position.
     *
     * Reserved for future use.
     */
    std::uint64_t position{0};

    /**
     * @brief Creates an empty index entry.
     */
    IndexEntry() = default;

    /**
     * @brief Creates an index entry from a materialized entry.
     *
     * The index version is initialized from entry.version.
     *
     * @param store_entry Materialized store entry.
     */
    explicit IndexEntry(core::Entry store_entry)
        : entry(std::move(store_entry)),
          version(entry.version)
    {
    }

    /**
     * @brief Creates an index entry with explicit metadata.
     *
     * @param store_entry Materialized store entry.
     * @param entry_version Last applied version.
     * @param entry_position Optional storage position.
     */
    IndexEntry(
        core::Entry store_entry,
        std::uint64_t entry_version,
        std::uint64_t entry_position = 0)
        : entry(std::move(store_entry)),
          version(entry_version),
          position(entry_position)
    {
    }

    /**
     * @brief Returns true if this index entry has a valid key.
     *
     * @return true when the underlying entry key is valid.
     */
    [[nodiscard]] bool has_key() const noexcept
    {
      return entry.has_key();
    }

    /**
     * @brief Returns true if this index entry has a version.
     *
     * @return true when version is greater than zero.
     */
    [[nodiscard]] bool has_version() const noexcept
    {
      return version > 0;
    }

    /**
     * @brief Returns true if this index entry is structurally valid.
     *
     * @return true when entry is valid and version is greater than zero.
     */
    [[nodiscard]] bool is_valid() const noexcept
    {
      return entry.is_valid() && version > 0;
    }

    /**
     * @brief Updates the entry and refreshes the index version.
     *
     * @param store_entry New materialized store entry.
     */
    void update(core::Entry store_entry)
    {
      entry = std::move(store_entry);
      version = entry.version;
    }

    /**
     * @brief Clears the index entry.
     */
    void clear() noexcept
    {
      entry.clear();
      version = 0;
      position = 0;
    }
  };

} // namespace softadastra::store::index

#endif // SOFTADASTRA_STORE_INDEX_ENTRY_HPP
