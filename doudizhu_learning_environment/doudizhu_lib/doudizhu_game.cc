//
// Created by qzz on 2024/5/30.
//

#include <iostream>
#include <utility>
#include "doudizhu_game.h"
namespace doudizhu_learning_env {
DoudizhuGame::DoudizhuGame(const GameParameters &parameters) {
  parameters_ = parameters;
//  std::cout << "Enter constructor" << std::endl;
  seed_ = ParameterValue<int>(parameters_, "seed", 42);
  rng_.seed(seed_);

  // Possible trio with solos.
  for (int trio_rank = 0; trio_rank < kNumCardsPerSuit; ++trio_rank) {
    const TrioComb trio_comb{/*kt=*/kSolo, /*r=*/trio_rank};
    const auto kickers = GetPossibleKickers(trio_comb);
    for (const auto &ks : kickers) {
      const DoudizhuMove move{
          /*play_type=*/DoudizhuMove::PlayType::kTrioWithSolo,
          /*single_rank=*/{},
          /*chain=*/{},
          /*trio_comb=*/trio_comb,
          /*quad_comb=*/{},
          /*plane=*/{},
          /*kickers=*/ks
      };
      trio_with_solos_.push_back(move);
    }
  }

  // Possible trio with pairs.
  for (int trio_rank = 0; trio_rank < kNumCardsPerSuit; ++trio_rank) {
    const TrioComb trio_comb{/*kt=*/kPair, /*r=*/trio_rank};
    const auto kickers = GetPossibleKickers(trio_comb);
    for (const auto &ks : kickers) {
      const DoudizhuMove move{
          /*play_type=*/DoudizhuMove::PlayType::kTrioWithPair,
          /*single_rank=*/{},
          /*chain=*/{},
          /*trio_comb=*/trio_comb,
          /*quad_comb=*/{},
          /*plane=*/{},
          /*kickers=*/ks
      };
      trio_with_pairs_.push_back(move);
    }
  }


  // Possible plane with solos.
  for (int length = kPlaneWithSoloMinLength; length <= kPlaneWithSoloMaxLength; ++length) {
    for (int start_rank = 0; start_rank < kNumCardsPerSuit - length; ++start_rank) {
      const Plane plane{/*kt=*/kSolo, /*l*/length, /*sr=*/start_rank};
      const auto kickers = GetPossibleKickers(plane);
      for (const auto &ks : kickers) {
        const DoudizhuMove move{
            /*play_type=*/DoudizhuMove::PlayType::kPlaneWithSolo,
            /*single_rank=*/{},
            /*chain=*/{},
            /*trio_comb=*/{},
            /*quad_comb=*/{},
            /*plane=*/plane,
            /*kickers=*/ks
        };
        plane_with_solo_per_length_[length].push_back(move);
      }
    }
  }
  // Possible plane with pairs.
  for (int length = kPlaneWithPairMinLength; length <= kPlaneWithPairMaxLength; ++length) {
    for (int start_rank = 0; start_rank < kNumCardsPerSuit - length; ++start_rank) {
      const Plane plane{/*kt=*/kPair, /*l*/length, /*sr=*/start_rank};
      const auto kickers = GetPossibleKickers(plane);
      for (const auto &ks : kickers) {
        const DoudizhuMove move{
            /*play_type=*/DoudizhuMove::PlayType::kPlaneWithPair,
            /*single_rank=*/{},
            /*chain=*/{},
            /*trio_comb=*/{},
            /*quad_comb=*/{},
            /*plane=*/plane,
            /*kickers=*/ks
        };
        plane_with_pair_per_length_[length].push_back(move);
      }
    }
  }

  // Possible quad with solos.
  for (int quad_rank = 0; quad_rank < kNumCardsPerSuit; ++quad_rank) {
    const QuadComb quad_comb{/*kt=*/kSolo, /*qr*/quad_rank};
    const auto kickers = GetPossibleKickers(quad_comb);
    for (const auto &ks : kickers) {
      const DoudizhuMove move{
          /*play_type=*/DoudizhuMove::PlayType::kQuadWithSolo,
          /*single_rank=*/{},
          /*chain=*/{},
          /*trio_comb=*/{},
          /*quad_comb=*/quad_comb,
          /*plane=*/{},
          /*kickers=*/ks
      };
      quad_with_solos_.push_back(move);
    }
  }

  // Possible quad with pairs.
  for (int quad_rank = 0; quad_rank < kNumCardsPerSuit; ++quad_rank) {
    const QuadComb quad_comb{/*kt=*/kPair, /*qr*/quad_rank};
    const auto kickers = GetPossibleKickers(quad_comb);
    for (const auto &ks : kickers) {
      const DoudizhuMove move{
          /*play_type=*/DoudizhuMove::PlayType::kQuadWithPair,
          /*single_rank=*/{},
          /*chain=*/{},
          /*trio_comb=*/{},
          /*quad_comb=*/quad_comb,
          /*plane=*/{},
          /*kickers=*/ks
      };
      quad_with_pairs_.push_back(move);
    }
  }

  for (int chance_outcome_uid = 0; chance_outcome_uid < MaxChanceOutcomes(); ++chance_outcome_uid) {
    chance_outcomes_.push_back(ConstructChanceOutcome(chance_outcome_uid));
  }
//
  for (int move_uid = 0; move_uid < MaxMoves(); ++move_uid) {
    moves_.push_back(ConstructMove(move_uid));
  }
}
DoudizhuMove DoudizhuGame::ConstructChanceOutcome(int uid) const {
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
        /*kickers=*/{}
    };
  }
//  std::cout << "uid: " << uid << ", rank: " << Uid2Rank(uid) << ", suit: " << Uid2Suit(uid) << std::endl;
  const DoudizhuCard deal_card{/*rank=*/Uid2Rank(uid), /*suit*/Uid2Suit(uid)};
  return {/*deal_card=*/deal_card};
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
        /*kickers=*/{}
    };
  }
  if (uid < MaxAuctionMoves()) {
    // 0-pass, 1-bid 1, 2-bid 2, 3-bid3.
    return {/*auction_type=*/DoudizhuMove::AuctionType(uid)};
  }
  uid -= MaxAuctionMoves();
  // Play move.
  if (uid < kNumSolos) {
    // Solo.
    return {
        /*play_type=*/DoudizhuMove::PlayType::kSolo,
        /*single_rank=*/{/*r=*/uid, /*n=*/1},
        /*chain=*/{},
        /*trio_comb=*/{},
        /*quad_comb=*/{},
        /*plane=*/{},
        /*kickers=*/{}
    };
  }
  uid -= kNumSolos;

  if (uid < kNumPairs) {
    // Pair.
    return {
        /*play_type=*/DoudizhuMove::PlayType::kPair,
        /*single_rank=*/{/*r=*/uid, /*n=*/2},
        /*chain=*/{},
        /*trio_comb=*/{},
        /*quad_comb=*/{},
        /*plane=*/{},
        /*kickers=*/{}
    };
  }
  uid -= kNumPairs;

  if (uid < kNumTrios) {
    // Trio.
    return {
        /*play_type=*/DoudizhuMove::PlayType::kTrio,
        /*single_rank=*/{/*r=*/uid, /*n=*/3},
        /*chain=*/{},
        /*trio_comb=*/{},
        /*quad_comb=*/{},
        /*plane=*/{},
        /*kickers=*/{}
    };
  }
  uid -= kNumTrios;

  if (uid < kNumTrioWithSolos) {
    // Trio with solo.
    return trio_with_solos_[uid];
  }
  uid -= kNumTrioWithSolos;

  if (uid < kNumTrioWithPairs) {
    // Trio with pairs.
    return trio_with_pairs_[uid];
  }
  uid -= kNumTrioWithPairs;

  if (uid < kNumChainOfSolos) {
    // Chain of solos.
    const auto [chain_length, start_rank] = GetChainOfSoloLengthAndStartRank(uid);
    const Chain chain{/*chain_type=*/ChainType::kSolo, /*length=*/chain_length, /*start_rank=*/start_rank};
    return {
        /*play_type=*/DoudizhuMove::PlayType::kChainOfSolo,
        /*single_rank=*/{},
        /*chain=*/chain,
        /*trio_comb=*/{},
        /*quad_comb=*/{},
        /*plane=*/{},
        /*kickers=*/{}
    };
  }

  uid -= kNumChainOfSolos;

  if (uid < kNumChainOfPairs) {
    // Chain with pairs.
    const auto [chain_length, start_rank] = GetChainOfPairLengthAndStartRank(uid);
    const Chain chain{/*chain_type=*/ChainType::kPair, /*length=*/chain_length, /*start_rank=*/start_rank};
    return {
        /*play_type=*/DoudizhuMove::PlayType::kChainOfPair,
        /*single_rank=*/{},
        /*chain=*/chain,
        /*trio_comb=*/{},
        /*quad_comb=*/{},
        /*plane=*/{},
        /*kickers=*/{}
    };
  }

  uid -= kNumChainOfPairs;

  if (uid < kNumChainOfTrios) {
    // Chain with pairs.
    const auto [chain_length, start_rank] = GetChainOfTrioLengthAndStartRank(uid);
    const Chain chain{/*chain_type=*/ChainType::kTrio, /*length=*/chain_length, /*start_rank=*/start_rank};
    return {
        /*play_type=*/DoudizhuMove::PlayType::kChainOfTrio,
        /*single_rank=*/{},
        /*chain=*/chain,
        /*trio_comb=*/{},
        /*quad_comb=*/{},
        /*plane=*/{},
        /*kickers=*/{}
    };
  }

  uid -= kNumChainOfTrios;

  if (uid < kNumLength2PlaneWithSolos) {
    return plane_with_solo_per_length_.at(2)[uid];
  }
  uid -= kNumLength2PlaneWithSolos;

  if (uid < kNumLength3PlaneWithSolos) {
    return plane_with_solo_per_length_.at(3)[uid];
  }
  uid -= kNumLength3PlaneWithSolos;

  if (uid < kNumLength4PlaneWithSolos) {
    return plane_with_solo_per_length_.at(4)[uid];
  }
  uid -= kNumLength4PlaneWithSolos;

  if (uid < kNumLength5PlaneWithSolos) {
    return plane_with_solo_per_length_.at(5)[uid];
  }
  uid -= kNumLength5PlaneWithSolos;

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
  if (uid < kNumQuadWithSolos) {
    return quad_with_solos_[uid];
  }
  uid -= kNumQuadWithSolos;

  // Quad with pairs.
  if (uid < kNumQuadWithPairs) {
    return quad_with_pairs_[uid];
  }
  uid -= kNumQuadWithPairs;

  // Bombs.
  if (uid < kNumBombs) {
    const SingleRank single_rank{/*r=*/uid, /*n=*/4};
    return {
        /*play_type=*/DoudizhuMove::PlayType::kBomb,
        /*single_rank=*/single_rank,
        /*chain=*/{},
        /*trio_comb=*/{},
        /*quad_comb=*/{},
        /*plane=*/{},
        /*kickers=*/{}
    };
  }

  uid -= kNumBombs;

  // Rocket.
  if (uid < kNumRockets) {
    return {
        /*play_type=*/DoudizhuMove::PlayType::kRocket,
        /*single_rank=*/{},
        /*chain=*/{},
        /*trio_comb=*/{},
        /*quad_comb=*/{},
        /*plane=*/{},
        /*kickers=*/{}
    };
  }

  uid -= kNumRockets;
  if (uid < kNumPasses) {
    return {
        /*play_type=*/DoudizhuMove::PlayType::kPass,
        /*single_rank=*/{},
        /*chain=*/{},
        /*trio_comb=*/{},
        /*quad_comb=*/{},
        /*plane=*/{},
        /*kickers=*/{}
    };
  }

  return {};

}
int DoudizhuGame::GetTrioWithSoloRank(int rank, int remainder) const {
  int num = 0;
  for (int i = 0; i < kNumRanks && num <= remainder; ++i) {
    if (i != rank) ++num;
  }
  return num;
}
int DoudizhuGame::GetTrioWithPairRank(int rank, int remainder) const {
  int num = 0;
  for (int i = 0; i < kNumCardsPerSuit && num < remainder; ++i) {
    if (i != rank) ++num;
  }
  return num;
}
std::pair<int, int> DoudizhuGame::GetChainOfSoloLengthAndStartRank(int uid) const {
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
std::pair<int, int> DoudizhuGame::GetChainOfPairLengthAndStartRank(int uid) const {
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
std::pair<int, int> DoudizhuGame::GetChainOfTrioLengthAndStartRank(int uid) const {
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
DoudizhuMove DoudizhuGame::PickRandomChance(const std::pair<std::vector<DoudizhuMove>,
                                                            std::vector<double>> &chance_outcomes) const {
  std::discrete_distribution<std::mt19937::result_type> dist(
      chance_outcomes.second.begin(), chance_outcomes.second.end());
  return chance_outcomes.first[dist(rng_)];
}

}


