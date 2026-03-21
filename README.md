# softadastra/store

> Content storage layer for local-first systems.

The `store` module is responsible for **managing the actual data (file contents)** in Softadastra.

It handles:

> How data is stored, retrieved, and materialized on disk.

## Purpose

The goal of `softadastra/store` is simple:

> Store and retrieve file content reliably, independently from how it is observed or synchronized.

## Core Principle

> Separate data from observation.

* `fs` observes files
* `store` manages their content

## Responsibilities

The `store` module provides:

* Storage of file content (blobs)
* Reading and writing file data
* Materializing files on disk
* Preparing for chunk-based storage (future)

## What this module does NOT do

* No filesystem observation (fs module)
* No sync logic (sync module)
* No network communication (transport module)
* No operation durability (wal module)

👉 It manages content only.

## Design Principles

### 1. Content-centric

The module focuses on:

* Bytes
* Blobs
* Data representation

Not on file events or sync decisions.

### 2. Decoupled from filesystem

Even if it writes to disk:

* It does not observe filesystem changes
* It does not emit events

### 3. Extensible

Must support future evolution:

* Chunking
* Deduplication
* Compression

### 4. Deterministic

Same input → same stored output.

## Module Structure

```id="st0r3x"
modules/store/
├── include/softadastra/store/
│   ├── BlobStore.hpp
│   ├── LocalStore.hpp
│   ├── Chunker.hpp
│   └── FileMaterializer.hpp
└── src/
```

## Core Components

### BlobStore

Abstract interface.

Provides:

* Store data
* Retrieve data
* Identify content (hash-based)

### LocalStore

Concrete implementation using local disk.

Responsibilities:

* Store blobs as files
* Manage local storage layout

### Chunker

Prepares for:

* Splitting large files into chunks
* Deduplication (future)

### FileMaterializer

Responsible for:

* Reconstructing files from stored data
* Writing files to the filesystem

## Example Usage

```cpp id="ex10"
#include <softadastra/store/engine/StoreEngine.hpp>

using namespace softadastra::store;

int main()
{
  core::StoreConfig config;
  config.wal_path = "data/store.log";

  engine::StoreEngine store(config);

  // Create key
  types::Key key;
  key.value = "message";

  // Create value
  types::Value value;
  value.data = {'H','e','l','l','o'};

  // PUT
  auto res = store.put(key, value);

  if (!res.success)
    return 1;

  // GET
  auto entry = store.get(key);

  if (entry)
  {
    std::string content(entry->value.data.begin(), entry->value.data.end());
    std::cout << content << "\n";
  }

  return 0;
}
```

## Data Flow

### Write

1. Sync receives operation
2. Store saves content
3. Metadata updated
4. File materialized (optional)

### Read

1. Request for file content
2. Store retrieves blob
3. File reconstructed if needed

## Integration

Used by:

* sync (primary)
* metadata (indirectly)
* app layer

## Storage Model

### Blob-based

Content is stored as:

* Immutable blobs
* Identified by hash

### File materialization

Files can be:

* Reconstructed from blobs
* Written to filesystem

## Dependencies

### Internal

* softadastra/core

### External

* Filesystem APIs

## MVP Scope

* Full file storage (no chunking yet)
* Simple local disk layout
* No deduplication
* No compression

## Roadmap

* Chunk-based storage
* Deduplication
* Compression
* Content-addressable storage
* Remote storage support
* Versioned storage

## Rules

* Never depend on filesystem events
* Never include sync logic
* Never mutate stored blobs
* Always treat data as immutable

## Philosophy

The store is not about files.

> It is about data.

## Summary

* Stores file content
* Retrieves data
* Supports file reconstruction
* Decoupled from sync and fs

## Installation

```bash
vix add @softadastra/store
vix deps
```

## License

See root LICENSE file.
