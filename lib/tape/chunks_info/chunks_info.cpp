#include "chunks_info.hpp"

namespace tape {
ChunksInfo::ChunksInfo(ChunkSize chunk_size, TapeSize tape_size) {
  max_chunk_size_ = chunk_size;
  chunks_number_ = (tape_size - 1) / max_chunk_size_ + 1;
  last_chunk_size_ = tape_size % max_chunk_size_ == 0
                         ? max_chunk_size_
                         : tape_size % max_chunk_size_;
}
}  // namespace tape
