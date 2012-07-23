// Copyright 2012 Eugen Sawin <sawine@me73.com>
#ifndef SRC_VOTING_SYSTEM_H_
#define SRC_VOTING_SYSTEM_H_

#include "./clock.h"

namespace bush {

class VotingSystem {
 public:
  static const base::Clock::Diff kDefTimeLimit = 10 * base::Clock::kMicroInSec;
  enum Strategy { kSimple, kComplete, kIndependent };
};
}  // namespace bush
#endif  // SRC_VOTING_SYSTEM_H_
