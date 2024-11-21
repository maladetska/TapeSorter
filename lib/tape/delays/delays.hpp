#pragma once

#include <chrono>
#include <future>

namespace tape {
////////////////////////////////////////////////////////////////////////////////
/// \brief Delays for processes.
////////////////////////////////////////////////////////////////////////////////
struct Delays {
  //////////////////////////////////////////////////////////////////////////////
  /// \brief Delays defult constructor.
  //////////////////////////////////////////////////////////////////////////////
  Delays() = default;
  //////////////////////////////////////////////////////////////////////////////
  /// \brief Delays constructor by delays for read write and shift.
  //////////////////////////////////////////////////////////////////////////////
  Delays(std::chrono::milliseconds delay_for_read,
         std::chrono::milliseconds delay_for_write,
         std::chrono::milliseconds delay_for_shift);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Delay in reading the number indicated by the magnetic head.
  //////////////////////////////////////////////////////////////////////////////
  std::chrono::milliseconds delay_for_read_{};
  //////////////////////////////////////////////////////////////////////////////
  /// \brief Delay in putting the number to the position indicated by the
  /// magnetic head.
  //////////////////////////////////////////////////////////////////////////////
  std::chrono::milliseconds delay_for_write_{};
  //////////////////////////////////////////////////////////////////////////////
  /// \brief Delay for moving the tape by one position.
  //////////////////////////////////////////////////////////////////////////////
  std::chrono::milliseconds delay_for_shift_{};
};
}  // namespace tape