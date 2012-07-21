// Copyright 2012 Eugen Sawin <sawine@me73.com>
#include "./irv-system.h"
#include <cassert>
#include <vector>
#include <algorithm>
#include <queue>
#include <limits>
#include <iostream>
#include "./vote.h"
#include "./random.h"
#include "./clock.h"

using std::vector;
using std::pair;
using std::make_pair;
using std::sort;
using std::priority_queue;
using std::reverse;
using std::swap;
using std::numeric_limits;
using base::RandomGenerator;
using base::Clock;

namespace bush {

struct Compare {
  bool operator()(const pair<int, int>& lhs, const pair<int, int>& rhs) const {
    // Prefer lower rating and lower id (reversed!).
    return lhs.first > rhs.first ||
           (lhs.first == rhs.first && lhs.second > rhs.second);
  }
};

const Clock::Diff Irv::kDefTimeLimit = 10 * Clock::kMicroInSec;

Irv::Irv(const Vote& vote, const int selected_voter_id)
    : vote_(vote),
      selected_voter_(selected_voter_id),
      time_limit_(kDefTimeLimit) {
  Preprocess();
}

void Irv::Preprocess() {
  const Clock beg;

  RandomGenerator<float> random(12);
  const int num_candidates = vote_.num_candidates();
  const int max_utility = num_candidates - 1;

  strategic_preference_ = vote_.preference(selected_voter_);
  vector<int> preference = vote_.preference(selected_voter_);
  int best_utility = Utility(preference);

  while (best_utility < max_utility && Clock() - beg < time_limit_) {
    swap(preference[random.Next() * num_candidates],
         preference[random.Next() * num_candidates]);
    const int utility = Utility(preference);
    if (utility > best_utility) {
      best_utility = utility;
      strategic_preference_.swap(preference);
    }
  }
}

int Irv::FindWinner(const vector<int>& preference) const {
  static const int kInvalidId = -1;

  const int num_voters = vote_.num_voters();
  const int num_candidates = vote_.num_candidates();
  vector<vector<int> > prefs;
  prefs.reserve(num_voters);
  for (int v = 0; v < num_voters; ++v) {
    if (v == selected_voter_) {
      prefs.push_back(preference);
    } else {
      prefs.push_back(vote_.preference(v));
    }
    reverse(prefs.back().begin(), prefs.back().end());
  }
  vector<bool> active(num_candidates, true);
  const int plurality = num_voters / 2;
  int winner = kInvalidId;
  while (winner == kInvalidId) {
    vector<int> ratings(num_candidates, 0);
    for (auto it = prefs.begin(), end = prefs.end(); it != end; ++it) {
      vector<int>& voter_prefs = *it;
      while (voter_prefs.size() && !active[voter_prefs.back()]) {
        voter_prefs.pop_back();
      }
      if (voter_prefs.size()) {
        if (++ratings[voter_prefs.back()] > plurality) {
          // Winner found.
          winner = voter_prefs.back();
          break;
        }
      }
    }
    int min_rating = numeric_limits<int>::max();
    int min_candidate = kInvalidId;
    for (int i = 0; i < num_candidates; ++i) {
      const int rating = ratings[i];
      if (active[i] && rating < min_rating) {
        min_rating = rating;
        min_candidate = i;
        if (min_rating == 0) {
          // Found minimum.
          break;
        }
      }
    }
    assert(min_candidate != kInvalidId);
    // Deactive the candidate with the least first preferences.
    active[min_candidate] = false;
  }
  return winner;
}

const vector<int>& Irv::strategic_preference() const {
  return strategic_preference_;
}


int Irv::Utility(const vector<int>& pref) const {
  return vote_.ratings(selected_voter_)[FindWinner(pref)];
}

}  // namespace bush
