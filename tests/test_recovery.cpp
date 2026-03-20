/*
 * test_recovery.cpp
 */

#include <cassert>
#include <iostream>

#include <softadastra/store/engine/StoreEngine.hpp>
#include <softadastra/store/snapshot/SnapshotBuilderStore.hpp>
#include <softadastra/store/types/Key.hpp>
#include <softadastra/store/types/Value.hpp>
#include <softadastra/store/core/StoreConfig.hpp>

using namespace softadastra;

static store::types::Value make_value(const std::string &s)
{
  store::types::Value v;
  v.data.assign(s.begin(), s.end());
  return v;
}

void test_recovery_basic()
{
  std::cout << "[test] recovery_basic\n";

  std::string wal_path = "test_wal.log";

  // ========================
  // Phase 1: write data
  // ========================
  store::core::StoreConfig config;
  config.enable_wal = true;
  config.wal_path = wal_path;
  config.auto_flush = true;

  {
    store::engine::StoreEngine engine(config);

    engine.put({"user:1"}, make_value("Alice"));
    engine.put({"user:2"}, make_value("Bob"));
    engine.put({"user:3"}, make_value("Charlie"));

    engine.remove({"user:2"});
  }

  // ========================
  // Phase 2: recovery
  // ========================
  auto snapshot = store::snapshot::SnapshotBuilderStore::build(wal_path);

  // ========================
  // Assertions
  // ========================
  {
    auto v1 = snapshot.get({"user:1"});
    assert(v1 != nullptr);
    assert(std::string(v1->data.begin(), v1->data.end()) == "Alice");

    auto v2 = snapshot.get({"user:2"});
    assert(v2 == nullptr); // deleted

    auto v3 = snapshot.get({"user:3"});
    assert(v3 != nullptr);
    assert(std::string(v3->data.begin(), v3->data.end()) == "Charlie");

    assert(snapshot.size() == 2);
  }

  std::cout << "[ok] recovery_basic\n";
}

void test_recovery_overwrite()
{
  std::cout << "[test] recovery_overwrite\n";

  std::string wal_path = "test_wal_overwrite.log";

  store::core::StoreConfig config;
  config.enable_wal = true;
  config.wal_path = wal_path;
  config.auto_flush = true;

  {
    store::engine::StoreEngine engine(config);

    engine.put({"key"}, make_value("v1"));
    engine.put({"key"}, make_value("v2"));
    engine.put({"key"}, make_value("v3"));
  }

  auto snapshot = store::snapshot::SnapshotBuilderStore::build(wal_path);

  auto v = snapshot.get({"key"});
  assert(v != nullptr);
  assert(std::string(v->data.begin(), v->data.end()) == "v3");

  std::cout << "[ok] recovery_overwrite\n";
}

void test_recovery_empty()
{
  std::cout << "[test] recovery_empty\n";

  std::string wal_path = "test_empty.log";

  auto snapshot = store::snapshot::SnapshotBuilderStore::build(wal_path);

  assert(snapshot.empty());

  std::cout << "[ok] recovery_empty\n";
}

int main()
{
  test_recovery_basic();
  test_recovery_overwrite();
  test_recovery_empty();

  std::cout << "\nAll recovery tests passed.\n";
  return 0;
}
