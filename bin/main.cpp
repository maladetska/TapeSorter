#include <iostream>

#include "lib/config_reader/simple_yaml_reader.hpp"
#include "lib/tape/sorter/tape_sorter.hpp"

using namespace std::chrono_literals;

int main(int argc, char *argv[]) {
  std::filesystem::path path = argv[1];

  config_reader::SimpleYamlReader config(path);
  config.ReadConfig();

  const uint32_t size = config["N"].AsInt32();
  const uint32_t memory = config["M"].AsInt32();

  const std::chrono::milliseconds delay_for_read =
      config["delay_for_read"].AsMilliseconds();
  const std::chrono::milliseconds delay_for_write =
      config["delay_for_write"].AsMilliseconds();
  const std::chrono::milliseconds delay_for_shift =
      config["delay_for_shift"].AsMilliseconds();

  const std::filesystem::path path_in = config["path_iån"].AsPath();
  const std::filesystem::path path_out = config["path_out"].AsPath();

  tape::Tape<int32_t> tape_in{
      path_in, size, memory, delay_for_read, delay_for_write, delay_for_shift};
  tape::Tape<int32_t> tape_out{path_out, delay_for_read, delay_for_write,
                               delay_for_shift};

  tape::TapeSorter sorter{tape_in, tape_out};

  sorter.Sort();

  return 0;
}
