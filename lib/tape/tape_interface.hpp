#pragma once

#include <cstdint>
#include <filesystem>

namespace tape {

using TapeSize = uint32_t;
using MemorySize = uint32_t;

template <typename T>
class ITape {
 public:
  virtual ~ITape() {};

  // [[nodiscard]] virtual T Read() const = 0;
  virtual void Put(const T&) = 0;

  virtual bool MoveRight() = 0;  // to left cell
  virtual bool MoveLeft() = 0;   // to right cell
};
}  // namespace tape
