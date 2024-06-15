//
// Created by qzz on 2024/5/30.
//
#include <iostream>
#include <map>
#include <set>
#include "utils.h"
#include "doudizhu_hand.h"
#include "doudizhu_move.h"
#include "doudizhu_game.h"
#include "doudizhu_state.h"
#include "doudizhu_observation.h"
#include "douzero_encoder.h"

using namespace doudizhu_learning_env;
using namespace std;

bool HasThreeOrGreaterEqualWithTarget(const std::vector<int> &nums, int target) {
  std::unordered_map<int, int> m{};
  for (const int num : nums) {
    ++m[num - target];
    if (m[0] >= 3) {
      return true;
    }
  }
  return false;
}

void Test(std::array<int, 3> &arr) {
  ++(arr)[0];
}

void ApplyRandomMove(DoudizhuState &state, std::mt19937 &rng) {
  if (state.IsTerminal()) {
    return;
  }
  const auto legal_moves = state.LegalMoves();
  const auto random_move = UniformSample(legal_moves, rng);
  state.ApplyMove(random_move);
}

std::ostream &operator<<(std::ostream &stream, const DoudizhuMove::PlayType play_type) {
  switch (play_type) {

    case DoudizhuMove::PlayType::kInvalid:stream << "Invalid.";
      break;
    case DoudizhuMove::PlayType::kPass:stream << "pass";
      break;
    case DoudizhuMove::PlayType::kSolo:stream << "solo";
      break;
    case DoudizhuMove::PlayType::kPair:stream << "pair";
      break;
    case DoudizhuMove::PlayType::kTrio:stream << "trio";
      break;
    case DoudizhuMove::PlayType::kBomb:stream << "bomb";
      break;
    case DoudizhuMove::PlayType::kTrioWithSolo:stream << "trio with solo";
      break;
    case DoudizhuMove::PlayType::kTrioWithPair:stream << "trio with pair";
      break;
    case DoudizhuMove::PlayType::kChainOfSolo:stream << "chain of solo";
      break;
    case DoudizhuMove::PlayType::kChainOfPair:stream << "chain of pair";
      break;
    case DoudizhuMove::PlayType::kChainOfTrio:stream << "chain of trio";
      break;
    case DoudizhuMove::PlayType::kPlaneWithSolo:stream << "plane with solo";
      break;
    case DoudizhuMove::PlayType::kPlaneWithPair:stream << "plane with pair";
      break;
    case DoudizhuMove::PlayType::kQuadWithSolo:stream << "quad with solo";
      break;
    case DoudizhuMove::PlayType::kQuadWithPair:stream << "quad with pair";
      break;
    case DoudizhuMove::PlayType::kRocket:stream << "rocket";
      break;
  }
  return stream;
}

int main() {
//  TrioComb trio_comb{kSolo, 0};
  DoudizhuState state{default_game};
  while (state.IsChanceNode()) {
    state.ApplyRandomChance();
  }
  std::mt19937 rng;
  rng.seed(3);
  while (state.CurrentPhase() == Phase::kAuction) {
    ApplyRandomMove(state, rng);
  }
  for (int i = 0; i <= 6; ++i) {
    ApplyRandomMove(state, rng);
  }

  std::cout << state.ToString() << std::endl;
  std::cout << state.CurrentPlayer() << std::endl;
  const DoudizhuObservation obs{state};
  std::cout << obs.ToString() << std::endl;
//
//  const auto hand_feature = EncodeMyHand(obs);
//  std::cout << hand_feature.encoding << std::endl;
//
//  const auto other_hand_feature = EncodeOtherHands(obs);
//  std::cout << other_hand_feature.encoding << std::endl;
//
//  const auto last_move_feature = EncodeLastMove(obs);
//  std::cout << last_move_feature.encoding << std::endl;
//
//  auto res = GetPlayedCardsPerRankByPlayer(obs);
//  std::cout << res << std::endl;
  DouzeroEncoder encoder;
  auto features = encoder.Encode(obs);

  for (const auto &kv : features) {
    std::cout << kv.first << ":\n";
    std::cout << "Encoding: " << kv.second.encoding << "\n";
    std::cout << "Dims: " << kv.second.dims << "\n";
  }

  return 0;
}