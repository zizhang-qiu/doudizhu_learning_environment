//
// Created by qzz on 2024/6/1.
//

#ifndef DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_DOUDIZHU_HAND_H_
#define DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_DOUDIZHU_HAND_H_
#include "doudizhu_card.h"
namespace doudizhu_learning_env {
class DoudizhuHand {

 public:
  DoudizhuHand() : num_cards_(0) {
    std::fill(cards_per_rank_.begin(), cards_per_rank_.end(), 0);
  }

  DoudizhuHand(const DoudizhuHand &) = default;

  std::string ToString() const;

  void AddCard(const DoudizhuCard &card);

  void RemoveFromHand(int rank);

  void RemoveFromHand(int rank, std::array<uint8_t, kNumRanks> *played_cards_per_rank);

  int Size() const { return num_cards_; }

  std::array<uint8_t, kNumRanks> CardsPerRank() const { return cards_per_rank_; }

  bool CanAdd(int rank) const;

  bool CanRemove(int rank) const;

 private:
  std::array<uint8_t, kNumRanks> cards_per_rank_{};
  int num_cards_;

};
}
#endif //DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_DOUDIZHU_HAND_H_
