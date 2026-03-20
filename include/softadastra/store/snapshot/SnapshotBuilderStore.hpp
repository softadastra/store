/*
 * SnapshotBuilderStore.hpp
 */

#ifndef SOFTADASTRA_STORE_SNAPSHOT_BUILDER_STORE_HPP
#define SOFTADASTRA_STORE_SNAPSHOT_BUILDER_STORE_HPP

#include <softadastra/store/snapshot/SnapshotStore.hpp>
#include <softadastra/store/engine/StoreEngine.hpp>
#include <softadastra/store/types/Key.hpp>
#include <softadastra/wal/reader/WalReader.hpp>
#include <softadastra/wal/core/WalRecord.hpp>

namespace softadastra::store::snapshot
{
  namespace engine = softadastra::store::engine;
  namespace types = softadastra::store::types;

  namespace wal_reader = softadastra::wal::reader;
  namespace wal_core = softadastra::wal::core;

  class SnapshotBuilderStore
  {
  public:
    static SnapshotStore build(const std::string &wal_path)
    {
      softadastra::store::core::StoreConfig config;
      config.enable_wal = false;

      engine::StoreEngine store(config);

      wal_reader::WalReader reader(wal_path);

      reader.for_each([&](const wal_core::WalRecord &record)
                      { store.apply_from_wal(record); });

      SnapshotStore snapshot;

      for (const auto &[key, entry] : store.entries())
      {
        snapshot.put(
            types::Key{key},
            entry.value);
      }

      return snapshot;
    }
  };

} // namespace softadastra::store::snapshot

#endif
