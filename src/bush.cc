// Copyright 2012 Eugen Sawin <sawine@me73.com>
#include <gflags/gflags.h>
#include <cassert>
#include <iostream>
#include <vector>
#include <unordered_set>
#include "./clock.h"
#include "./profiler.h"
#include "./parser.h"
#include "./vote.h"
#include "./plurality-system.h"
#include "./borda-system.h"

using std::cout;
using std::endl;
using std::string;
using std::unordered_set;
using std::vector;
using base::Clock;
using base::Profiler;
using bush::Parser;
using bush::Vote;
using bush::Plurality;
using bush::Borda;

// Command-line flag for verbose output.
DEFINE_bool(verbose, false, "Verbose output");

// Command-line flag for brief output.
DEFINE_bool(brief, true, "Brief output, outputs only the strategic preference");

// Command-line flag for execution time limit.
DEFINE_int32(timelimit, 300, "Execution time limit in seconds");

// The command-line usage text.
const string kUsage =  // NOLINT
  string("Usage:\n") +
         "  $ bush <preferences> <voter id> <voting system>\n" +
         "  <preferences> is a preferences file in the vote format\n" +
         "  <voter id> is the id (index) of the selected voter\n" +
         "  <voting system> is one of these: plurality, irv, borda";

int main(int argc, char* argv[]) {
  google::SetUsageMessage(kUsage);
  // Parse command line flags and remove them from the argc and argv.
  google::ParseCommandLineFlags(&argc, &argv, true);
  if (argc != 4) {
    cout << "Wrong argument number provided, use -help for help.\n"
         << kUsage << "\n";
    return 1;
  } else if (!Parser::FileSize(argv[1])) {
    cout << "File " << argv[1] << " is empty or does not exist.\n";
    return 1;
  }

  const string input_path = argv[1];
  const int selected_voter_id = Parser::Convert<int>(argv[2]);
  const string voting_system = argv[3];
  Parser parser(input_path);
  Vote vote = parser.ParseVote();

  unordered_set<string> voting_systems({"plurality", "irv", "borda"});

  if (selected_voter_id >= vote.num_voters()) {
    cout << "Invalid selected voter id " << selected_voter_id << ".\n";
    return 1;
  } else if (voting_systems.find(voting_system) == voting_systems.end()) {
    cout << "Invalid voting system " << voting_system << ".\n";
    return 1;
  }

  if (!FLAGS_brief || FLAGS_verbose) {
    cout << "File: " << input_path << "\n"
         << "Selected voter: " << selected_voter_id << "\n"
         << "Voting system: " << voting_system << "\n";
    if (FLAGS_verbose) {
      cout << "Vote input:\n" << vote.str() << "\n";
    }
  }
 
  if (voting_system == "plurality") {
    // Plurality voting-system.
    Plurality system(vote, selected_voter_id);
    if (FLAGS_verbose) {
      cout << "Ratings: ";
      const vector<int>& ratings = system.base_ratings();
      for (auto it = ratings.cbegin(), end = ratings.cend();
           it != end; ++it) {
        if (it != ratings.cbegin()) {
          cout << " ";
        }
        cout << *it;
      }
      cout << "\n";
    }
    const vector<int>& strategic_pref = system.strategic_preference();
    for (auto it = strategic_pref.cbegin(), end = strategic_pref.cend();
         it != end; ++it) {
      if (it != strategic_pref.cbegin()) {
        cout << " ";
      }
      cout << *it;
    }
  } else if (voting_system == "borda") {
    // Borda count voting-system.
    Borda system(vote, selected_voter_id);
    if (FLAGS_verbose) {
      cout << "Ratings: ";
      const vector<int>& ratings = system.base_ratings();
      for (auto it = ratings.cbegin(), end = ratings.cend();
           it != end; ++it) {
        if (it != ratings.cbegin()) {
          cout << " ";
        }
        cout << *it;
      }
      cout << "\n";
    }
    const vector<int>& strategic_pref = system.strategic_preference();
    for (auto it = strategic_pref.cbegin(), end = strategic_pref.cend();
         it != end; ++it) {
      if (it != strategic_pref.cbegin()) {
        cout << " ";
      }
      cout << *it;
    }
  }
  cout << endl;
  return 0;
}
