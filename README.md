# softadastra/store

> WAL-backed key-value store for local-first systems.

`softadastra/store` is the materialized state layer of Softadastra.

It provides a simple key-value store backed by a durable Write-Ahead Log.

The core rule is:

> Persist first. Apply after.

## Purpose

The store module turns durable operations into current application state.

It is designed for local-first systems that must survive:

- process crashes
- restarts
- network failures
- offline periods
- interrupted synchronization
- replay-based recovery

The store keeps an in-memory index for fast reads and uses the WAL for durability.

## What this module does

`softadastra/store` provides:

- binary-safe key-value entries
- logical store operations
- operation encoding and decoding
- in-memory materialized state
- WAL-backed persistence
- deterministic recovery from WAL
- snapshot building from WAL

## What this module does not do

This module does not implement:

- networking
- peer-to-peer sync
- conflict resolution
- distributed consensus
- filesystem watching
- long-term database indexing

It stores and replays local state changes.

Higher-level modules handle synchronization and conflict policies.

## Design Principles

### Local-first

The store can apply operations locally and recover them later from WAL.

### Durable

When WAL is enabled, mutations are persisted before being applied to memory.

### Deterministic

Replaying the same WAL produces the same materialized state.

### Binary-safe

Values are raw bytes.

The store does not interpret them.

### Simple API

The public API is intentionally small:

```cpp
store.put(key, value);
store.get(key);
store.remove(key);
store.recover();
```

## Module Structure

```
include/softadastra/store/
├── core/
│   ├── Entry.hpp
│   ├── Operation.hpp
│   └── StoreConfig.hpp
├── encoding/
│   ├── OperationDecoder.hpp
│   └── OperationEncoder.hpp
├── engine/
│   ├── ApplyResult.hpp
│   └── StoreEngine.hpp
├── index/
│   ├── IndexEntry.hpp
│   └── InMemoryIndex.hpp
├── snapshot/
│   ├── SnapshotBuilderStore.hpp
│   └── SnapshotStore.hpp
├── types/
│   ├── Key.hpp
│   ├── OperationType.hpp
│   └── Value.hpp
└── utils/
    └── Serializer.hpp
```

## Installation

```
vix add @softadastra/store
```

## Core Types

### Key

Key identifies an entry in the store.

```cpp
store::types::Key key{"user:1"};

if (key.is_valid())
{
  auto raw = key.str();
}
```

A valid key must not be empty.

### Value

Value stores binary-safe bytes.

```cpp
auto value =
    store::types::Value::from_string("Gaspard");
```

Or from raw bytes:

```cpp
auto value =
    store::types::Value::from_bytes({1, 2, 3});
```

### Operation

Operation describes a logical mutation.

```cpp
auto op = store::core::Operation::put(
    store::types::Key{"user:1"},
    store::types::Value::from_string("Gaspard"));
```

Delete operation:

```cpp
auto op = store::core::Operation::remove(
    store::types::Key{"user:1"});
```

### Entry

Entry is the materialized state of a key.

It contains:

- key
- value
- version
- timestamp

The version usually maps to the WAL sequence that produced the entry.

## Basic Usage

```cpp
#include <softadastra/store/engine/StoreEngine.hpp>

using namespace softadastra;

int main()
{
  store::engine::StoreEngine engine{
      store::core::StoreConfig::durable("data/store.wal")};

  auto result = engine.put(
      store::types::Key{"user:1"},
      store::types::Value::from_string("Gaspard"));

  if (result.is_err())
  {
    return 1;
  }

  auto entry = engine.get(store::types::Key{"user:1"});

  if (!entry.has_value())
  {
    return 1;
  }

  return 0;
}
```

## Store Configuration

### Durable mode

Use this for production.

```cpp
auto config =
    store::core::StoreConfig::durable("data/store.wal");
```

This enables:

- WAL persistence
- automatic flush
- durable operation ordering

### Fast mode

Use this for tests or benchmarks.

```cpp
auto config =
    store::core::StoreConfig::fast("data/store.wal");
```

This keeps WAL enabled but disables automatic flush.

### Memory-only mode

Use this for temporary state or tests.

```cpp
auto config =
    store::core::StoreConfig::memory_only();
```

This disables WAL persistence.

## Put

```cpp
auto result = engine.put(
    store::types::Key{"user:1"},
    store::types::Value::from_string("Gaspard"));

if (result.is_ok())
{
  auto apply = result.value();

  if (apply.created)
  {
    // new entry
  }

  if (apply.updated)
  {
    // existing entry replaced
  }
}
```

## Get

```cpp
auto entry = engine.get(
    store::types::Key{"user:1"});

if (entry.has_value())
{
  auto value = entry->value.to_string();
}
```

## Remove

```cpp
auto result = engine.remove(
    store::types::Key{"user:1"});

if (result.is_ok())
{
  auto apply = result.value();

  if (apply.deleted)
  {
    // entry removed
  }

  if (apply.is_noop())
  {
    // key did not exist
  }
}
```

Delete is idempotent. Removing a missing key returns a successful no-op.

## Apply External Operation

Use apply_operation() when the caller already has an operation and wants to preserve its timestamp.

```cpp
auto op = store::core::Operation::put(
    store::types::Key{"user:1"},
    store::types::Value::from_string("Gaspard"));

auto result = engine.apply_operation(op);
```

This is useful for:

- remote sync operations
- replay-like flows
- imported operation streams
- deterministic tests

## Recovery

When WAL is enabled, StoreEngine attempts recovery during construction.

You can also call recovery explicitly:

```cpp
auto result = engine.recover();

if (result.is_err())
{
  return 1;
}
```

Recovery flow:

1. Read WAL records
2. Decode store operations
3. Apply operations in WAL order
4. Rebuild in-memory state
5. Restore writer sequence

## Reading Entries

```cpp
for (const auto &[key, entry] : engine.entries())
{
  // key is std::string
  // entry is store::core::Entry
}
```

Helpers:

- engine.size();
- engine.empty();
- engine.contains(store::types::Key{"user:1"});

## Operation Encoding

Store operations can be encoded into stable binary payloads.

```cpp
auto op = store::core::Operation::put(
    store::types::Key{"user:1"},
    store::types::Value::from_string("Gaspard"));

auto payload =
    store::encoding::OperationEncoder::encode(op);
```

Decode:

```cpp
auto decoded =
    store::encoding::OperationDecoder::decode(payload);

if (!decoded)
{
  return 1;
}
```

## Operation Binary Format

Current operation payload format: version 1.

```
uint8  version
uint8  operation_type
uint32 key_size
bytes  key
uint32 value_size
bytes  value
int64  timestamp_millis
```

All integer values are encoded in little-endian order.

## WAL Integration

When WAL is enabled, store operations are encoded and appended to the WAL before being applied.

```
Operation
  -> OperationEncoder
  -> WAL payload
  -> WalWriter
  -> StoreEngine apply
```

The WAL sequence becomes the store entry version.

## Snapshot Store

SnapshotStore stores a point-in-time key-value view.

```cpp
store::snapshot::SnapshotStore snapshot;

snapshot.put(
    store::types::Key{"user:1"},
    store::types::Value::from_string("Gaspard"));
```

Read:

```cpp
auto value = snapshot.get(
    store::types::Key{"user:1"});

if (value)
{
  auto text = value->to_string();
}
```

## Build Snapshot from WAL

```cpp
auto result =
    store::snapshot::SnapshotBuilderStore::build("data/store.wal");

if (result.is_err())
{
  return 1;
}

auto snapshot = std::move(result.value());
```

This replays WAL records into a temporary memory-only store and exports the final state as a SnapshotStore.

## Error Handling

The store API uses softadastra::core::types::Result.

Example:

```cpp
auto result = engine.put(
    store::types::Key{"user:1"},
    store::types::Value::from_string("Gaspard"));

if (result.is_err())
{
  const auto &error = result.error();
}
```

Public operations return errors instead of throwing for normal failures.

## ApplyResult

ApplyResult describes what happened after applying an operation.

```cpp
if (apply.created)
{
  // created
}

if (apply.updated)
{
  // updated
}

if (apply.deleted)
{
  // deleted
}

if (apply.is_noop())
{
  // no state mutation
}
```

The result also includes:

- apply.version;
- apply.success;

## Serializer Utilities

Serializer provides small deterministic helpers.

```cpp
std::vector<std::uint8_t> out;

store::utils::Serializer::append_u32(out, 42);
store::utils::Serializer::append_i64(out, 123456);
```

Read:

```cpp
std::size_t offset = 0;
std::uint32_t value = 0;

auto ok = store::utils::Serializer::read_u32(
    std::span<const std::uint8_t>(out.data(), out.size()),
    offset,
    value);
```

## Recommended Write Flow

1. Build Operation
2. Encode Operation
3. Append payload to WAL
4. Receive WAL sequence
5. Apply operation to memory
6. Use WAL sequence as entry version

This ensures the state can be rebuilt after restart.

## Production Notes

Use durable mode for production:

```cpp
auto config =
    store::core::StoreConfig::durable("data/store.wal");
```

Use memory-only mode only for temporary or test state:

```cpp
auto config =
    store::core::StoreConfig::memory_only();
```

For high-throughput workloads, batching and snapshot compaction can be added later without changing the basic user-facing API.

## Rules

- Persist before apply
- Keep operation payloads deterministic
- Treat WAL sequence as the materialized version
- Do not interpret Value bytes inside the store core
- Use recover() to rebuild state after restart
- Use snapshots for compacted point-in-time state

## Dependencies

### Internal

- softadastra/core
- softadastra/wal

### External

- C++20 standard library

## Roadmap

- Snapshot compaction
- Segment-aware snapshot building
- Persistent index backend
- Batched writes
- Atomic snapshot export
- Store metrics
- Optional typed value helpers

## Summary

softadastra/store is the WAL-backed materialized state layer of Softadastra.

It provides:

- simple key-value operations
- binary-safe values
- WAL-backed durability
- deterministic recovery
- snapshot building
- local-first state management

Its job is simple:

turn durable operations into recoverable state.

