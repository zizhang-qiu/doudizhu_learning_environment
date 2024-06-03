//
// Created by qzz on 2024/5/30.
//

#include "doudizhu_card.h"
#include "doudizhu_utils.h"
namespace doudizhu_learning_env {
std::string DoudizhuCard::ToString() const {
  // Invalid card.
  if (!IsValid()) {
    return "I";
  }
  if (rank_ == kRedJoker) {
    return "RJ";
  }
  if (rank_ == kBlackJoker) {
    return "BJ";
  }

  return {kSuitChar[suit_], kRankChar[rank_]};
}

bool DoudizhuCard::operator==(const DoudizhuCard &other_card) const {
  // Valid card.
  if (rank_ >= 0) {
    return (suit_ == other_card.suit_) && (rank_ == other_card.rank_);
  }

  return true;
}

}

