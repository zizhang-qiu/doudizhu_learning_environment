//
// Created by qzz on 2024/6/5.
//

#ifndef DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_DOUDIZHU_DECK_H_
#define DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_DOUDIZHU_DECK_H_
#include <random>
#include "doudizhu_card.h"

namespace doudizhu_learning_env {
class DoudizhuDeck {
 public:
  // The deck of Doudizhu contains 54 cards, no other arguments.
  DoudizhuDeck() : card_in_deck_(kNumCards, true), total_count_(kNumCards) {}
  DoudizhuCard DealCard(int rank, Suit suit);
  DoudizhuCard DealCard(int card_index);
  DoudizhuCard DealCard(std::mt19937 &rng);

  int Size() const{return total_count_;}
  bool Empty() const{return total_count_ == 0;}
  bool CardInDeck(const int rank, const Suit suit) const{
    return card_in_deck_[CardIndex(rank, suit)];
  }
  bool CardInDeck(const DoudizhuCard& card) const{
    return CardInDeck(card.CardRank(), card.CardSuit());
  }

 private:
  std::vector<bool> card_in_deck_;
  int total_count_;
};
}
#endif //DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_DOUDIZHU_DECK_H_
