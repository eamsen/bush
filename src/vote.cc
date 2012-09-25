// Copyright 2012 Eugen Sawin <esawin@me73.com>
#include "./vote.h"
#include <cassert>
#include <sstream>

using std::vector;
using std::string;
using std::ostringstream;

namespace bush {

Vote::Vote(const int num_candidates, const int num_voters)
    : preferences_(num_voters),
      ratings_(num_voters, vector<int>(num_candidates, 0)),
      num_candidates_(num_candidates),
      num_voters_(num_voters) {}

void Vote::AddPreference(const int voter_id, const vector<int>& pref) {
  assert(voter_id >= 0 && voter_id < num_voters());
  assert(static_cast<int>(pref.size()) == num_candidates());
  UpdateRatings(voter_id, pref);
  preferences_[voter_id] = pref;
}

void Vote::AddPreference(const int voter_id, vector<int>&& pref) {
  assert(voter_id >= 0 && voter_id < num_voters());
  assert(static_cast<int>(pref.size()) == num_candidates());
  UpdateRatings(voter_id, pref);
  preferences_[voter_id].swap(pref);
}

const vector<int>& Vote::preference(const int voter_id) const {
  assert(voter_id >= 0 && voter_id < num_voters());
  return preferences_[voter_id];
}

const vector<int>& Vote::ratings(const int voter_id) const {
  assert(voter_id >= 0 && voter_id < num_voters());
  return ratings_[voter_id];
}

int Vote::num_candidates() const {
  return num_candidates_;
}

int Vote::num_voters() const {
  return num_voters_;
}

string Vote::str() const {
  ostringstream ss;
  ss << num_candidates() << " " << num_voters() << "\n";
  for (auto it = preferences_.cbegin(), end = preferences_.cend();
       it != end; ++it) {
    const vector<int>& voter_pref = *it;
    for (auto it2 = voter_pref.cbegin(), end2 = voter_pref.cend();
         it2 != end2; ++it2) {
      if (it2 != voter_pref.cbegin()) {
        ss << " ";
      }
      ss << *it2;
    }
    ss << "\n";
  }
  return ss.str();
}

void Vote::UpdateRatings(const int voter_id, const vector<int>& pref) {
  assert(voter_id >= 0 && voter_id < num_voters());
  assert(static_cast<int>(pref.size()) == num_candidates());
  vector<int>& ratings = ratings_[voter_id];
  for (int i = 0; i < num_candidates_; ++i) {
    ratings[pref[i]] += num_candidates_ - i - 1;
  }
}

}  // namespace bush
