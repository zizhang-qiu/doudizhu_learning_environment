//
// Created by qzz on 2024/6/2.
//

#ifndef DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_DOUDIZHU_MOVE_H_
#define DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_DOUDIZHU_MOVE_H_
#include "doudizhu_hand.h"
namespace doudizhu_learning_env {

// Solo, pair, trio and bomb.
struct SingleRank {
  int rank = -1;
  int num_cards = 0;
  SingleRank(int r, int n) : rank(r), num_cards(n) {}
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
  Chain(ChainType chain_type, int length, int start_rank) :
      chain_type(chain_type), length(length), start_rank(start_rank) {}

  Chain() = default;
  bool operator==(const Chain &other) const {
    return chain_type == other.chain_type
        && length == other.length
        && start_rank == other.start_rank;
  }
};

enum KickerType { kUnknown = -1, kSolo = 1, kPair };

// Trio with solo/pair.
struct TrioComb {
  KickerType kicker_type{kUnknown};
  int trio_rank{};
  TrioComb(KickerType kt, int r) : kicker_type(kt), trio_rank(r) {}
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
  QuadComb(KickerType kt, int qr) : kicker_type(kt), quad_rank(qr) {}
  QuadComb() = default;
  bool operator==(const QuadComb &other) const {
    return kicker_type == other.kicker_type
        && quad_rank == other.quad_rank;
  }
};

std::vector<std::vector<int>> GetPossibleKickers(const TrioComb& trio_comb);

std::vector<std::vector<int>> GetPossibleKickers(const QuadComb& quad_comb);


// Plane with solo/pair.
struct Plane {
  KickerType kicker_type{kUnknown};
  int length{};
  int start_rank{};
  Plane(KickerType kt, int l, int sr) : kicker_type(kt), length(l), start_rank(sr) {}
  Plane() = default;
  bool operator==(const Plane &other) const {
    return kicker_type == other.kicker_type
        && length == other.length
        && start_rank == other.start_rank;
  }
};

std::vector<std::vector<int>> GetPossibleKickers(const Plane& plane);

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
    kPass,
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
    kTrioWithSolo,
    kTrioWithPair,
    kChainOfSolo,
    kChainOfPair,
    kChainOfTrio,
    kPlaneWithSolo,
    kPlaneWithPair,
    kQuadWithSolo,
    kQuadWithPair,
    kBomb,
    kRocket
  };

  DoudizhuMove(Type move_type,
               AuctionType auction_type,
               PlayType play_type,
               const DoudizhuCard &deal_card,
               SingleRank single_rank,
               Chain chain,
               TrioComb trio_comb,
               QuadComb quad_comb,
               Plane plane,
               const std::vector<int> &kickers)
      : move_type_(move_type),
        auction_type_(auction_type),
        play_type_(play_type),
        deal_card_(deal_card),
        single_rank_(single_rank),
        chain_(chain),
        trio_comb_(trio_comb),
        quad_comb_(quad_comb),
        plane_(plane),
        kickers_(kickers) {}

  DoudizhuMove() = default;

  // Fast constructor for a deal move.
  DoudizhuMove(const DoudizhuCard &deal_card) :
      move_type_(kDeal),
      deal_card_(deal_card) {}

  // Fast constructor for an auction move.
  DoudizhuMove(AuctionType auction_type) :
      move_type_(kAuction),
      auction_type_(auction_type) {}

  // Fast constructor for a play move.
  DoudizhuMove(PlayType play_type,
               SingleRank single_rank,
               Chain chain,
               TrioComb trio_comb,
               QuadComb quad_comb,
               Plane plane,
               const std::vector<int> &kickers)
      : move_type_(kPlay),
        play_type_(play_type),
        single_rank_(single_rank),
        chain_(chain),
        trio_comb_(trio_comb),
        quad_comb_(quad_comb),
        plane_(plane),
        kickers_(kickers) {}

  std::string ToString() const;

  bool operator==(const DoudizhuMove &other_move) const;

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
  std::vector<int> kickers_{};

};
}
#endif //DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_DOUDIZHU_MOVE_H_
