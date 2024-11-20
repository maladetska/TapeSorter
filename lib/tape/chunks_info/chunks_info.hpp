#pragma once

#include "../chunk/chunk.hpp"
#include "../tape_interface.hpp"

namespace tape {
struct ChunksInfo {
  ChunksInfo() = default;
  ChunksInfo(ChunkSize chunk_size, TapeSize tape_size);

  ChunksNumber chunks_number_{};
  ChunkSize max_size_chunk_{};
  ChunkSize last_size_chunk_{};
};
}  // namespace tape
