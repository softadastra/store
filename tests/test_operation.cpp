/*
 * test_operation.cpp
 */

#include <cassert>
#include <cstdint>
#include <iostream>
#include <vector>

#include <softadastra/store/core/Operation.hpp>
#include <softadastra/store/encoding/OperationDecoder.hpp>
#include <softadastra/store/encoding/OperationEncoder.hpp>

using namespace softadastra::store;

int main()
{
  core::Operation op = core::Operation::put(
      types::Key::from("key1"),
      types::Value::from_bytes(std::vector<std::uint8_t>{1, 2, 3}));

  assert(op.is_valid());
  assert(op.is_put());
  assert(op.has_key());
  assert(op.key.value() == "key1");
  assert(op.value.size() == 3);
  assert(op.timestamp.is_valid());

  auto bytes = encoding::OperationEncoder::encode(op);

  auto decoded = encoding::OperationDecoder::decode(
      bytes.data(),
      bytes.size());

  assert(decoded.has_value());
  assert(decoded->is_valid());
  assert(decoded->is_put());
  assert(decoded->key.value() == "key1");
  assert(decoded->value.size() == 3);
  assert(decoded->value.data()[0] == 1);
  assert(decoded->value.data()[1] == 2);
  assert(decoded->value.data()[2] == 3);
  assert(decoded->timestamp.is_valid());

  std::cout << "test_operation passed\n";

  return 0;
}
