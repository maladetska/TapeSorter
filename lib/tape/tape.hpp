#pragma once

#include <fstream>

#include "chunks_info/chunks_info.hpp"
#include "delays/delays.hpp"
#include "sorter/tape_sorter.hpp"

namespace tape {

using TapeSize = uint32_t;
using MemorySize = uint32_t;

////////////////////////////////////////////////////////////////////////////////
/// \brief Tape Interface.
////////////////////////////////////////////////////////////////////////////////
template <typename TapeType>
class Tape : public ITape<TapeType> {
 public:
  Tape() = default;

  Tape(const std::filesystem::path &file, TapeSize size, MemorySize memory_size,
       const Delays &delays);

  Tape(const std::filesystem::path &file, TapeSize size, MemorySize memory_size,
       const std::chrono::milliseconds &delay_for_read,
       const std::chrono::milliseconds &delay_for_write,
       const std::chrono::milliseconds &delay_for_shift);
  Tape(const std::filesystem::path &file,
       const std::chrono::milliseconds &delay_for_read,
       const std::chrono::milliseconds &delay_for_write,
       const std::chrono::milliseconds &delay_for_shift);

  Tape(const Delays &delays);

  Tape(const Tape &);
  Tape(const Tape &, std::filesystem::path &path);
  Tape &operator=(const Tape &);

  Tape(Tape &&) noexcept;
  Tape &operator=(Tape &&) noexcept;

  ~Tape();

  [[nodiscard]] TapeType ReadCell() override;
  void WriteToCell(const TapeType &) override;

  bool MoveRight() override;
  bool MoveLeft() override;

  [[nodiscard]] std::filesystem::path GetTapeFilePath() const;
  [[nodiscard]] TapeSize GetSize() const;
  [[nodiscard]] TapeSize GetMemorySize() const;
  [[nodiscard]] TapeSize GetChunksNumber() const;
  [[nodiscard]] ChunkSize GetMaxChunkSize() const;
  [[nodiscard]] ChunkSize GetMinChunkSize() const;
  [[nodiscard]] std::vector<TapeType> GetChunkElements() const;

  void ClearChunkInTape();

  template <typename T>
  friend class TapeSorter;

 private:
  Tape(const std::filesystem::path &file, TapeSize size,
       ChunkSize max_chunk_size);

  bool InitFirstChunk();

  void ReadChunkToTheRight();
  void ReadChunkToTheLeft();

  static void RewriteFromTo(std::fstream &from, std::fstream &to);

  void PutElementInNewChunk(std::fstream &to, ChunkSize size, ChunkSize pos,
                            TapeType element);

  void ReadAndWriteNewChunk(std::fstream &from, std::fstream &to,
                            ChunksNumber new_chunk_number, ChunkSize new_size);

  [[nodiscard]] static ChunkSize CalculateChunkSize(MemorySize memory,
                                                    TapeSize size);

  std::fstream stream_from_;
  std::filesystem::path tape_location_{};

  // N, размер в байтах всей ленты: kN, где k -- размер одного эл-та
  TapeSize size_{};
  MemorySize memory_size_{};  // кол-во элементов на одной tmp ленте: m_ / k

  Delays delays_{};

  ChunksInfo chunks_info_;

  Chunk<TapeType> current_chunk_{};

  bool unused_ = true;

  static const MemorySize kDivider = 16;
  const std::filesystem::path kDirForTempTapes_ = "./kDirForTempTapes_/";
};

template <typename TapeType>
Tape<TapeType>::Tape(const std::filesystem::path &file, TapeSize size,
                     MemorySize memory_size, const Delays &delays)
    : tape_location_(file),
      size_(size),
      memory_size_(memory_size),
      delays_(delays) {
  chunks_info_ = ChunksInfo(CalculateChunkSize(memory_size_, size_), size_);
  current_chunk_ = Chunk<TapeType>(delays_, 0, chunks_info_.max_chunk_size_);
  stream_from_.open(tape_location_);
}

template <typename TapeType>
Tape<TapeType>::Tape(const std::filesystem::path &file, TapeSize size,
                     MemorySize memory_size,
                     const std::chrono::milliseconds &delay_for_read,
                     const std::chrono::milliseconds &delay_for_write,
                     const std::chrono::milliseconds &delay_for_shift)
    : Tape(file, size, memory_size,
           Delays(delay_for_read, delay_for_write, delay_for_shift)) {}

template <typename TapeType>
Tape<TapeType>::Tape(const std::filesystem::path &file,
                     const std::chrono::milliseconds &delay_for_read,
                     const std::chrono::milliseconds &delay_for_write,
                     const std::chrono::milliseconds &delay_for_shift)
    : tape_location_(file),
      delays_(delay_for_read, delay_for_write, delay_for_shift) {
  stream_from_.open(tape_location_);
}

template <typename TapeType>
Tape<TapeType>::Tape(const std::filesystem::path &file, TapeSize size,
                     ChunkSize max_chunk_size)
    : Tape(file, std::chrono::milliseconds::zero(),
           std::chrono::milliseconds::zero(),
           std::chrono::milliseconds::zero()) {
  size_ = size;
  memory_size_ = size_;
  chunks_info_ = ChunksInfo(max_chunk_size, size_);
  current_chunk_ = Chunk<TapeType>(delays_, 0, chunks_info_.max_chunk_size_);
}

template <typename TapeType>
Tape<TapeType>::Tape(const Delays &delays) : delays_(delays) {}

template <typename TapeType>
Tape<TapeType>::Tape(const Tape &other)
    : tape_location_(other.tape_location_),
      delays_(other.delays_),
      size_(other.size_),
      memory_size_(other.memory_size_),
      chunks_info_(other.chunks_info_),
      current_chunk_(other.current_chunk_) {}

template <typename TapeType>
Tape<TapeType>::Tape(const Tape &other, std::filesystem::path &path)
    : Tape(other) {
  tape_location_ = path;
  stream_from_.open(tape_location_);
  std::fstream other_file(other.tape_location_);
  RewriteFromTo(other_file, stream_from_);
  stream_from_.close();
  other_file.close();
}

template <typename TapeType>
Tape<TapeType> &Tape<TapeType>::operator=(const Tape &other) {
  tape_location_ = other.tape_location_;
  delays_ = other.delays_;
  size_ = other.size_;
  memory_size_ = other.memory_size_;
  chunks_info_ = other.chunks_info_;
  current_chunk_ = other.current_chunk_;
  unused_ = other.unused_;

  if (exists(tape_location_)) {
    if (stream_from_.is_open()) {
      stream_from_.close();
    }
    stream_from_.open(tape_location_, std::ios::in | std::ios::out);
    std::fstream other_file(other.tape_location_);
    RewriteFromTo(other_file, stream_from_);
    stream_from_.close();
  } else {
    tape_location_ = other.tape_location_;
  }

  return *this;
}

template <typename TapeType>
Tape<TapeType>::Tape(Tape &&other) noexcept {
  *this = std::move(other);
}

template <typename TapeType>
Tape<TapeType> &Tape<TapeType>::operator=(Tape &&other) noexcept {
  if (&other == this) {
    return *this;
  }

  std::swap(other.delays_, delays_);
  std::swap(other.size_, size_);
  std::swap(other.memory_size_, memory_size_);
  std::swap(other.chunks_info_, chunks_info_);
  std::swap(other.current_chunk_, current_chunk_);
  std::swap(other.unused_, unused_);

  if (exists(tape_location_)) {
    if (stream_from_.is_open()) stream_from_.close();
    stream_from_.open(tape_location_, std::ios::in | std::ios::out);
    other.stream_from_.close();
    other.stream_from_.open(other.tape_location_, std::ios::in | std::ios::out);
    RewriteFromTo(other.stream_from_, stream_from_);
    stream_from_.close();
  } else {
    tape_location_ = other.tape_location_;
  }
  other.tape_location_ = "";
  other.stream_from_.close();

  return *this;
}

template <typename TapeType>
Tape<TapeType>::~Tape() {
  stream_from_.close();
}

template <typename TapeType>
TapeType Tape<TapeType>::ReadCell() {
  if (InitFirstChunk()) {
    current_chunk_.MoveToLeftEdge();
  }

  return current_chunk_.GetCurrentElement();
}

template <typename TapeType>
void Tape<TapeType>::WriteToCell(const TapeType &element) {
  ChunkSize current_pos = current_chunk_.GetPos();
  ChunksNumber current_chunk_number = current_chunk_.GetChunkNumber();
  if (InitFirstChunk()) {
    current_chunk_.MoveRightPos();
  }
  std::filesystem::create_directories(kDirForTempTapes_);

  std::filesystem::path tmp_path(kDirForTempTapes_);
  tmp_path += "print_tmp.txt";
  std::fstream tmp_to(tmp_path, std::fstream::out);

  stream_from_.seekg(0);
  stream_from_.seekp(0);

  if (chunks_info_.chunks_number_ == 1) {
    PutElementInNewChunk(tmp_to, chunks_info_.max_chunk_size_, current_pos,
                         element);
  } else if (current_chunk_number == chunks_info_.chunks_number_ - 1) {
    for (TapeSize i = 0; i < chunks_info_.chunks_number_ - 1; i++) {
      ReadAndWriteNewChunk(stream_from_, tmp_to, i,
                           chunks_info_.max_chunk_size_);
    }
    PutElementInNewChunk(tmp_to, chunks_info_.last_chunk_size_, current_pos,
                         element);
  } else if (current_chunk_number == 0) {
    PutElementInNewChunk(tmp_to, chunks_info_.max_chunk_size_, current_pos,
                         element);
    for (TapeSize i = 1; i < chunks_info_.chunks_number_ - 1; i++) {
      ReadAndWriteNewChunk(stream_from_, tmp_to, i,
                           chunks_info_.max_chunk_size_);
    }
    ReadAndWriteNewChunk(stream_from_, tmp_to, chunks_info_.chunks_number_ - 1,
                         chunks_info_.last_chunk_size_);
  } else {
    for (TapeSize i = 0; i < current_chunk_number; i++) {
      ReadAndWriteNewChunk(stream_from_, tmp_to, i,
                           chunks_info_.max_chunk_size_);
    }
    PutElementInNewChunk(tmp_to, chunks_info_.max_chunk_size_, current_pos,
                         element);
    for (TapeSize i = current_chunk_number + 1;
         i < chunks_info_.chunks_number_ - 1; i++) {
      ReadAndWriteNewChunk(stream_from_, tmp_to, i,
                           chunks_info_.max_chunk_size_);
    }
    ReadAndWriteNewChunk(stream_from_, tmp_to, chunks_info_.chunks_number_ - 1,
                         chunks_info_.last_chunk_size_);
  }

  stream_from_.close();
  stream_from_.open(tape_location_, std::ios::in | std::ios::out);
  tmp_to.close();
  tmp_to.open(tmp_path, std::ios::in);
  stream_from_.seekg(0);
  stream_from_.seekp(0);
  tmp_to.seekg(0);
  tmp_to.seekp(0);

  for (TapeSize i = 0; i < chunks_info_.chunks_number_ - 1; i++) {
    ReadAndWriteNewChunk(tmp_to, stream_from_, i, chunks_info_.max_chunk_size_);
  }
  ReadAndWriteNewChunk(tmp_to, stream_from_, chunks_info_.chunks_number_ - 1,
                       chunks_info_.last_chunk_size_);

  tmp_to.close();
  std::filesystem::remove_all(kDirForTempTapes_);

  while (!current_chunk_.IsMatchWith(current_pos, current_chunk_number)) {
    MoveRight();
  }
}

template <typename TapeType>
bool Tape<TapeType>::MoveRight() {
  if (InitFirstChunk()) {
    current_chunk_.MoveToLeftEdge();
  }

  if (current_chunk_.IsPossibleTakeLeftElement()) {
    if (!current_chunk_.MoveRightPos()) {
      ReadChunkToTheLeft();
    }
    return true;
  }
  return false;
}

template <typename TapeType>
bool Tape<TapeType>::MoveLeft() {
  if (InitFirstChunk()) {
    current_chunk_.MoveToLeftEdge();
  }

  if (current_chunk_.IsPossibleTakeRightElement(chunks_info_.chunks_number_)) {
    if (!current_chunk_.MoveLeftPos()) {
      ReadChunkToTheRight();
    }
    return true;
  }
  return false;
}

template <typename TapeType>
std::filesystem::path Tape<TapeType>::GetTapeFilePath() const {
  return tape_location_;
}

template <typename TapeType>
TapeSize Tape<TapeType>::GetSize() const {
  return size_;
}

template <typename TapeType>
TapeSize Tape<TapeType>::GetMemorySize() const {
  return memory_size_;
}

template <typename TapeType>
TapeSize Tape<TapeType>::GetChunksNumber() const {
  return chunks_info_.chunks_number_;
}

template <typename TapeType>
ChunkSize Tape<TapeType>::GetMaxChunkSize() const {
  return chunks_info_.max_chunk_size_;
}

template <typename TapeType>
ChunkSize Tape<TapeType>::GetMinChunkSize() const {
  return chunks_info_.last_chunk_size_;
}

template <typename TapeType>
std::vector<TapeType> Tape<TapeType>::GetChunkElements() const {
  return current_chunk_.GetChunkElements();
}

template <typename TapeType>
void Tape<TapeType>::ClearChunkInTape() {
  current_chunk_.Destroy();
}

template <typename TapeType>
bool Tape<TapeType>::InitFirstChunk() {
  if (!unused_) {
    return false;
  }
  stream_from_.open(tape_location_);
  current_chunk_.ReadNewChunk(stream_from_, 0, chunks_info_.max_chunk_size_);
  unused_ = false;

  return true;
}

template <typename TapeType>
void Tape<TapeType>::ReadChunkToTheRight() {
  if (InitFirstChunk()) {
    return;
  }

  ChunksNumber current_chunk_number = current_chunk_.GetChunkNumber();
  current_chunk_.ReadNewChunk(
      stream_from_, current_chunk_number + 1,
      current_chunk_number + 1 == chunks_info_.chunks_number_ - 1
          ? chunks_info_.last_chunk_size_
          : chunks_info_.max_chunk_size_);
  current_chunk_.MoveToLeftEdge();
}

template <typename TapeType>
void Tape<TapeType>::ReadChunkToTheLeft() {
  stream_from_.seekp(0);
  stream_from_.seekg(0);

  ChunksNumber current_chunk_number = current_chunk_.GetChunkNumber();

  for (ChunkSize i = 0;
       i < (current_chunk_number - 1) * chunks_info_.max_chunk_size_; i++) {
    TapeType element;
    stream_from_ >> element;
  }

  current_chunk_.ReadNewChunk(stream_from_, current_chunk_number - 1,
                              chunks_info_.max_chunk_size_);
  current_chunk_.MoveToRightEdge();
}

template <typename TapeType>
void Tape<TapeType>::RewriteFromTo(std::fstream &from, std::fstream &to) {
  TapeType element;
  while (from >> element) {
    to << element << ' ';
  }
}

template <typename TapeType>
void Tape<TapeType>::PutElementInNewChunk(std::fstream &to, ChunkSize size,
                                          ChunkSize pos, TapeType element) {
  current_chunk_.ReadNewChunk(stream_from_, current_chunk_.GetChunkNumber(),
                              size);
  current_chunk_.PutElementInArrayByPos(element, pos);
  current_chunk_.PrintChunk(to);
}

template <typename TapeType>
ChunkSize Tape<TapeType>::CalculateChunkSize(MemorySize memory, TapeSize size) {
  return std::min(memory / kDivider, size);
}

template <typename TapeType>
void Tape<TapeType>::ReadAndWriteNewChunk(std::fstream &from, std::fstream &to,
                                          ChunksNumber new_chunk_number,
                                          ChunkSize new_size) {
  current_chunk_.ReadNewChunk(from, new_chunk_number, new_size);
  current_chunk_.PrintChunk(to);
}
}  // namespace tape
