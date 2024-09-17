//
// Created by qzz on 2024/5/30.
//

#include <algorithm>
#include <iostream>
#include <numeric>
#include <utility>

#include "doudizhu_game.h"

namespace doudizhu_learning_env {
DoudizhuGame::DoudizhuGame(const GameParameters &parameters) {
  parameters_ = parameters;
  //  std::cout << "Enter constructor" << std::endl;
  seed_ = ParameterValue<int>(parameters_, "seed", 42);
  rng_.seed(seed_);
  allow_repeated_kickers_ =
      ParameterValue<bool>(parameters_, "allow_repeated_kickers", true);
  allow_space_shuttle_ =
      ParameterValue<bool>(parameters_, "allow_space_shuttle", false);

  // Possible trio with solos.
  for (int trio_rank = 0; trio_rank < kNumCardsPerSuit; ++trio_rank) {
    const TrioComb trio_comb{
      /*kt=*/kSolo, /*tr=*/trio_rank
    };
    const auto kickers = GetPossibleKickers(trio_comb);
    for (const auto &ks : kickers) {
      const DoudizhuMove move{
        /*trio_comb=*/trio_comb,
        /*kickers=*/RanksToCounter(ks)
      };
      trio_with_solos_.push_back(move);
    }
  }

  // Possible trio with pairs.
  for (int trio_rank = 0; trio_rank < kNumCardsPerSuit; ++trio_rank) {
    const TrioComb trio_comb{
      /*kt=*/kPair, /*tr=*/trio_rank
    };
    const auto kickers = GetPossibleKickers(trio_comb);
    for (const auto &ks : kickers) {
      const DoudizhuMove move{
        /*trio_comb=*/trio_comb,
        /*kickers=*/RanksToCounter(ks)
      };
      trio_with_pairs_.push_back(move);
    }
  }

  // Possible plane with solos.
  for (int length = kPlaneWithSoloMinLength; length <= kPlaneWithSoloMaxLength;
       ++length) {
    for (int start_rank = 0; start_rank < kNumCardsPerSuit - length;
         ++start_rank) {
      const Plane plane{
        /*kt=*/kSolo, /*l*/ length, /*sr=*/start_rank
      };
      const auto kickers = GetPossibleKickers(plane, allow_repeated_kickers_);
      for (const auto &ks : kickers) {
        const DoudizhuMove move{
          /*plane=*/plane,
          /*kickers=*/RanksToCounter(ks)
        };
        plane_with_solo_per_length_[length].push_back(move);
      }
    }
  }
  // Possible plane with pairs.
  for (int length = kPlaneWithPairMinLength; length <= kPlaneWithPairMaxLength;
       ++length) {
    for (int start_rank = 0; start_rank < kNumCardsPerSuit - length;
         ++start_rank) {
      const Plane plane{
        /*kt=*/kPair, /*l*/ length, /*sr=*/start_rank
      };
      const auto kickers = GetPossibleKickers(plane, allow_repeated_kickers_);
      for (const auto &ks : kickers) {
        const DoudizhuMove move{
          /*plane=*/plane,
          /*kickers=*/RanksToCounter(ks)
        };
        plane_with_pair_per_length_[length].push_back(move);
      }
    }
  }

  // Possible quad with solos.
  for (int quad_rank = 0; quad_rank < kNumCardsPerSuit; ++quad_rank) {
    const QuadComb quad_comb{
      /*kt=*/kSolo, /*qr*/ quad_rank
    };
    const auto kickers = GetPossibleKickers(quad_comb, allow_repeated_kickers_);
    for (const auto &ks : kickers) {
      const DoudizhuMove move{
        /*quad_comb=*/quad_comb,
        /*kickers=*/RanksToCounter(ks)
      };
      quad_with_solos_.push_back(move);
    }
  }

  // Possible quad with pairs.
  for (int quad_rank = 0; quad_rank < kNumCardsPerSuit; ++quad_rank) {
    const QuadComb quad_comb{
      /*kt=*/kPair, /*qr*/ quad_rank
    };
    const auto kickers = GetPossibleKickers(quad_comb, allow_repeated_kickers_);
    for (const auto &ks : kickers) {
      const DoudizhuMove move{
        /*quad_comb=*/quad_comb,
        /*kickers=*/RanksToCounter(ks)
      };
      quad_with_pairs_.push_back(move);
    }
  }

  if (allow_space_shuttle_) {
    // Space shuttle.
    for (int length = kSpaceShuttleMinLength; length <= kSpaceShuttleMaxLength;
         ++length) {
      for (int start_rank = 0; start_rank < kNumCardsPerSuit - length;
           ++start_rank) {
        for (const KickerType kt : {kNoKicker, kSolo, kPair}) {
          const SpaceShuttle space_shuttle{
            /*kt=*/kt, /*l*/ length, /*sr=*/start_rank
          };
          const auto kickers = GetPossibleKickers(space_shuttle);
          if (!kickers.empty()) {
            for (const auto &ks : kickers) {
              const DoudizhuMove move{
                /*space_shuttle=*/space_shuttle,
                /*kickers=*/RanksToCounter(ks)
              };
              if (kt == kSolo) {
                space_shuttle_with_solo_per_length_[length].push_back(move);
              } else if (kt == kPair) {
                space_shuttle_with_pair_per_length_[length].push_back(move);
              } else {
                FatalError("Unknown KickerType");
              }
            }
          } else {
            const DoudizhuMove move{space_shuttle, std::array<int, kNumRanks>{}};
            space_shuttle_per_length_[length].push_back(move);
          }
        }
      }
    }
  }

  for (int chance_outcome_uid = 0; chance_outcome_uid < MaxChanceOutcomes();
       ++chance_outcome_uid) {
    chance_outcomes_.push_back(ConstructChanceOutcome(chance_outcome_uid));
  }
  //
  for (int move_uid = 0; move_uid < MaxMoves(); ++move_uid) {
    moves_.push_back(ConstructMove(move_uid));
  }
}
DoudizhuMove DoudizhuGame::ConstructChanceOutcome(const int uid) const {
  if (uid < 0 && uid >= MaxChanceOutcomes()) {
    // Invalid.
    return {
      /*move_type=*/DoudizhuMove::kInvalid,
      /*auction_type=*/DoudizhuMove::AuctionType::kInvalid,
      /*play_type=*/DoudizhuMove::PlayType::kInvalid,
      /*deal_card=*/{},
      /*single_rank=*/{},
      /*chain=*/{},
      /*trio_comb=*/{},
      /*quad_comb=*/{},
      /*plane=*/{},
      /*space_shuttle=*/{},
      /*kickers=*/{}
    };
  }
  //  std::cout << "uid: " << uid << ", rank: " << Uid2Rank(uid) << ", suit: "
  //  << Uid2Suit(uid) << std::endl;
  const DoudizhuCard deal_card{
    /*rank=*/Uid2Rank(uid), /*suit*/ Uid2Suit(uid)
  };
  return DoudizhuMove{
    /*deal_card=*/deal_card
  };
}
DoudizhuMove DoudizhuGame::ConstructMove(int uid) const {
  if (uid < 0 || uid >= MaxMoves()) {
    // Invalid move.
    return {
      /*move_type=*/DoudizhuMove::kInvalid,
      /*auction_type=*/DoudizhuMove::AuctionType::kInvalid,
      /*play_type=*/DoudizhuMove::PlayType::kInvalid,
      /*deal_card=*/{},
      /*single_rank=*/{},
      /*chain=*/{},
      /*trio_comb=*/{},
      /*quad_comb=*/{},
      /*plane=*/{},
      /*space_shuttle=*/{},
      /*kickers=*/{}
    };
  }
  if (uid < MaxAuctionMoves()) {
    // 0-pass, 1-bid 1, 2-bid 2, 3-bid3.
    return DoudizhuMove{
      /*auction_type=*/static_cast<DoudizhuMove::AuctionType>(uid)
    };
  }
  uid -= MaxAuctionMoves();
  // Play move.
  if (uid < MaxSoloMoves()) {
    // Solo.
    return DoudizhuMove{
      /*single_rank=*/SingleRank{
        /*r=*/uid, /*n=*/1
      }
    };
  }
  uid -= MaxSoloMoves();

  if (uid < MaxPairMoves()) {
    // Pair.
    return DoudizhuMove{
      /*single_rank=*/{
        /*r=*/uid, /*n=*/kPairLength
      }
    };
  }
  uid -= MaxPairMoves();

  if (uid < MaxTrioMoves()) {
    // Trio.
    return DoudizhuMove{
      /*single_rank=*/{
        /*r=*/uid, /*n=*/kTrioLength
      },
    };
  }
  uid -= MaxTrioMoves();

  if (uid < MaxTrioWithSoloMoves()) {
    // Trio with solo.
    return trio_with_solos_[uid];
  }
  uid -= MaxTrioWithSoloMoves();

  if (uid < MaxTrioWithPairMoves()) {
    // Trio with pairs.
    return trio_with_pairs_[uid];
  }
  uid -= MaxTrioWithPairMoves();

  if (uid < MaxChainOfSoloMoves()) {
    // Chain of solos.
    const auto [chain_length, start_rank] =
        GetChainOfSoloLengthAndStartRank(uid);
    const Chain chain{
      /*chain_type=*/ChainType::kSolo, /*length=*/chain_length,
      /*start_rank=*/start_rank
    };
    return DoudizhuMove{
      /*chain=*/chain,
    };
  }

  uid -= MaxChainOfSoloMoves();

  if (uid < MaxChainOfPairMoves()) {
    // Chain of pairs.
    const auto [chain_length, start_rank] =
        GetChainOfPairLengthAndStartRank(uid);
    const Chain chain{
      /*chain_type=*/ChainType::kPair, /*length=*/chain_length,
      /*start_rank=*/start_rank
    };
    return DoudizhuMove{
      /*chain=*/chain
    };
  }

  uid -= MaxChainOfPairMoves();

  if (uid < MaxChainOfTrioMoves()) {
    // Chain of trios.
    const auto [chain_length, start_rank] =
        GetChainOfTrioLengthAndStartRank(uid);
    const Chain chain{
      /*chain_type=*/ChainType::kTrio, /*length=*/chain_length,
      /*start_rank=*/start_rank
    };
    return DoudizhuMove{
      /*chain=*/chain,
    };
  }

  uid -= MaxChainOfTrioMoves();

  if (uid < GetNumPlaneWithSoloByLength(2, allow_repeated_kickers_)) {
    return plane_with_solo_per_length_.at(2)[uid];
  }
  uid -= GetNumPlaneWithSoloByLength(2, allow_repeated_kickers_);

  if (uid < GetNumPlaneWithSoloByLength(3, allow_repeated_kickers_)) {
    return plane_with_solo_per_length_.at(3)[uid];
  }
  uid -= GetNumPlaneWithSoloByLength(3, allow_repeated_kickers_);

  if (uid < GetNumPlaneWithSoloByLength(4, allow_repeated_kickers_)) {
    return plane_with_solo_per_length_.at(4)[uid];
  }
  uid -= GetNumPlaneWithSoloByLength(4, allow_repeated_kickers_);

  if (uid < GetNumPlaneWithSoloByLength(5, allow_repeated_kickers_)) {
    return plane_with_solo_per_length_.at(5)[uid];
  }
  uid -= GetNumPlaneWithSoloByLength(5, allow_repeated_kickers_);

  // Plane with pairs.
  if (uid < kNumLength2PlaneWithPairs) {
    return plane_with_pair_per_length_.at(2)[uid];
  }
  uid -= kNumLength2PlaneWithPairs;

  if (uid < kNumLength3PlaneWithPairs) {
    return plane_with_pair_per_length_.at(3)[uid];
  }
  uid -= kNumLength3PlaneWithPairs;

  if (uid < kNumLength4PlaneWithPairs) {
    return plane_with_pair_per_length_.at(4)[uid];
  }
  uid -= kNumLength4PlaneWithPairs;

  // Quad with solo.
  if (uid < MaxQuadWithSoloMoves()) {
    return quad_with_solos_[uid];
  }
  uid -= MaxQuadWithSoloMoves();

  // Quad with pairs.
  if (uid < MaxQuadWithPairMoves()) {
    return quad_with_pairs_[uid];
  }
  uid -= MaxQuadWithPairMoves();

  // Bombs.
  if (uid < MaxBombMoves()) {
    const SingleRank single_rank{
      /*r=*/uid, /*n=*/4
    };
    return DoudizhuMove{
      /*single_rank=*/single_rank,
    };
  }

  uid -= MaxBombMoves();

  // Rocket.
  if (uid < MaxRocketMoves()) {
    return DoudizhuMove{
      /*play_type=*/DoudizhuMove::PlayType::kRocket,
    };
  }

  uid -= MaxRocketMoves();

  if (allow_space_shuttle_) {
    for (int length = kSpaceShuttleMinLength; length <= kSpaceShuttleMaxLength; length++) {
      if (uid < GetNumSpaceShuttleByLength(length)) {
        return space_shuttle_per_length_.at(length)[uid];
      }
      uid -= GetNumSpaceShuttleByLength(length);
    }

    for (int length = kSpaceShuttleMinLength; length <= kSpaceShuttleWithSoloMaxLength; length++) {
      if (uid < GetNumSpaceShuttleWithSoloByLength(length)) {
        return space_shuttle_with_solo_per_length_.at(length)[uid];
      }
      uid -= GetNumSpaceShuttleWithSoloByLength(length);
    }

    for (int length = kSpaceShuttleMinLength; length <= kSpaceShuttleWithPairMaxLength; length++) {
      if (uid < GetNumSpaceShuttleWithPairByLength(length)) {
        return space_shuttle_with_pair_per_length_.at(length)[uid];
      }
      uid -= GetNumSpaceShuttleWithPairByLength(length);
    }
  }

  if (uid < MaxPassMoves()) {
    return DoudizhuMove{
      /*play_type=*/DoudizhuMove::PlayType::kPass,
    };
  }
  uid -= MaxPassMoves();

  return {};
}
std::pair<int, int>
DoudizhuGame::GetChainOfSoloLengthAndStartRank(int uid) const {
  int sum = 0;
  for (int i = 8; i >= 1; --i) {
    sum += i;
    if (uid < sum) {
      const int length = 13 - i;
      const int start_rank = uid - (sum - i);
      return {length, start_rank};
    }
  }
  FatalError("Error getting chain of solo length, uid=" + std::to_string(uid));
}
std::pair<int, int>
DoudizhuGame::GetChainOfPairLengthAndStartRank(int uid) const {
  int sum = 0;
  for (int i = 10; i >= 3; --i) {
    sum += i;
    if (uid < sum) {
      const int length = 13 - i;
      const int start_rank = uid - (sum - i);
      return {length, start_rank};
    }
  }
  FatalError("Error getting chain of solo length, uid=" + std::to_string(uid));
}
std::pair<int, int>
DoudizhuGame::GetChainOfTrioLengthAndStartRank(int uid) const {
  int sum = 0;
  for (int i = 11; i >= 7; --i) {
    sum += i;
    if (uid < sum) {
      const int length = 13 - i;
      const int start_rank = uid - (sum - i);
      return {length, start_rank};
    }
  }
  FatalError("Error getting chain of solo length, uid=" + std::to_string(uid));
}
DoudizhuMove DoudizhuGame::PickRandomChance(
  const std::pair<std::vector<DoudizhuMove>, std::vector<double> >
  &chance_outcomes) const {
  std::discrete_distribution<std::mt19937::result_type> dist(
    chance_outcomes.second.begin(),
    chance_outcomes.second.end());
  return chance_outcomes.first[dist(rng_)];
}
int DoudizhuGame::GetChanceOutcomeUid(const DoudizhuMove &chance) const {
  if (chance.MoveType() != DoudizhuMove::kDeal) {
    return -1;
  }
  const auto deal_card = chance.DealCard();
  return CardIndex(deal_card);
}
int DoudizhuGame::GetMoveUid(const DoudizhuMove &move) const {
  switch (move.MoveType()) {
    case DoudizhuMove::kAuction: {
      return static_cast<int>(move.Auction());
    }
    case DoudizhuMove::kPlay: {
      int uid_base = kNumBids + 1;
      const auto play_type = move.GetPlayType();
      if (play_type == DoudizhuMove::PlayType::kSolo) {
        return uid_base + move.GetSingleRank().rank;
      }
      uid_base += MaxSoloMoves();
      if (play_type == DoudizhuMove::PlayType::kPair) {
        return uid_base + move.GetSingleRank().rank;
      }
      uid_base += MaxPairMoves();
      if (play_type == DoudizhuMove::PlayType::kTrio) {
        return uid_base + move.GetSingleRank().rank;
      }
      uid_base += MaxTrioMoves();
      if (play_type == DoudizhuMove::PlayType::kTrioWithSolo) {
        // TODO: Find a better way instead of std::find.
        return uid_base +
            static_cast<int>((std::find(trio_with_solos_.begin(),
                                        trio_with_solos_.end(),
                                        move) -
              trio_with_solos_.begin()));
      }
      uid_base += MaxTrioWithSoloMoves();
      if (play_type == DoudizhuMove::PlayType::kTrioWithPair) {
        return uid_base +
            +static_cast<int>((std::find(trio_with_pairs_.begin(),
                                         trio_with_pairs_.end(),
                                         move) -
              trio_with_pairs_.begin()));
      }
      uid_base += MaxTrioWithPairMoves();
      if (play_type == DoudizhuMove::PlayType::kChainOfSolo) {
        const std::vector<int> lengths =
            Range(NumberOfChainOrPlane(kChainOfSoloMinLength),
                  NumberOfChainOrPlane(kChainOfSoloMaxLength),
                  -1);
        const int uid =
            uid_base +
            std::accumulate(lengths.begin(),
                            lengths.begin() +
                            (move.GetChain().length - kChainOfSoloMinLength),
                            0) +
            move.GetChain().start_rank;
        return uid;
      }
      uid_base += MaxChainOfSoloMoves();
      if (play_type == DoudizhuMove::PlayType::kChainOfPair) {
        const std::vector<int> lengths =
            Range(NumberOfChainOrPlane(kChainOfPairMinLength),
                  NumberOfChainOrPlane(kChainOfPairMaxLength),
                  -1);
        const int uid =
            uid_base +
            std::accumulate(lengths.begin(),
                            lengths.begin() +
                            (move.GetChain().length - kChainOfPairMinLength),
                            0) +
            move.GetChain().start_rank;
        return uid;
      }
      uid_base += MaxChainOfPairMoves();
      if (play_type == DoudizhuMove::PlayType::kChainOfTrio) {
        const std::vector<int> lengths =
            Range(NumberOfChainOrPlane(kChainOfTrioMinLength),
                  NumberOfChainOrPlane(kChainOfTrioMaxLength),
                  -1);
        const int uid =
            uid_base +
            std::accumulate(lengths.begin(),
                            lengths.begin() +
                            (move.GetChain().length - kChainOfTrioMinLength),
                            0) +
            move.GetChain().start_rank;
        return uid;
      }
      uid_base += MaxChainOfTrioMoves();
      if (play_type == DoudizhuMove::PlayType::kPlaneWithSolo) {
        const Plane plane = move.GetPlane();
        const auto get_index = [&](const int length) {
          return static_cast<int>(
            std::find(plane_with_solo_per_length_.at(length).begin(),
                      plane_with_solo_per_length_.at(length).end(),
                      move) -
            plane_with_solo_per_length_.at(length).begin());
        };
        CHECK_GE(plane.length, kPlaneWithSoloMinLength);
        CHECK_LE(plane.length, kPlaneWithSoloMaxLength);
        if (plane.length == 2) {
          return uid_base + get_index(2);
        }
        uid_base += GetNumPlaneWithSoloByLength(2, allow_repeated_kickers_);
        if (plane.length == 3) {
          return uid_base + get_index(3);
        }
        uid_base += GetNumPlaneWithSoloByLength(3, allow_repeated_kickers_);
        if (plane.length == 4) {
          return uid_base + get_index(4);
        }
        uid_base += GetNumPlaneWithSoloByLength(4, allow_repeated_kickers_);
        if (plane.length == 5) {
          return uid_base + get_index(5);
        }
        FatalError("Should not reach here.");
      }
      uid_base += MaxPlaneWithSoloMoves();

      if (play_type == DoudizhuMove::PlayType::kPlaneWithPair) {
        const Plane plane = move.GetPlane();
        const auto get_index = [&](const int length) {
          return static_cast<int>(
            std::find(plane_with_pair_per_length_.at(length).begin(),
                      plane_with_pair_per_length_.at(length).end(),
                      move) -
            plane_with_pair_per_length_.at(length).begin());
        };
        CHECK_GE(plane.length, kPlaneWithPairMinLength);
        CHECK_LE(plane.length, kPlaneWithPairMaxLength);
        if (plane.length == 2) {
          return uid_base + get_index(2);
        }
        uid_base += kNumLength2PlaneWithPairs;
        if (plane.length == 3) {
          return uid_base + get_index(3);
        }
        uid_base += kNumLength3PlaneWithPairs;
        if (plane.length == 4) {
          return uid_base + get_index(4);
        }
        FatalError("Should not reach here.");
      }
      uid_base += MaxPlaneWithPairMoves();

      if (play_type == DoudizhuMove::PlayType::kQuadWithSolo) {
        return uid_base +
            static_cast<int>(std::find(quad_with_solos_.begin(),
                                       quad_with_solos_.end(),
                                       move) -
              quad_with_solos_.begin());
      }
      uid_base += MaxQuadWithSoloMoves();

      if (play_type == DoudizhuMove::PlayType::kQuadWithPair) {
        return uid_base +
            static_cast<int>(std::find(quad_with_pairs_.begin(),
                                       quad_with_pairs_.end(),
                                       move) -
              quad_with_pairs_.begin());
      }
      uid_base += MaxQuadWithPairMoves();

      if (play_type == DoudizhuMove::PlayType::kBomb) {
        return uid_base + move.GetSingleRank().rank;
      }
      uid_base += MaxBombMoves();

      if (play_type == DoudizhuMove::PlayType::kRocket) {
        return uid_base;
      }
      uid_base += MaxRocketMoves();

      if (play_type == DoudizhuMove::PlayType::kSpaceShuttle) {
        if (!allow_space_shuttle_) {
          return -1;
        }
        const auto get_index = [&](const int length) {
          return static_cast<int>(
            std::find(space_shuttle_per_length_.at(length).begin(),
                      space_shuttle_per_length_.at(length).end(),
                      move) -
            space_shuttle_per_length_.at(length).begin());
        };
        for (int length = kSpaceShuttleMinLength; length <= kSpaceShuttleMaxLength; length++) {
          if (move.GetSpaceShuttle().length == length) {
            return uid_base + get_index(length);
          }
          uid_base += GetNumSpaceShuttleByLength(length);
        }
      }
      if (allow_space_shuttle_) {
        uid_base += MaxSpaceShuttleMoves();
      }

      if (play_type == DoudizhuMove::PlayType::kSpaceShuttleWithSolo) {
        if (!allow_space_shuttle_) {
          return -1;
        }
        const auto get_index = [&](const int length) {
          return static_cast<int>(
            std::find(space_shuttle_with_solo_per_length_.at(length).begin(),
                      space_shuttle_with_solo_per_length_.at(length).end(),
                      move) -
            space_shuttle_with_solo_per_length_.at(length).begin());
        };
        for (int length = kSpaceShuttleMinLength; length <= kSpaceShuttleWithSoloMaxLength; length++) {
          if (move.GetSpaceShuttle().length == length) {
            return uid_base + get_index(length);
          }
          uid_base += GetNumSpaceShuttleWithSoloByLength(length);
        }
      }
      if (allow_space_shuttle_) {
        uid_base += MaxSpaceShuttleWithSoloMoves();
      }

      if (play_type == DoudizhuMove::PlayType::kSpaceShuttleWithPair) {
        if (!allow_space_shuttle_) {
          return -1;
        }
        const auto get_index = [&](const int length) {
          return static_cast<int>(
            std::find(space_shuttle_with_pair_per_length_.at(length).begin(),
                      space_shuttle_with_pair_per_length_.at(length).end(),
                      move) -
            space_shuttle_with_pair_per_length_.at(length).begin());
        };
        for (int length = kSpaceShuttleMinLength; length <= kSpaceShuttleWithPairMaxLength; length++) {
          if (move.GetSpaceShuttle().length == length) {
            return uid_base + get_index(length);
          }
          uid_base += GetNumSpaceShuttleWithPairByLength(length);
        }
      }
      if (allow_space_shuttle_) {
        uid_base += MaxSpaceShuttleWithPairMoves();
      }

      if (play_type == DoudizhuMove::PlayType::kPass) {
        return uid_base;
      }

      FatalError("Should not reach here, the move is " + move.ToString());
    }
    default:
      return -1;
  }
}
} // namespace doudizhu_learning_env
