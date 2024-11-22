#pragma once

#include <vector>

#include "../delays/delays.hpp"

namespace tape {

using ChunkSize = uint32_t;
using ChunksNumber = uint32_t;

////////////////////////////////////////////////////////////////////////////////
/// \brief A class for storing and processing numbers in RAM.
///
/// \tparam TapeType type of elements in the chunk.
////////////////////////////////////////////////////////////////////////////////
template <typename TapeType>
class Chunk {
 public:
  //////////////////////////////////////////////////////////////////////////////
  /// \brief Chunk default constructor.
  //////////////////////////////////////////////////////////////////////////////
  Chunk() = default;

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Chunk constructor.
  ///
  /// \param delays delays in reading, putting, moving.
  /// \param chunk_number chunk number/position/id.
  /// \param size number of elements of the chunk.
  //////////////////////////////////////////////////////////////////////////////
  Chunk(Delays delays, ChunksNumber chunk_number, ChunkSize size);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Read new chunk from a file.
  ///
  /// \param from file stream from where the new chunk will be read.
  /// \param new_chunk_number number of the new chunk.
  /// \param new_size size of the new chunk.
  //////////////////////////////////////////////////////////////////////////////
  void ReadNewChunk(std::fstream& from, ChunksNumber new_chunk_number,
                    ChunkSize new_size);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Put a new number in the chunk array.
  ///
  /// \param elem new element.
  /// \param pos position where the new number is placed.
  //////////////////////////////////////////////////////////////////////////////
  void PutElementInArrayByPos(const TapeType& elem, ChunkSize pos);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Output a chunk to a file.
  ///
  /// \param to file stream into which the chunk will be printed.
  //////////////////////////////////////////////////////////////////////////////
  void PrintChunk(std::fstream& to);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Clear chunk without changing delays.
  //////////////////////////////////////////////////////////////////////////////
  void Destroy();

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Get the position in the chunk indicated by the magnetic head.
  ///
  /// \return position in the chunk.
  //////////////////////////////////////////////////////////////////////////////
  [[nodiscard]] ChunkSize GetPos() const;

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Get the chunk numbe r (id).
  ///
  /// \return @return chunk number
  //////////////////////////////////////////////////////////////////////////////
  [[nodiscard]] ChunksNumber GetChunkNumber() const;

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Get the element in the chunk that the magnetic head points to.
  ///
  /// \return element.
  //////////////////////////////////////////////////////////////////////////////
  [[nodiscard]] TapeType GetCurrentElement() const;

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Get all elements in the chunk.
  ///
  /// \return vector of elements.
  //////////////////////////////////////////////////////////////////////////////
  [[nodiscard]] std::vector<TapeType> GetChunkElements() const;

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Checking for the element to the left of the magnetic head. It does
  /// not exist if this chunk is the leftmost and the magnetic head points to
  /// the leftmost element in it.
  ///
  /// \return true if such an element exists else false.
  //////////////////////////////////////////////////////////////////////////////
  [[nodiscard]] bool IsPossibleTakeLeftElement() const;

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Checking for the element to the left of the magnetic head. It does
  /// not exist if this chunk is the rightmost and the magnetic head points to
  /// the rightmost element in it.
  ///
  /// \param chunks_number total number of chunks on the tape.
  /// \return true if such an element exists else false.
  //////////////////////////////////////////////////////////////////////////////
  [[nodiscard]] bool IsPossibleTakeRightElement(
      ChunksNumber chunks_number) const;

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The position and the number of the chunk are the same as the
  /// current ones.
  ///
  /// \param another_pos
  /// \param another_chunk_number
  /// \return if the position and the chunk number are the same else false.
  //////////////////////////////////////////////////////////////////////////////
  [[nodiscard]] bool IsMatchWith(ChunkSize another_pos,
                                 ChunksNumber another_chunk_number) const;

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Move the chunk to the right by one position. That is, the magnetic
  /// head will be on the element of the chunk on the left.
  ///
  /// \return true if the move succeeded else false.
  //////////////////////////////////////////////////////////////////////////////
  bool MoveRightPos();

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Move the chunk to the left by one position. That is, the magnetic
  /// head will be on the element of the chunk on the right.
  ///
  /// \return true if the move succeeded else false.
  //////////////////////////////////////////////////////////////////////////////
  bool MoveLeftPos();

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Move the tape to the right until the magnetic head points to the
  /// leftmost position of the chunk.
  //////////////////////////////////////////////////////////////////////////////
  void MoveToLeftEdge();

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Move the tape to the left until the magnetic head points to the
  /// rightmost position of the chunk.
  //////////////////////////////////////////////////////////////////////////////
  void MoveToRightEdge();

 private:
  //////////////////////////////////////////////////////////////////////////////
  /// \brief Checking that the current position is the leftmost in the chunk.
  ///
  /// \return true if the current position is the leftmost else false.
  //////////////////////////////////////////////////////////////////////////////
  [[nodiscard]] bool IsLeftEdge() const;

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Checking that the current position is the rightmost in the chunk.
  ///
  /// \return rue if the current position is the rightmost else false.
  //////////////////////////////////////////////////////////////////////////////
  [[nodiscard]] bool IsRightEdge() const;

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Delays in reading, putting, moving.
  //////////////////////////////////////////////////////////////////////////////
  Delays delays_{};

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Chunk number/position/id.
  //////////////////////////////////////////////////////////////////////////////
  ChunksNumber chunk_number_{};

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Number of elements.
  //////////////////////////////////////////////////////////////////////////////
  ChunkSize size_{};

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Position in the chunk from cell indicated by the magnetic head.
  //////////////////////////////////////////////////////////////////////////////
  ChunkSize pos_{};

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Array of chunk elements.
  //////////////////////////////////////////////////////////////////////////////
  std::vector<TapeType> elements_{};
};

template <typename TapeType>
Chunk<TapeType>::Chunk(Delays delays, ChunksNumber chunk_number, ChunkSize size)
    : delays_(delays), chunk_number_(chunk_number), size_(size), pos_(0) {}

template <typename TapeType>
void Chunk<TapeType>::ReadNewChunk(std::fstream& from,
                                   ChunksNumber new_chunk_number,
                                   ChunkSize new_size) {
  size_ = new_size;
  pos_ = new_chunk_number >= chunk_number_ ? size_ - 1 : 0;
  chunk_number_ = new_chunk_number;
  elements_.clear();
  elements_.resize(size_);
  for (TapeType& elem : elements_) {
    std::this_thread::sleep_for(delays_.delay_for_shift_);
    std::this_thread::sleep_for(delays_.delay_for_reading_);
    from >> elem;
  }
}

template <typename TapeType>
void Chunk<TapeType>::PutElementInArrayByPos(const TapeType& elem,
                                             ChunkSize pos) {
  std::this_thread::sleep_for(delays_.delay_for_writing_);
  elements_[pos] = elem;
}

template <typename TapeType>
void Chunk<TapeType>::PrintChunk(std::fstream& to) {
  for (TapeType elem : elements_) {
    to << elem << ' ';
  }
}

template <typename TapeType>
void Chunk<TapeType>::Destroy() {
  chunk_number_ = 0;
  size_ = 0;
  pos_ = 0;
  elements_.clear();
}

template <typename TapeType>
ChunkSize Chunk<TapeType>::GetPos() const {
  return pos_;
}

template <typename TapeType>
ChunksNumber Chunk<TapeType>::GetChunkNumber() const {
  return chunk_number_;
}

template <typename TapeType>
TapeType Chunk<TapeType>::GetCurrentElement() const {
  std::this_thread::sleep_for(delays_.delay_for_reading_);
  return elements_[pos_];
}

template <typename TapeType>
std::vector<TapeType> Chunk<TapeType>::GetChunkElements() const {
  return elements_;
}

template <typename TapeType>
bool Chunk<TapeType>::IsPossibleTakeLeftElement() const {
  return !(chunk_number_ == 0 && pos_ == 0);
}

template <typename TapeType>
bool Chunk<TapeType>::IsPossibleTakeRightElement(
    ChunksNumber chunks_number) const {
  return !(chunk_number_ == chunks_number - 1 && pos_ == size_ - 1);
}

template <typename TapeType>
bool Chunk<TapeType>::IsMatchWith(ChunkSize another_pos,
                                  ChunksNumber another_chunk_number) const {
  return pos_ == another_pos && another_chunk_number == chunk_number_;
}

template <typename TapeType>
bool Chunk<TapeType>::MoveRightPos() {
  if (!IsPossibleTakeLeftElement() || IsLeftEdge()) {
    return false;
  }
  std::this_thread::sleep_for(delays_.delay_for_shift_);
  pos_--;

  return true;
}

template <typename TapeType>
bool Chunk<TapeType>::MoveLeftPos() {
  if (IsRightEdge()) {
    return false;
  }
  std::this_thread::sleep_for(delays_.delay_for_shift_);
  pos_++;

  return true;
}

template <typename TapeType>
void Chunk<TapeType>::MoveToLeftEdge() {
  while (!IsLeftEdge()) {
    MoveRightPos();
  }
}

template <typename TapeType>
void Chunk<TapeType>::MoveToRightEdge() {
  while (!IsRightEdge()) {
    MoveLeftPos();
  }
}

template <typename TapeType>
bool Chunk<TapeType>::IsLeftEdge() const {
  return pos_ == 0;
}

template <typename TapeType>
bool Chunk<TapeType>::IsRightEdge() const {
  return pos_ == size_ - 1;
}
}  // namespace tape
