/*
 * ApplyResult.hpp
 */

#ifndef SOFTADASTRA_STORE_APPLY_RESULT_HPP
#define SOFTADASTRA_STORE_APPLY_RESULT_HPP

#include <cstdint>

namespace softadastra::store::engine
{

  struct ApplyResult
  {
    bool success{false};

    std::uint64_t version{0}; // WAL sequence

    bool created{false};
    bool updated{false};
    bool deleted{false};
  };

} // namespace softadastra::store::engine

#endif
