/*
 * Entry.hpp
 */

#ifndef SOFTADASTRA_STORE_ENTRY_HPP
#define SOFTADASTRA_STORE_ENTRY_HPP

#include <cstdint>

#include <softadastra/store/types/Key.hpp>
#include <softadastra/store/types/Value.hpp>

namespace softadastra::store::core
{
  namespace types = softadastra::store::types;

  /**
   * @brief Materialized entry in the store
   *
   * Represents the current state after applying operations.
   */
  struct Entry
  {
    types::Key key;
    types::Value value;

    std::uint64_t version{0}; // WAL sequence
    std::uint64_t timestamp{0};
  };

} // namespace softadastra::store::core

#endif
