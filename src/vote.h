// Copyright 2012 Eugen Sawin <sawine@me73.com>
#ifndef SRC_VOTE_H_
#define SRC_VOTE_H_

#include <vector>

namespace bush {

class Vote {
 public:
  Vote(const int num_candidates, const int num_voters);
  void AddPreference(const int voter_id, const std::vector<int>& pref);
  int num_candidates() const;
  int num_voters() const;

 private:
  std::vector<std::vector<int> > preferences_;
  int num_candidates_;
  int num_voters_;
};

}  // namespace bush
#endif  // SRC_VOTE_H_
