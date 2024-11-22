#include "../lib/tape/sorter/tape_sorter.hpp"

#include <gtest/gtest.h>

#include "../lib/config_reader/simple_yaml_reader.hpp"

TEST(TapeStructure, EmptyTapeTest) {
  const std::filesystem::path path = "./resources/config0.yaml";

  config_reader::SimpleYamlReader config(path);
  config.ReadConfig();

  const size_t size = config["N"].AsInt32();
  const size_t memory = config["M"].AsInt32();

  const std::chrono::milliseconds delay_for_read =
      config["delay_for_read"].AsMilliseconds();
  const std::chrono::milliseconds delay_for_write =
      config["delay_for_write"].AsMilliseconds();
  const std::chrono::milliseconds delay_for_shift =
      config["delay_for_shift"].AsMilliseconds();

  const std::filesystem::path path_in = config["path_in"].AsPath();
  const std::filesystem::path path_out = config["path_out"].AsPath();

  tape::Tape<int32_t> tape_in(path_in, size, memory, delay_for_read,
                              delay_for_write, delay_for_shift);
  tape::Tape<int32_t> tape_out(path_out, delay_for_read, delay_for_write,
                               delay_for_shift);

  tape::TapeSorter sorter(tape_in, tape_out);

  sorter.Sort();

  std::ifstream fin(path_out);

  std::string result;
  std::getline(fin, result);

  const std::string kExpected = "";
  EXPECT_EQ(result, kExpected);
}

TEST(TapeStructure, OneElementTapeTest) {
  const std::filesystem::path path = "./resources/config4.yaml";

  config_reader::SimpleYamlReader config(path);
  config.ReadConfig();

  const size_t size = config["N"].AsInt32();
  const size_t memory = config["M"].AsInt32();

  const std::chrono::milliseconds delay_for_read =
      config["delay_for_read"].AsMilliseconds();
  const std::chrono::milliseconds delay_for_write =
      config["delay_for_write"].AsMilliseconds();
  const std::chrono::milliseconds delay_for_shift =
      config["delay_for_shift"].AsMilliseconds();

  const std::filesystem::path path_in = config["path_in"].AsPath();
  const std::filesystem::path path_out = config["path_out"].AsPath();

  tape::Tape<int32_t> tape_in(path_in, size, memory, delay_for_read,
                              delay_for_write, delay_for_shift);
  tape::Tape<int32_t> tape_out(path_out, delay_for_read, delay_for_write,
                               delay_for_shift);

  tape::TapeSorter sorter(tape_in, tape_out);

  sorter.Sort();

  std::ifstream fin(path_out);

  std::string result;
  std::getline(fin, result);

  const std::string kExpected = "14524647 ";
  EXPECT_EQ(result, kExpected);
}

TEST(TapeStructure, TestFile1) {
  const std::filesystem::path path = "./resources/config1.yaml";

  config_reader::SimpleYamlReader config(path);
  config.ReadConfig();

  const size_t size = config["N"].AsInt32();
  const size_t memory = config["M"].AsInt32();

  const std::chrono::milliseconds delay_for_read =
      config["delay_for_read"].AsMilliseconds();
  const std::chrono::milliseconds delay_for_write =
      config["delay_for_write"].AsMilliseconds();
  const std::chrono::milliseconds delay_for_shift =
      config["delay_for_shift"].AsMilliseconds();

  const std::filesystem::path path_in = config["path_in"].AsPath();
  const std::filesystem::path path_out = config["path_out"].AsPath();

  tape::Tape<int32_t> tape_in(path_in, size, memory, delay_for_read,
                              delay_for_write, delay_for_shift);
  tape::Tape<int32_t> tape_out(path_out, delay_for_read, delay_for_write,
                               delay_for_shift);

  tape::TapeSorter sorter(tape_in, tape_out);

  sorter.Sort();

  std::ifstream fin(path_out);

  std::string result;
  std::getline(fin, result);

  const std::string kExpected =
      "5 5 11 22 22 33 44 54 55 66 77 88 92 99 111 122 144 148 155 12345 ";
  EXPECT_EQ(result, kExpected);
}

TEST(TapeStructure, TestFile2) {
  const std::filesystem::path path = "./resources/config2.yaml";

  config_reader::SimpleYamlReader config(path);
  config.ReadConfig();

  const size_t size = config["N"].AsInt32();
  const size_t memory = config["M"].AsInt32();

  const std::chrono::milliseconds delay_for_read =
      config["delay_for_read"].AsMilliseconds();
  const std::chrono::milliseconds delay_for_write =
      config["delay_for_write"].AsMilliseconds();
  const std::chrono::milliseconds delay_for_shift =
      config["delay_for_shift"].AsMilliseconds();

  const std::filesystem::path path_in = config["path_in"].AsPath();
  const std::filesystem::path path_out = config["path_out"].AsPath();

  tape::Tape<int32_t> tape_in(path_in, size, memory, delay_for_read,
                              delay_for_write, delay_for_shift);

  tape::Tape<int32_t> tape_out(path_out, delay_for_read, delay_for_write,
                               delay_for_shift);
  tape::TapeSorter sorter(tape_in, tape_out);

  sorter.Sort();

  std::ifstream fin(path_out);

  std::string result;
  std::getline(fin, result);

  const std::string kExpected =
      "5 5 11 22 22 33 44 54 55 66 77 88 92 99 111 122 144 148 155 12345 ";
  EXPECT_EQ(result, kExpected);
}

TEST(TapeStructure, TestFile3) {
  const std::filesystem::path path = "./resources/config3.yaml";

  config_reader::SimpleYamlReader config(path);
  config.ReadConfig();

  const size_t size = config["N"].AsInt32();
  const size_t memory = config["M"].AsInt32();

  const std::chrono::milliseconds delay_for_read =
      config["delay_for_read"].AsMilliseconds();
  const std::chrono::milliseconds delay_for_write =
      config["delay_for_write"].AsMilliseconds();
  const std::chrono::milliseconds delay_for_shift =
      config["delay_for_shift"].AsMilliseconds();

  const std::filesystem::path path_in = config["path_in"].AsPath();
  const std::filesystem::path path_out = config["path_out"].AsPath();

  tape::Tape<int32_t> tape_in(path_in, size, memory, delay_for_read,
                              delay_for_write, delay_for_shift);

  tape::Tape<int32_t> tape_out(path_out, delay_for_read, delay_for_write,
                               delay_for_shift);
  tape::TapeSorter sorter(tape_in, tape_out);

  sorter.Sort();

  std::ifstream fin(path_out);

  std::string result;
  std::getline(fin, result);

  const std::string kExpected =
      "-21435246 -6374869 -675162 -76854 -48130 -9876"
      " -6254 0 6 865 34578 56342 84613 87645 235646"
      " 314526 358128 3481364 5343127 5463276 7231462"
      " 8125637 8745637 56142738 61432576 659298456 ";
  EXPECT_EQ(result, kExpected);
}