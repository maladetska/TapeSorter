#pragma once

#include <algorithm>

#include "../tape.hpp"

namespace tape {

////////////////////////////////////////////////////////////////////////////////
/// \brief A class for sorting the tape.
/// \tparam TapeType type of elements in tapes.
////////////////////////////////////////////////////////////////////////////////
template <typename TapeType>
class TapeSorter {
 public:
  //////////////////////////////////////////////////////////////////////////////
  /// \brief TapeSorter default constructor.
  //////////////////////////////////////////////////////////////////////////////
  TapeSorter() = default;

  //////////////////////////////////////////////////////////////////////////////
  /// \brief TapeSorter constructor.
  ///
  /// \param tape_in tape that needs to be sorted.
  /// \param tape_out tape in which the sorted tape will be recorded.
  //////////////////////////////////////////////////////////////////////////////
  TapeSorter(Tape<TapeType> &tape_in, Tape<TapeType> &tape_out);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief TapeSorter destractor.
  //////////////////////////////////////////////////////////////////////////////
  ~TapeSorter() = default;

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Launch sorting the tape.
  //////////////////////////////////////////////////////////////////////////////
  void Sort();

 private:
  //////////////////////////////////////////////////////////////////////////////
  /// \brief Launch splitting tapes into array of tapes.
  ///
  /// \param path file path where the tapes should be stored.
  /// \param tapes split tapes.
  //////////////////////////////////////////////////////////////////////////////
  void Split(std::filesystem::path &path, std::vector<Tape<TapeType>> &tapes);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Create a new split tape.
  ///
  /// \param path path to the file where new tape will be located.
  /// \param tape new tape.
  /// \param tape_number number of new tape.
  //////////////////////////////////////////////////////////////////////////////
  void MakeSplitTape(std::filesystem::path &path, Tape<TapeType> &tape,
                     ChunksNumber tape_number);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Starting of the assembly of split tapes together.
  ///
  /// \param dir
  /// \param tapes split tapes
  //////////////////////////////////////////////////////////////////////////////
  void Assembly(ChunksNumber dir, std::vector<Tape<TapeType>> &tapes);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Merge two sorted tapes into one sorted tape.
  ///
  /// \param path path to the file of new tape file to which the result is
  /// written.
  /// \param tape0 first sorted tape.
  /// \param tape1 second sorted tape.
  /// \return sorted tape consisting of two introductory tapes.
  //////////////////////////////////////////////////////////////////////////////
  static Tape<TapeType> Merge(std::filesystem::path path, Tape<TapeType> &tape0,
                              Tape<TapeType> &tape1);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Create new sorted chunk from two tapes by merging.
  ///
  /// \param tape_result writing a new chunk to this tape.
  /// \param tape0 first sorted resource tape.
  /// \param tape1 second sorted resource tape.
  /// \param end0 true if the position on the tape0 is the rightmost and the
  /// tape1 is passed to the end.
  /// \param end1 true if the position on the tape1 is the rightmost and the
  /// tape2 is passed to the end.
  /// \param size size of new chunk.
  /// \return new value of end1 and end2 params.
  //////////////////////////////////////////////////////////////////////////////
  static std::pair<bool, bool> MergeOneChunk(Tape<TapeType> &tape_result,
                                             Tape<TapeType> &tape0,
                                             Tape<TapeType> &tape1, bool end0,
                                             bool end1, ChunkSize size);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Put the remaining numbers of the tape in the buffer.
  ///
  /// \param tape tape from which the current number is taken.
  /// \param buffer buffer of numbers.
  /// \param size size that the buffer should have.
  //////////////////////////////////////////////////////////////////////////////
  static void PutTapeRestToBuffer(Tape<TapeType> &tape,
                                  std::vector<TapeType> &buffer,
                                  ChunkSize size);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Put the current element from the tape (indicated by the magnetic
  /// head) in the buffer.
  ///
  /// \param tape tape from which the current number is taken.
  /// \param buffer buffer of numbers.
  /// \param end true if the magnetic head points to the rightmost position of
  /// the tape else false.
  /// \return true if the magnetic head points to the rightmost position of the
  /// tape else false.
  //////////////////////////////////////////////////////////////////////////////
  bool static PutElementInBuffer(Tape<TapeType> &tape,
                                 std::vector<TapeType> &buffer, bool &end);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Tape that needs to be sorted.
  //////////////////////////////////////////////////////////////////////////////
  Tape<TapeType> tape_in_;
  //////////////////////////////////////////////////////////////////////////////
  /// \brief Tape in which the sorted tape will be recorded.
  //////////////////////////////////////////////////////////////////////////////
  Tape<TapeType> tape_out_;

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Directory for storing temporary tapes.
  //////////////////////////////////////////////////////////////////////////////
  const std::filesystem::path dir_for_tmp_tapes_ = "./tmp";
};

template <typename TapeType>
TapeSorter<TapeType>::TapeSorter(Tape<TapeType> &tape_in,
                                 Tape<TapeType> &tape_out)
    : tape_in_(tape_in), tape_out_(tape_out) {}

template <typename TapeType>
void TapeSorter<TapeType>::Sort() {
  std::filesystem::create_directories(dir_for_tmp_tapes_);
  std::filesystem::path tmp_path(dir_for_tmp_tapes_);
  ChunksNumber chunks_number = tape_in_.GetChunksNumber();

  std::vector<Tape<TapeType>> tapes(chunks_number,
                                    Tape<TapeType>{tape_in_.delays_});

  Split(tmp_path, tapes);

  if (chunks_number == 1) {
    tape_out_ = std::move(tapes[0]);
  } else {
    for (ChunksNumber i = chunks_number, j = 1; i != 2;
         i = (i - 1) / 2 + 1, j++) {
      Assembly(j, tapes);
      std::filesystem::path prev(dir_for_tmp_tapes_);
      prev += "/" + std::to_string(j - 1) + "/";
      std::filesystem::remove_all(prev);
    }

    tape_out_ =
        std::move(Merge(tape_out_.GetTapeFilePath(), tapes[0], tapes[1]));
  }
  std::filesystem::remove_all(dir_for_tmp_tapes_);
}

template <typename TapeType>
void TapeSorter<TapeType>::Split(std::filesystem::path &path,
                                 std::vector<Tape<TapeType>> &tapes) {
  path += "/" + std::to_string(0) + "/";
  std::filesystem::create_directories(path);
  ChunksNumber chunks_number = tape_in_.GetChunksNumber();
  for (ChunksNumber i = 0; i < chunks_number; i++) {
    MakeSplitTape(path, tapes[i], i);
  }
}

template <typename TapeType>
void TapeSorter<TapeType>::Assembly(ChunksNumber dir,
                                    std::vector<Tape<TapeType>> &tapes) {
  std::filesystem::path curr_path(dir_for_tmp_tapes_);
  curr_path += "/" + std::to_string(dir) + "/";
  std::filesystem::create_directories(curr_path);

  TapeSize tapes_size = tapes.size();
  std::vector<Tape<TapeType>> new_tapes(
      tapes_size % 2 == 0 ? tapes_size / 2 : (tapes_size / 2 + 1));
  TapeSize i = 0;
  for (TapeSize j = 0; i < tapes_size / 2; i++, j += 2) {
    std::filesystem::path tmp_file = curr_path;
    tmp_file += std::to_string(i) + ".txt";
    new_tapes[i] = std::move(Merge(tmp_file, tapes[j], tapes[j + 1]));
  }
  if (tapes_size % 2 != 0) {
    std::filesystem::path tmp_file = curr_path;
    tmp_file += std::to_string(i) + ".txt";
    std::fstream stream_out(tmp_file, std::fstream::out);
    Tape<TapeType> curr_tape{tapes[tapes_size - 1], tmp_file};
    new_tapes[new_tapes.size() - 1] = curr_tape;
  }
  tapes.clear();
  tapes = new_tapes;
}

template <typename TapeType>
void TapeSorter<TapeType>::MakeSplitTape(std::filesystem::path &path,
                                         Tape<TapeType> &tape,
                                         ChunksNumber tape_number) {
  std::filesystem::path tmp_file = path;
  tmp_file += std::to_string(tape_number) + ".txt";
  std::fstream stream_to(tmp_file, std::fstream::out);

  tape_in_.ReadChunkToTheRight();

  std::vector<TapeType> buffer = tape_in_.GetChunkElements();
  std::sort(buffer.begin(), buffer.end());

  for (TapeType &element : buffer) {
    stream_to << element << ' ';
  }

  stream_to.close();
  Tape<TapeType> result_tape{tmp_file, static_cast<TapeSize>(buffer.size()),
                             static_cast<ChunkSize>(buffer.size())};
  tape = std::move(result_tape);
}

template <typename TapeType>
Tape<TapeType> TapeSorter<TapeType>::Merge(std::filesystem::path path,
                                           Tape<TapeType> &tape0,
                                           Tape<TapeType> &tape1) {
  std::pair<bool, bool> check_ends = {false, false};

  std::fstream result_file_stream(path, std::fstream::out);
  Tape<TapeType> result_tape{path, tape0.GetSize() + tape1.GetSize(),
                             tape0.GetMaxChunkSize()};
  for (ChunksNumber i = 0; i < result_tape.GetChunksNumber() - 1; i++) {
    check_ends =
        MergeOneChunk(result_tape, tape0, tape1, check_ends.first,
                      check_ends.second, result_tape.GetMaxChunkSize());
  }
  MergeOneChunk(result_tape, tape0, tape1, check_ends.first, check_ends.second,
                result_tape.GetMinChunkSize());

  result_file_stream.close();
  tape0.ClearChunkInTape();
  tape1.ClearChunkInTape();

  return result_tape;
}

template <typename TapeType>
std::pair<bool, bool> TapeSorter<TapeType>::MergeOneChunk(
    Tape<TapeType> &tape_result, Tape<TapeType> &tape0, Tape<TapeType> &tape1,
    bool end0, bool end1, ChunkSize size) {
  std::vector<TapeType> buffer;
  if (end0 && !end1) {
    PutTapeRestToBuffer(tape1, buffer, size);
  } else if (end1 && !end0) {
    PutTapeRestToBuffer(tape0, buffer, size);
  } else {
    while (buffer.size() != size) {
      while (buffer.size() != size) {
        if (tape0.ReadCell() < tape1.ReadCell()) {
          if (PutElementInBuffer(tape0, buffer, end0)) {
            break;
          }
        } else {
          if (PutElementInBuffer(tape1, buffer, end1)) {
            break;
          }
        }
      }
      if (buffer.size() != size) {
        if (!end0 && end1) {
          PutTapeRestToBuffer(tape0, buffer, size);
        } else if (end0 && !end1) {
          PutTapeRestToBuffer(tape1, buffer, size);
        }
      }
    }
  }

  for (TapeType &element : buffer) {
    tape_result.WriteToCell(element);
    tape_result.MoveLeft();
  }

  return {end0, end1};
}

template <typename TapeType>
void TapeSorter<TapeType>::PutTapeRestToBuffer(Tape<TapeType> &tape,
                                               std::vector<TapeType> &buffer,
                                               ChunkSize size) {
  buffer.push_back(tape.ReadCell());
  while (tape.MoveLeft() && buffer.size() != size) {
    buffer.push_back(tape.ReadCell());
  }
}

template <typename TapeType>
bool TapeSorter<TapeType>::PutElementInBuffer(Tape<TapeType> &tape,
                                              std::vector<TapeType> &buffer,
                                              bool &end) {
  buffer.push_back(tape.ReadCell());
  if (tape.MoveLeft()) {
    return false;
  }
  end = true;
  return true;
}
}  // namespace tape
