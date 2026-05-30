/*
 * test_recovery.cpp
 */

#include <cassert>
#include <cstdio>
#include <iostream>
#include <string>
#include <fstream>

#include <softadastra/store/core/StoreConfig.hpp>
#include <softadastra/store/engine/StoreEngine.hpp>
#include <softadastra/store/snapshot/SnapshotBuilderStore.hpp>
#include <softadastra/store/types/Key.hpp>
#include <softadastra/store/types/Value.hpp>

using namespace softadastra;

static store::types::Key key(const std::string &value)
{
  return store::types::Key::from(value);
}

static store::types::Value make_value(const std::string &value)
{
  return store::types::Value::from_string(value);
}

void test_recovery_basic()
{
  std::cout << "[test] recovery_basic\n";

  const std::string wal_path = "test_wal.log";
  std::remove(wal_path.c_str());

  // ========================
  // Phase 1: write data
  // ========================
  store::core::StoreConfig config;
  config.enable_wal = true;
  config.wal_path = wal_path;
  config.auto_flush = true;

  {
    store::engine::StoreEngine engine(config);

    auto r1 = engine.put(key("user:1"), make_value("Alice"));
    auto r2 = engine.put(key("user:2"), make_value("Bob"));
    auto r3 = engine.put(key("user:3"), make_value("Charlie"));
    auto r4 = engine.remove(key("user:2"));

    assert(r1.is_ok());
    assert(r2.is_ok());
    assert(r3.is_ok());
    assert(r4.is_ok());

    assert(r1.value().success);
    assert(r2.value().success);
    assert(r3.value().success);
    assert(r4.value().success);
  }

  // ========================
  // Phase 2: recovery
  // ========================
  auto snapshot_result = store::snapshot::SnapshotBuilderStore::build(wal_path);
  assert(snapshot_result.is_ok());

  const auto snapshot = snapshot_result.value();

  // ========================
  // Assertions
  // ========================
  {
    const auto *v1 = snapshot.get(key("user:1"));
    assert(v1 != nullptr);
    assert(v1->to_string() == "Alice");

    const auto *v2 = snapshot.get(key("user:2"));
    assert(v2 == nullptr); // deleted

    const auto *v3 = snapshot.get(key("user:3"));
    assert(v3 != nullptr);
    assert(v3->to_string() == "Charlie");

    assert(snapshot.size() == 2);
  }

  std::remove(wal_path.c_str());

  std::cout << "[ok] recovery_basic\n";
}

void test_recovery_overwrite()
{
  std::cout << "[test] recovery_overwrite\n";

  const std::string wal_path = "test_wal_overwrite.log";
  std::remove(wal_path.c_str());

  store::core::StoreConfig config;
  config.enable_wal = true;
  config.wal_path = wal_path;
  config.auto_flush = true;

  {
    store::engine::StoreEngine engine(config);

    auto r1 = engine.put(key("key"), make_value("v1"));
    auto r2 = engine.put(key("key"), make_value("v2"));
    auto r3 = engine.put(key("key"), make_value("v3"));

    assert(r1.is_ok());
    assert(r2.is_ok());
    assert(r3.is_ok());

    assert(r1.value().success);
    assert(r2.value().success);
    assert(r3.value().success);
  }

  auto snapshot_result = store::snapshot::SnapshotBuilderStore::build(wal_path);
  assert(snapshot_result.is_ok());

  const auto snapshot = snapshot_result.value();

  const auto *v = snapshot.get(key("key"));
  assert(v != nullptr);
  assert(v->to_string() == "v3");

  std::remove(wal_path.c_str());

  std::cout << "[ok] recovery_overwrite\n";
}

void test_recovery_empty()
{
  std::cout << "[test] recovery_empty\n";

  const std::string wal_path = "test_empty.log";
  std::remove(wal_path.c_str());

  {
    std::ofstream file(wal_path, std::ios::binary);
    assert(file.good());
  }

  auto snapshot_result = store::snapshot::SnapshotBuilderStore::build(wal_path);
  assert(snapshot_result.is_ok());

  const auto snapshot = snapshot_result.value();
  assert(snapshot.empty());

  std::remove(wal_path.c_str());

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
