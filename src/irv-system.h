// Copyright 2012 Eugen Sawin <sawine@me73.com>
#ifndef SRC_IRV_SYSTEM_H_
#define SRC_IRV_SYSTEM_H_

#include <vector>
#include "./clock.h"

namespace bush {

class Vote;

class Irv {
 public:
  static const base::Clock::Diff kDefTimeLimit;

  Irv(const Vote& vote, const int selected_voter_id);
  const std::vector<int>& strategic_preference() const;
  void time_limit(const base::Clock::Diff limit);
  base::Clock::Diff time_limit() const;

 private:
  void Preprocess();
  int FindWinner(const std::vector<int>& preference) const;
  int Utility(const std::vector<int>& preference) const;

  const Vote& vote_;
  int selected_voter_;
  std::vector<int> strategic_preference_;
  base::Clock::Diff time_limit_;
};

}  // namespace bush
#endif  // SRC_IRV_SYSTEM_H_
