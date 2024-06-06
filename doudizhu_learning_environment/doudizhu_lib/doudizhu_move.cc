//
// Created by qzz on 2024/6/2.
//

#include "doudizhu_move.h"
namespace doudizhu_learning_env {
std::pair<int, int> MinAndMaxChainLength(const ChainType chain_type) {
  switch (chain_type) {
    case ChainType::kNotChain:return {-1, -1};
    case ChainType::kSolo:return {kChainOfSoloMinLength, kChainOfSoloMaxLength};
    case ChainType::kPair:return {kChainOfPairMinLength, kChainOfPairMaxLength};
    case ChainType::kTrio:return {kChainOfTrioMinLength, kChainOfTrioMaxLength};
    default:FatalError("Should not reach here.");
  }
}

bool KickersContainBR(const std::vector<int> &kickers) {
  return std::find(kickers.begin(), kickers.end(), kBlackJoker) != kickers.end()
      && std::find(kickers.begin(), kickers.end(), kRedJoker) != kickers.end();
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

std::array<uint8_t, kNumRanks> KickersToCounter(const std::vector<int> &kickers) {
  std::array<uint8_t, kNumRanks> counter{};
  for (const int k : kickers) {
    ++counter[k];
  }
  return counter;
}

std::string DoudizhuMove::ToString() const {
  switch (move_type_) {
    case kInvalid:return "I";
    case kDeal:return "(Deal " + deal_card_.ToString() + ")";
    case kAuction:
      if (auction_type_ == AuctionType::kPass) {
        return "(Pass)";
      }
      return "(Bid " + std::to_string(static_cast<int>(auction_type_)) + ")";
    case kPlay: {
      std::string rv{"(Play "};
      switch (play_type_) {
        case PlayType::kPass:return "(Pass)";
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
          for (const int kicker : kickers_) {
            rv.push_back(kRankChar[kicker]);
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
          for (const int kicker : kickers_) {
            rv.push_back(kRankChar[kicker]);
          }
          return rv + ")";
        }
        case PlayType::kQuadWithSolo:
        case PlayType::kQuadWithPair: {
          for (int i = 0; i < kQuadLength; ++i) {
            rv.push_back(kRankChar[quad_comb_.quad_rank]);
          }
          for (const int kicker : kickers_) {
            rv.push_back(kRankChar[kicker]);
          }
          return rv + ")";
        }

        case PlayType::kRocket:return rv + "BR)";
        default:FatalError("Should not reach here.");
      }
    }
  }
  FatalError("Should not reach here.");
}
bool DoudizhuMove::operator==(const DoudizhuMove &other_move) const {
  switch (move_type_) {
    case kInvalid:return true;
    case kDeal:return deal_card_ == other_move.deal_card_;
    case kAuction: {
      return auction_type_ == other_move.auction_type_;
    }
    case kPlay: {
      if (play_type_ != other_move.play_type_) {
        return false;
      }
      switch (play_type_) {
        case PlayType::kSolo:
        case PlayType::kPair:
        case PlayType::kTrio:
        case PlayType::kBomb:return single_rank_ == other_move.single_rank_;
        case PlayType::kTrioWithSolo:
        case PlayType::kTrioWithPair:
          return (trio_comb_.trio_rank == other_move.trio_comb_.trio_rank && kickers_ == other_move.kickers_);
        case PlayType::kChainOfSolo:
        case PlayType::kChainOfPair:
        case PlayType::kChainOfTrio:return chain_ == other_move.chain_;
        case PlayType::kPlaneWithSolo:
        case PlayType::kPlaneWithPair:
          return (plane_ == other_move.plane_
              && kickers_ == other_move.kickers_);
        case PlayType::kQuadWithSolo:
        case PlayType::kQuadWithPair:return (quad_comb_ == other_move.quad_comb_ && kickers_ == other_move.kickers_);

        default:FatalError("Should not reach here.");
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
    case PlayType::kPass:return {};
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
    }
    case PlayType::kChainOfSolo:
    case PlayType::kChainOfPair:
    case PlayType::kChainOfTrio: {
      for (int rank = chain_.start_rank; rank < chain_.start_rank + chain_.length; ++rank) {
        for (int i = 0; i < static_cast<int>(chain_.chain_type); ++i) {
          ranks.push_back(rank);
        }
      }
      return ranks;
    }
    case PlayType::kPlaneWithSolo:
    case PlayType::kPlaneWithPair: {
      for (int rank = plane_.start_rank; rank < plane_.start_rank + plane_.length; ++rank) {
        for (int i = 0; i < kTrioLength; ++i) {
          ranks.push_back(rank);
        }
      }
      for (const int k : kickers_) {
        ranks.push_back(k);
      }
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

    case PlayType::kRocket:return {kBlackJoker, kRedJoker};
    default:FatalError("Should not reach here.");
  }
}
bool DoudizhuMove::IsValid(bool check_kickers) const {
  if (move_type_ == kInvalid) { return false; }
  if (move_type_ == kDeal) { return deal_card_.IsValid(); }
  if (move_type_ == kAuction) {
    int bid_level = static_cast<int>(auction_type_);
    return bid_level >= static_cast<int>(AuctionType::kPass) &&
        bid_level <= static_cast<int>(AuctionType::kThree);
  }
  if (move_type_ == kPlay) {
    switch (play_type_) {

      case PlayType::kInvalid:return false;
      case PlayType::kPass:return true;
      case PlayType::kSolo:
        return single_rank_.num_cards == 1
            && single_rank_.rank >= 0 && single_rank_.rank < kNumRanks;
      case PlayType::kPair:
      case PlayType::kTrio:
      case PlayType::kBomb:
        return single_rank_.num_cards == static_cast<int>(play_type_)
            && single_rank_.rank >= 0 && single_rank_.rank < kNumCardsPerSuit;
      case PlayType::kTrioWithSolo:
      case PlayType::kTrioWithPair: {
        bool is_valid = trio_comb_.trio_rank >= 0 && trio_comb_.trio_rank < kNumCardsPerSuit
            && trio_comb_.kicker_type != kUnknown && kickers_.size() == static_cast<int>(trio_comb_.kicker_type);
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
        const auto [min_length, max_length] = MinAndMaxChainLength(chain_.chain_type);
        return chain_.chain_type != ChainType::kNotChain && chain_.length >= min_length
            && chain_.length <= max_length && chain_.start_rank >= 0
            && chain_.start_rank + chain_.length - 1 < kNumCardsPerSuit - 1;
      }
      case PlayType::kPlaneWithSolo: {
        bool is_valid = plane_.length >= kPlaneWithSoloMinLength && plane_.length <= kPlaneWithSoloMaxLength
            && kickers_.size() != kUnknown
            && kickers_.size() == static_cast<int>(plane_.kicker_type) * plane_.length;
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
          const auto counter = KickersToCounter(kickers_);
          if (plane_.start_rank > 0) {
            is_valid &= (counter[plane_.start_rank - 1] != kTrioLength);
          }
          if (plane_.start_rank + plane_.length < kNumRanks - 1) {
            is_valid &= (counter[plane_.start_rank + plane_.length] != kTrioLength);
          }
        }
        return is_valid;
      }
      case PlayType::kPlaneWithPair: {
        bool is_valid = plane_.length >= kPlaneWithSoloMinLength && plane_.length <= kPlaneWithSoloMaxLength
            && plane_.kicker_type == kPair
            && kickers_.size() == static_cast<int>(kPair) * plane_.length;
        if (check_kickers) {
          // No rank in plane.
          for (const int k : kickers_) {
            is_valid &= !IsRankInPlane(plane_, k);
          }
          const auto counter = KickersToCounter(kickers_);
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
        bool is_valid = quad_comb_.quad_rank >= 0 && quad_comb_.quad_rank < kNumCardsPerSuit
            && quad_comb_.kicker_type != kUnknown
            && kickers_.size() == 2;
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
        bool is_valid = quad_comb_.quad_rank >= 0 && quad_comb_.quad_rank < kNumCardsPerSuit
            && quad_comb_.kicker_type != kUnknown
            && kickers_.size() == 2 * kPairLength;
        if (check_kickers) {
          // No same rank as quad.
          for (const int k : kickers_) {
            is_valid &= k != quad_comb_.quad_rank;
          }
          // No BR in kickers.
          is_valid &= !KickersContainBR(kickers_);

          const auto counter = KickersToCounter(kickers_);
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

      case PlayType::kRocket:return true;
      default:FatalError("Should not reach here.");
    }
  }
  FatalError("Should not reach here");
}

std::vector<std::vector<int>> GetPossibleKickers(const TrioComb &trio_comb) {
  switch (trio_comb.kicker_type) {

    case kUnknown:return {};
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
    default:FatalError("Should not reach here.");
  }
}

std::vector<std::vector<int>> GetPossibleKickers(const QuadComb &quad_comb) {
  switch (quad_comb.kicker_type) {

    case kUnknown:return {};
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
          kickers.push_back({rank, another_rank});
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
        for (int another_rank = rank + 1; another_rank < kNumCardsPerSuit; ++another_rank) {
          if (another_rank == quad_comb.quad_rank) {
            continue;
          }
          kickers.push_back({rank, rank, another_rank, another_rank});
        }
      }
      return kickers;
    }
    default:FatalError("Should not reach here.");
  }
}

std::vector<std::vector<int>> GetPossibleKickers(const Plane &plane) {
  switch (plane.kicker_type) {

    case kUnknown:return {};
    case kSolo: {
      // TODO: We are unclear with the encoding method used in RLCard, however we keep consistent with it here.
      if (plane.length == 3) {
        // Three different cards.
        std::vector<int> remained_ranks{};
        for (int rank = 0; rank < kNumRanks; ++rank) {
          if (rank >= plane.start_rank && rank < plane.start_rank + plane.length) {
            continue;
          }
          remained_ranks.push_back(rank);
        }
        const auto combs = Combine(remained_ranks, 3);
        std::vector<std::vector<int>> res = combs;
        // Three same cards.
        int end_rank = plane.start_rank + plane.length - 1;
        for (int rank = 0; rank < kNumCardsPerSuit; ++rank) {
          if (rank >= plane.start_rank && rank < plane.start_rank + plane.length)continue;
          if (rank == plane.start_rank - 1 // We can't have 3 card of rank like 444555666333
              || (rank == end_rank + 1 && rank != kBlackJoker - 1)) {
            continue;
          }
          res.push_back({rank, rank, rank});
        }

        // A pair with a solo.
        for (int pair_rank = 0; pair_rank < kNumCardsPerSuit; ++pair_rank) {
          if (pair_rank >= plane.start_rank && pair_rank < plane.start_rank + plane.length) {
            continue;
          }
          for (int solo_rank = 0; solo_rank < kNumRanks; ++solo_rank) {
            if ((solo_rank >= plane.start_rank && solo_rank < plane.start_rank + plane.length)
                || solo_rank == pair_rank) {
              continue;
            }
            res.push_back({pair_rank, pair_rank, solo_rank});
          }
        }
        auto contains_br = [](const std::vector<int> &comb) {
          return std::find(comb.begin(), comb.end(), kBlackJoker) != comb.end() &&
              std::find(comb.begin(), comb.end(), kRedJoker) != comb.end();
        };
        res.erase(std::remove_if(res.begin(), res.end(), contains_br),
                  res.end());
        return res;
      }
      if (plane.length == 4) {

        std::vector<std::vector<int>> res{};
        // xyzB/R
        for (int x_rank = 0; x_rank < kNumCardsPerSuit; ++x_rank) {
          if (IsRankInPlane(plane, x_rank)) continue;
          for (int y_rank = x_rank; y_rank < kNumCardsPerSuit; ++y_rank) {
            if (IsRankInPlane(plane, y_rank)) continue;
            for (int z_rank = y_rank; z_rank < kNumCardsPerSuit; ++z_rank) {
              if (IsRankInPlane(plane, z_rank) ||
                  ((z_rank == y_rank && y_rank == x_rank)
                      && (y_rank == plane.start_rank - 1 || (y_rank == plane.start_rank + plane.length
                          && plane.start_rank + plane.length < kNumCardsPerSuit - 1)))) {
                continue;
              }
              for (const int joker : {kBlackJoker, kRedJoker}) {
                res.push_back({x_rank, y_rank, z_rank, joker});
              }
            }
          }
        }
        // abcd (not B/R)
        for (int a_rank = 0; a_rank < kNumCardsPerSuit; ++a_rank) {
          if (IsRankInPlane(plane, a_rank)) continue;
          for (int b_rank = a_rank; b_rank < kNumCardsPerSuit; ++b_rank) {
            if (IsRankInPlane(plane, b_rank)) continue;
            for (int c_rank = b_rank; c_rank < kNumCardsPerSuit; ++c_rank) {
              if (IsRankInPlane(plane, c_rank)) {
                continue;
              }
              for (int d_rank = c_rank; d_rank < kNumCardsPerSuit; ++d_rank) {
                if (IsRankInPlane(plane, d_rank)
                    || (d_rank == c_rank && c_rank == b_rank && b_rank == a_rank)) // A bomb.
                  continue;
                if ((plane.start_rank > 0 &&
                    (HasKElemEqualWithTarget({a_rank, b_rank, c_rank, d_rank}, 3, plane.start_rank - 1)))
                    || (plane.start_rank + plane.length < kNumCardsPerSuit - 1 &&
                        HasKElemEqualWithTarget({a_rank, b_rank, c_rank, d_rank}, 3,
                                                plane.start_rank + plane.length))) {
                  continue;
                }
                res.push_back({a_rank, b_rank, c_rank, d_rank});
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
          if (IsRankInPlane(plane, a_rank))continue;
          for (int b_rank = a_rank; b_rank < kNumCardsPerSuit; ++b_rank) {
            if (IsRankInPlane(plane, b_rank)) continue;
            for (int c_rank = b_rank; c_rank < kNumCardsPerSuit; ++c_rank) {
              if (IsRankInPlane(plane, c_rank)) continue;
              for (int d_rank = c_rank; d_rank < kNumCardsPerSuit; ++d_rank) {
                if (IsRankInPlane(plane, d_rank)) continue;
                if (a_rank == b_rank && b_rank == c_rank && c_rank == d_rank) {
                  continue;
                }
                if ((plane.start_rank > 0 &&
                    (HasKElemEqualWithTarget({a_rank, b_rank, c_rank, d_rank}, 3, plane.start_rank - 1)))
                    || (plane.start_rank + plane.length < kNumCardsPerSuit - 1 &&
                        HasKElemEqualWithTarget({a_rank, b_rank, c_rank, d_rank}, 3,
                                                plane.start_rank + plane.length))) {
                  continue;
                }
                for (const int joker : {kBlackJoker, kRedJoker}) {
                  res.push_back({a_rank, b_rank, c_rank, d_rank, joker});
                }
              }
            }
          }
        }

        // abcde (not B/R)
        for (int a_rank = 0; a_rank < kNumCardsPerSuit; ++a_rank) {
          if (IsRankInPlane(plane, a_rank))continue;
          for (int b_rank = a_rank; b_rank < kNumCardsPerSuit; ++b_rank) {
            if (IsRankInPlane(plane, b_rank)) continue;
            for (int c_rank = b_rank; c_rank < kNumCardsPerSuit; ++c_rank) {
              if (IsRankInPlane(plane, c_rank)) continue;
              for (int d_rank = c_rank; d_rank < kNumCardsPerSuit; ++d_rank) {
                if (IsRankInPlane(plane, d_rank)) continue;
                for (int e_rank = d_rank; e_rank < kNumCardsPerSuit; ++e_rank) {
                  if (IsRankInPlane(plane, e_rank)) continue;
                  const std::vector<int> rank_comb = {a_rank, b_rank, c_rank, d_rank, e_rank};
                  if ((plane.start_rank > 0 &&
                      (HasKElemEqualWithTarget(rank_comb, 3, plane.start_rank - 1)))
                      || (plane.start_rank + plane.length < kNumCardsPerSuit - 1 &&
                          HasKElemEqualWithTarget(rank_comb, 3,
                                                  plane.start_rank + plane.length))) {
                    continue;
                  }
                  if (HasBomb(rank_comb))continue;
                  res.push_back(rank_comb);
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
          if (i == 2
              && (rank == plane.start_rank - 1 // We can't have 3 card of rank like 444555666333
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
      possible_combs.erase(std::remove_if(possible_combs.begin(), possible_combs.end(), contains_br),
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
    default:FatalError("Should not reach here");
  }

}
bool HandCanMakeMove(const DoudizhuHand &hand, const DoudizhuMove &move) {
  if (move.GetPlayType() == DoudizhuMove::PlayType::kPass)return true;
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
}
