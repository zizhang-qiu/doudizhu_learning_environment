//
// Created by qzz on 2024/5/30.
//

#ifndef DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_DOUDIZHU_CARD_H_
#define DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_DOUDIZHU_CARD_H_
#include <array>

#include "utils.h"
#include "doudizhu_utils.h"
namespace doudizhu_learning_env {

enum Suit {
  kInvalidSuit = -1,
  kClubsSuit = 0,
  kDiamondSuit,
  kHeartSuit,
  kSpadeSuit
};

inline constexpr int kBlackJoker = 13;
inline constexpr int kRedJoker = 14;

constexpr std::array<Suit, kNumSuits> kAllSuits = {kClubsSuit, kDiamondSuit, kHeartSuit, kSpadeSuit};

class DoudizhuCard {
 public:
  DoudizhuCard() = default; // Create an invalid card.
  DoudizhuCard(int rank, Suit suit) : rank_(rank), suit_(suit) {}
  bool operator==(const DoudizhuCard &other_card) const;
  bool IsValid() const {
    return (rank_ >= 0);
  }
  bool IsJoker() const { return rank_ == kBlackJoker || rank_ == kRedJoker; }
  std::string ToString() const;
  int CardRank() const { return rank_; }
  Suit CardSuit() const { return suit_; }

 private:
  int rank_ = -1;
  Suit suit_ = kInvalidSuit;
};

int Uid2Rank(int card_uid);
Suit Uid2Suit(int card_uid);
int CardIndex(int rank, Suit suit);
int CardIndex(const DoudizhuCard &card);
}
#endif //DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_DOUDIZHU_CARD_H_
