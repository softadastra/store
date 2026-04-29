# Store Guide

The Softadastra Store module provides a WAL-backed key-value store for local-first systems.

It turns durable operations into current application state.

The core rule is:

> Persist first. Apply after.

## Why Softadastra needs a Store

The WAL guarantees durability, but it does not provide current state by itself.

A WAL contains an ordered history of operations.

The Store replays and materializes those operations into a fast in-memory key-value state.

This gives Softadastra:

- fast local reads
- durable local writes
- deterministic recovery
- replayable state
- snapshot-friendly storage
- a foundation for sync and offline-first systems

## What the Store guarantees

When WAL persistence is enabled, the Store guarantees:

- operations are written before being applied
- applied entries receive a monotonic version
- the current state can be rebuilt from WAL
- replay order is deterministic
- values are binary-safe
- delete operations are idempotent

The Store does not guarantee distributed consistency by itself.

Conflict resolution and synchronization belong to higher-level modules.

## What the Store does not do

The Store does not implement:

- network synchronization
- peer discovery
- conflict resolution
- distributed consensus
- filesystem watching
- application-specific schemas

It only stores and materializes local operations.

## Installation

```bash
vix add @softadastra/store
```

## Main concepts

The Store is built around five concepts:

- Key
- Value
- Operation
- Entry
- StoreEngine

## Key

A Key identifies an entry.

```cpp
store::types::Key key{"user:1"};
```

A valid key must not be empty.

```cpp
if (key.is_valid())
{
  auto raw = key.str();
}
```

## Value

A Value stores binary-safe bytes.

From string:

```cpp
auto value =
    store::types::Value::from_string("Gaspard");
```

From bytes:

```cpp
auto value =
    store::types::Value::from_bytes({1, 2, 3});
```

The Store does not interpret the bytes.

## Operation

An Operation describes a logical mutation.

Put:

```cpp
auto op = store::core::Operation::put(
    store::types::Key{"user:1"},
    store::types::Value::from_string("Gaspard"));
```

Delete:

```cpp
auto op = store::core::Operation::remove(
    store::types::Key{"user:1"});
```

## Entry

An Entry is the current materialized state for a key.

It contains:

- key
- value
- version
- timestamp

The version usually maps to the WAL sequence that produced the entry.

## StoreEngine

StoreEngine is the main public API.

It applies operations, persists them through WAL when enabled, and keeps current state in memory.

## Basic usage

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

  auto entry = engine.get(
      store::types::Key{"user:1"});

  if (!entry.has_value())
  {
    return 1;
  }

  return 0;
}
```

## Configuration

### Durable mode

Use durable mode for production.

```cpp
auto config =
    store::core::StoreConfig::durable("data/store.wal");
```

Enables:

- WAL persistence
- automatic flush
- recoverable writes

### Fast mode

```cpp
auto config =
    store::core::StoreConfig::fast("data/store.wal");
```

### Memory-only mode

```cpp
auto config =
    store::core::StoreConfig::memory_only();
```

## Put

```cpp
auto result = engine.put(
    store::types::Key{"user:1"},
    store::types::Value::from_string("Gaspard"));
```

## Get

```cpp
auto entry = engine.get(
    store::types::Key{"user:1"});
```

## Remove

```cpp
auto result = engine.remove(
    store::types::Key{"user:1"});
```

## Apply external operations

```cpp
auto result = engine.apply_operation(op);
```

## WAL-backed write flow

1. Build Operation
2. Encode Operation
3. Append operation payload to WAL
4. Receive WAL sequence
5. Apply operation to memory
6. Use WAL sequence as Entry version

## Recovery

```cpp
auto result = engine.recover();
```

## Reading all entries

```cpp
for (const auto &[key, entry] : engine.entries())
{
}
```

## Operation encoding

```cpp
auto payload =
    store::encoding::OperationEncoder::encode(op);
```

## SnapshotStore

```cpp
store::snapshot::SnapshotStore snapshot;
```

## Build snapshot from WAL

```cpp
auto snapshot =
    store::snapshot::SnapshotBuilderStore::build("data/store.wal");
```

## Error handling

```cpp
if (result.is_err())
{
  const auto &error = result.error();
}
```

## ApplyResult

```cpp
if (apply.created) {}
if (apply.updated) {}
if (apply.deleted) {}
if (apply.is_noop()) {}
```

## Serializer helpers

```cpp
store::utils::Serializer::append_u32(out, 42);
```

## Recommended production flow

```cpp
engine.put(key, value);
engine.get(key);
engine.remove(key);
```

## Design rules

- persist before apply
- deterministic operations
- WAL defines version
- binary-safe values
- ordered replay
- snapshots for compaction

## Example: complete flow

```cpp
int main()
{
  store::engine::StoreEngine engine{
      store::core::StoreConfig::durable("data/store.wal")};

  engine.put(
      store::types::Key{"user:1"},
      store::types::Value::from_string("Gaspard"));

  engine.remove(store::types::Key{"user:1"});

  return 0;
}
```

## Summary

softadastra/store is the WAL-backed materialized state layer of Softadastra.

It provides:

- simple key-value operations
- binary-safe values
- WAL-backed durability
- deterministic recovery
- snapshot building
- local-first state materialization

Its job is simple:

turn durable operations into recoverable state.

