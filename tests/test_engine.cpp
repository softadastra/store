/*
 * test_engine.cpp
 */

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <vector>

#include <softadastra/store/core/StoreConfig.hpp>
#include <softadastra/store/engine/StoreEngine.hpp>
#include <softadastra/store/types/Key.hpp>
#include <softadastra/store/types/Value.hpp>

using namespace softadastra::store;

int main()
{
  std::remove("test_store.log");

  core::StoreConfig config;
  config.wal_path = "test_store.log";
  config.enable_wal = true;
  config.auto_flush = true;

  engine::StoreEngine store(config);

  types::Key key = types::Key::from("user");

  types::Value value = types::Value::from_bytes(
      std::vector<std::uint8_t>{10, 20});

  // PUT
  auto r1 = store.put(key, value);
  assert(r1.is_ok());
  assert(r1.value().success);
  assert(r1.value().created);

  // GET
  auto entry = store.get(key);
  assert(entry.has_value());
  assert(entry->key.value() == "user");
  assert(entry->value.size() == 2);
  assert(entry->value.data()[0] == 10);
  assert(entry->value.data()[1] == 20);

  // DELETE
  auto r2 = store.remove(key);
  assert(r2.is_ok());
  assert(r2.value().success);
  assert(r2.value().deleted);

  auto entry2 = store.get(key);
  assert(!entry2.has_value());

  std::remove("test_store.log");

  std::cout << "test_engine passed\n";

  return 0;
}
