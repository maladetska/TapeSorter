#include "delays.hpp"

namespace tape {
Delays::Delays(std::chrono::milliseconds delay_for_read,
               std::chrono::milliseconds delay_for_write,
               std::chrono::milliseconds delay_for_shift)
    : delay_for_read_(delay_for_read),
      delay_for_write_(delay_for_write),
      delay_for_shift_(delay_for_shift) {}
}  // namespace tape
