/**
 *
 *  @file SnapshotBuilderStore.hpp
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

#ifndef SOFTADASTRA_STORE_SNAPSHOT_BUILDER_STORE_HPP
#define SOFTADASTRA_STORE_SNAPSHOT_BUILDER_STORE_HPP

#include <string>
#include <utility>

#include <softadastra/core/Core.hpp>
#include <softadastra/store/core/StoreConfig.hpp>
#include <softadastra/store/engine/StoreEngine.hpp>
#include <softadastra/store/snapshot/SnapshotStore.hpp>
#include <softadastra/store/types/Key.hpp>
#include <softadastra/wal/core/WalRecord.hpp>
#include <softadastra/wal/reader/WalReader.hpp>

namespace softadastra::store::snapshot
{
  namespace engine = softadastra::store::engine;
  namespace types = softadastra::store::types;
  namespace store_core = softadastra::store::core;

  namespace wal_reader = softadastra::wal::reader;
  namespace wal_core = softadastra::wal::core;

  namespace core_types = softadastra::core::types;
  namespace core_errors = softadastra::core::errors;

  /**
   * @brief Builds a point-in-time snapshot from a WAL file.
   *
   * SnapshotBuilderStore replays store operations from a WAL file into a
   * temporary StoreEngine, then exports the materialized key-value state as a
   * SnapshotStore.
   *
   * It is useful for:
   * - compaction
   * - diagnostics
   * - state export
   * - rebuilding compacted views
   * - tests
   *
   * The builder does not modify the WAL file.
   */
  class SnapshotBuilderStore
  {
  public:
    /**
     * @brief Result returned by build().
     */
    using Result =
        core_types::Result<SnapshotStore, core_errors::Error>;

    /**
     * @brief Builds a snapshot from a WAL file.
     *
     * The WAL is read sequentially. Valid records are applied to a temporary
     * memory-only StoreEngine. The resulting materialized state is copied into
     * a SnapshotStore.
     *
     * @param wal_path WAL file path.
     * @return SnapshotStore on success, Error on failure.
     */
    [[nodiscard]] static Result build(const std::string &wal_path)
    {
      if (wal_path.empty())
      {
        return Result::err(
            core_errors::Error::make(
                core_errors::ErrorCode::InvalidArgument,
                "WAL path cannot be empty"));
      }

      store_core::StoreConfig config =
          store_core::StoreConfig::memory_only();

      engine::StoreEngine store{std::move(config)};

      wal_reader::WalReader reader{wal_path};

      auto read_result = reader.for_each(
          [&](const wal_core::WalRecord &record)
          {
            auto applied = store.apply_from_wal(record);

            /**
             * Invalid operation payloads are ignored here because WalReader
             * already guarantees binary WAL record integrity. A malformed
             * store payload should not prevent building a snapshot from valid
             * records that follow the same WAL stream.
             */
            (void)applied;
          });

      if (read_result.is_err())
      {
        return Result::err(read_result.error());
      }

      SnapshotStore snapshot;
      snapshot.reserve(store.size());

      for (const auto &[key, entry] : store.entries())
      {
        snapshot.put(
            types::Key{key},
            entry.value);
      }

      return Result::ok(std::move(snapshot));
    }
  };

} // namespace softadastra::store::snapshot

#endif // SOFTADASTRA_STORE_SNAPSHOT_BUILDER_STORE_HPP
