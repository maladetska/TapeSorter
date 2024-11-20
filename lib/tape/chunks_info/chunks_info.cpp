#include "chunks_info.hpp"

namespace tape {
ChunksInfo::ChunksInfo(ChunkSize chunk_size, TapeSize tape_size) {
  max_size_chunk_ = chunk_size;
  chunks_number_ = (tape_size - 1) / max_size_chunk_ + 1;
  last_size_chunk_ = tape_size % max_size_chunk_ == 0
                         ? max_size_chunk_
                         : tape_size % max_size_chunk_;
}
}  // namespace tape
