/*
 * test_index.cpp
 */

#include <cassert>
#include <iostream>

#include <softadastra/store/index/InMemoryIndex.hpp>

using namespace softadastra::store;

int main()
{
  index::InMemoryIndex idx;

  types::Key key;
  key.value = "hello";

  index::IndexEntry entry;
  entry.entry.key = key;
  entry.entry.value.data = {42};
  entry.version = 1;

  idx.put(key, entry);

  assert(idx.contains(key));

  auto result = idx.get(key);
  assert(result.has_value());
  assert(result->entry.value.data[0] == 42);

  idx.remove(key);
  assert(!idx.contains(key));

  std::cout << "test_index passed\n";
}
