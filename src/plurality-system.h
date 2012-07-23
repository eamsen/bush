// Copyright 2012 Eugen Sawin <sawine@me73.com>
#ifndef SRC_PLURALITY_SYSTEM_H_
#define SRC_PLURALITY_SYSTEM_H_

#include <vector>
#include "./voting-system.h"
#include "./clock.h"

namespace bush {

class Vote;

class Plurality : public VotingSystem {
 public:
  static int Utility(const Vote& vote, const int selected_voter);

  Plurality(const Vote& vote, const int selected_voter_id,
            const VotingSystem::Strategy strategy);
  const std::vector<int>& base_ratings() const;
  const std::vector<int>& strategic_preference() const;

 private:
  void Preprocess(const VotingSystem::Strategy strategy);
  static std::vector<int> FindStrategicPreference(const Vote& vote,
                                                  const int selected_voter);

  const Vote& vote_;
  int selected_voter_;
  mutable std::vector<int> base_ratings_;
  std::vector<int> strategic_preference_;
  base::Clock::Diff time_limit_;
};

}  // namespace bush
#endif  // SRC_PLURALITY_SYSTEM_H_
