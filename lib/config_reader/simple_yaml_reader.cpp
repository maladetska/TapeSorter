#include "simple_yaml_reader.hpp"

namespace config_reader {
SimpleYamlReader::SimpleYamlReader(const char *path) : path_(path) {}

SimpleYamlReader::SimpleYamlReader(const std::string &path) : path_(path) {}

void SimpleYamlReader::ReadConfig() {
  std::ifstream fin(path_);

  if (!fin.is_open()) {
    throw std::runtime_error("Could not open config file_");
  }

  std::string field_name;
  std::string dummy;
  while (fin >> field_name) {
    std::string value;
    fin >> value;

    field_name.pop_back();
    fields_[field_name] = Value(value);
  }

  fin.close();
}

SimpleYamlReader::Value SimpleYamlReader::operator[](
    const std::string &field_name) {
  return fields_[field_name];
}

SimpleYamlReader::Value::Value(std::string value) : value_(std::move(value)) {}

[[nodiscard]] std::chrono::milliseconds
SimpleYamlReader::Value::AsMilliseconds() const {
  return std::chrono::milliseconds(std::stol(value_));
}

[[nodiscard]] std::chrono::seconds SimpleYamlReader::Value::AsSeconds() const {
  return std::chrono::seconds(std::stol(value_));
}

[[nodiscard]] std::filesystem::path SimpleYamlReader::Value::AsPath() const {
  return value_;
}

[[nodiscard]] std::string SimpleYamlReader::Value::AsString() const {
  return value_;
}

[[nodiscard]] int SimpleYamlReader::Value::AsInt32() const {
  return std::stoi(value_);
}

[[nodiscard]] long long SimpleYamlReader::Value::AsLongLong() const {
  return std::stoll(value_);
}

[[nodiscard]] long SimpleYamlReader::Value::AsLong() const {
  return std::stol(value_);
}

[[nodiscard]] double SimpleYamlReader::Value::AsDouble() const {
  return std::stod(value_);
}
}  // namespace config_reader
