#pragma once

#include <vector>

#include "../delays/delays.hpp"

namespace tape {

using ChunkSize = uint32_t;
using ChunksNumber = uint32_t;

template <typename TapeType>
class Chunk {
 public:
  Chunk() = default;
  Chunk(Delays delays, ChunksNumber chunk_number, ChunkSize size);

  void ReadNewChunk(std::fstream& from, ChunksNumber new_chunk_number,
                    ChunkSize new_size);

  void PutElementInArrayByPos(const TapeType& elem, ChunkSize pos);

  void PrintChunk(std::fstream& to);

  void Destroy();

  [[nodiscard]] ChunkSize GetPos() const;

  [[nodiscard]] ChunksNumber GetChunkNumber() const;

  [[nodiscard]] TapeType GetCurrentElement() const;

  [[nodiscard]] std::vector<TapeType> GetChunkElements() const;

  [[nodiscard]] bool IsPossibleTakeLeftElement() const;
  [[nodiscard]] bool IsPossibleTakeRightElement(
      ChunksNumber chunks_number) const;

  [[nodiscard]] bool IsMatchWith(ChunkSize another_pos,
                                 ChunksNumber another_chunk_number) const;

  bool MoveRightPos();
  bool MoveLeftPos();

  void MoveToLeftEdge();
  void MoveToRightEdge();

 private:
  [[nodiscard]] bool IsLeftEdge() const;
  [[nodiscard]] bool IsRightEdge() const;

  Delays delays_{};

  ChunksNumber chunk_number_{};

  ChunkSize size_{};

  ChunkSize pos_{};

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
    std::this_thread::sleep_for(delays_.delay_for_read_);
    from >> elem;
  }
}

template <typename TapeType>
void Chunk<TapeType>::PutElementInArrayByPos(const TapeType& elem,
                                             ChunkSize pos) {
  std::this_thread::sleep_for(delays_.delay_for_write_);
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
  std::this_thread::sleep_for(delays_.delay_for_read_);
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
