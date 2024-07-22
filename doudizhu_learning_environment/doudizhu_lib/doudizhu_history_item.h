//
// Created by qzz on 2024/6/5.
//

#ifndef DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_DOUDIZHU_HISTORY_ITEM_H_
#define DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_DOUDIZHU_HISTORY_ITEM_H_
#include "doudizhu_move.h"
namespace doudizhu_learning_env {
struct DoudizhuHistoryItem {
  explicit DoudizhuHistoryItem(const DoudizhuMove &move_made) : move(move_made) {}

  DoudizhuHistoryItem(const DoudizhuHistoryItem &past_move) = default;

  std::string ToString() const;
  bool operator==(const DoudizhuHistoryItem &other_item) const {
    return move == other_item.move && player == other_item.player && deal_to_player == other_item.deal_to_player;
  }

  DoudizhuMove move;
  int player = kInvalidPlayer;
  int deal_to_player = kInvalidPlayer;

  DoudizhuMove::AuctionType auction_type = DoudizhuMove::AuctionType::kInvalid;
  DoudizhuMove::PlayType play_type = DoudizhuMove::PlayType::kInvalid;
  DoudizhuCard deal_card;

  SingleRank single_rank;
  Chain chain;
  TrioComb trio_comb;
  QuadComb quad_comb;
  Plane plane;
  std::array<int, kNumRanks> kickers;
};
}
#endif //DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_DOUDIZHU_HISTORY_ITEM_H_
