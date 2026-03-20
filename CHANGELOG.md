# Changelog - softadastra/store

All notable changes to the **store module** are documented in this file.

The `store` module is responsible for **managing file content (data)**, independent from filesystem observation and synchronization logic.

---

## [0.1.0] - Initial Storage Layer

### Added

* Core storage abstraction (`BlobStore`)

  * Store data blobs
  * Retrieve data by identifier (hash)
* Local storage implementation (`LocalStore`)

  * File-based storage on disk
  * Simple directory layout
* File materialization system (`FileMaterializer`)

  * Reconstruct files from stored data
  * Write files to filesystem
* Basic content handling:

  * Store full file content
  * Retrieve full file content

### Design

* Content-based storage model
* Immutable data storage
* No dependency on sync or filesystem observation

---

## [0.1.1] - Stability Improvements

### Improved

* Safer write operations to prevent partial file corruption
* More consistent file read/write behavior
* Better error handling during disk operations

### Fixed

* Edge cases with incomplete writes
* Incorrect file reconstruction in failure scenarios

---

## [0.2.0] - Content Addressing

### Added

* Hash-based identification of blobs
* Deterministic content hashing
* Mapping between content and storage location

### Improved

* Reliability of data retrieval
* Consistency between stored and retrieved content

---

## [0.3.0] - File Materialization Improvements

### Added

* Safer file reconstruction process
* Overwrite protection strategies
* Basic validation during file writes

### Improved

* Correctness of file materialization
* Handling of existing files on disk

---

## [0.4.0] - Storage Organization

### Added

* Structured storage layout for blobs
* Directory partitioning (basic)
* Internal storage management utilities

### Improved

* Scalability of storage structure
* Reduced filesystem overhead

---

## [0.5.0] - Performance Improvements

### Added

* Buffered I/O operations
* Basic caching for recent reads

### Improved

* Faster read/write operations
* Reduced disk I/O overhead

---

## [0.6.0] - Safety & Integrity

### Added

* Basic integrity checks on stored data
* Validation during read operations
* Detection of corrupted blobs

### Improved

* Resilience against disk errors
* Safer recovery scenarios

---

## [0.7.0] - Extraction Ready

### Added

* Namespace stabilization (`softadastra::store`)
* Public API cleanup
* Documentation for all exposed interfaces

### Improved

* Decoupling from application-specific assumptions
* Prepared for reuse in:

  * Softadastra Sync OS
  * SDK
  * Distributed storage systems

---

## Roadmap

### Planned

* Chunk-based storage
* Deduplication
* Compression
* Content-addressable storage optimization
* Remote storage support
* Versioned storage model

---

## Philosophy

The store is not about files.

> It is about immutable data.

---

## Rules

* Never mutate stored data
* Never depend on filesystem observation
* Never include sync logic
* Always ensure data integrity

---

## Summary

The `store` module ensures:

* Reliable data storage
* Deterministic retrieval
* File reconstruction

It is the **data layer of Softadastra**.
