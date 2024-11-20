#pragma once

#include <fstream>

#include "chunk/chunk.hpp"
#include "chunks_info/chunks_info.hpp"
#include "delays/delays.hpp"
#include "tape_interface.hpp"

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
  Tape(const std::filesystem::path& file, TapeSize size, MemorySize memory_size,
       Delays delays);
  Tape(const std::filesystem::path& file, TapeSize size, MemorySize memory_size,
       std::chrono::milliseconds delay_for_read,
       std::chrono::milliseconds delay_for_write,
       std::chrono::milliseconds delay_for_shift);
  Tape(const std::filesystem::path& file,
       std::chrono::milliseconds delay_for_read,
       std::chrono::milliseconds delay_for_write,
       std::chrono::milliseconds delay_for_shift);
  Tape(const std::filesystem::path& file, TapeSize size,
       MemorySize memory_size);

  Tape(const Tape&);
  Tape(const Tape&, std::filesystem::path& path);
  //   Tape& operator=(const Tape&);

  //   Tape(Tape&&) noexcept;
  //   Tape& operator=(Tape&&) noexcept;

  ~Tape();

  // TapeType Read() const override;
  void Put(const TapeType&) override;

  bool MoveRight() override;
  bool MoveLeft() override;

  [[nodiscard]] std::filesystem::path GetTapeFilePath() const;
  [[nodiscard]] TapeSize GetSize() const;
  [[nodiscard]] TapeSize GetChunksNumber() const;
  [[nodiscard]] ChunkSize GetMaxChunkSize() const;
  [[nodiscard]] ChunkSize GetMinChunkSize() const;
  [[nodiscard]] std::vector<TapeType> GetChunkElements() const;
  [[nodiscard]] TapeType GetCurrentElement();

  void ClearChunkInTape();

 private:
  bool InitFirstChunk();

  void ReadChunkToTheRight();
  void ReadChunkToTheLeft();

  static void RewriteFromTo(std::fstream& from, std::fstream& to);

  void PutElementInNewChunk(std::fstream& to, ChunkSize size, ChunkSize pos,
                            TapeType element);

  void ReadAndWriteNewChunk(std::fstream& from, std::fstream& to,
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
}  // namespace tape
