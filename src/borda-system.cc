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

Borda::Borda(const Vote& vote, const int selected_voter_id)
    : vote_(vote),
      selected_voter_(selected_voter_id),
      base_ratings_(vote.num_candidates(), 0) {
  Preprocess();
}

void Borda::Preprocess() {
  typedef priority_queue<pair<int, int>, vector<pair<int, int> >,
                         Compare> Queue;

  const int num_voters = vote_.num_voters();
  const int num_candidates = vote_.num_candidates();
  // Vector of (rating, candidate id) pairs.
  vector<pair<int, int> > ratings;
  ratings.reserve(num_candidates);
  for (int c = 0; c < num_candidates; ++c) {
    ratings.push_back(make_pair(0, c));
  }
  for (int v = 0; v < num_voters; ++v) {
    if (v == selected_voter_) {
      // Ignore the selected voter.
      continue;
    }
    const vector<int>& voter_ratings = vote_.ratings(v);
    // Accumulate ratings.
    for (int c = 0; c < num_candidates; ++c) {
      ratings[c].first += voter_ratings[c];
      base_ratings_[c] += voter_ratings[c];
    }
  }
  sort(ratings.begin(), ratings.end(), Compare());
  const int max_rating = ratings.back().first;
  const vector<int>& selected_voter_ratings = vote_.ratings(selected_voter_);
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
  strategic_preference_.reserve(num_candidates);
  // Only boost the top candidate to reduce the chance of second-choice winners.
  strategic_preference_.push_back(best_candidate);
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
    strategic_preference_.push_back(queue.top().second);
    queue.pop();
  }
}

const vector<int>& Borda::base_ratings() const {
  return base_ratings_;
}

const vector<int>& Borda::strategic_preference() const {
  return strategic_preference_;
}

}  // namespace bush
