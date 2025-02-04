#ifndef TEMPLAR_HPP
#define TEMPLAR_HPP

#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>

#include "Block.hpp"
#include "json.hpp"

namespace Convey {

class Templar {
  const std::string page_dir;
  std::unordered_map<std::string, Block> block_set;

 public:
  Templar(const std::string& pages) : page_dir(pages) { buildBlockSet(); }

  void addVariables(const std::string& block_name, const std::string& var_name, const std::string& value) {
    auto block = block_set[block_name];
    block.variables[var_name] = value;
  }

  void addVariables(const std::string& block_name, nlohmann::json json_var) {
    auto block = block_set[block_name];
    block.replaceVariable(json_var);
  }

  std::string prepFile(const std::string& file) {
    auto content = readFile(file);
    auto blocks = findUsedBlocks(content);

    for (const auto& block_name : blocks) {
      replaceBlock(content, block_name.substr(1));
    }

    return content;
  }

 private:
  void replaceBlock(std::string& html, const std::string& block_name) {
    std::string pattern = R"(\{\{\.)" + block_name + R"(\s*\}\})";
    std::regex block_regex(pattern);
    if (!block_set.contains(block_name) || !std::regex_search(html, block_regex)) return;

    std::cout << block_name << std::endl;
    html = std::regex_replace(html, block_regex, block_set[block_name].str());
  }

  std::vector<std::string> findUsedBlocks(const std::string& html_content) {
    std::regex block_pattern(R"(\{\{\s*(\.\w+)\s*\}\})");
    std::vector<std::string> matches;

    auto begin = std::sregex_iterator(html_content.begin(), html_content.end(), block_pattern);
    auto end = std::sregex_iterator();
    for (auto it = begin; it != end; ++it) {
      auto match = *it;
      matches.push_back(match[1].str());
    }

    return matches;
  }

  void buildBlockSet() {
    for (const auto& entry : std::filesystem::directory_iterator(page_dir)) {
      if (!std::filesystem::is_regular_file(entry.status())) continue;
      if (entry.path().extension() != ".html" && entry.path().extension() != ".htmx") continue;

      std::string page_content = readFile(entry.path());
      extractBlocks(page_content);
    }
  }

  void extractBlocks(const std::string& html_content) {
    std::regex block_pattern(R"(\{\{\s*block\s+(\w+)\s*\}\}([\w\W]*?)\{\{\s*end\s*\}\})");
    auto begin = std::sregex_iterator(html_content.begin(), html_content.end(), block_pattern);
    auto end = std::sregex_iterator();

    for (auto it = begin; it != end; ++it) {
      auto match = *it;
      auto block_name = match[1].str();
      Block block(match[2].str());

      block_set[block_name] = block;
    }
  }

  std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    std::stringstream buffer;

    if (!file.is_open()) return "";

    buffer << file.rdbuf();

    file.close();
    return buffer.str();
  }
};

}  // namespace Convey

#endif
