// Copyright 2012 Eugen Sawin <sawine@me73.com>
#include "./vote.h"
#include <cassert>

using std::vector;

namespace bush {

Vote::Vote(const int num_candidates, const int num_voters)
    : preferences_(num_voters),
      num_candidates_(num_candidates),
      num_voters_(num_voters) {}

void Vote::AddPreference(const int voter_id, const vector<int>& pref) {
  assert(voter_id >= 0 && voter_id < num_voters());
  assert(static_cast<int>(pref.size()) == num_candidates());
  preferences_[voter_id] = pref;
}

int Vote::num_candidates() const {
  return num_candidates_;
}

int Vote::num_voters() const {
  return num_voters_;
}

}  // namespace bush
