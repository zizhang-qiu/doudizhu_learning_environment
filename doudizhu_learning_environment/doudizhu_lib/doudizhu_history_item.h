//
// Created by qzz on 2024/6/5.
//

#ifndef DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_DOUDIZHU_HISTORY_ITEM_H_
#define DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_DOUDIZHU_HISTORY_ITEM_H_
#include "doudizhu_move.h"
namespace doudizhu_learning_env {
struct DoudizhuHistoryItem {
  DoudizhuHistoryItem(const DoudizhuMove &move_made) : move(move_made) {}

  DoudizhuHistoryItem(const DoudizhuHistoryItem &past_move) = default;

  std::string ToString() const;

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
  std::vector<int> kickers;
};
}
#endif //DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_DOUDIZHU_HISTORY_ITEM_H_
