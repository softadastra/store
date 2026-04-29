/**
 *
 *  @file Store.hpp
 *  @author Gaspard Kirira
 *
 *  Copyright 2026, Softadastra.
 *  All rights reserved.
 *  https://github.com/softadastra/softadastra
 *
 *  Licensed under the Apache License, Version 2.0.
 *
 *  Softadastra Store
 *
 */

#ifndef SOFTADASTRA_STORE_HPP
#define SOFTADASTRA_STORE_HPP

/**
 * @brief Main public aggregator header for the Softadastra Store module.
 *
 * Include this file when you want access to the complete Store public API.
 *
 * @code
 * #include <softadastra/store/Store.hpp>
 *
 * using namespace softadastra;
 *
 * int main()
 * {
 *   store::engine::StoreEngine engine{
 *       store::core::StoreConfig::durable("data/store.wal")};
 *
 *   auto result = engine.put(
 *       store::types::Key{"user:1"},
 *       store::types::Value::from_string("Gaspard"));
 *
 *   return result.is_ok() ? 0 : 1;
 * }
 * @endcode
 */

/* Core */
#include <softadastra/store/core/Entry.hpp>
#include <softadastra/store/core/Operation.hpp>
#include <softadastra/store/core/StoreConfig.hpp>

/* Encoding */
#include <softadastra/store/encoding/OperationDecoder.hpp>
#include <softadastra/store/encoding/OperationEncoder.hpp>

/* Engine */
#include <softadastra/store/engine/ApplyResult.hpp>
#include <softadastra/store/engine/StoreEngine.hpp>

/* Index */
#include <softadastra/store/index/IndexEntry.hpp>
#include <softadastra/store/index/InMemoryIndex.hpp>

/* Snapshot */
#include <softadastra/store/snapshot/SnapshotBuilderStore.hpp>
#include <softadastra/store/snapshot/SnapshotStore.hpp>

/* Types */
#include <softadastra/store/types/Key.hpp>
#include <softadastra/store/types/OperationType.hpp>
#include <softadastra/store/types/Value.hpp>

/* Utils */
#include <softadastra/store/utils/Serializer.hpp>

#endif // SOFTADASTRA_STORE_HPP
