/*
 * SnapshotStore.hpp
 */

#ifndef SOFTADASTRA_STORE_SNAPSHOT_STORE_HPP
#define SOFTADASTRA_STORE_SNAPSHOT_STORE_HPP

#include <unordered_map>

#include <softadastra/store/types/Key.hpp>
#include <softadastra/store/types/Value.hpp>

namespace softadastra::store::snapshot
{
  namespace types = softadastra::store::types;

  class SnapshotStore
  {
  public:
    using Map = std::unordered_map<std::string, types::Value>;

    SnapshotStore() = default;

    // Insert / Update
    void put(const types::Key &key, const types::Value &value)
    {
      data_[key.value] = value;
    }

    void put(types::Key &&key, types::Value &&value)
    {
      data_[std::move(key.value)] = std::move(value);
    }

    // Remove
    void remove(const types::Key &key)
    {
      data_.erase(key.value);
    }

    // Access
    const types::Value *get(const types::Key &key) const noexcept
    {
      auto it = data_.find(key.value);
      if (it == data_.end())
        return nullptr;

      return &it->second;
    }

    bool contains(const types::Key &key) const noexcept
    {
      return data_.find(key.value) != data_.end();
    }

    const Map &all() const noexcept
    {
      return data_;
    }

    std::size_t size() const noexcept
    {
      return data_.size();
    }

    bool empty() const noexcept
    {
      return data_.empty();
    }

    // Reset
    void clear()
    {
      data_.clear();
    }

  private:
    Map data_;
  };

} // namespace softadastra::store::snapshot

#endif
