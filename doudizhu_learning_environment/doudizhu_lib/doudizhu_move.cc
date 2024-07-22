//
// Created by qzz on 2024/6/2.
//

#include "doudizhu_move.h"

namespace doudizhu_learning_env {
std::pair<int, int> MinAndMaxChainLength(const ChainType chain_type) {
  switch (chain_type) {
  case ChainType::kNotChain:
    return {-1, -1};
  case ChainType::kSolo:
    return {kChainOfSoloMinLength, kChainOfSoloMaxLength};
  case ChainType::kPair:
    return {kChainOfPairMinLength, kChainOfPairMaxLength};
  case ChainType::kTrio:
    return {kChainOfTrioMinLength, kChainOfTrioMaxLength};
  default:
    FatalError("Should not reach here.");
  }
}

bool KickersContainBR(const std::array<int, kNumRanks> &kickers) {
  return kickers[kBlackJoker] > 0 && kickers[kRedJoker] > 0;
}

bool IsRankInPlane(const Plane &plane, const int rank) {
  const int end_rank = plane.start_rank + plane.length - 1;
  return rank >= plane.start_rank && rank <= end_rank;
}

bool HasKElemEqualWithTarget(const std::vector<int> &nums, int k, int target) {
  std::unordered_map<int, int> m{};
  for (const int num : nums) {
    ++m[num - target];

    if (m[0] >= k) {
      return true;
    }
  }
  return false;
}

bool HasBomb(const std::vector<int> &nums) {
  if (nums.size() < 4) {
    return false;
  }
  std::unordered_map<int, int> m{};
  for (const int num : nums) {
    if (m.count(num) > 0 && m[num] == 3) {
      return true;
    }
    ++m[num];
  }
  return false;
}

bool HasBomb(const std::array<int, kNumRanks> &kickers) {
  for (int rank = 0; rank < kNumCardsPerSuit; ++rank) {
    if (kickers[rank] == kQuadLength) {
      return true;
    }
  }
  return false;
}

std::array<int, kNumRanks> RanksToCounter(const std::vector<int> &kickers) {
  std::array<int, kNumRanks> counter{};
  for (const int k : kickers) {
    ++counter[k];
  }
  return counter;
}

std::string DoudizhuMove::ToString() const {
  switch (move_type_) {
  case kInvalid:
    return "I";
  case kDeal:
    return "(Deal " + deal_card_.ToString() + ")";
  case kAuction:
    if (auction_type_ == AuctionType::kPass) {
      return "(Bid Pass)";
    }
    return "(Bid " + std::to_string(static_cast<int>(auction_type_)) + ")";
  case kPlay: {
    std::string rv{"(Play "};
    switch (play_type_) {
    case PlayType::kPass:
      return "(Play Pass)";
    case PlayType::kSolo:
    case PlayType::kPair:
    case PlayType::kTrio:
    case PlayType::kBomb: {
      for (int i = 0; i < single_rank_.num_cards; ++i) {
        rv.push_back(kRankChar[single_rank_.rank]);
      }
      return rv + ")";
    }
    case PlayType::kTrioWithSolo:
    case PlayType::kTrioWithPair: {
      const char rank_char = kRankChar[trio_comb_.trio_rank];
      for (int i = 0; i < kTrioLength; ++i) {
        rv.push_back(rank_char);
      }
      for (int rank = 0; rank < kNumRanks; ++rank) {
        for (int i = 0; i < kickers_[rank]; ++i) {
          rv.push_back(kRankChar[rank]);
        }
      }
      return rv + ")";
    }
    case PlayType::kChainOfSolo:
    case PlayType::kChainOfPair:
    case PlayType::kChainOfTrio: {
      for (int i = 0; i < chain_.length; ++i) {
        for (int j = 0; j < static_cast<int>(chain_.chain_type); ++j) {
          rv.push_back(kRankChar[chain_.start_rank + i]);
        }
      }
      return rv + ")";
    }
    case PlayType::kPlaneWithSolo:
    case PlayType::kPlaneWithPair: {
      for (int i = 0; i < plane_.length; ++i) {
        for (int j = 0; j < kTrioLength; ++j) {
          rv.push_back(kRankChar[plane_.start_rank + i]);
        }
      }
      for (int rank = 0; rank < kNumRanks; ++rank) {
        for (int i = 0; i < kickers_[rank]; ++i) {
          rv.push_back(kRankChar[rank]);
        }
      }
      return rv + ")";
    }
    case PlayType::kQuadWithSolo:
    case PlayType::kQuadWithPair: {
      for (int i = 0; i < kQuadLength; ++i) {
        rv.push_back(kRankChar[quad_comb_.quad_rank]);
      }
      for (int rank = 0; rank < kNumRanks; ++rank) {
        for (int i = 0; i < kickers_[rank]; ++i) {
          rv.push_back(kRankChar[rank]);
        }
      }
      return rv + ")";
    }

    case PlayType::kRocket:
      return rv + "BR)";
    default:
      FatalError("Should not reach here.");
    }
  }
  }
  FatalError("Should not reach here.");
}
bool DoudizhuMove::operator==(const DoudizhuMove &other_move) const {
  switch (move_type_) {
  case kInvalid:
    return true;
  case kDeal:
    return deal_card_ == other_move.deal_card_;
  case kAuction: {
    return auction_type_ == other_move.auction_type_;
  }
  case kPlay: {
    if (play_type_ != other_move.play_type_) {
      return false;
    }
    switch (play_type_) {
    case PlayType::kPass:
      return true;
    case PlayType::kSolo:
    case PlayType::kPair:
    case PlayType::kTrio:
    case PlayType::kBomb:
      return single_rank_ == other_move.single_rank_;
    case PlayType::kTrioWithSolo:
    case PlayType::kTrioWithPair:
      return (trio_comb_.trio_rank == other_move.trio_comb_.trio_rank &&
              kickers_ == other_move.kickers_);
    case PlayType::kChainOfSolo:
    case PlayType::kChainOfPair:
    case PlayType::kChainOfTrio:
      return chain_ == other_move.chain_;
    case PlayType::kPlaneWithSolo:
    case PlayType::kPlaneWithPair:
      return (plane_ == other_move.plane_ && kickers_ == other_move.kickers_);
    case PlayType::kQuadWithSolo:
    case PlayType::kQuadWithPair:
      return (quad_comb_ == other_move.quad_comb_ &&
              kickers_ == other_move.kickers_);
    case PlayType::kRocket:
      return true;

    default:
      FatalError("Should not reach here, move_type=" +
                 std::to_string(move_type_));
    }
  }
  }
  FatalError("Should not reach here.");
}
std::vector<int> DoudizhuMove::ToRanks() const {
  if (move_type_ != kPlay) {
    return {};
  }
  std::vector<int> ranks;
  switch (play_type_) {

  case PlayType::kInvalid:
  case PlayType::kPass:
    return {};
  case PlayType::kSolo:
  case PlayType::kPair:
  case PlayType::kTrio:
  case PlayType::kBomb: {
    for (int i = 0; i < single_rank_.num_cards; ++i) {
      ranks.push_back(single_rank_.rank);
    }
    return ranks;
  }
  case PlayType::kTrioWithSolo:
  case PlayType::kTrioWithPair: {
    for (int i = 0; i < kTrioLength; ++i) {
      ranks.push_back(trio_comb_.trio_rank);
    }
    for (const int k : kickers_) {
      ranks.push_back(k);
    }
    return ranks;
  }
  case PlayType::kChainOfSolo:
  case PlayType::kChainOfPair:
  case PlayType::kChainOfTrio: {
    for (int rank = chain_.start_rank; rank < chain_.start_rank + chain_.length;
         ++rank) {
      for (int i = 0; i < static_cast<int>(chain_.chain_type); ++i) {
        ranks.push_back(rank);
      }
    }
    return ranks;
  }
  case PlayType::kPlaneWithSolo:
  case PlayType::kPlaneWithPair: {
    for (int rank = plane_.start_rank; rank < plane_.start_rank + plane_.length;
         ++rank) {
      for (int i = 0; i < kTrioLength; ++i) {
        ranks.push_back(rank);
      }
    }
    for (const int k : kickers_) {
      ranks.push_back(k);
    }
    return ranks;
  }
  case PlayType::kQuadWithSolo:
  case PlayType::kQuadWithPair: {
    for (int i = 0; i < kQuadLength; ++i) {
      ranks.push_back(quad_comb_.quad_rank);
    }
    for (const int k : kickers_) {
      ranks.push_back(k);
    }
    return ranks;
  }

  case PlayType::kRocket:
    return {kBlackJoker, kRedJoker};
  default:
    FatalError("Should not reach here.");
  }
}
bool DoudizhuMove::IsValid(bool check_kickers) const {
  if (move_type_ == kInvalid) {
    return false;
  }
  if (move_type_ == kDeal) {
    return deal_card_.IsValid();
  }
  if (move_type_ == kAuction) {
    int bid_level = static_cast<int>(auction_type_);
    return bid_level >= static_cast<int>(AuctionType::kPass) &&
           bid_level <= static_cast<int>(AuctionType::kThree);
  }
  if (move_type_ == kPlay) {
    switch (play_type_) {

    case PlayType::kInvalid:
      return false;
    case PlayType::kPass:
      return true;
    case PlayType::kSolo:
      return single_rank_.num_cards == 1 && single_rank_.rank >= 0 &&
             single_rank_.rank < kNumRanks;
    case PlayType::kPair:
    case PlayType::kTrio:
    case PlayType::kBomb:
      return single_rank_.num_cards == static_cast<int>(play_type_) &&
             single_rank_.rank >= 0 && single_rank_.rank < kNumCardsPerSuit;
    case PlayType::kTrioWithSolo:
    case PlayType::kTrioWithPair: {
      bool is_valid =
          trio_comb_.trio_rank >= 0 &&
          trio_comb_.trio_rank < kNumCardsPerSuit &&
          trio_comb_.kicker_type != kUnknown &&
          kickers_.size() == static_cast<int>(trio_comb_.kicker_type);
      if (check_kickers) {
        bool is_kickers_valid = true;
        if (trio_comb_.kicker_type == kSolo) {
          is_kickers_valid &= kickers_[0] != trio_comb_.trio_rank;
        } else {
          is_kickers_valid &= !KickersContainBR(kickers_);
          for (const int k : kickers_) {
            is_kickers_valid &= k != trio_comb_.trio_rank;
          }
        }
        is_valid &= is_kickers_valid;
      }
      return is_valid;
    }
    case PlayType::kChainOfSolo:
    case PlayType::kChainOfPair:
    case PlayType::kChainOfTrio: {
      const auto [min_length, max_length] =
          MinAndMaxChainLength(chain_.chain_type);
      return chain_.chain_type != ChainType::kNotChain &&
             chain_.length >= min_length && chain_.length <= max_length &&
             chain_.start_rank >= 0 &&
             chain_.start_rank + chain_.length - 1 < kNumCardsPerSuit - 1;
    }
    case PlayType::kPlaneWithSolo: {
      bool is_valid = plane_.length >= kPlaneWithSoloMinLength &&
                      plane_.length <= kPlaneWithSoloMaxLength &&
                      kickers_.size() != kUnknown &&
                      kickers_.size() ==
                          static_cast<int>(plane_.kicker_type) * plane_.length;
      if (check_kickers) {
        // No rank in plane.
        for (const int k : kickers_) {
          is_valid &= !IsRankInPlane(plane_, k);
        }
        // BR.
        is_valid &= !KickersContainBR(kickers_);
        // Bomb.
        is_valid &= !HasBomb(kickers_);
        // Consecutive trios with start or end.
        const auto counter = kickers_;
        if (plane_.start_rank > 0) {
          is_valid &= (counter[plane_.start_rank - 1] != kTrioLength);
        }
        if (plane_.start_rank + plane_.length < kNumRanks - 1) {
          is_valid &=
              (counter[plane_.start_rank + plane_.length] != kTrioLength);
        }
      }
      return is_valid;
    }
    case PlayType::kPlaneWithPair: {
      bool is_valid =
          plane_.length >= kPlaneWithSoloMinLength &&
          plane_.length <= kPlaneWithSoloMaxLength &&
          plane_.kicker_type == kPair &&
          kickers_.size() == static_cast<int>(kPair) * plane_.length;
      if (check_kickers) {
        // No rank in plane.
        for (const int k : kickers_) {
          is_valid &= !IsRankInPlane(plane_, k);
        }
        const auto counter = kickers_;
        for (int rank = 0; rank < kNumRanks; ++rank) {
          if (rank > kNumCardsPerSuit) {
            // No jokers as kickers.
            is_valid &= counter[rank] == 0;
          } else {
            is_valid &= (counter[rank] == 0 || counter[rank] == 2);
          }
        }
      }
      return is_valid;
    }
    case PlayType::kQuadWithSolo: {
      bool is_valid = quad_comb_.quad_rank >= 0 &&
                      quad_comb_.quad_rank < kNumCardsPerSuit &&
                      quad_comb_.kicker_type != kUnknown &&
                      kickers_.size() == 2;
      if (check_kickers) {
        // No same rank as quad.
        for (const int k : kickers_) {
          is_valid &= k != quad_comb_.quad_rank;
        }
        // No BR in kickers.
        is_valid &= !KickersContainBR(kickers_);
      }
      return is_valid;
    }
    case PlayType::kQuadWithPair: {
      bool is_valid = quad_comb_.quad_rank >= 0 &&
                      quad_comb_.quad_rank < kNumCardsPerSuit &&
                      quad_comb_.kicker_type != kUnknown &&
                      kickers_.size() == 2 * kPairLength;
      if (check_kickers) {
        // No same rank as quad.
        for (const int k : kickers_) {
          is_valid &= k != quad_comb_.quad_rank;
        }
        // No BR in kickers.
        is_valid &= !KickersContainBR(kickers_);

        const auto counter = kickers_;
        for (int rank = 0; rank < kNumRanks; ++rank) {
          if (rank > kNumCardsPerSuit) {
            // No jokers as kickers.
            is_valid &= counter[rank] == 0;
          } else {
            is_valid &= (counter[rank] == 0 || counter[rank] == 2);
          }
        }
      }
      return is_valid;
    }

    case PlayType::kRocket:
      return true;
    default:
      FatalError("Should not reach here.");
    }
  }
  FatalError("Should not reach here");
}

std::vector<std::vector<int>> GetPossibleKickers(const TrioComb &trio_comb) {
  switch (trio_comb.kicker_type) {

  case kUnknown:
    return {};
  case kSolo: {
    std::vector<std::vector<int>> res{};
    for (int rank = 0; rank < kNumRanks; ++rank) {
      if (rank == trio_comb.trio_rank) {
        continue;
      }
      res.push_back({rank});
    }
    return res;
  }
  case kPair: {
    std::vector<std::vector<int>> res{};
    for (int rank = 0; rank < kNumCardsPerSuit; ++rank) {
      if (rank == trio_comb.trio_rank) {
        continue;
      }
      res.push_back({rank, rank});
    }
    return res;
  }
  default:
    FatalError("Should not reach here.");
  }
}

std::vector<std::vector<int>> GetPossibleKickers(const QuadComb &quad_comb) {
  switch (quad_comb.kicker_type) {

  case kUnknown:
    return {};
  case kSolo: {
    std::vector<std::vector<int>> kickers{};
    for (int rank = 0; rank < kNumRanks; ++rank) {
      if (rank == quad_comb.quad_rank) {
        continue;
      }
      for (int another_rank = rank; another_rank < kNumRanks; ++another_rank) {
        if (another_rank == quad_comb.quad_rank) {
          continue;
        }
        // Can't use rocket as kickers.
        if (rank + another_rank == kRedJoker + kBlackJoker) {
          continue;
        }

        // Each joker has only one card.
        if (rank == another_rank && rank >= kBlackJoker) {
          continue;
        }
        kickers.push_back(SortedCopy(std::vector<int>{rank, another_rank},
                                     /*ascending=*/true));
      }
    }
    return kickers;
  }
  case kPair: {
    std::vector<std::vector<int>> kickers{};
    for (int rank = 0; rank < kNumCardsPerSuit; ++rank) {
      if (rank == quad_comb.quad_rank) {
        continue;
      }
      for (int another_rank = rank + 1; another_rank < kNumCardsPerSuit;
           ++another_rank) {
        if (another_rank == quad_comb.quad_rank) {
          continue;
        }
        kickers.push_back(
            SortedCopy(std::vector<int>{rank, rank, another_rank, another_rank},
                       /*ascending=*/true));
      }
    }
    return kickers;
  }
  default:
    FatalError("Should not reach here.");
  }
}

std::vector<std::vector<int>> GetPossibleKickers(const Plane &plane) {
  switch (plane.kicker_type) {

  case kUnknown:
    return {};
  case kSolo: {
    // TODO: We are unclear with the encoding method used in RLCard, however we
    // keep consistent with it here.
    if (plane.length == 3) {
      // Three different cards.
      std::vector<int> remained_ranks{};
      for (int rank = 0; rank < kNumRanks; ++rank) {
        if (rank >= plane.start_rank &&
            rank < plane.start_rank + plane.length) {
          continue;
        }
        remained_ranks.push_back(rank);
      }
      const auto combs = Combine(remained_ranks, 3);
      std::vector<std::vector<int>> res = combs;
      // Three same cards.
      int end_rank = plane.start_rank + plane.length - 1;
      for (int rank = 0; rank < kNumCardsPerSuit; ++rank) {
        if (rank >= plane.start_rank && rank < plane.start_rank + plane.length)
          continue;
        if (rank == plane.start_rank -
                        1 // We can't have 3 card of rank like 444555666333
            || (rank == end_rank + 1 && rank != kBlackJoker - 1)) {
          continue;
        }
        res.push_back({rank, rank, rank});
      }

      // A pair with a solo.
      for (int pair_rank = 0; pair_rank < kNumCardsPerSuit; ++pair_rank) {
        if (pair_rank >= plane.start_rank &&
            pair_rank < plane.start_rank + plane.length) {
          continue;
        }
        for (int solo_rank = 0; solo_rank < kNumRanks; ++solo_rank) {
          if ((solo_rank >= plane.start_rank &&
               solo_rank < plane.start_rank + plane.length) ||
              solo_rank == pair_rank) {
            continue;
          }
          if (pair_rank > solo_rank) {
            res.push_back({solo_rank, pair_rank, pair_rank});
          } else {
            res.push_back({pair_rank, pair_rank, solo_rank});
          }
        }
      }
      auto contains_br = [](const std::vector<int> &comb) {
        return std::find(comb.begin(), comb.end(), kBlackJoker) != comb.end() &&
               std::find(comb.begin(), comb.end(), kRedJoker) != comb.end();
      };
      res.erase(std::remove_if(res.begin(), res.end(), contains_br), res.end());
      return res;
    }
    if (plane.length == 4) {

      std::vector<std::vector<int>> res{};
      // xyzB/R
      for (int x_rank = 0; x_rank < kNumCardsPerSuit; ++x_rank) {
        if (IsRankInPlane(plane, x_rank))
          continue;
        for (int y_rank = x_rank; y_rank < kNumCardsPerSuit; ++y_rank) {
          if (IsRankInPlane(plane, y_rank))
            continue;
          for (int z_rank = y_rank; z_rank < kNumCardsPerSuit; ++z_rank) {
            if (IsRankInPlane(plane, z_rank) ||
                ((z_rank == y_rank && y_rank == x_rank) &&
                 (y_rank == plane.start_rank - 1 ||
                  (y_rank == plane.start_rank + plane.length &&
                   plane.start_rank + plane.length < kNumCardsPerSuit - 1)))) {
              continue;
            }
            for (const int joker : {kBlackJoker, kRedJoker}) {
              res.push_back(
                  SortedCopy(std::vector<int>{x_rank, y_rank, z_rank, joker},
                             /*ascending=*/true));
            }
          }
        }
      }
      // abcd (not B/R)
      for (int a_rank = 0; a_rank < kNumCardsPerSuit; ++a_rank) {
        if (IsRankInPlane(plane, a_rank))
          continue;
        for (int b_rank = a_rank; b_rank < kNumCardsPerSuit; ++b_rank) {
          if (IsRankInPlane(plane, b_rank))
            continue;
          for (int c_rank = b_rank; c_rank < kNumCardsPerSuit; ++c_rank) {
            if (IsRankInPlane(plane, c_rank)) {
              continue;
            }
            for (int d_rank = c_rank; d_rank < kNumCardsPerSuit; ++d_rank) {
              if (IsRankInPlane(plane, d_rank) ||
                  (d_rank == c_rank && c_rank == b_rank &&
                   b_rank == a_rank)) // A bomb.
                continue;
              if ((plane.start_rank > 0 &&
                   (HasKElemEqualWithTarget({a_rank, b_rank, c_rank, d_rank}, 3,
                                            plane.start_rank - 1))) ||
                  (plane.start_rank + plane.length < kNumCardsPerSuit - 1 &&
                   HasKElemEqualWithTarget({a_rank, b_rank, c_rank, d_rank}, 3,
                                           plane.start_rank + plane.length))) {
                continue;
              }
              res.push_back(
                  SortedCopy(std::vector<int>{a_rank, b_rank, c_rank, d_rank},
                             /*ascending=*/true));
            }
          }
        }
      }
      return res;
    }
    if (plane.length == 5) {
      std::vector<std::vector<int>> res{};
      // abcdB/R
      for (int a_rank = 0; a_rank < kNumCardsPerSuit; ++a_rank) {
        if (IsRankInPlane(plane, a_rank))
          continue;
        for (int b_rank = a_rank; b_rank < kNumCardsPerSuit; ++b_rank) {
          if (IsRankInPlane(plane, b_rank))
            continue;
          for (int c_rank = b_rank; c_rank < kNumCardsPerSuit; ++c_rank) {
            if (IsRankInPlane(plane, c_rank))
              continue;
            for (int d_rank = c_rank; d_rank < kNumCardsPerSuit; ++d_rank) {
              if (IsRankInPlane(plane, d_rank))
                continue;
              if (a_rank == b_rank && b_rank == c_rank && c_rank == d_rank) {
                continue;
              }
              if ((plane.start_rank > 0 &&
                   (HasKElemEqualWithTarget({a_rank, b_rank, c_rank, d_rank}, 3,
                                            plane.start_rank - 1))) ||
                  (plane.start_rank + plane.length < kNumCardsPerSuit - 1 &&
                   HasKElemEqualWithTarget({a_rank, b_rank, c_rank, d_rank}, 3,
                                           plane.start_rank + plane.length))) {
                continue;
              }
              for (const int joker : {kBlackJoker, kRedJoker}) {
                res.push_back(
                    SortedCopy(std::vector<int>{a_rank, b_rank, c_rank, d_rank,
                                                joker}, /*ascending=*/
                               true));
              }
            }
          }
        }
      }

      // abcde (not B/R)
      for (int a_rank = 0; a_rank < kNumCardsPerSuit; ++a_rank) {
        if (IsRankInPlane(plane, a_rank))
          continue;
        for (int b_rank = a_rank; b_rank < kNumCardsPerSuit; ++b_rank) {
          if (IsRankInPlane(plane, b_rank))
            continue;
          for (int c_rank = b_rank; c_rank < kNumCardsPerSuit; ++c_rank) {
            if (IsRankInPlane(plane, c_rank))
              continue;
            for (int d_rank = c_rank; d_rank < kNumCardsPerSuit; ++d_rank) {
              if (IsRankInPlane(plane, d_rank))
                continue;
              for (int e_rank = d_rank; e_rank < kNumCardsPerSuit; ++e_rank) {
                if (IsRankInPlane(plane, e_rank))
                  continue;
                const std::vector<int> rank_comb = {a_rank, b_rank, c_rank,
                                                    d_rank, e_rank};
                if ((plane.start_rank > 0 &&
                     (HasKElemEqualWithTarget(rank_comb, 3,
                                              plane.start_rank - 1))) ||
                    (plane.start_rank + plane.length < kNumCardsPerSuit - 1 &&
                     HasKElemEqualWithTarget(
                         rank_comb, 3, plane.start_rank + plane.length))) {
                  continue;
                }
                if (HasBomb(rank_comb))
                  continue;
                res.push_back(SortedCopy(rank_comb, /*ascending=*/true));
              }
            }
          }
        }
      }
      return res;
    }
    std::vector<std::vector<int>> kickers{};
    std::vector<int> remained_ranks{};
    int end_rank = plane.start_rank + plane.length - 1;
    for (int rank = 0; rank < kNumRanks; ++rank) {
      if (rank >= plane.start_rank && rank <= end_rank) {
        continue;
      }
      for (int i = 0; i < std::min(kTrioLength, plane.length); ++i) {
        if (i == 2 &&
            (rank == plane.start_rank -
                         1 // We can't have 3 card of rank like 444555666333
             || (rank == end_rank + 1 && rank != kBlackJoker - 1))) {
          break;
        }
        remained_ranks.push_back(rank);
        // One joker for each.
        if (rank >= kBlackJoker) {
          break;
        }
      }
    }
    auto possible_combs = Combine(remained_ranks, plane.length);
    auto contains_br = [](const std::vector<int> &comb) {
      return std::find(comb.begin(), comb.end(), kBlackJoker) != comb.end() &&
             std::find(comb.begin(), comb.end(), kRedJoker) != comb.end();
    };
    possible_combs.erase(std::remove_if(possible_combs.begin(),
                                        possible_combs.end(), contains_br),
                         possible_combs.end());
    return possible_combs;
  }
  case kPair: {
    std::vector<std::vector<int>> kickers{};
    std::vector<int> remained_ranks{};
    for (int rank = 0; rank < kNumCardsPerSuit; ++rank) {
      if (rank >= plane.start_rank && rank < plane.start_rank + plane.length) {
        continue;
      }
      remained_ranks.push_back(rank);
    }

    const auto possible_combs = Combine(remained_ranks, plane.length);
    for (const auto &comb : possible_combs) {
      std::vector<int> temp{};
      for (const int r : comb) {
        temp.push_back(r);
        temp.push_back(r);
      }
      kickers.push_back(temp);
    }
    return kickers;
  }
  default:
    FatalError("Should not reach here");
  }
}

bool HandCanMakeMove(const DoudizhuHand &hand, const DoudizhuMove &move) {
  if (move.GetPlayType() == DoudizhuMove::PlayType::kPass)
    return true;
  const auto ranks = move.ToRanks();
  bool flag = true;
  auto hand_copy = hand;
  for (const int rank : ranks) {
    if (!hand_copy.CanRemove(rank)) {
      flag = false;
      break;
    }
    hand_copy.RemoveFromHand(rank);
  }
  return flag;
}

std::vector<int> PlayMoveStringToRanks(const std::string &play_move_str) {
  std::vector<int> ranks{};
  ranks.reserve(play_move_str.size());
  for (const auto &c : play_move_str) {
    ranks.push_back(RankCharToRank(c));
  }
  return ranks;
}

std::vector<int>
PlayMovesStringToSortedRanks(const std::string &play_move_str) {
  auto ranks = PlayMoveStringToRanks(play_move_str);
  std::sort(ranks.begin(), ranks.end());
  return ranks;
}

std::tuple<std::vector<int>, std::vector<int>>
GetTargetRanksAndKickers(const std::unordered_map<char, int> &m,
                         const int target) {
  CHECK_GE(target, kTrioLength);
  CHECK_LE(target, kQuadLength);
  std::vector<int> target_rank{};
  std::vector<int> kickers{};
  for (const auto &kv : m) {
    if (kv.second == target &&
        RankCharToRank(kv.first) <= kChainAndPlaneMaxRank) {
      target_rank.push_back(RankCharToRank(kv.first));
    } else {
      for (int i = 0; i < kv.second; ++i) {
        kickers.push_back(RankCharToRank(kv.first));
      }
    }
  }
  if (!IsVectorConsecutive(SortedCopy(target_rank))) {
    const int rank = FindNonContinuousNumber(target_rank);
    target_rank.erase(std::remove(target_rank.begin(), target_rank.end(), rank),
                      target_rank.end());
    for (int i = 0; i < m.at(kRankChar[rank]); ++i) {
      kickers.push_back(rank);
    }
  }
  return std::make_tuple(target_rank, SortedCopy(kickers));
}

DoudizhuMove GetChainMoveFromString(const std::string &move_str,
                                    const int num_ranks,
                                    const ChainType chain_type) {
  auto sorted_ranks = PlayMovesStringToSortedRanks(move_str);
  auto last = std::unique(sorted_ranks.begin(), sorted_ranks.end());
  sorted_ranks.erase(last, sorted_ranks.end());
  CHECK_TRUE(IsVectorConsecutive(sorted_ranks));
  const int start_rank = sorted_ranks[0];
  CHECK_GE(start_rank, 0);
  CHECK_LE(start_rank, kChainAndPlaneMaxRank - num_ranks + 1);
  return DoudizhuMove{/*chain=*/
                      Chain{/*chain_type=*/chain_type, /*length=*/num_ranks,
                            /*start_rank=*/start_rank}};
}

DoudizhuMove GetMoveFromString(const std::string &move_str,
                               DoudizhuMove::Type move_type) {
  if (move_type == DoudizhuMove::kInvalid) {
    return {};
  }
  if (move_type == DoudizhuMove::kDeal) {
    CHECK_LT(move_str.length(), 2);
    // A deal move should be a card including suit and rank or a joker.
    if (move_str.length() == 1) {
      if (std::toupper(move_str[0]) == 'R') {
        return DoudizhuMove{/*deal_card=*/DoudizhuCard{/*rank=*/kRedJoker,
                                                       /*suit=*/kInvalidSuit}};
      }
      if (std::toupper(move_str[0]) == 'B') {
        return DoudizhuMove{/*deal_card=*/DoudizhuCard{/*rank=*/kBlackJoker,
                                                       /*suit=*/kInvalidSuit}};
      }
      FatalError("Deal move " + move_str + " cannot be converted to a move.");
    }
    if (move_str.length() == 2) {
      Suit suit = SuitCharToSuit(move_str[0]);
      int rank = RankCharToRank(move_str[1]);
      return DoudizhuMove{
          /*deal_card=*/DoudizhuCard{/*rank=*/rank, /*suit=*/suit}};
    }
  }
  if (move_type == DoudizhuMove::kAuction) {
    CHECK_GT(move_str.length(), 0);
    if (std::toupper(move_str[0]) == 'P') {
      return DoudizhuMove{/*auction_type=*/DoudizhuMove::AuctionType::kPass};
    }
    const int bid = move_str[0] - '0';
    CHECK_GE(bid, 1);
    CHECK_LE(bid, kNumBids);
    return DoudizhuMove{/*auction_type=*/DoudizhuMove::AuctionType(bid)};
  }
  if (move_type == DoudizhuMove::kPlay) {
    if (std::toupper(move_str[0]) == 'P') {
      return DoudizhuMove{/*play_type=*/DoudizhuMove::PlayType::kPass};
    }
    const auto counter = GetStringCounter(move_str);
    const int num_cards = static_cast<int>(move_str.size());
    int num_ranks = static_cast<int>(counter.size());
    std::vector<char> keys{};
    std::vector<int> values{};
    std::vector<int> ranks{};
    keys.reserve(counter.size());
    values.reserve(counter.size());
    ranks.reserve(counter.size());
    for (const auto &kv : counter) {
      keys.push_back(kv.first);
      values.push_back(kv.second);
      ranks.push_back(RankCharToRank(kv.first));
    }
    if (num_cards == 1) {
      // Single card.
      const SingleRank single_rank{/*r=*/ranks[0], /*n=*/1};
      return DoudizhuMove{/*single_rank=*/single_rank};
    }
    if (num_cards == 2) {
      // Pair or joker.
      if (num_ranks == 1) {
        // Pair.
        return DoudizhuMove{
            /*single_rank=*/SingleRank{/*r=*/ranks[0], /*n=*/2}};
      }
      if (num_ranks == 2) {
        // Should be rockets.
        CHECK_TRUE(counter.count('B'));
        CHECK_TRUE(counter.count('R'));
        return DoudizhuMove{/*play_type*/ DoudizhuMove::PlayType::kRocket};
      }
      FatalError("Should not reach here.");
    }
    if (num_cards == 3) {
      // Trio.
      CHECK_EQ(num_ranks, 1);
      CHECK_GE(ranks[0], 0);
      CHECK_LT(ranks[0], kBlackJoker);
      return DoudizhuMove{/*single_rank=*/SingleRank{/*r=*/ranks[0], /*n=*/3}};
    }
    if (num_cards == 4) {
      // Trio with solo or bomb.
      if (num_ranks == 1) {
        // Bomb.
        CHECK_GE(ranks[0], 0);
        CHECK_LT(ranks[0], kBlackJoker);
        return DoudizhuMove{
            /*single_rank=*/SingleRank{/*r=*/ranks[0], /*n=*/4}};
      } else {
        // Trio with solo.
        CHECK_EQ(num_ranks, 2);
        int trio_rank, kicker_rank;
        if (values[0] == kTrioLength) {
          trio_rank = ranks[0];
          kicker_rank = ranks[1];
        } else {
          trio_rank = ranks[1];
          kicker_rank = ranks[0];
        }
        CHECK_LT(trio_rank, kBlackJoker);
        return DoudizhuMove{
            /*trio_comb=*/TrioComb{/*kt=*/kSolo, /*tr=*/trio_rank},
            /*kickers=*/RanksToCounter({kicker_rank})};
      }
    }
    if (num_cards == 5) {
      // Trio with pair or chain of solo.
      if (num_ranks == 2) {
        int trio_rank, kicker_rank;
        if (values[0] == kTrioLength) {
          trio_rank = ranks[0];
          kicker_rank = ranks[1];
        } else {
          trio_rank = ranks[1];
          kicker_rank = ranks[0];
        }
        CHECK_LT(trio_rank, kBlackJoker);
        CHECK_LT(kicker_rank, kBlackJoker);
        return DoudizhuMove{
            /*trio_comb=*/TrioComb{/*kt=*/kPair, /*tr=*/trio_rank},
            /*kickers=*/RanksToCounter({kicker_rank, kicker_rank})};
      } else {
        const auto sorted_ranks = PlayMovesStringToSortedRanks(move_str);
        CHECK_TRUE(IsVectorConsecutive(sorted_ranks));
        const int start_rank = sorted_ranks[0];
        CHECK_GE(start_rank, 0);
        CHECK_LE(start_rank, kChainAndPlaneMaxRank - num_ranks + 1);
        return DoudizhuMove{
            /*chain=*/
            Chain{/*chain_type=*/ChainType::kSolo, /*length=*/num_ranks,
                  /*start_rank=*/start_rank}};
      }
    }
    if (num_cards == 6) {
      // Chain of pair, Chain of solo, Chain of Trio, quad with solo.
      if (num_ranks == 6) {
        // Chain of solo.
        const auto sorted_ranks = PlayMovesStringToSortedRanks(move_str);
        CHECK_TRUE(IsVectorConsecutive(sorted_ranks));
        const int start_rank = sorted_ranks[0];
        CHECK_GE(start_rank, 0);
        CHECK_LE(start_rank, kChainAndPlaneMaxRank - num_ranks + 1);
        return DoudizhuMove{
            /*chain=*/
            Chain{/*chain_type=*/ChainType::kSolo, /*length=*/num_ranks,
                  /*start_rank=*/start_rank}};
      }
      if (num_ranks == 3) {
        // Chain of pair or quad with solo.
        if (values == std::vector<int>{2, 2, 2}) {
          // Chain of pair.
          CHECK_TRUE(IsVectorConsecutive(ranks));
          const int start_rank = SortedCopy(ranks)[0];
          CHECK_GE(start_rank, 0);
          CHECK_LE(start_rank, kChainAndPlaneMaxRank - num_ranks + 1);
          return DoudizhuMove{
              /*chain=*/
              Chain{/*chain_type=*/ChainType::kPair, /*length=*/num_ranks,
                    /*start_rank=*/start_rank}};
        }
        const auto sorted_values = SortedCopy(values, /*ascending=*/true);
        if (sorted_values != std::vector<int>{1, 1, 4}) {
          FatalError("Can not convert move string " + move_str + " to a move.");
        }
        int quad_rank;
        std::vector<int> kickers{};
        for (const auto &kv : counter) {
          if (kv.second == kQuadLength) {
            quad_rank = RankCharToRank(kv.first);
          } else {
            kickers.push_back(RankCharToRank(kv.first));
          }
        }
        CHECK_LT(quad_rank, kBlackJoker);
        return DoudizhuMove{
            /*quad_comb=*/QuadComb{/*kt=*/kSolo, /*qr*/ quad_rank},
            /*kickers=*/RanksToCounter(kickers)};
      }
      if (num_ranks == 2) {
        // Quad with solo or chain of trio.
        if (values[0] == 3) {
          // chain of trio.
          CHECK_EQ(values[1], 3);
          const auto sorted_ranks = PlayMovesStringToSortedRanks(move_str);
          CHECK_TRUE(IsVectorConsecutive(SortedCopy(ranks)));
          const int start_rank = sorted_ranks[0];
          CHECK_GE(start_rank, 0);
          CHECK_LE(start_rank, kChainAndPlaneMaxRank - num_ranks + 1);
          return DoudizhuMove{
              /*chain=*/
              Chain{/*chain_type=*/ChainType::kTrio, /*length=*/num_ranks,
                    /*start_rank=*/start_rank}};
        }
        const auto sorted_values = SortedCopy(values, /*ascending=*/true);
        if (sorted_values != std::vector<int>{2, 4}) {
          FatalError("Can not convert move string " + move_str + " to a move.");
        }
        int quad_rank;
        std::vector<int> kickers{};
        for (const auto &kv : counter) {
          if (kv.second == kQuadLength) {
            quad_rank = RankCharToRank(kv.first);
          } else {
            kickers.push_back(RankCharToRank(kv.first));
            kickers.push_back(RankCharToRank(kv.first));
          }
        }
        CHECK_LT(quad_rank, kBlackJoker);
        return DoudizhuMove{
            /*quad_comb=*/QuadComb{/*kt=*/kSolo, /*qr*/ quad_rank},
            /*kickers=*/RanksToCounter(kickers)};
      }
    }
    if (num_cards == 7) {
      // Only chain of solo.
      CHECK_EQ(num_ranks, num_cards);
      const auto sorted_ranks = PlayMovesStringToSortedRanks(move_str);
      CHECK_TRUE(IsVectorConsecutive(sorted_ranks));
      const int start_rank = sorted_ranks[0];
      CHECK_GE(start_rank, 0);
      CHECK_LE(start_rank, kChainAndPlaneMaxRank - num_ranks + 1);
      return DoudizhuMove{
          /*chain=*/
          Chain{/*chain_type=*/ChainType::kSolo, /*length=*/num_ranks,
                /*start_rank=*/start_rank}};
    }
    if (num_cards == 8) {
      // Chain of solo, chain of pair, plane with solo, quad with pair.
      if (num_ranks == num_cards) {
        // Chain of solo.
        const auto sorted_ranks = PlayMovesStringToSortedRanks(move_str);
        CHECK_TRUE(IsVectorConsecutive(sorted_ranks));
        const int start_rank = sorted_ranks[0];
        CHECK_GE(start_rank, 0);
        CHECK_LE(start_rank, kChainAndPlaneMaxRank - num_ranks + 1);
        return DoudizhuMove{
            /*chain=*/
            Chain{/*chain_type=*/ChainType::kSolo, /*length=*/num_ranks,
                  /*start_rank=*/start_rank}};
      }
      if (num_ranks == 4) {
        // Chain of pair or plane with solo.
        const auto sorted_values = SortedCopy(values, /*ascending=*/true);
        if (sorted_values == std::vector<int>{2, 2, 2, 2}) {
          const int start_rank = SortedCopy(ranks)[0];
          CHECK_GE(start_rank, 0);
          CHECK_LE(start_rank, kChainAndPlaneMaxRank - num_ranks + 1);
          return DoudizhuMove{
              /*chain=*/
              Chain{/*chain_type=*/ChainType::kPair, /*length=*/num_ranks,
                    /*start_rank=*/start_rank}};
        }

        if (sorted_values == std::vector<int>{1, 1, 3, 3}) {
          // Plane with solo.
          const auto [trio_ranks, kickers] =
              GetTargetRanksAndKickers(counter, kTrioLength);
          const auto sorted_ranks = SortedCopy(trio_ranks);
          CHECK_TRUE(IsVectorConsecutive(sorted_ranks));
          const int start_rank = sorted_ranks[0];
          return DoudizhuMove{
              /*plane=*/Plane{/*kt*/ kSolo, /*l=*/2, /*sr=*/start_rank},
              /*kickers=*/RanksToCounter(kickers)};
        }
        FatalError("Can not convert move string " + move_str + " to a move.");
      }
      if (num_ranks == 3) {
        // Quad with pair ot plane with solo.
        const auto sorted_values = SortedCopy(values, /*ascending=*/true);
        if (sorted_values == std::vector<int>{2, 2, 4}) {
          // Quad with pair.
          int quad_rank;
          std::vector<int> kickers{};
          for (const auto &kv : counter) {
            if (kv.second == kQuadLength) {
              quad_rank = RankCharToRank(kv.first);
            } else {
              for (int i = 0; i < kPairLength; ++i) {
                kickers.push_back(RankCharToRank(kv.first));
              }
            }
          }
          CHECK_EQ(kickers.size(), kPairLength * 2);
          CHECK_LT(quad_rank, kBlackJoker);
          return DoudizhuMove{
              /*quad_comb=*/QuadComb{/*kt=*/kPair, /*qr*/ quad_rank},
              /*kickers=*/RanksToCounter(kickers)};
        }
        if (sorted_values == std::vector<int>{2, 3, 3}) {
          const auto [trio_ranks, kickers] =
              GetTargetRanksAndKickers(counter, kTrioLength);
          const auto sorted_ranks = SortedCopy(trio_ranks);
          CHECK_TRUE(IsVectorConsecutive(sorted_ranks));
          const int start_rank = sorted_ranks[0];
          return DoudizhuMove{
              /*plane=*/Plane{/*kt*/ kSolo, /*l=*/2, /*sr=*/start_rank},
              /*kickers=*/RanksToCounter(kickers)};
        }
        FatalError("Can not convert move string " + move_str + " to a move.");
      }
      FatalError("Can not convert move string " + move_str + " to a move.");
    }
    if (num_cards == 9) {
      // Chain of trio, Chain of solo.
      if (num_ranks == 3) {
        const auto [trio_ranks, _] =
            GetTargetRanksAndKickers(counter, kTrioLength);
        const auto sorted_ranks = SortedCopy(trio_ranks, /*ascending=*/true);
        CHECK_TRUE(IsVectorConsecutive(sorted_ranks));
        return DoudizhuMove{/*chain=*/
                            Chain{/*chain_type=*/ChainType::kTrio, /*length=*/3,
                                  /*start_rank=*/sorted_ranks[0]}};
      }
      if (num_ranks == 9) {
        return GetChainMoveFromString(move_str, num_ranks, ChainType::kSolo);
      }
      FatalError("Can not convert move string " + move_str + " to a move.");
    }
    if (num_cards == 10) {
      // Chain of solo or chain of pair or plane with pair.
      if (num_ranks == num_cards) {
        const auto sorted_ranks = PlayMovesStringToSortedRanks(move_str);
        CHECK_TRUE(IsVectorConsecutive(sorted_ranks));
        const int start_rank = sorted_ranks[0];
        CHECK_GE(start_rank, 0);
        CHECK_LE(start_rank, kChainAndPlaneMaxRank - num_ranks + 1);
        return DoudizhuMove{
            /*chain=*/
            Chain{/*chain_type=*/ChainType::kSolo, /*length=*/num_ranks,
                  /*start_rank=*/start_rank}};
      }
      if (num_ranks == 5) {
        const int start_rank = SortedCopy(ranks)[0];
        CHECK_GE(start_rank, 0);
        CHECK_LE(start_rank, kChainAndPlaneMaxRank - num_ranks + 1);
        return DoudizhuMove{
            /*chain=*/
            Chain{/*chain_type=*/ChainType::kPair, /*length=*/num_ranks,
                  /*start_rank=*/start_rank}};
      }
      if (num_ranks == 4) {
        // Plane with pair.
        auto [trio_ranks, kickers] =
            GetTargetRanksAndKickers(counter, kTrioLength);
        std::sort(trio_ranks.begin(), trio_ranks.end());
        return DoudizhuMove{
            /*plane=*/Plane{/*kt=*/kPair, /*l=*/2, /*sr=*/trio_ranks[0]},
            /*kickers=*/RanksToCounter(kickers)};
      }
      FatalError("Can not convert move string " + move_str + " to a move.");
    }
    if (num_cards == 11) {
      // Can only be a chain of solo.
      CHECK_EQ(num_cards, num_ranks);
      return GetChainMoveFromString(move_str, num_ranks, ChainType::kSolo);
    }
    if (num_cards == 12) {
      // Chain of solo, chain of pair, chain of trio, plane with solo.
      if (num_ranks == num_cards) {
        // Chain of solo.
        return GetChainMoveFromString(move_str, num_ranks, ChainType::kSolo);
      }
      if (num_ranks == 6) {
        // Chain of pair.
        const std::vector<int> required_counts = {2, 2, 2, 2, 2, 2};
        if (values == required_counts) {
          return GetChainMoveFromString(move_str, num_ranks, ChainType::kPair);
        }
      }
      const auto [trio_ranks, _] =
          GetTargetRanksAndKickers(counter, kTrioLength);
      auto sorted_trio_ranks = SortedCopy(trio_ranks);
      if (num_ranks == 4 && IsVectorConsecutive(sorted_trio_ranks) &&
          sorted_trio_ranks.size() == 4 &&
          sorted_trio_ranks.back() <= kChainAndPlaneMaxRank) {
        // Chain of trio.
        return GetChainMoveFromString(move_str, num_ranks, ChainType::kTrio);
      }
      // Should be Plane with solo.
      //      auto sorted_ranks = PlayMovesStringToSortedRanks(move_str);
      //      auto last = std::unique(sorted_ranks.begin(), sorted_ranks.end());
      //      sorted_ranks.erase(last, sorted_ranks.end());
      std::vector<int> sorted_plane_ranks;
      if (IsVectorConsecutive(std::vector<int>(
              sorted_trio_ranks.begin(), sorted_trio_ranks.begin() + 3))) {
        sorted_plane_ranks = std::vector<int>(sorted_trio_ranks.begin(),
                                              sorted_trio_ranks.begin() + 3);
      } else {
        sorted_plane_ranks = std::vector<int>(sorted_trio_ranks.begin() + 1,
                                              sorted_trio_ranks.begin() + 4);
      }

      //      auto plane_ranks = std::vector<int>(sorted_ranks.begin(),
      //      sorted_ranks.begin() + 3);
      CHECK_TRUE(IsVectorConsecutive(sorted_plane_ranks));
      std::vector<int> kickers{};
      for (const auto &kv : counter) {
        if (RankCharToRank(kv.first) < sorted_plane_ranks[0] ||
            RankCharToRank(kv.first) > sorted_plane_ranks.back()) {
          for (int i = 0; i < kv.second; ++i) {
            kickers.push_back(RankCharToRank(kv.first));
          }
        }
      }
      CHECK_EQ(kickers.size(), 3);
      return DoudizhuMove{
          /*plane=*/Plane{/*kt=*/kSolo, /*l=*/3,
                          /*sr=*/sorted_plane_ranks[0]}, /*kickers=*/
          RanksToCounter(kickers)};
    }
    if (num_cards == 14) {
      // Only chain of pair.
      return GetChainMoveFromString(move_str, /*num_ranks=*/7,
                                    /*chain_type=*/ChainType::kPair);
    }
    if (num_cards == 15) {
      // Chain of trio or plane with pair.
      if (num_ranks == 5) {
        // Chain of trio.
        return GetChainMoveFromString(move_str, /*num_ranks=*/5,
                                      /*chain_type=*/ChainType::kTrio);
      }
      CHECK_EQ(num_ranks, 6);
      const auto [trio_ranks, kickers] =
          GetTargetRanksAndKickers(counter, /*target=*/kTrioLength);
      return DoudizhuMove{
          /*plane=*/Plane{/*kt=*/kPair, /*l=*/3,
                          /*sr=*/SortedCopy(trio_ranks)[0]}, /*kickers=*/
          RanksToCounter(kickers)};
    }
    if (num_cards == 16) {
      // Chain of pair ot plane with solo.
      const std::vector<int> required_chain_of_pair_values(8, kPairLength);
      if (num_ranks == 8 && values == required_chain_of_pair_values) {
        return GetChainMoveFromString(move_str, /*num_ranks=*/num_ranks,
                                      /*chain_type=*/ChainType::kPair);
      }
      const auto [trio_ranks, kickers] =
          GetTargetRanksAndKickers(counter, /*target=*/kTrioLength);
      CHECK_EQ(trio_ranks.size(), 4);
      CHECK_EQ(kickers.size(), 4);
      CHECK_TRUE(IsVectorConsecutive(SortedCopy(trio_ranks)));
      return DoudizhuMove{/*plane=*/Plane{/*kt=*/kSolo, /*l=*/4,
                                          /*sr=*/SortedCopy(trio_ranks)[0]},
                          /*kickers=*/RanksToCounter(kickers)};
    }
    if (num_cards == 18) {
      // Chain of pair or chain of trio.
      if (num_ranks == num_cards / kPairLength) {
        // Chain of pair.
        return GetChainMoveFromString(/*move_str=*/move_str,
                                      /*num_ranks=*/num_ranks,
                                      /*chain_type=*/ChainType::kPair);
      }
      // Chain of trio.
      CHECK_EQ(num_ranks, num_cards / kTrioLength);
      return GetChainMoveFromString(/*move_str=*/move_str,
                                    /*num_ranks=*/num_ranks,
                                    /*chain_type=*/ChainType::kTrio);
    }
    if (num_cards == 20) {
      // Chain of pair, plane with solo, plane with pair.
      if (num_ranks == num_cards / (kTrioLength + kPairLength) * 2) {
        // Plane with pair.
        const int plane_length = num_cards / (kTrioLength + kPairLength);
        const auto [trio_ranks, kickers] =
            GetTargetRanksAndKickers(counter, /*target=*/kTrioLength);
        if (trio_ranks.size() == plane_length) {
          CHECK_TRUE(IsVectorConsecutive(SortedCopy(trio_ranks)));
          CHECK_EQ(trio_ranks.size(), plane_length);
          CHECK_EQ(kickers.size(), plane_length * kPairLength);
          return DoudizhuMove{/*plane=*/Plane{/*kt=*/kPair, /*l=*/plane_length,
                                              /*sr=*/SortedCopy(trio_ranks)[0]},
                              /*kickers=*/RanksToCounter(kickers)};
        }
      }
      const int chain_length = num_cards / kPairLength;
      const std::vector<int> required_chain_of_pair_values(
          num_cards / kPairLength, kPairLength);
      if (num_ranks == chain_length &&
          values == required_chain_of_pair_values) {
        // Chain of pair.
        return GetChainMoveFromString(/*move_str=*/move_str,
                                      /*num_ranks=*/num_ranks,
                                      /*chain_type=*/ChainType::kPair);
      }
      // Should be Plane with solo.
      const int plane_length = num_cards / (kTrioLength + 1);
      const auto [trio_ranks, kickers] =
          GetTargetRanksAndKickers(counter, /*target=*/kTrioLength);
      CHECK_TRUE(IsVectorConsecutive(SortedCopy(trio_ranks)));
      CHECK_EQ(trio_ranks.size(), plane_length);
      CHECK_EQ(kickers.size(), plane_length);
      return DoudizhuMove{/*plane=*/Plane{/*kt=*/kSolo, /*l=*/plane_length,
                                          /*sr=*/SortedCopy(trio_ranks)[0]},
                          /*kickers=*/RanksToCounter(kickers)};
    }
    FatalError("Should not reach here.");
  }
  FatalError("move_type " + std::to_string(move_type) + "is wrong");
}

std::ostream &operator<<(std::ostream &os, const DoudizhuMove &move) {
  os << move.ToString();
  return os;
}
} // namespace doudizhu_learning_env
