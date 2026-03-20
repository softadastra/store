/*
 * IndexEntry.hpp
 */

#ifndef SOFTADASTRA_STORE_INDEX_ENTRY_HPP
#define SOFTADASTRA_STORE_INDEX_ENTRY_HPP

#include <cstdint>

#include <softadastra/store/core/Entry.hpp>

namespace softadastra::store::index
{
  namespace core = softadastra::store::core;

  /**
   * @brief Index entry
   *
   * Wraps a store entry with index-specific metadata.
   */
  struct IndexEntry
  {
    core::Entry entry;

    /**
     * Last WAL sequence applied
     */
    std::uint64_t version{0};

    /**
     * Optional: future (offset in WAL / segment id)
     */
    std::uint64_t position{0};
  };

} // namespace softadastra::store::index

#endif
