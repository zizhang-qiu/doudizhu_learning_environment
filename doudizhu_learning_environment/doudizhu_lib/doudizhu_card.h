//
// Created by qzz on 2024/5/30.
//

#ifndef DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_DOUDIZHU_CARD_H_
#define DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_DOUDIZHU_CARD_H_
#include <array>

#include "doudizhu_utils.h"
#include "utils.h"

namespace doudizhu_learning_env {
constexpr std::array<Suit, kNumSuits> kAllSuits = {
  Suit::kClubsSuit, Suit::kDiamondsSuit,
  Suit::kHeartsSuit, Suit::kSpadesSuit
};

class DoudizhuCard {
  public:
    DoudizhuCard() = default; // Create an invalid card.
    constexpr DoudizhuCard(const int rank, const Suit suit) : rank_(rank), suit_(suit) {
    }
    bool operator==(const DoudizhuCard &other_card) const;
    bool IsValid() const { return (rank_ >= 0); }
    bool IsJoker() const { return rank_ == kBlackJoker || rank_ == kRedJoker; }
    std::string ToString() const;
    int CardRank() const { return rank_; }
    Suit CardSuit() const { return suit_; }

  private:
    int rank_ = -1;
    Suit suit_ = Suit::kInvalidSuit;
};

int Uid2Rank(int card_uid);
Suit Uid2Suit(int card_uid);
int CardIndex(int rank, Suit suit);
int CardIndex(const DoudizhuCard &card);

constexpr std::array<DoudizhuCard, kNumCards> AllCards() {
  std::array<DoudizhuCard, kNumCards> all_cards{};
  for (int index = 0; index < kNumCards; ++index) {
    const Suit suit =
        index >= kNumNormalCards ? Suit::kInvalidSuit : static_cast<Suit>(index % kNumSuits);
    const int rank = index >= kNumNormalCards
                       ? index - kNumNormalCards + kBlackJoker
                       : index / kNumSuits;
    all_cards[index] = DoudizhuCard{
      /*rank=*/rank, /*suit=*/suit
    };
  }
  return all_cards;
}

inline constexpr std::array<DoudizhuCard, kNumCards> kAllCards = AllCards();
} // namespace doudizhu_learning_env
#endif // DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_DOUDIZHU_CARD_H_
