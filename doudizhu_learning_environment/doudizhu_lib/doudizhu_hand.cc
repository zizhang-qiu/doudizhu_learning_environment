//
// Created by qzz on 2024/6/1.
//

#include <iostream>
#include "doudizhu_hand.h"
namespace doudizhu_learning_env {

void DoudizhuHand::AddCard(const DoudizhuCard &card) {
  CHECK_TRUE(card.IsValid());
  CHECK_TRUE(CanAdd(card.CardRank()));
//  std::cout << ToString() << std::endl;
  ++cards_per_rank_[card.CardRank()];
  ++num_cards_;
}

void DoudizhuHand::AddCard(int rank) {
  CHECK_TRUE(CanAdd(rank));
  ++cards_per_rank_[rank];
  ++num_cards_;
}

void DoudizhuHand::RemoveFromHand(int rank) {
  CHECK_GE(rank, 0);
  CHECK_LE(rank, kRedJoker);
  CHECK_TRUE(CanRemove(rank));
  --cards_per_rank_[rank];
  --num_cards_;
}

void DoudizhuHand::RemoveFromHand(int rank, std::array<int, kNumRanks> &played_cards_per_rank) {
  RemoveFromHand(rank);
  ++(played_cards_per_rank)[rank];
}

bool DoudizhuHand::CanAdd(int rank) const {
  if (rank < 0 || rank > kRedJoker) {
    return false;
  }
  // A hand can only have 1 R/B Joker.
  if (rank == kBlackJoker || rank == kRedJoker) {
    return cards_per_rank_[rank] == 0;
  }

  // Others.
  return cards_per_rank_[rank] < kNumSuits;

}
bool DoudizhuHand::CanRemove(int rank) const {
  if (rank < 0 || rank > kRedJoker) {
    return false;
  }
  return cards_per_rank_[rank] > 0;
}

std::string DoudizhuHand::ToString() const {
  std::string rv{};
  for (int rank = kRedJoker; rank >= 0; --rank) {
    for (int remained_cards = cards_per_rank_[rank]; remained_cards > 0; --remained_cards) {
      rv.push_back(kRankChar[rank]);
    }
  }
  return rv;
}

}