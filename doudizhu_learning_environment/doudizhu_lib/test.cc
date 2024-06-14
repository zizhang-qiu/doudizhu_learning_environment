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
//  DoudizhuState state{default_game};
  const auto &all_moves = default_game->AllMoves();
//  std::map<int, std::set<DoudizhuMove::PlayType>> m;
//  for (const auto &move : all_moves) {
//    auto move_str = move.ToString();
//    move_str = move_str.substr(6, move_str.length() - 6 - 1);
//    m[move_str.length()].emplace(move.GetPlayType());
//  }
//  for (const auto &kv : m) {
//    std::cout << kv.first << ": ";
//    for (const auto t : kv.second) {
//      std::cout << t << ", ";
//    }
//    std::cout << "\n";
//  }
//  for (const auto &move : all_moves) {
//    const int prefix_len = move.MoveType() == doudizhu_learning_env::DoudizhuMove::kAuction ? 5 : 6;
//    const auto move_str = move.ToString().substr(
//        prefix_len,
//        move.ToString().length() - prefix_len - 1);
////    std::cout << move_str << std::endl;
//    const auto move_from_str = GetMoveFromString(move_str, move.MoveType());
//    if (!(move_from_str == move)){
//      std::cout << move_str << ", " << move_from_str.ToString() << std::endl;
//    }
//  }
//  for (const std::string str : {"R", "B"}) {
//    auto move = GetMoveFromString(str, doudizhu_learning_env::DoudizhuMove::kDeal);
//    std::cout << move.ToString() << std::endl;
//  }
//  for (const std::string str : {"p", "pass", "1", "2", "3"}) {
//    auto move = GetMoveFromString(str, doudizhu_learning_env::DoudizhuMove::kAuction);
//    std::cout << move.ToString() << std::endl;
//  }
//  std::cout << std::boolalpha << IsVectorConsecutive({1, 2, 4}) << std::endl;
//  auto move = GetMoveFromString("QQKKAA", doudizhu_learning_env::DoudizhuMove::kPlay);
//  std::cout << move.ToString() << std::endl;
//  DoudizhuMove::PlayType play_type = DoudizhuMove::PlayType::kRocket;
//  std::cout << std::boolalpha << (play_type == DoudizhuMove::PlayType::kRocket) << std::endl;
  const Chain chain{/*chain_type=*/doudizhu_learning_env::ChainType::kSolo, /*length=*/8, /*start_rank=*/0};
  const DoudizhuMove move{/*chain=*/chain};
  int uid = default_game->GetMoveUid(move);
  std::cout << uid << std::endl;
  return 0;
}