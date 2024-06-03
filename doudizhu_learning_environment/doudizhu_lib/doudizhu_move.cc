//
// Created by qzz on 2024/6/2.
//

#include "doudizhu_move.h"
namespace doudizhu_learning_env {

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
}
