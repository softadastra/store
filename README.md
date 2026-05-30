# softadastra/store

> WAL-backed key-value store primitives for reliable Softadastra products.

`softadastra/store` provides the materialized state layer of the Softadastra C++ stack.

Softadastra builds reliability-first products for local-first, offline-first, and distributed applications. This module turns durable operations into recoverable local state.

## Purpose

`softadastra/store` exists to provide a simple local state engine backed by a Write-Ahead Log.

It is used by higher-level modules such as Sync, Metadata, SDKs, and product infrastructure.

The core rule is simple:

> Persist first. Apply after.

It is designed to be:

- Local-first
- WAL-backed
- Binary-safe
- Deterministic
- Recoverable
- Product-ready

## What it provides

This module provides store primitives such as:

- Binary-safe keys and values
- Logical store operations
- In-memory materialized state
- WAL-backed persistence
- Operation encoding and decoding
- Deterministic recovery
- Snapshot building from WAL

## What it does not do

`softadastra/store` does not contain:

- Network transport
- Peer-to-peer sync
- Conflict resolution
- Distributed consensus
- Filesystem watching
- Product-specific logic

It stores and replays local state changes. Higher-level modules decide how state is synchronized, merged, or exposed to products.

## Core model

```txt
Operation
    |
Append to WAL
    |
Apply to memory
    |
Recoverable state
```

When WAL is enabled, mutations are persisted before being applied to the in-memory index.

## Where it fits

```txt
Softadastra products
        |
SDKs and product APIs
        |
Sync, Metadata
        |
softadastra/store
        |
softadastra/wal
        |
softadastra/core
```

`softadastra/store` depends on `softadastra/wal` and `softadastra/core`. It provides durable local state for higher-level modules.

## Installation

```bash
vix add @softadastra/store
```

## Usage

```cpp
#include <softadastra/store/Store.hpp>
```

## Example

```cpp
#include <softadastra/store/Store.hpp>
#include <iostream>

int main()
{
    softadastra::store::engine::StoreEngine store{
        softadastra::store::core::StoreConfig::durable("data/store.wal")
    };

    auto result = store.put(
        softadastra::store::types::Key{"user:1"},
        softadastra::store::types::Value::from_string("Gaspard")
    );

    if (result.is_err())
    {
        std::cout << result.error().message() << "\n";
        return 1;
    }

    auto entry = store.get(
        softadastra::store::types::Key{"user:1"}
    );

    if (!entry.has_value())
    {
        std::cout << "entry not found\n";
        return 1;
    }

    std::cout << entry->value.to_string() << "\n";
    return 0;
}
```

## Requirements

- C++20
- `softadastra/core`
- `softadastra/wal`

## Documentation

For the full documentation, visit [docs.softadastra.com](https://docs.softadastra.com).

## License

Licensed under the Apache License, Version 2.0.
