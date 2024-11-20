#pragma once

#include <algorithm>

#include "../tape.hpp"

namespace tape {

template <typename TapeType>
class TapeSorter {
 public:
  TapeSorter() = default;

  TapeSorter(Tape<TapeType> &tape_in, Tape<TapeType> &tape_out);

  ~TapeSorter() = default;

  void Sort();

 private:
  void Split(std::filesystem::path &path, std::vector<Tape<TapeType>> &tapes);
  void MakeSplitTape(std::filesystem::path &path, Tape<TapeType> &tape,
                     TapeSize tape_number);

  static Tape<TapeType> Merge(std::filesystem::path path, Tape<TapeType> &tape0,
                              Tape<TapeType> &tape1);
  static std::pair<bool, bool> MergeOneChunk(Tape<TapeType> &tape_result,
                                             Tape<TapeType> &tape0,
                                             Tape<TapeType> &tape1, bool end0,
                                             bool end1, ChunkSize size);

  void Assembly(TapeSize dir, std::vector<Tape<TapeType>> &tapes);

  static void PutTapeRestToBuffer(Tape<TapeType> &tape,
                                  std::vector<TapeType> &buffer,
                                  ChunkSize size);
  bool static PutNumberInBuffer(Tape<TapeType> &tape,
                                std::vector<TapeType> &buffer, bool &end);

  Tape<TapeType> tape_in_;
  Tape<TapeType> tape_out_;

  const std::filesystem::path dir_for_tmp_tapes_ = "./tmp";
};
}  // namespace tape
