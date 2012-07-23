// Copyright 2012 Eugen Sawin <sawine@me73.com>
#include "./irv-system.h"
#include <unordered_map>
#include <unordered_set>
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
using std::unordered_map;
using std::unordered_set;
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

struct IntVectorHash {
  size_t operator()(const vector<int>& vec) const {
    const int size = vec.size();
    size_t h = size ^ 0x550924F3;
    for (int i = 0; i < size; ++i) {
      const int j = i * 3;
      h ^= (vec[i] << j) ^ (h >> j);
    }
    return h;
  }
};

Irv::Irv(const Vote& vote, const int selected_voter_id,
         const VotingSystem::Strategy strategy)
    : vote_(vote),
      selected_voter_(selected_voter_id),
      time_limit_(VotingSystem::kDefTimeLimit) {
  Preprocess(strategy);
}

void Irv::Preprocess(const VotingSystem::Strategy strategy) {
  if (strategy == VotingSystem::kSimple) {
    strategic_preference_ = FindStrategicPreference(vote_, selected_voter_,
                                                    time_limit_);
  } else if (strategy == VotingSystem::kComplete) {
    const Clock beg;

    const int num_voters = vote_.num_voters();
    const Clock::Diff voter_time = time_limit_ * 0.66 / num_voters;
    Vote strategic_vote(vote_.num_candidates(), num_voters);
    strategic_vote.AddPreference(selected_voter_,
                                 vote_.preference(selected_voter_));
    for (int v = 0; v < num_voters; ++v) {
      if (v == selected_voter_) {
        continue;
      }
      strategic_vote.AddPreference(v, FindStrategicPreference(vote_, v,
                                                              voter_time));
    }
    const Clock::Diff rest_time = time_limit_ - (Clock() - beg);
    strategic_preference_ = FindStrategicPreference(strategic_vote,
                                                    selected_voter_,
                                                    rest_time);
  } else if (strategy == VotingSystem::kIndependent) {
    const Clock beg;
    RandomGenerator<float> random(13);
    const int num_voters = vote_.num_voters();
    const int num_candidates = vote_.num_candidates();
    const Clock::Diff voter_time = time_limit_ * 0.1 / num_voters;
    int checked_hits = 0;
    int best_utility = 0;
    const int rand_candidates = num_candidates / 3;
    const int max_checked_hits = num_candidates * num_voters * rand_candidates;
    strategic_preference_ = vote_.preference(selected_voter_);
    unordered_map<vector<int>, int, IntVectorHash> pref_map;
    while (checked_hits < max_checked_hits &&
           Clock() - beg < time_limit_) {
      Vote strategic_vote(vote_.num_candidates(), num_voters);
      strategic_vote.AddPreference(selected_voter_,
                                   vote_.preference(selected_voter_));
      for (int v = 0; v < num_voters; ++v) {
        if (v == selected_voter_) {
          continue;
        }
        vector<int> voter_pref = vote_.preference(v);
        for (int r = 0; r < rand_candidates; ++r) {
          swap(voter_pref[random.Next() * num_candidates],
               voter_pref[random.Next() * num_candidates]);
        }
        strategic_vote.AddPreference(v, voter_pref);
      }
      vector<int> preference = FindStrategicPreference(strategic_vote,
                                                       selected_voter_,
                                                       voter_time);
      const int utility = Utility(vote_, selected_voter_, preference);
      auto find = pref_map.find(preference);
      if (find == pref_map.end()) {
        find = pref_map.insert(make_pair(preference, utility)).first;
      } else {
        ++checked_hits;
        find->second += utility;
      }
      if (find->second > best_utility) {
        strategic_preference_ = find->first;
        best_utility = find->second;
      }
    }
  } else {
    strategic_preference_ = vote_.preference(selected_voter_);
  }
}

vector<int> Irv::FindStrategicPreference(const Vote& vote,
                                         const int selected_voter,
                                         const Clock::Diff time_limit) {
  const Clock beg;

  unordered_set<vector<int>, IntVectorHash> checked;
  RandomGenerator<float> random(12);
  const int num_candidates = vote.num_candidates();
  const int max_utility = num_candidates - 1;

  vector<int> strategic_preference = vote.preference(selected_voter);
  vector<int> preference = vote.preference(selected_voter);
  int best_utility = Utility(vote, selected_voter, preference);
  int checked_hits = 0;
  const int max_checked_hits = num_candidates;
  while (checked_hits < max_checked_hits &&
         best_utility < max_utility &&
         Clock() - beg < time_limit) {
    swap(preference[random.Next() * num_candidates],
         preference[random.Next() * num_candidates]);
    if (checked.find(preference) != checked.end()) {
      ++checked_hits;
      continue;
    }
    checked_hits = 0;
    checked.insert(preference);
    const int utility = Utility(vote, selected_voter, preference);
    if (utility > best_utility) {
      best_utility = utility;
      strategic_preference.swap(preference);
    }
  }
  return strategic_preference;
}

int Irv::FindWinner(const Vote& vote, const int selected_voter,
                    const vector<int>& preference) {
  static const int kInvalidId = -1;

  const int num_voters = vote.num_voters();
  const int num_candidates = vote.num_candidates();
  vector<vector<int> > prefs;
  prefs.reserve(num_voters);
  for (int v = 0; v < num_voters; ++v) {
    if (v == selected_voter) {
      prefs.push_back(preference);
    } else {
      prefs.push_back(vote.preference(v));
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


int Irv::Utility(const Vote& vote, const int selected_voter,
                 const vector<int>& pref) {
  return vote.ratings(selected_voter)[FindWinner(vote, selected_voter, pref)];
}

}  // namespace bush
