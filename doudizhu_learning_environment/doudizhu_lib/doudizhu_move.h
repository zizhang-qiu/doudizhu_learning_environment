//
// Created by qzz on 2024/6/2.
//

#ifndef DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_DOUDIZHU_MOVE_H_
#define DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_DOUDIZHU_MOVE_H_
#include "doudizhu_hand.h"
#include <algorithm>

namespace doudizhu_learning_env {
// Solo, pair, trio and bomb.
struct SingleRank {
  int rank = -1;
  int num_cards = 0;
  SingleRank(int r, int n) : rank(r), num_cards(n) {
  }
  SingleRank() = default;
  bool operator==(const SingleRank &other) const {
    return rank == other.rank && num_cards == other.num_cards;
  }
};

enum class ChainType {
  kNotChain = -1,
  kSolo = 1,
  kPair,
  kTrio
};

// Chain of solo/pair/trio.
struct Chain {
  ChainType chain_type{ChainType::kNotChain};
  int length{};
  int start_rank{};
  Chain(const ChainType chain_type, const int length, const int start_rank) : chain_type(chain_type), length(length),
                                                                              start_rank(start_rank) {
  }

  Chain() = default;
  bool operator==(const Chain &other) const {
    return chain_type == other.chain_type
        && length == other.length
        && start_rank == other.start_rank;
  }
};

enum KickerType { kUnknown = -1, kNoKicker = 0, kSolo = 1, kPair };

// Trio with solo/pair.
struct TrioComb {
  KickerType kicker_type{kUnknown};
  int trio_rank{};
  TrioComb(const KickerType kt, const int tr) : kicker_type(kt), trio_rank(tr) {
  }
  TrioComb() = default;
  bool operator==(const TrioComb &other) const {
    return kicker_type == other.kicker_type
        && trio_rank == other.trio_rank;
  }
};

// Quad with solo/pair.
struct QuadComb {
  KickerType kicker_type{kUnknown};
  int quad_rank{};
  QuadComb(const KickerType kt, const int qr) : kicker_type(kt), quad_rank(qr) {
  }
  QuadComb() = default;
  bool operator==(const QuadComb &other) const {
    return kicker_type == other.kicker_type
        && quad_rank == other.quad_rank;
  }
};

std::vector<std::vector<int> > GetPossibleKickers(const TrioComb &trio_comb);

std::vector<std::vector<int> > GetPossibleKickers(const QuadComb &quad_comb, bool allow_repeated_kickers = true);

// Plane with solo/pair.
struct Plane {
  KickerType kicker_type{kUnknown};
  int length{};
  int start_rank{};
  Plane(const KickerType kt, const int l, const int sr) : kicker_type(kt), length(l), start_rank(sr) {
  }
  Plane() = default;
  bool operator==(const Plane &other) const {
    return kicker_type == other.kicker_type
        && length == other.length
        && start_rank == other.start_rank;
  }
};

std::vector<std::vector<int> > GetPossibleKickers(const Plane &plane, bool allow_repeated_kickers = true);

// A space shuttle refers to two or more consecutive fours-of-a-kind with solo or pair kickers.
// This kind of move is not allowed in formal game settings but used for some variants.
// See https://wiki.botzone.org.cn/index.php?title=FightTheLandlord for more details.
struct SpaceShuttle {
  KickerType kicker_type{kUnknown};
  int length{};
  int start_rank{};
  SpaceShuttle(const KickerType kt, const int l, const int sr) : kicker_type(kt), length(l), start_rank(sr) {
  }
  SpaceShuttle() = default;
  bool operator==(const SpaceShuttle &other) const {
    return kicker_type == other.kicker_type
        && length == other.length
        && start_rank == other.start_rank;
  }
};

// We assume that the kickers cannot be repeated in space shuttle since this comb only exists in botzone.
std::vector<std::vector<int> > GetPossibleKickers(const SpaceShuttle &space_shuttle);

class DoudizhuMove {
  public:
    enum Type {
      kInvalid = -1,
      kDeal,
      kAuction,
      kPlay
    };

    enum class AuctionType {
      kInvalid = -1,
      kPass = 0,
      kOne,
      kTwo,
      kThree
    };

    enum class PlayType {
      kInvalid = -1,
      kPass,
      kSolo,
      kPair,
      kTrio,
      kBomb,
      kTrioWithSolo,
      kTrioWithPair,
      kChainOfSolo,
      kChainOfPair,
      kChainOfTrio,
      kPlaneWithSolo,
      kPlaneWithPair,
      kQuadWithSolo,
      kQuadWithPair,
      kRocket,
      kSpaceShuttle,
      kSpaceShuttleWithSolo,
      kSpaceShuttleWithPair
    };

    DoudizhuMove(const Type move_type,
                 const AuctionType auction_type,
                 const PlayType play_type,
                 const DoudizhuCard &deal_card,
                 const SingleRank single_rank,
                 const Chain chain,
                 const TrioComb trio_comb,
                 const QuadComb quad_comb,
                 const Plane plane,
                 const SpaceShuttle space_shuttle,
                 const std::array<int, kNumRanks> &kickers)
      : move_type_(move_type),
        auction_type_(auction_type),
        play_type_(play_type),
        deal_card_(deal_card),
        single_rank_(single_rank),
        chain_(chain),
        trio_comb_(trio_comb),
        quad_comb_(quad_comb),
        plane_(plane),
        space_shuttle_(space_shuttle),
        kickers_(kickers) {
    }

    DoudizhuMove() = default;

    // Fast constructor for a deal move.
    explicit DoudizhuMove(const DoudizhuCard &deal_card) : move_type_(kDeal),
                                                           deal_card_(deal_card) {
    }

    // Fast constructor for an auction move.
    explicit DoudizhuMove(AuctionType auction_type) : move_type_(kAuction),
                                                      auction_type_(auction_type) {
    }

    // Fast constructor for a play move.
    DoudizhuMove(const PlayType play_type,
                 const SingleRank single_rank,
                 const Chain chain,
                 const TrioComb trio_comb,
                 const QuadComb quad_comb,
                 const Plane plane,
                 const SpaceShuttle space_shuttle,
                 const std::array<int, kNumRanks> &kickers)
      : move_type_(kPlay),
        play_type_(play_type),
        single_rank_(single_rank),
        chain_(chain),
        trio_comb_(trio_comb),
        quad_comb_(quad_comb),
        plane_(plane),
        space_shuttle_(space_shuttle),
        kickers_(kickers) {
    }

    // Fast constructor for single ranks.
    explicit DoudizhuMove(const SingleRank single_rank)
      : move_type_(kPlay),
        single_rank_(single_rank) {
      switch (single_rank_.num_cards) {
        case 1: play_type_ = PlayType::kSolo;
          break;
        case 2: play_type_ = PlayType::kPair;
          break;
        case 3: play_type_ = PlayType::kTrio;
          break;
        case 4: play_type_ = PlayType::kBomb;
          break;
        default: FatalError("Should not reach here.");
      }
    }

    // Fast constructor for chains.
    explicit DoudizhuMove(const Chain chain)
      : move_type_(kPlay),
        chain_(chain) {
      switch (chain_.chain_type) {
        case ChainType::kSolo: play_type_ = PlayType::kChainOfSolo;
          break;
        case ChainType::kPair: play_type_ = PlayType::kChainOfPair;
          break;
        case ChainType::kTrio: play_type_ = PlayType::kChainOfTrio;
          break;
        default: FatalError("Should not reach here.");
      }
    }

    // Fast constructor for trio combs.
    DoudizhuMove(const TrioComb trio_comb,
                 const std::array<int, kNumRanks> &kickers)
      : move_type_(kPlay),
        trio_comb_(trio_comb),
        kickers_(kickers) {
      switch (trio_comb_.kicker_type) {
        case kSolo: play_type_ = PlayType::kTrioWithSolo;
          break;
        case kPair: play_type_ = PlayType::kTrioWithPair;
          break;
        default: FatalError("Should not reach here.");
      }
    }

    // Fast constructor for quad combs.
    DoudizhuMove(const QuadComb quad_comb,
                 const std::array<int, kNumRanks> &kickers)
      : move_type_(kPlay),
        quad_comb_(quad_comb),
        kickers_(kickers) {
      switch (quad_comb_.kicker_type) {
        case kSolo: play_type_ = PlayType::kQuadWithSolo;
          break;
        case kPair: play_type_ = PlayType::kQuadWithPair;
          break;
        default: FatalError("Should not reach here.");
      }
    }

    // Fast constructor for planes.
    DoudizhuMove(const Plane plane,
                 const std::array<int, kNumRanks> &kickers)
      : move_type_(kPlay),
        plane_(plane),
        kickers_(kickers) {
      switch (plane_.kicker_type) {
        case kSolo: play_type_ = PlayType::kPlaneWithSolo;
          break;
        case kPair: play_type_ = PlayType::kPlaneWithPair;
          break;
        default: FatalError("Should not reach here.");
      }
    }

    // Fast constructor for rockets and pass.
    explicit DoudizhuMove(const PlayType play_type)
      : move_type_(kPlay) {
      if (play_type != PlayType::kRocket && play_type != PlayType::kPass) {
        FatalError(
          "Play type " + std::to_string(static_cast<int>(play_type)) + " can not be used in this constructor.");
      }
      play_type_ = play_type;
    }

    // Fast constructor for space shuttles.
    DoudizhuMove(const SpaceShuttle space_shuttle,
                 const std::array<int, kNumRanks> &kickers)
      : move_type_(kPlay),
        space_shuttle_(space_shuttle),
        kickers_(kickers) {
      switch (space_shuttle_.kicker_type) {
        case kNoKicker:
          play_type_ = PlayType::kSpaceShuttle;
          break;
        case kSolo:
          play_type_ = PlayType::kSpaceShuttleWithSolo;
          break;
        case kPair:
          play_type_ = PlayType::kSpaceShuttleWithPair;
          break;
        default:
          FatalError("No matching play type with chain type " + std::to_string(space_shuttle_.kicker_type));
      }
    }

    DoudizhuMove(const DoudizhuMove &) = default;

    [[nodiscard]] std::string ToString() const;

    bool operator==(const DoudizhuMove &other_move) const;

    [[nodiscard]] Type MoveType() const { return move_type_; }

    [[nodiscard]] DoudizhuCard DealCard() const { return deal_card_; }

    [[nodiscard]] AuctionType Auction() const { return auction_type_; }

    [[nodiscard]] PlayType GetPlayType() const { return play_type_; }

    [[nodiscard]] SingleRank GetSingleRank() const { return single_rank_; }

    [[nodiscard]] Chain GetChain() const { return chain_; }

    [[nodiscard]] TrioComb GetTrioComb() const { return trio_comb_; }

    [[nodiscard]] QuadComb GetQuadComb() const { return quad_comb_; }

    [[nodiscard]] Plane GetPlane() const { return plane_; }

    [[nodiscard]] SpaceShuttle GetSpaceShuttle() const { return space_shuttle_; }

    [[nodiscard]] const std::array<int, kNumRanks> &Kickers() const { return kickers_; }

    [[nodiscard]] bool IsBomb() const { return play_type_ == PlayType::kBomb || play_type_ == PlayType::kRocket; }

    [[nodiscard]] int BombRank() const {
      CHECK_TRUE(IsBomb());
      if (play_type_ == PlayType::kRocket) {
        return kBlackJoker;
      }
      if (play_type_ == PlayType::kBomb) {
        return single_rank_.rank;
      }
      return -1;
    }

    [[nodiscard]] std::vector<int> ToRanks() const;

    // Some moves created by users may be invalid, use this function to check validity.
    [[nodiscard]] bool IsValid(bool check_kickers = false) const;

  private:
    Type move_type_ = kInvalid;
    AuctionType auction_type_ = AuctionType::kInvalid;
    PlayType play_type_ = PlayType::kInvalid;
    DoudizhuCard deal_card_{};
    SingleRank single_rank_;
    Chain chain_{};
    TrioComb trio_comb_{};
    QuadComb quad_comb_{};
    Plane plane_{};
    SpaceShuttle space_shuttle_{};
    std::array<int, kNumRanks> kickers_{};
};

std::ostream &operator<<(std::ostream &os, const DoudizhuMove &move);

bool HandCanMakeMove(const DoudizhuHand &hand, const DoudizhuMove &move);

std::array<int, kNumRanks> RanksToCounter(const std::vector<int> &kickers);

DoudizhuMove GetChainMoveFromString(const std::string &move_str,
                                    int num_ranks,
                                    ChainType chain_type);

DoudizhuMove GetMoveFromString(const std::string &move_str, DoudizhuMove::Type move_type);
}
#endif //DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_DOUDIZHU_MOVE_H_
