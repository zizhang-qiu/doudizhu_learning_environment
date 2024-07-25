//
// Created by qzz on 2024/6/2.
//

#include "doudizhu_move.h"
#include <iostream>
#include <numeric>
#include <set>

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
    for (int rank = 0; rank < kNumRanks; ++rank) {
      for (int i = 0; i < kickers_[rank]; ++i) {
        ranks.push_back(rank);
      }
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
    for (int rank = 0; rank < kNumRanks; ++rank) {
      for (int i = 0; i < kickers_[rank]; ++i) {
        ranks.push_back(rank);
      }
    }
    return ranks;
  }
  case PlayType::kQuadWithSolo:
  case PlayType::kQuadWithPair: {
    for (int i = 0; i < kQuadLength; ++i) {
      ranks.push_back(quad_comb_.quad_rank);
    }
    for (int rank = 0; rank < kNumRanks; ++rank) {
      for (int i = 0; i < kickers_[rank]; ++i) {
        ranks.push_back(rank);
      }
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
      const int kickers_count =
          std::accumulate(kickers_.begin(), kickers_.end(), 0);
      bool is_valid = trio_comb_.trio_rank >= 0 &&
                      trio_comb_.trio_rank < kNumCardsPerSuit &&
                      trio_comb_.kicker_type != kUnknown &&
                      kickers_count == static_cast<int>(trio_comb_.kicker_type);
      if (check_kickers) {
        if (trio_comb_.kicker_type == kSolo) {
          return kickers_[trio_comb_.trio_rank] == 0;
        }
        if (KickersContainBR(kickers_)) {
          return false;
        }
        bool has_pair = false;
        for (int rank = 0; rank < kNumRanks; ++rank) {
          if (kickers_[rank] == kPairLength) {
            has_pair = true;
          }
          if (kickers_[rank] != 0 && kickers_[rank] != kPairLength) {
            return false;
          }
        }
        return has_pair;
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
      const int kickers_count =
          std::accumulate(kickers_.begin(), kickers_.end(), 0);
      bool is_valid =
          plane_.length >= kPlaneWithSoloMinLength &&
          plane_.length <= kPlaneWithSoloMaxLength &&
          plane_.kicker_type == kSolo &&
          kickers_count == static_cast<int>(plane_.kicker_type) * plane_.length;
      if (check_kickers) {
        // No rank in plane.
        for (int rank = 0; rank < kNumRanks; ++rank) {
          if (kickers_[rank] > 0) {
            is_valid &= !IsRankInPlane(plane_, rank);
          }
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
      const int kickers_count =
          std::accumulate(kickers_.begin(), kickers_.end(), 0);
      bool is_valid = plane_.length >= kPlaneWithSoloMinLength &&
                      plane_.length <= kPlaneWithSoloMaxLength &&
                      plane_.kicker_type == kPair &&
                      kickers_count == static_cast<int>(kPair) * plane_.length;
      if (check_kickers) {
        // No rank in plane.
        for (int rank = 0; rank < kNumRanks; ++rank) {
          if (kickers_[rank] > 0) {
            is_valid &= !IsRankInPlane(plane_, rank);
          }
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
      const int kickers_count =
          std::accumulate(kickers_.begin(), kickers_.end(), 0);
      bool is_valid = quad_comb_.quad_rank >= 0 &&
                      quad_comb_.quad_rank < kNumCardsPerSuit &&
                      quad_comb_.kicker_type == kSolo && kickers_count == 2;
      if (check_kickers) {
        // No same rank as quad.
        for (int rank = 0; rank < kNumRanks; ++rank) {
          if (kickers_[rank] > 0) {
            is_valid &= rank != quad_comb_.quad_rank;
          }
        }
        // No BR in kickers.
        is_valid &= !KickersContainBR(kickers_);
      }
      return is_valid;
    }
    case PlayType::kQuadWithPair: {
      const int kickers_count =
          std::accumulate(kickers_.begin(), kickers_.end(), 0);
      bool is_valid = quad_comb_.quad_rank >= 0 &&
                      quad_comb_.quad_rank < kNumCardsPerSuit &&
                      quad_comb_.kicker_type == kPair &&
                      kickers_.size() == 2 * kPairLength;
      if (check_kickers) {
        // No same rank as quad.
        for (int rank = 0; rank < kNumRanks; ++rank) {
          if (kickers_[rank] > 0) {
            is_valid &= rank != quad_comb_.quad_rank;
          }
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
            // 333444555666777B/R is not allowed.
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

// Transform a play move string to ranks, this function will ignore wrong
// characters.
std::vector<int> PlayMoveStringToRanks(const std::string &play_move_str) {
  std::vector<int> ranks{};
  ranks.reserve(play_move_str.size());
  for (const auto &c : play_move_str) {
    const int rank = RankCharToRank(c);
    if (rank != -1) {
      ranks.push_back(rank);
    }
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
GetTargetRanksAndKickers(const std::array<int, kNumRanks> &arr,
                         const int target) {
  CHECK_GE(target, kTrioLength);
  CHECK_LE(target, kQuadLength);
  std::vector<int> target_rank{};
  std::vector<int> kickers{};
  for (int rank = 0; rank < kNumRanks; ++rank) {
    if (arr[rank] == target && rank <= kChainAndPlaneMaxRank) {
      target_rank.push_back(rank);
    } else {
      for (int i = 0; i < arr[rank]; ++i) {
        kickers.push_back(rank);
      }
    }
  }
  if (!IsVectorConsecutive(SortedCopy(target_rank))) {
    const int rank = FindNonContinuousNumber(target_rank);
    target_rank.erase(std::remove(target_rank.begin(), target_rank.end(), rank),
                      target_rank.end());
    for (int i = 0; i < arr[rank]; ++i) {
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

// Transform a play move string to an array counter.
// This function will ignore wrong characters.
std::array<int, kNumRanks>
PlayMoveStringToArray(const std::string &play_move_str) {
  std::array<int, kNumRanks> res{};
  for (const char c : play_move_str) {
    const int rank = RankCharToRank(c);
    if (rank != -1) {
      ++res[rank];
    }
  }
  return res;
}

DoudizhuMove GetDealMoveFromString(std::string_view move_str) {
  // A deal move should have length le 2, e.g. 'B' or 'C2'.
  if (move_str.length() > 2) {
    return {};
  }
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
    return {};
  }
  if (move_str.length() == 2) {
    Suit suit = SuitCharToSuit(move_str[0]);
    int rank = RankCharToRank(move_str[1]);
    if (suit == kInvalidSuit || rank == -1) {
      return {};
    }
    return DoudizhuMove{
        /*deal_card=*/DoudizhuCard{/*rank=*/rank, /*suit=*/suit}};
  }
  return {};
}

DoudizhuMove GetAuctionMoveFromString(std::string_view move_str) {
  // Use 'P(p)' to identify pass.
  if (std::toupper(move_str[0]) == 'P') {
    // Pass.
    return DoudizhuMove{/*auction_type=*/DoudizhuMove::AuctionType::kPass};
  }

  // Otherwise, the string should be a character of {1, 2, 3}.
  const int bid = move_str[0] - '0';
  if (bid < 1 || bid > kNumBids) {
    return {};
  }

  return DoudizhuMove{/*auction_type=*/DoudizhuMove::AuctionType(bid)};
}

bool IsChain(const std::vector<int> &unique_sorted_ranks,
             const std::vector<int> &count_per_rank, const int num_ranks,
             const ChainType chain_type) {
  if (!IsVectorConsecutive(unique_sorted_ranks)) {
    return false;
  }
  if (unique_sorted_ranks[0] > kChainAndPlaneMaxRank - num_ranks + 1) {
    return false;
  }
  std::vector<int> expected_count;
  switch (chain_type) {
  case ChainType::kNotChain:
    break;
  case ChainType::kSolo:
    for (int i = 0; i < num_ranks; ++i) {
      expected_count.push_back(1);
    }
    break;
  case ChainType::kPair:
    for (int i = 0; i < num_ranks; ++i) {
      expected_count.push_back(kPairLength);
    }
    break;
  case ChainType::kTrio:
    for (int i = 0; i < num_ranks; ++i) {
      expected_count.push_back(kTrioLength);
    }
    break;
  }
  return expected_count == count_per_rank;
}

DoudizhuMove
GetChainOfSoloMoveFromRanks(const std::vector<int> &sorted_unique_ranks,
                            const int num_ranks) {
  if (!IsVectorConsecutive(sorted_unique_ranks)) {
    return {};
  }
  const int start_rank = sorted_unique_ranks[0];
  if (start_rank > kChainAndPlaneMaxRank - num_ranks + 1) {
    return {};
  }
  return DoudizhuMove{
      /*chain=*/
      Chain{/*chain_type=*/ChainType::kSolo, /*length=*/num_ranks,
            /*start_rank=*/start_rank}};
}

DoudizhuMove
GetChainOfPairMoveFromRanks(const std::vector<int> &sorted_unique_ranks,
                            const int num_ranks) {
  if (!IsVectorConsecutive(sorted_unique_ranks)) {
    return {};
  }
  const int start_rank = sorted_unique_ranks[0];
  if (start_rank > kChainAndPlaneMaxRank - num_ranks + 1) {
    return {};
  }
  return DoudizhuMove{
      /*chain=*/
      Chain{/*chain_type=*/ChainType::kPair, /*length=*/num_ranks,
            /*start_rank=*/start_rank}};
}

DoudizhuMove GetChainMoveFromRanks(const std::vector<int> &sorted_unique_ranks,
                                   const int num_ranks, ChainType chain_type) {
  if (!IsVectorConsecutive(sorted_unique_ranks)) {
    return {};
  }
  const int start_rank = sorted_unique_ranks[0];
  if (start_rank > kChainAndPlaneMaxRank - num_ranks + 1) {
    return {};
  }
  return DoudizhuMove{/*chain=*/
                      Chain{/*chain_type=*/chain_type, /*length=*/num_ranks,
                            /*start_rank=*/start_rank}};
}

DoudizhuMove GetPlayMoveFromString(const std::string &move_str) {
  // Pass. Use 'P(p)' to identify pass.
  if (std::toupper(move_str[0]) == 'P') {
    return DoudizhuMove{/*play_type=*/DoudizhuMove::PlayType::kPass};
  }

  // Ranks in the move, e.g., {3334}.
  const std::vector<int> ranks = PlayMoveStringToRanks(move_str);
  const auto sorted_ranks = SortedCopy(ranks);
  // Number of cards played in the move.
  const int num_cards = static_cast<int>(ranks.size());
  // Number of ranks used in the move.
  const int num_ranks = CountUniqueElements(ranks);
  const auto move_array = PlayMoveStringToArray(move_str);
  std::vector<int> count_per_rank;
  for (int rank = 0; rank < kNumRanks; ++rank) {
    if (move_array[rank]) {
      count_per_rank.push_back(move_array[rank]);
    }
  }

  // Unique ranks.
  std::vector<int> sorted_unique_ranks;
  for (int rank = 0; rank < kNumRanks; ++rank) {
    if (move_array[rank] > 0) {
      sorted_unique_ranks.push_back(rank);
    }
  }

  switch (num_cards) {
  case 1: {
    // Single card.
    if (ranks[0] < 0 || ranks[0] > kNumRanks) {
      return {};
    }
    const SingleRank single_rank{/*r=*/ranks[0], /*n=*/1};
    return DoudizhuMove{/*single_rank=*/single_rank};
  }

  case 2: {
    // Pair or joker.
    if (num_ranks == 1) {
      // Pair.
      if (ranks[0] < 0 || ranks[0] >= kBlackJoker) {
        return {};
      }
      return DoudizhuMove{/*single_rank=*/SingleRank{/*r=*/ranks[0], /*n=*/2}};
    }
    if (num_ranks == 2) {
      // Should be rockets.
      if (!(move_array[kBlackJoker] == 1 && move_array[kRedJoker] == 1)) {
        return {};
      }
      return DoudizhuMove{/*play_type*/ DoudizhuMove::PlayType::kRocket};
    }
  }

  case 3: {
    // Trio.
    if (num_ranks != 1 || (ranks[0] < 0 || ranks[0] >= kBlackJoker)) {
      return {};
    }
    return DoudizhuMove{/*single_rank=*/SingleRank{/*r=*/ranks[0], /*n=*/3}};
  }

  case 4: {
    // Trio with solo or bomb.
    if (num_ranks == 1) {
      // Bomb.
      if (ranks[0] < 0 || ranks[0] >= kBlackJoker) {
        return {};
      }
      return DoudizhuMove{/*single_rank=*/SingleRank{/*r=*/ranks[0], /*n=*/4}};
    } else {
      // Trio with solo.
      if (num_ranks != 2) {
        return {};
      }
      int trio_rank, kicker_rank;
      // Find trio rank and kicker rank.
      const auto ranks_found = FindIndices(move_array, {kTrioLength, 1});
      if (ranks_found.at(kTrioLength).size() != 1 ||
          ranks_found.at(1).size() != 1) {
        // The number of ranks for trio and kicker should be 1.
        return {};
      }
      trio_rank = ranks_found.at(kTrioLength)[0];
      kicker_rank = ranks_found.at(1)[0];
      if (trio_rank >= kBlackJoker) {
        return {};
      }
      return DoudizhuMove{
          /*trio_comb=*/TrioComb{/*kt=*/kSolo, /*tr=*/trio_rank},
          /*kickers=*/RanksToCounter({kicker_rank})};
    }
  }

  case 5: {
    // Trio with pair or chain of solo.
    if (num_ranks == 2) {
      // Find trio rank and kicker rank.
      const auto ranks_found = FindIndices(move_array, {kTrioLength, 2});
      int trio_rank, kicker_rank;
      if (ranks_found.at(kTrioLength).size() != 1 ||
          ranks_found.at(2).size() != 1) {
        // The number of ranks for trio and kicker should be 1.
        return {};
      }
      trio_rank = ranks_found.at(kTrioLength)[0];
      kicker_rank = ranks_found.at(2)[0];
      if (trio_rank >= kBlackJoker || kicker_rank >= kBlackJoker) {
        return {};
      }
      return DoudizhuMove{
          /*trio_comb=*/TrioComb{/*kt=*/kPair, /*tr=*/trio_rank},
          /*kickers=*/RanksToCounter({kicker_rank, kicker_rank})};
    } else {
      // Chain of solo.
      return GetChainMoveFromRanks(sorted_unique_ranks, num_ranks,
                                   ChainType::kSolo);
    }
  }

  case 6: {
    if (num_ranks == 6) {
      return GetChainMoveFromRanks(sorted_unique_ranks, num_ranks,
                                   ChainType::kSolo);
    }
    if (num_ranks == 3) {
      if (IsChain(sorted_unique_ranks, count_per_rank, num_ranks,
                  ChainType::kPair)) {
        return GetChainMoveFromRanks(sorted_unique_ranks, num_ranks,
                                     ChainType::kPair);
      }

      // Quad with solo (without repetition).
      if (const auto sorted_counts = SortedCopy(count_per_rank);
          sorted_counts != std::vector<int>{1, 1, kQuadLength}) {
        return {};
      }
      int quad_rank;
      std::vector<int> kickers{};
      const auto indices_found = FindIndices(move_array, {kQuadLength, 1});
      if (indices_found.at(kQuadLength).size() != 1 ||
          (indices_found.at(1).size() != 2)) {
        return {};
      }
      quad_rank = indices_found.at(kQuadLength)[0];
      kickers = indices_found.at(1);
      return DoudizhuMove{
          /*quad_comb=*/QuadComb{/*kt=*/kSolo, /*qr*/ quad_rank},
          /*kickers=*/RanksToCounter(kickers)};
    }
    if (num_ranks == 2) {
      if (IsChain(sorted_unique_ranks, count_per_rank, num_ranks,
                  ChainType::kTrio)) {
        return GetChainMoveFromRanks(sorted_unique_ranks, num_ranks,
                                     ChainType::kTrio);
      }
      // Quad with solo (repeated kickers).
      const auto sorted_count_per_rank =
          SortedCopy(count_per_rank, /*ascending=*/true);
      if (sorted_count_per_rank != std::vector<int>{2, kQuadLength}) {
        return {};
      }
      int quad_rank;
      std::vector<int> kickers{};
      const auto indices_found = FindIndices(move_array, {kQuadLength, 2});
      if (indices_found.at(kQuadLength).size() != 1 ||
          indices_found.at(2).size() != 1) {
        return {};
      }
      quad_rank = indices_found.at(kQuadLength)[0];
      if (quad_rank >= kBlackJoker) {
        return {};
      }
      kickers = {indices_found.at(2)[0], indices_found.at(2)[0]};
      return DoudizhuMove{
          /*quad_comb=*/QuadComb{/*kt=*/kSolo, /*qr*/ quad_rank},
          /*kickers=*/RanksToCounter(kickers)};
    }
    return {};
  }

  case 7: {
    if (!IsChain(sorted_unique_ranks, count_per_rank, num_ranks,
                 ChainType::kSolo)) {
      return {};
    }
    return GetChainMoveFromRanks(sorted_unique_ranks, num_ranks,
                                 ChainType::kSolo);
  }

  case 8: {
    // Chain of solo.
    if (IsChain(sorted_unique_ranks, count_per_rank, num_ranks,
                ChainType::kSolo)) {
      return GetChainMoveFromRanks(sorted_unique_ranks, num_ranks,
                                   ChainType::kSolo);
    }
    if (num_ranks == 4) {
      if (IsChain(sorted_unique_ranks, count_per_rank, num_ranks,
                  ChainType::kPair)) {
        return GetChainMoveFromRanks(sorted_unique_ranks, num_ranks,
                                     ChainType::kPair);
      }
      const auto sorted_values = SortedCopy(count_per_rank, /*ascending=*/true);
      if (sorted_values == std::vector<int>{1, 1, 3, 3}) {
        const auto [trio_ranks, kickers] =
            GetTargetRanksAndKickers(move_array, kTrioLength);
        const int start_rank = trio_ranks[0];
        return DoudizhuMove{
            /*plane=*/Plane{/*kt*/ kSolo, /*l=*/2, /*sr=*/start_rank},
            /*kickers=*/RanksToCounter(kickers)};
      }
    }
    if (num_ranks == 3) {
      const auto sorted_values = SortedCopy(count_per_rank, /*ascending=*/true);
      if (sorted_values == std::vector<int>{2, 2, 4}) {
        const auto ranks_found =
            FindIndices(move_array, {kQuadLength, kPairLength});
        if (ranks_found.at(kQuadLength).size() != 1 ||
            ranks_found.at(kPairLength).size() != 2) {
          return {};
        }
        const int quad_rank = ranks_found.at(kQuadLength)[0];
        auto kicker_ranks = ranks_found.at(kPairLength);
        std::array<int, kNumRanks> kickers{};
        for (int r : kicker_ranks) {
          kickers[r] += kPairLength;
        }
        if (quad_rank >= kBlackJoker) {
          return {};
        }
        return DoudizhuMove{
            /*quad_comb=*/QuadComb{/*kt=*/kPair, /*qr*/ quad_rank},
            /*kickers=*/kickers};
      }

      if (sorted_values == std::vector<int>{2, 3, 3}) {
        const auto [trio_ranks, kickers] =
            GetTargetRanksAndKickers(move_array, kTrioLength);
        const int start_rank = trio_ranks[0];
        return DoudizhuMove{
            /*plane=*/Plane{/*kt*/ kSolo, /*l=*/2, /*sr=*/start_rank},
            /*kickers=*/RanksToCounter(kickers)};
      }
    }
    return {};
  }

  case 9: {
    if (IsChain(sorted_unique_ranks, count_per_rank, num_ranks,
                ChainType::kSolo)) {
      return GetChainMoveFromRanks(sorted_unique_ranks, num_ranks,
                                   ChainType::kSolo);
    }

    if (num_ranks == 3) {
      if (IsChain(sorted_unique_ranks, count_per_rank, num_ranks,
                  ChainType::kTrio)) {
        return GetChainMoveFromRanks(sorted_unique_ranks, num_ranks,
                                     ChainType::kTrio);
      }
    }

    return {};
  }

  case 10: {
    if (IsChain(sorted_unique_ranks, count_per_rank, num_ranks,
                ChainType::kSolo)) {
      return GetChainMoveFromRanks(sorted_unique_ranks, num_ranks,
                                   ChainType::kSolo);
    }

    if (num_ranks == 5) {
      if (IsChain(sorted_unique_ranks, count_per_rank, num_ranks,
                  ChainType::kPair)) {
        return GetChainMoveFromRanks(sorted_unique_ranks, num_ranks,
                                     ChainType::kPair);
      }
      return {};
    }

    // Plane with pair.
    if (num_ranks == 4) {
      if (const auto sorted_count = SortedCopy(count_per_rank);
          sorted_count != std::vector<int>{2, 2, 3, 3}) {
        return {};
      }
      auto [trio_ranks, kickers] =
          GetTargetRanksAndKickers(move_array, kTrioLength);
      return DoudizhuMove{
          /*plane=*/Plane{/*kt=*/kPair, /*l=*/2, /*sr=*/trio_ranks[0]},
          /*kickers=*/RanksToCounter(kickers)};
    }
    return {};
  }

  case 11: {
    if (IsChain(sorted_unique_ranks, count_per_rank, num_ranks,
                ChainType::kSolo)) {
      return GetChainMoveFromRanks(sorted_unique_ranks, num_ranks,
                                   ChainType::kSolo);
    }
    return {};
  }

  case 12: {
    if (IsChain(sorted_unique_ranks, count_per_rank, num_ranks,
                ChainType::kSolo)) {
      return GetChainMoveFromRanks(sorted_unique_ranks, num_ranks,
                                   ChainType::kSolo);
    }
    if (num_ranks == 6) {
      if (IsChain(sorted_unique_ranks, count_per_rank, num_ranks,
                  ChainType::kPair)) {
        return GetChainMoveFromRanks(sorted_unique_ranks, num_ranks,
                                     ChainType::kPair);
      }
    }
    if (num_ranks == 4) {
      if (IsChain(sorted_unique_ranks, count_per_rank, num_ranks,
                  ChainType::kTrio)) {
        return GetChainMoveFromRanks(sorted_unique_ranks, num_ranks,
                                     ChainType::kTrio);
      }
    }

    auto [trio_ranks, kickers] =
        GetTargetRanksAndKickers(move_array, kTrioLength);
    if (trio_ranks.size() != 3) {
      return {};
    }

    return DoudizhuMove{
        /*plane=*/Plane{/*kt=*/kSolo, /*l=*/3, /*sr=*/trio_ranks[0]},
        /*kickers=*/RanksToCounter(kickers)};
  }

  case 14: {
    return GetChainMoveFromRanks(sorted_unique_ranks, num_ranks,
                                 ChainType::kPair);
  }

  case 15: {
    if (IsChain(sorted_unique_ranks, count_per_rank, num_ranks,
                ChainType::kTrio)) {
      return GetChainMoveFromRanks(sorted_unique_ranks, num_ranks,
                                   ChainType::kTrio);
    }
    auto [trio_ranks, kickers] =
        GetTargetRanksAndKickers(move_array, kTrioLength);
    if (trio_ranks.size() != 3) {
      return {};
    }

    return DoudizhuMove{
        /*plane=*/Plane{/*kt=*/kPair, /*l=*/3, /*sr=*/trio_ranks[0]},
        /*kickers=*/RanksToCounter(kickers)};
  }

  case 16: {
    if (IsChain(sorted_unique_ranks, count_per_rank, num_ranks,
                ChainType::kPair)) {
      return GetChainMoveFromRanks(sorted_unique_ranks, num_ranks,
                                   ChainType::kPair);
    }
    auto [trio_ranks, kickers] =
        GetTargetRanksAndKickers(move_array, kTrioLength);
    if (trio_ranks.size() != 4) {
      return {};
    }

    return DoudizhuMove{
        /*plane=*/Plane{/*kt=*/kSolo, /*l=*/4, /*sr=*/trio_ranks[0]},
        /*kickers=*/RanksToCounter(kickers)};
  }

  case 18: {
    if (IsChain(sorted_unique_ranks, count_per_rank, num_ranks,
                ChainType::kPair)) {
      return GetChainMoveFromRanks(sorted_unique_ranks, num_ranks,
                                   ChainType::kPair);
    }
    return GetChainMoveFromRanks(sorted_unique_ranks, num_ranks,
                                 ChainType::kTrio);
  }

  case 20: {
    if (IsChain(sorted_unique_ranks, count_per_rank, num_ranks,
                ChainType::kPair)) {
      return GetChainMoveFromRanks(sorted_unique_ranks, num_ranks,
                                   ChainType::kPair);
    }

    const auto [trio_ranks, kickers] =
        GetTargetRanksAndKickers(move_array, /*target=*/kTrioLength);
    // Plane with pair.
    if (trio_ranks.size() == 4) {
      if (kickers.size() != 4 * 2) {
        return {};
      }
      return DoudizhuMove{/*plane=*/Plane{/*kt=*/kPair, /*l=*/4,
                                          /*sr=*/SortedCopy(trio_ranks)[0]},
                          /*kickers=*/RanksToCounter(kickers)};
    }
    if (trio_ranks.size() == 5) {
      if (kickers.size() != 5 * 1) {
        return {};
      }
      return DoudizhuMove{/*plane=*/Plane{/*kt=*/kSolo, /*l=*/5,
                                          /*sr=*/SortedCopy(trio_ranks)[0]},
                          /*kickers=*/RanksToCounter(kickers)};
    }
    return {};
  }
  default:
    return {};
  }
}

DoudizhuMove GetMoveFromString(const std::string &move_str,
                               DoudizhuMove::Type move_type) {
  // Return invalid move for an empty string.
  if (move_str.empty()) {
    return {};
  }
  if (move_type == DoudizhuMove::kInvalid) {
    return {};
  }

  // Deal.
  if (move_type == DoudizhuMove::kDeal) {
    return GetDealMoveFromString(move_str);
  }

  // Auction.
  if (move_type == DoudizhuMove::kAuction) {
    return GetAuctionMoveFromString(move_str);
  }

  // Play.
  if (move_type == DoudizhuMove::kPlay) {
    return GetPlayMoveFromString(move_str);
  }

  return {};
}

std::ostream &operator<<(std::ostream &os, const DoudizhuMove &move) {
  os << move.ToString();
  return os;
}
} // namespace doudizhu_learning_env
