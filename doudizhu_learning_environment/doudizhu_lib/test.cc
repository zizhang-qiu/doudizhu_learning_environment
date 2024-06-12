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

void Test(std::array<int, 3> &arr) {
  ++(arr)[0];
}

int main() {
//  TrioComb trio_comb{kSolo, 0};
  DoudizhuState state{default_game};
  const auto &all_moves = default_game->AllMoves();
  for (const auto &move : all_moves) {
    const int uid = default_game->GetMoveUid(move);
    std::cout << move.ToString() << ": " << uid << std::endl;
  }
  return 0;
}