// Copyright 2012 Eugen Sawin <sawine@me73.com>
#include <gflags/gflags.h>
#include <cassert>
#include <iostream>
#include <vector>
#include <limits>
#include "./clock.h"
#include "./profiler.h"
#include "./parser.h"

using std::cout;
using std::string;
using std::vector;
using std::numeric_limits;
using base::Clock;
using base::Profiler;
using bush::Parser;
using bush::Vote;

// Command-line flag for verbose output.
DEFINE_bool(verbose, false, "Verbose output");
// Command-line flag for brief output.
DEFINE_bool(brief, false, "Brief output without explicit equilibria");

// The command-line usage text.
const string kUsage =  // NOLINT
  string("Usage:\n") +
         "  $ bush input.nfg\n" +
         "  input.nfg is a strategic game instance" +
         " in the Gambit outcome format";

int main(int argc, char* argv[]) {
  google::SetUsageMessage(kUsage);
  // Parse command line flags and remove them from the argc and argv.
  google::ParseCommandLineFlags(&argc, &argv, true);
  if (argc != 2) {
    cout << "Wrong argument number provided, use -help for help.\n"
         << kUsage << "\n";
    return 1;
  } else if (!Parser::FileSize(argv[1])) {
    cout << "File " << argv[1] << " is empty or does not exist.\n";
    return 1;
  } else if (FLAGS_verbose && FLAGS_brief) {
    cout << "Mutually exclusive flags selected (brief and verbose).\n";
    return 1;
  }

  const string input_path = argv[1];
  Parser parser(input_path);
  Vote vote = parser.ParseVote();
  cout << "File: " << input_path << "\n";
  return 0;
}
