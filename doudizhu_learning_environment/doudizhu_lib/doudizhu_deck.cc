//
// Created by qzz on 2024/6/5.
//

#include "doudizhu_deck.h"
namespace doudizhu_learning_env {

DoudizhuCard DoudizhuDeck::DealCard(int rank, Suit suit) {
  const int index = CardIndex(rank, suit);
  CHECK_TRUE(card_in_deck_[index]);
  card_in_deck_[index] = false;
  --total_count_;
  return {rank, suit};
}
DoudizhuCard DoudizhuDeck::DealCard(int card_index) {
  CHECK_TRUE(card_in_deck_[card_index]);
  card_in_deck_[card_index] = false;
  --total_count_;
  return {Uid2Rank(card_index), Uid2Suit(card_index)};
}
DoudizhuCard DoudizhuDeck::DealCard(std::mt19937 &rng) {
  if (Empty()) {
    return {};
  }
  std::discrete_distribution<std::mt19937::result_type> dist(card_in_deck_.begin(), card_in_deck_.end());
  const int index = static_cast<int>(dist(rng));
  CHECK_TRUE(card_in_deck_[index]);
  card_in_deck_[index] = false;
  --total_count_;
  return {Uid2Rank(index), Uid2Suit(index)};
}

}