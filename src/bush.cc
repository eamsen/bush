// Copyright 2012 Eugen Sawin <esawin@me73.com>
#include <gflags/gflags.h>
#include <unordered_set>
#include <unordered_map>
#include <cassert>
#include <iostream>
#include <vector>
#include "./clock.h"
#include "./profiler.h"
#include "./parser.h"
#include "./vote.h"
#include "./voting-system.h"
#include "./plurality-system.h"
#include "./borda-system.h"
#include "./irv-system.h"

using std::cout;
using std::endl;
using std::string;
using std::unordered_set;
using std::unordered_map;
using std::vector;
using base::Clock;
using base::Profiler;
using bush::Parser;
using bush::Vote;
using bush::VotingSystem;
using bush::Plurality;
using bush::Borda;
using bush::Irv;

#ifdef NIXON_STRATEGY_
static const char* kDefStrategy = "nixon";
#elif defined GANDHI_STRATEGY_
static const char* kDefStrategy = "gandhi";
#else
static const char* kDefStrategy = "bush";
#endif

// Command-line flag for the strategy of the strategic vote calculation.
DEFINE_string(strategy, kDefStrategy, "Voting strategy (bush, nixon, gandhi)");


// Command-line flag for verbose output.
DEFINE_bool(verbose, false, "Verbose output");

// Command-line flag for brief output.
DEFINE_bool(brief, true, "Brief output, outputs only the strategic preference");

// Command-line flag for execution time limit.
DEFINE_int32(timelimit, 10, "Maximum execution time limit in seconds");

// The command-line usage text.
const string kUsage =  // NOLINT
  string("Usage:\n") +
         "  $ bush <preferences> <voter id> <voting system>\n" +
         "  <preferences> is a preferences file in the vote format\n" +
         "  <voter id> is the index of the selected voter\n" +
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
  unordered_map<string, VotingSystem::Strategy>
    strategies({{"bush", VotingSystem::kSimple},
                {"nixon", VotingSystem::kComplete},
                {"gandhi", VotingSystem::kIndependent}});

  if (selected_voter_id >= vote.num_voters()) {
    cout << "Invalid selected voter id " << selected_voter_id << ".\n";
    return 1;
  } else if (voting_systems.find(voting_system) == voting_systems.end()) {
    cout << "Invalid voting system " << voting_system << ".\n";
    return 1;
  } else if (strategies.find(FLAGS_strategy) == strategies.end()) {
    cout << "Invalid voting strategy " << FLAGS_strategy << ".\n";
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

  const VotingSystem::Strategy strategy = strategies[FLAGS_strategy];
  if (voting_system == "plurality") {
    // Plurality voting system.
    Plurality system(vote, selected_voter_id, strategy);

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
    // Borda count voting system.
    Borda system(vote, selected_voter_id, strategy);
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
  } else if (voting_system == "irv") {
    // Instant-runoff voting system.
    Irv system(vote, selected_voter_id, strategy);
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
