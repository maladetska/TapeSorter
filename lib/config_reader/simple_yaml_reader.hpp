#pragma once

#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>

namespace config_reader {
class SimpleYamlReader {
 public:
  explicit SimpleYamlReader(const char *path);
  explicit SimpleYamlReader(const std::string &path);

  void ReadConfig();

  class Value {
   public:
    Value() = default;

    explicit Value(std::string value);

    [[nodiscard]] std::chrono::milliseconds AsMilliseconds() const;
    [[nodiscard]] std::chrono::seconds AsSeconds() const;
    [[nodiscard]] std::filesystem::path AsPath() const;
    [[nodiscard]] std::string AsString() const;
    [[nodiscard]] int32_t AsInt32() const;
    [[nodiscard]] long long AsLongLong() const;
    [[nodiscard]] long AsLong() const;
    [[nodiscard]] double AsDouble() const;

   private:
    std::string value_;
  };

  Value operator[](const std::string &field_name);

 private:
  std::filesystem::path path_;
  std::unordered_map<std::string, Value> fields_;
};
}  // namespace config_reader
