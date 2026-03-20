/*
 * InMemoryIndex.hpp
 */

#ifndef SOFTADASTRA_STORE_IN_MEMORY_INDEX_HPP
#define SOFTADASTRA_STORE_IN_MEMORY_INDEX_HPP

#include <unordered_map>
#include <optional>
#include <string>

#include <softadastra/store/types/Key.hpp>
#include <softadastra/store/index/IndexEntry.hpp>

namespace softadastra::store::index
{
  namespace types = softadastra::store::types;

  class InMemoryIndex
  {
  public:
    /**
     * @brief Insert or update entry
     */
    void put(const types::Key &key, const IndexEntry &entry)
    {
      index_[key.value] = entry;
    }

    /**
     * @brief Get entry
     */
    std::optional<IndexEntry> get(const types::Key &key) const
    {
      auto it = index_.find(key.value);
      if (it == index_.end())
        return std::nullopt;

      return it->second;
    }

    /**
     * @brief Remove entry
     */
    bool remove(const types::Key &key)
    {
      return index_.erase(key.value) > 0;
    }

    /**
     * @brief Check existence
     */
    bool contains(const types::Key &key) const
    {
      return index_.find(key.value) != index_.end();
    }

    /**
     * @brief Clear index
     */
    void clear()
    {
      index_.clear();
    }

    /**
     * @brief Size
     */
    std::size_t size() const
    {
      return index_.size();
    }

  private:
    std::unordered_map<std::string, IndexEntry> index_;
  };

} // namespace softadastra::store::index

#endif
