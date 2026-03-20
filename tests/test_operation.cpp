/*
 * test_operation.cpp
 */

#include <cassert>
#include <iostream>

#include <softadastra/store/core/Operation.hpp>
#include <softadastra/store/encoding/OperationEncoder.hpp>
#include <softadastra/store/encoding/OperationDecoder.hpp>

using namespace softadastra::store;

int main()
{
  core::Operation op;
  op.type = types::OperationType::Put;
  op.key.value = "key1";
  op.value.data = {1, 2, 3};
  op.timestamp = 123;

  auto bytes = encoding::OperationEncoder::encode(op);

  auto decoded = encoding::OperationDecoder::decode(bytes.data(), bytes.size());

  assert(decoded.has_value());
  assert(decoded->key.value == "key1");
  assert(decoded->value.data.size() == 3);

  std::cout << "test_operation passed\n";
}
