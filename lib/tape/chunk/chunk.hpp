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

  void ReadNewChunk(std::fstream &from, ChunksNumber new_chunk_number,
                    ChunkSize new_size);

  void PutElementInArrayByPos(const TapeType &elem, ChunkSize pos);

  void PrintChunk(std::fstream &to);

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
}  // namespace tape
