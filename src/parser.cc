// Copyright 2012 Eugen Sawin <sawine@me73.com>
#include "./parser.h"
#include <cassert>
#include <fstream>
#include <sstream>

using std::string;
using std::ifstream;
using std::stringstream;
using std::vector;
using std::set;
using std::min;

namespace bush {

const char* Parser::kNumbers = "-0123456789";
const char* Parser::kWhitespace = "\n\r\t ";

size_t Parser::FileSize(const string& path) {
  ifstream stream(path.c_str());
  size_t size = 0;
  if (stream.good()) {
    stream.seekg(0, std::ios::end);
    size = stream.tellg();
    stream.seekg(0, std::ios::beg);
  }
  stream.close();
  return size;
}

void Parser::CollectNumerals(const string& content, vector<int>* numerals) {
  assert(numerals);
  size_t pos = content.find_first_of(kNumbers);
  while (pos != string::npos) {
    size_t end = content.find_first_not_of(kNumbers, pos);
    if (end == string::npos) {
      numerals->push_back(Convert<int>(content.substr(pos)));
    } else {
      numerals->push_back(Convert<int>(content.substr(pos, end - pos)));
    }
    pos = content.find_first_of(kNumbers, end);
  }
}

vector<string> Parser::Split(const string& content) {
  vector<string> items;
  size_t pos = content.find_first_not_of(kWhitespace);
  while (pos != string::npos) {
    size_t end = content.find_first_of(kWhitespace, pos);
    if (end == string::npos) {
      // Last item found.
      items.push_back(content.substr(pos));
    } else {
      // Item found.
      items.push_back(content.substr(pos, end - pos));
    }
    pos = content.find_first_not_of(kWhitespace, end);
  }
  return items;
}

string Parser::StripWhitespace(const string& content) {
  size_t beg = content.find_first_not_of(kWhitespace);
  size_t end = content.find_last_not_of(kWhitespace);
  if (beg == end) {
    return "";
  }
  return content.substr(beg, end - beg + 1);
}

Parser::Parser(const string& path)
    : path_(path) {}

Vote Parser::ParseVote() {
  if (content_.size() == 0) {
    ReadAll();
  }
  assert(content_.size());
  vector<int> header;
  size_t pos = content_.find("\n");
  assert(pos != string::npos);
  CollectNumerals(content_.substr(0, pos), &header);
  assert(header.size() == 2);
  Vote vote(header[0], header[1]);
  return vote;
}

void Parser::ReadAll() {
  size_t file_size = FileSize(path_);
  char* buffer = new char[file_size + 1];
  buffer[file_size] = '\0';
  ifstream stream(path_);
  assert(stream.good());
  stream.read(buffer, file_size);
  assert(static_cast<size_t>(stream.gcount()) == file_size);
  content_ = string(buffer);
}

}  // namespace bush
