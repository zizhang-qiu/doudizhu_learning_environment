//
// Created by qzz on 2024/5/30.
//
#include <iostream>
#include "utils.h"
#include "doudizhu_hand.h"
#include "doudizhu_move.h"
#include "doudizhu_game.h"
#include "doudizhu_state.h"
#include "doudizhu_observation.h"

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
int main() {
//  TrioComb trio_comb{kSolo, 0};
//  auto kickers = GetPossibleKickers(trio_comb);
//  std::cout << kickers.size() << std::endl;
//  for(const auto& kicker: kickers){
//    std::cout << kicker << std::endl;
//    DoudizhuMove move{
//      /*play_type=*/DoudizhuMove::PlayType::kTrioWithSolo,
//      /*single_rank=*/{},
//      /*chain=*/{},
//      /*trio_comb=*/trio_comb,
//      /*quad_comb=*/{},
//      /*plane=*/{},
//      /*kickers=*/kicker
//    };
//    std::cout << move.ToString() << std::endl;
//  }
//  QuadComb quad_comb{kPair, 0};
//  auto kickers = GetPossibleKickers(quad_comb);
//  std::cout << kickers.size() << std::endl;
//  for (const auto &kicker : kickers) {
//    std::cout << kicker << std::endl;
//    DoudizhuMove move{
//        /*play_type=*/DoudizhuMove::PlayType::kQuadWithPair,
//        /*single_rank=*/{},
//        /*chain=*/{},
//        /*trio_comb=*/{},
//        /*quad_comb=*/quad_comb,
//        /*plane=*/{},
//        /*kickers=*/kicker
//    };
//    std::cout << move.ToString() << std::endl;
//  }
//for(int sr=0; sr <= 7; ++sr){
//  const Plane plane{/*kt=*/kSolo, /*l=*/5, /*sr=*/sr};
//  auto kickers = GetPossibleKickers(plane);
////  for (const auto &ks : kickers) {
////    for (const int k : ks) {
////      std::cout << kRankChar[k];
////    }
////    std::cout << std::endl;
////  }
//  std::cout << kickers.size() << std::endl;
//}

//  std::cout << std::boolalpha << HasThreeOrGreaterEqualWithTarget({6, 6, 6, 0}, 5) << std::endl;
  GameParameters params = {
      {"seed", "2"}
  };
  const auto game = DoudizhuGame(params);
  const auto &all_moves = game.AllChanceOutcomes();
//  for (const auto &move : all_moves) {
//    if (!move.IsValid()) {
//      std::cout << move.ToString() << std::endl;
//    }
//  }
  std::mt19937 rng(42);
  auto state = DoudizhuState(std::make_shared<DoudizhuGame>(game));
  std::cout << state.ToString() << std::endl;
  while (state.CurrentPhase() == Phase::kDeal) {
    state.ApplyRandomChance();
  }
  std::cout << state.ToString() << std::endl;
  while (!state.IsTerminal()) {
    auto legal_moves = state.LegalMoves();
    auto move = UniformSample(legal_moves, rng);
//    std::cout << move.ToString() << std::endl;
    auto obs = DoudizhuObservation(state);
    std::cout << obs.ToString() << std::endl;
    state.ApplyMove(move);
//    std::cout << state.ToString() << std::endl;
  }
  std::cout << state.ToString() << std::endl;
  return 0;
}