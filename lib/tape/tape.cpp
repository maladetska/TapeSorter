#include "tape.hpp"

namespace tape {

template <typename TapeType>
Tape<TapeType>::Tape(const std::filesystem::path &file, TapeSize size,
                     MemorySize memory_size, Delays delays)
    : tape_location_(file),
      size_(size),
      memory_size_(memory_size),
      delays_(delays) {
  chunks_info_ = ChunksInfo(CalculateChunkSize(memory_size_, size_), size_);
  current_chunk_ = Chunk<TapeType>(delays_, 0, chunks_info_.max_size_chunk_);
  stream_from_.open(tape_location_);
}

template <typename TapeType>
Tape<TapeType>::Tape(const std::filesystem::path &file, TapeSize size,
                     MemorySize memory_size,
                     std::chrono::milliseconds delay_for_read,
                     std::chrono::milliseconds delay_for_write,
                     std::chrono::milliseconds delay_for_shift)
    : Tape(file, size, memory_size,
           Delays(delay_for_read, delay_for_write, delay_for_shift)) {}

template <typename TapeType>
Tape<TapeType>::Tape(const std::filesystem::path &file,
                     std::chrono::milliseconds delay_for_read,
                     std::chrono::milliseconds delay_for_write,
                     std::chrono::milliseconds delay_for_shift)
    : tape_location_(file),
      delays_(delay_for_read, delay_for_write, delay_for_shift) {
  stream_from_.open(tape_location_);
}

template <typename TapeType>
Tape<TapeType>::Tape(const std::filesystem::path &file, TapeSize size,
                     MemorySize memory_size)
    : Tape(file, size, memory_size, std::chrono::milliseconds::zero(),
           std::chrono::milliseconds::zero(),
           std::chrono::milliseconds::zero()) {}

template <typename TapeType>
Tape<TapeType>::Tape(const Tape &other)
    : tape_location_(other.tape_location_),
      delays_(other.delays_),
      size_(other.size_),
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

// template <typename TapeType>
// Tape<TapeType>::Tape(Tape &&) noexcept {
//   *this = std::move(other);
// }

template <typename TapeType>
Tape<TapeType>::~Tape() {
  stream_from_.close();
}

template <typename TapeType>
void Tape<TapeType>::Put(const TapeType &element) {
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
    PutNumberInNewChunk(tmp_to, chunks_info_.max_size_chunk_, current_pos,
                        element);
  } else if (current_chunk_number == chunks_info_.chunks_number_ - 1) {
    for (TapeSize i = 0; i < chunks_info_.chunks_number_ - 1; i++) {
      ReadAndWriteNewChunk(stream_from_, tmp_to, i,
                           chunks_info_.max_size_chunk_);
    }
    PutNumberInNewChunk(tmp_to, chunks_info_.last_size_chunk_, current_pos,
                        element);
  } else if (current_chunk_number == 0) {
    PutNumberInNewChunk(tmp_to, chunks_info_.max_size_chunk_, current_pos,
                        element);
    for (TapeSize i = 1; i < chunks_info_.chunks_number_ - 1; i++) {
      ReadAndWriteNewChunk(stream_from_, tmp_to, i,
                           chunks_info_.max_size_chunk_);
    }
    ReadAndWriteNewChunk(stream_from_, tmp_to, chunks_info_.chunks_number_ - 1,
                         chunks_info_.last_size_chunk_);
  } else {
    for (TapeSize i = 0; i < current_chunk_number; i++) {
      ReadAndWriteNewChunk(stream_from_, tmp_to, i,
                           chunks_info_.max_size_chunk_);
    }
    PutNumberInNewChunk(tmp_to, chunks_info_.max_size_chunk_, current_pos,
                        element);
    for (TapeSize i = current_chunk_number + 1;
         i < chunks_info_.chunks_number_ - 1; i++) {
      ReadAndWriteNewChunk(stream_from_, tmp_to, i,
                           chunks_info_.max_size_chunk_);
    }
    ReadAndWriteNewChunk(stream_from_, tmp_to, chunks_info_.chunks_number_ - 1,
                         chunks_info_.last_size_chunk_);
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
    ReadAndWriteNewChunk(tmp_to, stream_from_, i, chunks_info_.max_size_chunk_);
  }
  ReadAndWriteNewChunk(tmp_to, stream_from_, chunks_info_.chunks_number_ - 1,
                       chunks_info_.last_size_chunk_);

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

  if (current_chunk_.IsPossibleTakeRightNumber(chunks_info_.chunks_number_)) {
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
TapeSize Tape<TapeType>::GetChunksNumber() const {
  return chunks_info_.chunks_number_;
}

template <typename TapeType>
ChunkSize Tape<TapeType>::GetMaxChunkSize() const {
  return chunks_info_.max_size_chunk_;
}

template <typename TapeType>
ChunkSize Tape<TapeType>::GetMinChunkSize() const {
  return chunks_info_.last_size_chunk_;
}

template <typename TapeType>
std::vector<TapeType> Tape<TapeType>::GetChunkElements() const {
  return current_chunk_.GetChunkElements();
}

template <typename TapeType>
TapeType Tape<TapeType>::GetCurrentElement() {
  if (InitFirstChunk()) {
    current_chunk_.MoveToLeftEdge();
  }

  return current_chunk_.GetCurrentElement();
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
  current_chunk_.ReadNewChunk(stream_from_, 0, chunks_info_.max_size_chunk_);
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
          ? chunks_info_.last_size_chunk_
          : chunks_info_.max_size_chunk_);
  current_chunk_.MoveToLeftEdge();
}

template <typename TapeType>
void Tape<TapeType>::ReadChunkToTheLeft() {
  stream_from_.seekp(0);
  stream_from_.seekg(0);

  ChunksNumber current_chunk_number = current_chunk_.GetChunkNumber();

  for (ChunkSize i = 0;
       i < (current_chunk_number - 1) * chunks_info_.max_size_chunk_; i++) {
    TapeType element;
    stream_from_ >> element;
  }

  current_chunk_.ReadNewChunk(stream_from_, current_chunk_number - 1,
                              chunks_info_.max_size_chunk_);
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
