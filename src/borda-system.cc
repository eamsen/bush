// Copyright 2012 Eugen Sawin <sawine@me73.com>
#include "./borda-system.h"
#include <vector>
#include <algorithm>
#include <queue>
#include <iostream>
#include "./vote.h"

using std::vector;
using std::pair;
using std::make_pair;
using std::sort;
using std::max;
using std::priority_queue;

namespace bush {

struct Compare {
  bool operator()(const pair<int, int>& lhs, const pair<int, int>& rhs) const {
    // Prefer greater rating but lower id (reversed!).
    return lhs.first < rhs.first ||
           (lhs.first == rhs.first && lhs.second > rhs.second);
  }
};

Borda::Borda(const Vote& vote, const int selected_voter_id,
             const VotingSystem::Strategy strategy)
    : vote_(vote),
      selected_voter_(selected_voter_id),
      base_ratings_(vote.num_candidates(), 0) {
  Preprocess(strategy);
}

void Borda::Preprocess(const VotingSystem::Strategy strategy) {
  if (strategy == VotingSystem::kSingle) {
    strategic_preference_ = FindStrategicPreference(vote_, selected_voter_);
  } else if (strategy == VotingSystem::kFull) {
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
  } else {
    strategic_preference_ = vote_.preference(selected_voter_);
  }
}

vector<int> Borda::FindStrategicPreference(const Vote& vote,
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
      // Ignore the selected voter.
      continue;
    }
    const vector<int>& voter_ratings = vote.ratings(v);
    // Accumulate ratings.
    for (int c = 0; c < num_candidates; ++c) {
      ratings[c].first += voter_ratings[c];
      // base_ratings_[c] += voter_ratings[c];
    }
  }
  sort(ratings.begin(), ratings.end(), Compare());
  const int max_rating = ratings.back().first;
  const vector<int>& selected_voter_ratings = vote.ratings(selected_voter);
  // Find the best winner candidate.
  int best_candidate = 0;
  int best_rating = 0;
  for (int i = 0; i < num_candidates; ++i) {
    const int rating = ratings[i].first;
    const int candidate = ratings[i].second;
    // Discretise rating: 1 for a winner candidate and 0 for a loser.
    const int discrete_rating = max_rating - rating < num_candidates;
    const int mod_rating = discrete_rating * selected_voter_ratings[candidate];
    if (mod_rating > best_rating) {
      best_rating = mod_rating;
      best_candidate = candidate;
    }
  }
  vector<int> strategic_preference;
  strategic_preference.reserve(num_candidates);
  // Only boost the top candidate to reduce the chance of second-choice winners.
  strategic_preference.push_back(best_candidate);
  Queue queue;
  // Reduce the chance of other candidates winning by prefering harmless ones.
  for (int i = 0; i < num_candidates; ++i) {
    const int rating = ratings[i].first;
    const int candidate = ratings[i].second;
    if (candidate == best_candidate) {
      // Ignore the boosted candidate.
      continue;
    }
    // Harmlessness rating is the inverted chance for this candidate to win.
    const int harmlessness = max_rating - rating;
    queue.push(make_pair(harmlessness, candidate));
  }
  while (queue.size()) {
    strategic_preference.push_back(queue.top().second);
    queue.pop();
  }
  return strategic_preference;
}

const vector<int>& Borda::base_ratings() const {
  return base_ratings_;
}

const vector<int>& Borda::strategic_preference() const {
  return strategic_preference_;
}

}  // namespace bush
