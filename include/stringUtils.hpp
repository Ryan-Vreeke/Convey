#ifndef STRINGUTILS_HPP
#define STRINGUTILS_HPP

#include <algorithm>
#include <string>
#include <vector>

namespace Utils {

inline bool ends_with(const std::string &str, const std::string &suffix) {
  return str.size() >= suffix.size() &&
         str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

inline std::vector<std::string> splitString(const std::string &str, const std::string &delimiter) {
  std::vector<std::string> tokens;
  size_t start = 0;
  size_t end = str.find(delimiter);

  while (end != std::string::npos) {
    tokens.push_back(str.substr(start, end - start));
    start = end + delimiter.length();
    end = str.find(delimiter, start);
  }

  tokens.push_back(str.substr(start));

  return tokens;
}

template <typename T>
bool contains(const std::vector<T> &v, T x) {
  return std::find(v.begin(), v.end(), x) != v.end();
}

inline bool contains(const std::string &str, const std::string &token) {
  return str.find(token) != std::string::npos;
}

}  // namespace Utils
//
#endif
