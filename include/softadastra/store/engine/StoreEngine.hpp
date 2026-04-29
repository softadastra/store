/**
 *
 *  @file StoreEngine.hpp
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

#ifndef SOFTADASTRA_STORE_ENGINE_HPP
#define SOFTADASTRA_STORE_ENGINE_HPP

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>

#include <softadastra/core/Core.hpp>
#include <softadastra/store/core/Entry.hpp>
#include <softadastra/store/core/Operation.hpp>
#include <softadastra/store/core/StoreConfig.hpp>
#include <softadastra/store/encoding/OperationDecoder.hpp>
#include <softadastra/store/encoding/OperationEncoder.hpp>
#include <softadastra/store/engine/ApplyResult.hpp>
#include <softadastra/store/types/Key.hpp>
#include <softadastra/store/types/OperationType.hpp>
#include <softadastra/store/types/Value.hpp>
#include <softadastra/wal/core/WalConfig.hpp>
#include <softadastra/wal/replay/WalReplayer.hpp>
#include <softadastra/wal/types/WalRecordType.hpp>
#include <softadastra/wal/writer/WalWriter.hpp>

namespace softadastra::store::engine
{
  namespace core = softadastra::store::core;
  namespace types = softadastra::store::types;
  namespace encoding = softadastra::store::encoding;

  namespace core_types = softadastra::core::types;
  namespace core_errors = softadastra::core::errors;
  namespace core_time = softadastra::core::time;

  namespace wal_core = softadastra::wal::core;
  namespace wal_types = softadastra::wal::types;
  namespace wal_writer = softadastra::wal::writer;
  namespace wal_replay = softadastra::wal::replay;

  /**
   * @brief Durable in-memory key-value store engine.
   *
   * StoreEngine materializes the current state of a key-value store and can
   * persist every mutation through the WAL before applying it to memory.
   *
   * It is used by higher-level Softadastra modules that need:
   * - local-first state
   * - WAL-backed durability
   * - deterministic replay
   * - simple key-value materialization
   *
   * Write flow when WAL is enabled:
   * - encode operation
   * - append operation payload to WAL
   * - receive WAL sequence
   * - apply operation to memory using that sequence as version
   *
   * Recovery flow:
   * - read WAL records
   * - decode operation payloads
   * - apply operations to memory in WAL order
   *
   * The store does not perform network synchronization or conflict resolution.
   */
  class StoreEngine : public core_types::NonCopyable
  {
  public:
    /**
     * @brief Internal entry map type.
     */
    using Map = std::unordered_map<std::string, core::Entry>;

    /**
     * @brief Result returned by apply operations.
     */
    using ApplyOperationResult =
        core_types::Result<ApplyResult, core_errors::Error>;

    /**
     * @brief Result returned by recovery operations.
     */
    using Result = core_types::Result<void, core_errors::Error>;

    /**
     * @brief Creates a store engine from a configuration.
     *
     * If WAL is enabled, the writer is created and recovery is attempted.
     * Use recover() explicitly after construction if the caller needs to
     * inspect recovery errors.
     *
     * @param config Store configuration.
     */
    explicit StoreEngine(core::StoreConfig config)
        : config_(std::move(config))
    {
      index_.reserve(config_.initial_capacity);

      if (config_.enable_wal)
      {
        wal_core::WalConfig wal_config =
            wal_core::WalConfig::durable(config_.wal_path);

        wal_config.auto_flush = config_.auto_flush;

        writer_ = std::make_unique<wal_writer::WalWriter>(
            std::move(wal_config));

        const auto recovered = recover();

        if (recovered.is_ok())
        {
          writer_->set_sequence(last_recovered_sequence_);
        }
      }
    }

    /**
     * @brief Moves a store engine.
     */
    StoreEngine(StoreEngine &&) noexcept = default;

    /**
     * @brief Move-assigns a store engine.
     */
    StoreEngine &operator=(StoreEngine &&) noexcept = default;

    /**
     * @brief Inserts or replaces a value for a key.
     *
     * The operation is persisted to WAL first when WAL is enabled.
     *
     * @param key Store key.
     * @param value Value to store.
     * @return Apply result on success, Error on failure.
     */
    [[nodiscard]] ApplyOperationResult put(
        types::Key key,
        types::Value value)
    {
      return apply_operation(
          core::Operation::put(
              std::move(key),
              std::move(value)));
    }

    /**
     * @brief Removes a key from the store.
     *
     * Delete is idempotent. Removing a missing key returns a successful no-op.
     *
     * @param key Store key.
     * @return Apply result on success, Error on failure.
     */
    [[nodiscard]] ApplyOperationResult remove(types::Key key)
    {
      return apply_operation(
          core::Operation::remove(std::move(key)));
    }

    /**
     * @brief Applies an externally provided operation.
     *
     * This is the correct entry point for remote sync operations, replay-like
     * flows, and callers that must preserve the original operation timestamp.
     *
     * When WAL is enabled, the operation is appended before being applied.
     *
     * @param operation Store operation.
     * @return Apply result on success, Error on failure.
     */
    [[nodiscard]] ApplyOperationResult apply_operation(
        const core::Operation &operation)
    {
      if (!operation.is_valid())
      {
        return ApplyOperationResult::err(
            core_errors::Error::make(
                core_errors::ErrorCode::InvalidArgument,
                "invalid store operation"));
      }

      return append_and_apply(operation);
    }

    /**
     * @brief Gets the current entry for a key.
     *
     * @param key Store key.
     * @return Entry if found, std::nullopt otherwise.
     */
    [[nodiscard]] std::optional<core::Entry>
    get(const types::Key &key) const
    {
      if (!key.is_valid())
      {
        return std::nullopt;
      }

      const auto it = index_.find(key.str());

      if (it == index_.end())
      {
        return std::nullopt;
      }

      return it->second;
    }

    /**
     * @brief Returns true if the key exists in the store.
     *
     * @param key Store key.
     * @return true if found.
     */
    [[nodiscard]] bool contains(const types::Key &key) const
    {
      if (!key.is_valid())
      {
        return false;
      }

      return index_.find(key.str()) != index_.end();
    }

    /**
     * @brief Applies a WAL record directly to memory.
     *
     * This is intended for replay and recovery only.
     *
     * The record payload must contain an encoded store Operation.
     *
     * @param record WAL record.
     * @return Apply result on success, Error on failure.
     */
    [[nodiscard]] ApplyOperationResult apply_from_wal(
        const softadastra::wal::core::WalRecord &record)
    {
      auto operation =
          encoding::OperationDecoder::decode(record.payload);

      if (!operation.has_value())
      {
        return ApplyOperationResult::err(
            core_errors::Error::make(
                core_errors::ErrorCode::InvalidArgument,
                "failed to decode store operation from WAL payload"));
      }

      return apply_to_memory(*operation, record.sequence);
    }

    /**
     * @brief Recovers the materialized state from the WAL.
     *
     * Existing in-memory entries are cleared before recovery.
     *
     * @return Result<void, Error>.
     */
    [[nodiscard]] Result recover()
    {
      if (!config_.enable_wal)
      {
        return Result::ok();
      }

      index_.clear();
      last_recovered_sequence_ = 0;

      wal_replay::WalReplayer replayer{config_.wal_path};

      auto replayed = replayer.replay(
          [&](const wal_core::WalRecord &record)
          {
            const auto applied = apply_from_wal(record);

            if (applied.is_ok())
            {
              last_recovered_sequence_ = record.sequence;
            }
          });

      if (replayed.is_err())
      {
        return Result::err(replayed.error());
      }

      if (writer_)
      {
        writer_->set_sequence(last_recovered_sequence_);
      }

      return Result::ok();
    }

    /**
     * @brief Returns read-only access to all materialized entries.
     *
     * @return Internal entry map.
     */
    [[nodiscard]] const Map &entries() const noexcept
    {
      return index_;
    }

    /**
     * @brief Returns the number of materialized entries.
     *
     * @return Entry count.
     */
    [[nodiscard]] std::size_t size() const noexcept
    {
      return index_.size();
    }

    /**
     * @brief Returns true if the store has no entries.
     *
     * @return true when empty.
     */
    [[nodiscard]] bool empty() const noexcept
    {
      return index_.empty();
    }

    /**
     * @brief Clears only the in-memory state.
     *
     * This does not delete or truncate the WAL.
     */
    void clear_memory() noexcept
    {
      index_.clear();
    }

    /**
     * @brief Returns the highest sequence recovered from WAL.
     *
     * @return Last recovered WAL sequence.
     */
    [[nodiscard]] std::uint64_t last_recovered_sequence() const noexcept
    {
      return last_recovered_sequence_;
    }

    /**
     * @brief Returns the store configuration.
     *
     * @return Store configuration.
     */
    [[nodiscard]] const core::StoreConfig &config() const noexcept
    {
      return config_;
    }

  private:
    /**
     * @brief Appends an operation to WAL when enabled, then applies it.
     */
    [[nodiscard]] ApplyOperationResult append_and_apply(
        const core::Operation &operation)
    {
      std::uint64_t version = next_memory_version();

      if (writer_)
      {
        auto payload = encoding::OperationEncoder::encode(operation);

        if (payload.empty())
        {
          return ApplyOperationResult::err(
              core_errors::Error::make(
                  core_errors::ErrorCode::InvalidArgument,
                  "failed to encode store operation"));
        }

        auto appended = writer_->append(
            wal_types::WalRecordType::Put,
            std::move(payload));

        if (appended.is_err())
        {
          return ApplyOperationResult::err(appended.error());
        }

        version = appended.value();
      }

      return apply_to_memory(operation, version);
    }

    /**
     * @brief Applies an operation directly to the in-memory state.
     */
    [[nodiscard]] ApplyOperationResult apply_to_memory(
        const core::Operation &operation,
        std::uint64_t version)
    {
      if (!operation.is_valid())
      {
        return ApplyOperationResult::err(
            core_errors::Error::make(
                core_errors::ErrorCode::InvalidArgument,
                "invalid store operation"));
      }

      auto it = index_.find(operation.key.str());

      switch (operation.type)
      {
      case types::OperationType::Put:
      {
        if (it == index_.end())
        {
          core::Entry entry =
              core::Entry::make(
                  operation.key,
                  operation.value,
                  version);

          entry.timestamp = operation.timestamp;

          index_[operation.key.str()] = std::move(entry);

          return ApplyOperationResult::ok(
              ApplyResult::created_entry(version));
        }

        it->second.value = operation.value;
        it->second.version = version;
        it->second.timestamp = operation.timestamp;

        return ApplyOperationResult::ok(
            ApplyResult::updated_entry(version));
      }

      case types::OperationType::Delete:
      {
        if (it == index_.end())
        {
          return ApplyOperationResult::ok(
              ApplyResult::noop(version));
        }

        index_.erase(it);

        return ApplyOperationResult::ok(
            ApplyResult::deleted_entry(version));
      }

      default:
        return ApplyOperationResult::err(
            core_errors::Error::make(
                core_errors::ErrorCode::InvalidArgument,
                "unsupported store operation type"));
      }
    }

    /**
     * @brief Returns the next version for memory-only mode.
     */
    [[nodiscard]] std::uint64_t next_memory_version() noexcept
    {
      return ++memory_sequence_;
    }

  private:
    core::StoreConfig config_{};
    Map index_{};
    std::unique_ptr<wal_writer::WalWriter> writer_{};

    std::uint64_t memory_sequence_{0};
    std::uint64_t last_recovered_sequence_{0};
  };

} // namespace softadastra::store::engine

#endif // SOFTADASTRA_STORE_ENGINE_HPP
