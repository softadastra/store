/*
 * test_engine.cpp
 */

#include <cassert>
#include <iostream>

#include <softadastra/store/engine/StoreEngine.hpp>

using namespace softadastra::store;

int main()
{
  std::remove("test_store.log");

  core::StoreConfig config;
  config.wal_path = "test_store.log";

  engine::StoreEngine store(config);

  types::Key key;
  key.value = "user";

  types::Value value;
  value.data = {10, 20};

  // PUT
  auto r1 = store.put(key, value);
  assert(r1.success);
  assert(r1.created);

  // GET
  auto entry = store.get(key);
  assert(entry.has_value());
  assert(entry->value.data.size() == 2);

  // DELETE
  auto r2 = store.remove(key);
  assert(r2.success);
  assert(r2.deleted);

  auto entry2 = store.get(key);
  assert(!entry2.has_value());

  std::cout << "test_engine passed\n";
}
