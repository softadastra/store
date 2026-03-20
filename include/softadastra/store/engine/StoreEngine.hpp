/*
 * StoreEngine.hpp
 */

#ifndef SOFTADASTRA_STORE_ENGINE_HPP
#define SOFTADASTRA_STORE_ENGINE_HPP

#include <unordered_map>
#include <optional>
#include <memory>
#include <ctime>

#include <softadastra/store/core/Operation.hpp>
#include <softadastra/store/core/Entry.hpp>
#include <softadastra/store/core/StoreConfig.hpp>

#include <softadastra/store/encoding/OperationEncoder.hpp>
#include <softadastra/store/encoding/OperationDecoder.hpp>

#include <softadastra/store/engine/ApplyResult.hpp>

#include <softadastra/wal/writer/WalWriter.hpp>
#include <softadastra/wal/replay/WalReplayer.hpp>

namespace softadastra::store::engine
{
  namespace core = softadastra::store::core;
  namespace types = softadastra::store::types;
  namespace encoding = softadastra::store::encoding;

  namespace wal_writer = softadastra::wal::writer;
  namespace wal_replay = softadastra::wal::replay;

  class StoreEngine
  {
  public:
    explicit StoreEngine(const core::StoreConfig &config)
        : config_(config)
    {
      if (config_.enable_wal)
      {
        wal::core::WalConfig wal_config;
        wal_config.path = config_.wal_path;
        wal_config.auto_flush = config_.auto_flush;

        writer_ = std::make_unique<wal_writer::WalWriter>(wal_config);

        recover();
      }
    }

    /**
     * PUT
     */
    ApplyResult put(const types::Key &key, const types::Value &value)
    {
      core::Operation op;
      op.type = types::OperationType::Put;
      op.key = key;
      op.value = value;
      op.timestamp = now();

      return apply_with_wal(op);
    }

    /**
     * DELETE
     */
    ApplyResult remove(const types::Key &key)
    {
      core::Operation op;
      op.type = types::OperationType::Delete;
      op.key = key;
      op.timestamp = now();

      return apply_with_wal(op);
    }

    /**
     * GET
     */
    std::optional<core::Entry> get(const types::Key &key) const
    {
      auto it = index_.find(key.value);
      if (it == index_.end())
        return std::nullopt;

      return it->second;
    }

    /**
     * @brief Apply WAL record (replay only)
     */
    void apply_from_wal(const softadastra::wal::core::WalRecord &record)
    {
      auto op = encoding::OperationDecoder::decode(
          record.payload.data(),
          record.payload.size());

      if (!op)
        return;

      apply(*op, record.sequence);
    }

    /**
     * @brief Read-only access to entries
     */
    const std::unordered_map<std::string, core::Entry> &entries() const noexcept
    {
      return index_;
    }

  private:
    /**
     * Apply with WAL (critical path)
     */
    ApplyResult apply_with_wal(core::Operation &op)
    {
      ApplyResult result;

      std::uint64_t version = 0;

      if (writer_)
      {
        // encode operation → payload
        auto payload = encoding::OperationEncoder::encode(op);

        wal::core::WalRecord record;
        record.type = wal::types::WalRecordType::Put;
        record.timestamp = op.timestamp;
        record.payload = std::move(payload);

        version = writer_->append(record);
      }

      result = apply(op, version);

      return result;
    }

    /**
     * Apply to in-memory state
     */
    ApplyResult apply(const core::Operation &op, std::uint64_t version)
    {
      ApplyResult result;
      result.success = true;
      result.version = version;

      auto it = index_.find(op.key.value);

      switch (op.type)
      {
      case types::OperationType::Put:
      {
        if (it == index_.end())
        {
          core::Entry entry;
          entry.key = op.key;
          entry.value = op.value;
          entry.version = version;
          entry.timestamp = op.timestamp;

          index_[op.key.value] = entry;
          result.created = true;
        }
        else
        {
          it->second.value = op.value;
          it->second.version = version;
          it->second.timestamp = op.timestamp;

          result.updated = true;
        }
        break;
      }

      case types::OperationType::Delete:
      {
        if (it != index_.end())
        {
          index_.erase(it);
          result.deleted = true;
        }
        break;
      }

      default:
        result.success = false;
        break;
      }

      return result;
    }

    /**
     * WAL recovery
     */
    void recover()
    {
      wal_replay::WalReplayer replayer(config_.wal_path);

      replayer.replay([&](const wal::core::WalRecord &record)
                      {
        auto op = encoding::OperationDecoder::decode(
            record.payload.data(),
            record.payload.size());

        if (!op)
          return;

        apply(*op, record.sequence); });
    }

    /**
     * simple timestamp
     */
    static std::uint64_t now()
    {
      return static_cast<std::uint64_t>(std::time(nullptr));
    }

  private:
    core::StoreConfig config_;

    std::unordered_map<std::string, core::Entry> index_;

    std::unique_ptr<wal_writer::WalWriter> writer_;
  };

} // namespace softadastra::store::engine

#endif
