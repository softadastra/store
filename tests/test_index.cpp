/*
 * test_index.cpp
 */

#include <cassert>
#include <cstdint>
#include <iostream>
#include <vector>

#include <softadastra/store/index/InMemoryIndex.hpp>
#include <softadastra/store/index/IndexEntry.hpp>
#include <softadastra/store/core/Entry.hpp>
#include <softadastra/store/types/Key.hpp>
#include <softadastra/store/types/Value.hpp>

using namespace softadastra::store;

int main()
{
  index::InMemoryIndex idx;

  types::Key key = types::Key::from("hello");

  core::Entry store_entry = core::Entry::make(
      key,
      types::Value::from_bytes(std::vector<std::uint8_t>{42}),
      1);

  index::IndexEntry entry(store_entry, 1);

  assert(entry.is_valid());
  assert(entry.has_key());
  assert(entry.has_version());

  idx.put(key, entry);

  assert(idx.contains(key));
  assert(idx.size() == 1);

  auto result = idx.get(key);
  assert(result.has_value());
  assert(result->is_valid());
  assert(result->entry.key.value() == "hello");
  assert(result->entry.value.data().size() == 1);
  assert(result->entry.value.data()[0] == 42);
  assert(result->version == 1);

  const auto *found = idx.find(key);
  assert(found != nullptr);
  assert(found->entry.value.bytes()[0] == 42);

  assert(idx.remove(key));
  assert(!idx.contains(key));
  assert(idx.empty());

  std::cout << "test_index passed\n";

  return 0;
}
