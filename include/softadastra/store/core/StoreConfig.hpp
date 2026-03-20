/*
 * StoreConfig.hpp
 */

#ifndef SOFTADASTRA_STORE_CONFIG_HPP
#define SOFTADASTRA_STORE_CONFIG_HPP

#include <string>

namespace softadastra::store::core
{
  /**
   * @brief Store configuration
   */
  struct StoreConfig
  {
    /**
     * WAL file path
     */
    std::string wal_path{"data/wal.log"};

    /**
     * Enable WAL persistence
     */
    bool enable_wal{true};

    /**
     * In-memory index capacity hint
     */
    std::size_t initial_capacity{1024};

    /**
     * Auto flush WAL
     */
    bool auto_flush{true};
  };

} // namespace softadastra::store::core

#endif
