// Copyright 2012 Eugen Sawin <sawine@me73.com>
#ifndef SRC_PARSER_H_
#define SRC_PARSER_H_

#include <vector>
#include <set>
#include <string>
#include "./vote.h"

namespace bush {

class Parser {
 public:
  static const char* kNumbers;
  static const char* kWhitespace;

  // Converts a value from one type to another.
  template<typename To, typename From>
  static To Convert(const From& from) {
    std::stringstream ss;
    ss << from;
    To to;
    ss >> to;
    return to;
  }

  // Returns the file size of given path in bytes.
  static size_t FileSize(const std::string& path);

  // Splits the given string at whitespaces and converts elements to int.
  static std::vector<int> SplitInts(const std::string& content);

  // Initialised the parser with given path.
  explicit Parser(const std::string& path);

  // Parses a vote file and returns its representative data structure.
  Vote ParseVote();

 private:
  // Reads the whole file into parser cache.
  void ReadAll();

  std::string path_;
  std::string content_;
};

}  // namespace bush
#endif  // SRC_PARSER_H_
