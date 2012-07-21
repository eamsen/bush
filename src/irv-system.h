// Copyright 2012 Eugen Sawin <sawine@me73.com>
#ifndef SRC_IRV_SYSTEM_H_
#define SRC_IRV_SYSTEM_H_

#include <vector>
#include "./clock.h"
#include "./voting-system.h"

namespace bush {

class Vote;

class Irv {
 public:
  static const base::Clock::Diff kDefTimeLimit;

  Irv(const Vote& vote, const int selected_voter_id,
      const VotingSystem::Strategy strategy);
  const std::vector<int>& strategic_preference() const;
  void time_limit(const base::Clock::Diff limit);
  base::Clock::Diff time_limit() const;

 private:
  void Preprocess(const VotingSystem::Strategy strategy);
  static std::vector<int> FindStrategicPreference(const Vote& vote,
                                                  const int selected_voter,
                                                  const base::Clock::Diff
                                                        time_limit);
  static int FindWinner(const Vote& vote, const int selected_voter,
                        const std::vector<int>& preference);
  static int Utility(const Vote& vote, const int selected_voter,
                     const std::vector<int>& preference);

  const Vote& vote_;
  int selected_voter_;
  std::vector<int> strategic_preference_;
  base::Clock::Diff time_limit_;
};

}  // namespace bush
#endif  // SRC_IRV_SYSTEM_H_
