// Copyright 2012 Eugen Sawin <esawin@me73.com>
#include "./plurality-system.h"
#include <unordered_map>
#include <cassert>
#include <vector>
#include <algorithm>
#include <queue>
#include "./clock.h"
#include "./random.h"
#include "./vote.h"

using std::vector;
using std::unordered_map;
using std::pair;
using std::make_pair;
using std::sort;
using std::priority_queue;
using std::swap;
using base::Clock;
using base::RandomGenerator;

namespace bush {

struct Compare {
  bool operator()(const pair<int, int>& lhs, const pair<int, int>& rhs) const {
    // Prefer greater rating but lower id (reversed!).
    return lhs.first < rhs.first ||
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

Plurality::Plurality(const Vote& vote, const int selected_voter_id,
                     const VotingSystem::Strategy strategy)
    : vote_(vote),
      selected_voter_(selected_voter_id),
      base_ratings_(vote.num_candidates(), 0),
      time_limit_(VotingSystem::kDefTimeLimit) {
  Preprocess(strategy);
}

void Plurality::Preprocess(const VotingSystem::Strategy strategy) {
  if (strategy == VotingSystem::kSimple) {
    strategic_preference_ = FindStrategicPreference(vote_, selected_voter_);
  } else if (strategy == VotingSystem::kComplete) {
    const int num_voters = vote_.num_voters();
    Vote strategic_vote(vote_.num_candidates(), num_voters);
    strategic_vote.AddPreference(selected_voter_,
                                 vote_.preference(selected_voter_));
    for (int v = 0; v < num_voters; ++v) {
      if (v == selected_voter_) {
        continue;
      }
      strategic_vote.AddPreference(v, FindStrategicPreference(vote_, v));
    }
    strategic_preference_ = FindStrategicPreference(strategic_vote,
                                                    selected_voter_);
  } else if (strategy == VotingSystem::kIndependent) {
    const Clock beg;
    RandomGenerator<float> random(13);
    const int num_voters = vote_.num_voters();
    const int num_candidates = vote_.num_candidates();
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
                                                       selected_voter_);
      const int utility = Utility(vote_, selected_voter_);
      auto find = pref_map.find(preference);
      if (find == pref_map.end()) {
        checked_hits = 0;
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

int Plurality::Utility(const Vote& vote, const int selected_voter) {
  typedef priority_queue<pair<int, int>, vector<pair<int, int> >,
                         Compare> Queue;

  const int num_voters = vote.num_voters();
  const int num_candidates = vote.num_candidates();
  // Vector of (rating, candidate id) pairs.
  vector<pair<int, int> > ratings;
  ratings.reserve(num_candidates);
  for (int c = 0; c < num_candidates; ++c) {
    ratings.push_back(make_pair(0, c));
  }
  for (int v = 0; v < num_voters; ++v) {
    if (v == selected_voter) {
      // Ignore selected voter.
      continue;
    }
    const vector<int>& pref = vote.preference(v);
    // Rate top ranked candidates only.
    const int candidate = pref[0];
    ++ratings[candidate].first;
  }
  sort(ratings.begin(), ratings.end(), Compare());
  return vote.ratings(selected_voter)[ratings.back().second];
}

vector<int> Plurality::FindStrategicPreference(const Vote& vote,
                                               const int selected_voter) {
  typedef priority_queue<pair<int, int>, vector<pair<int, int> >,
                         Compare> Queue;

  const int num_voters = vote.num_voters();
  const int num_candidates = vote.num_candidates();
  // Vector of (rating, candidate id) pairs.
  vector<pair<int, int> > ratings;
  ratings.reserve(num_candidates);
  for (int c = 0; c < num_candidates; ++c) {
    ratings.push_back(make_pair(0, c));
  }
  for (int v = 0; v < num_voters; ++v) {
    if (v == selected_voter) {
      // Ignore selected voter.
      continue;
    }
    const vector<int>& pref = vote.preference(v);
    // Rate top ranked candidates only.
    const int candidate = pref[0];
    ++ratings[candidate].first;
  }
  sort(ratings.begin(), ratings.end(), Compare());
  const int max_rating = ratings.back().first;
  const vector<int>& selected_voter_ratings = vote.ratings(selected_voter);
  Queue queue;
  for (int i = 0; i < num_candidates; ++i) {
    const int rating = ratings[i].first;
    const int candidate = ratings[i].second;
    // Discretise rating: 1 for a winner candidate and 0 for a loser.
    const int discrete_rating = max_rating - rating < 2;
    queue.push(make_pair(discrete_rating * selected_voter_ratings[candidate],
                         candidate));
  }

  vector<int> strategic_preference;
  strategic_preference.reserve(num_candidates);
  while (queue.size()) {
    strategic_preference.push_back(queue.top().second);
    queue.pop();
  }
  return strategic_preference;
}

const vector<int>& Plurality::base_ratings() const {
  return base_ratings_;
}

const vector<int>& Plurality::strategic_preference() const {
  return strategic_preference_;
}

}  // namespace bush
