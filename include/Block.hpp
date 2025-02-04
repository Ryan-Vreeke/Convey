#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <iostream>
#include <regex>
#include <string>
#include <unordered_map>

#include "json.hpp"

class Block {
  std::string block_text;

 public:
  std::unordered_map<std::string, std::string> variables;

  Block(const std::string& text) : block_text(text) {}
  Block() : block_text("") {}
  Block(const Block&) = default;
  Block(Block&&) = default;
  Block& operator=(const Block&) = default;
  Block& operator=(Block&&) = default;

  std::string str() {
    for (auto& [key, val] : variables) {
      replaceVariable(key, val);
    }
    return block_text;
  }

  void replaceVariable(const std::string& var, const std::string& value) {
    std::string pattern = R"(\{\{\s*)" + var + R"(\s*\}\})";
    std::regex regexPattern(pattern);

    std::cout << block_text << std::endl;

    block_text = std::regex_replace(block_text, regexPattern, value);
  }

  void replaceVariable(nlohmann::json json) {
    for (auto& [key, value] : json.items()) {
      if (value.is_object()) {
        std::string parentKey = key;
        parseValue(value, parentKey);
      } else {
        replaceVariable(key, asString(value));
      }
    }
  }

 private:
  void parseValue(nlohmann::json json, std::string out) {
    for (auto& [key, value] : json.items()) {
      if (value.is_object()) {
        parseValue(value, out + "." + key);
      } else {
        replaceVariable(out + "." + key, asString(value));
      }
    }
  }

  std::string asString(nlohmann::json value) {
    if (value.is_number_integer() || value.is_number_unsigned()) {
      return std::to_string(value.get<int>());
    } else if (value.is_number_float()) {
      return std::to_string(value.get<double>());
    } else if (value.is_boolean()) {
      return value.get<bool>() ? "true" : "false";
    } else {
      return value.get<std::string>();
    }
  }
};

#endif
