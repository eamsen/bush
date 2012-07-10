// Copyright 2012 Eugen Sawin <sawine@me73.com>
#include "./plurality-system.h"
#include <vector>
#include <algorithm>
#include <queue>
#include "./vote.h"

using std::vector;
using std::pair;
using std::make_pair;
using std::sort;
using std::priority_queue;

namespace bush {

struct Compare {
  bool operator()(const pair<int, int>& lhs, const pair<int, int>& rhs) const {
    // Prefer greater rating but lower id (reversed!).
    return lhs.first < rhs.first ||
           (lhs.first == rhs.first && lhs.second > rhs.second);
  }
};

Plurality::Plurality(const Vote& vote, const int selected_voter_id)
    : vote_(vote),
      selected_voter_(selected_voter_id),
      base_ratings_(vote.num_candidates(), 0) {
  Preprocess();
}

void Plurality::Preprocess() {
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
      // Ignore selected voter.
      continue;
    }
    const vector<int>& pref = vote_.preference(v);
    // Rate top ranked candidates only.
    const int candidate = pref[0];
    ++ratings[candidate].first;
    ++base_ratings_[candidate];
  }
  sort(ratings.begin(), ratings.end(), Compare());
  const int max_rating = ratings[num_candidates - 1].first;
  const vector<int>& selected_voter_ratings = vote_.ratings(selected_voter_);
  Queue queue;
  for (int i = 0; i < num_candidates; ++i) {
    const int rating = ratings[i].first;
    const int candidate = ratings[i].second;
    // Discretise rating: 1 for a winner candidate and 0 for a loser.
    const int discrete_rating = max_rating - rating < 2;
    queue.push(make_pair(discrete_rating * selected_voter_ratings[candidate],
                         candidate));
  }

  strategic_preference_.reserve(num_candidates);
  while (queue.size()) {
    strategic_preference_.push_back(queue.top().second);
    queue.pop();
  }
}

const vector<int>& Plurality::base_ratings() const {
  return base_ratings_;
}

const vector<int>& Plurality::strategic_preference() const {
  return strategic_preference_;
}

}  // namespace bush
