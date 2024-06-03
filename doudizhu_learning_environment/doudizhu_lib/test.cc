//
// Created by qzz on 2024/5/30.
//
#include <iostream>
#include "utils.h"
#include "doudizhu_hand.h"
#include "doudizhu_move.h"

using namespace doudizhu_learning_env;
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
  Plane plane{kSolo, 4, 1};
  auto kickers = GetPossibleKickers(plane);
  std::cout << kickers.size() << std::endl;
  for (const auto &kicker : kickers) {
    std::cout << kicker << std::endl;
    DoudizhuMove move{
        /*play_type=*/DoudizhuMove::PlayType::kPlaneWithPair,
        /*single_rank=*/{},
        /*chain=*/{},
        /*trio_comb=*/{},
        /*quad_comb=*/{},
        /*plane=*/plane,
        /*kickers=*/kicker
    };
    std::cout << move.ToString() << std::endl;
  }

  return 0;
}