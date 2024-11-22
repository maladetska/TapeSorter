#include "chunks_info.hpp"

namespace tape {
ChunksInfo::ChunksInfo(ChunkSize chunk_size, TapeSize tape_size)
    : max_chunk_size_(chunk_size),
      chunks_number_(chunk_size ? (tape_size - 1) / chunk_size + 1 : 0),
      last_chunk_size_(chunk_size ? (tape_size % chunk_size == 0
                                         ? chunk_size
                                         : tape_size % chunk_size)
                                  : 0) {}
}  // namespace tape
