//
// Created by qzz on 2024/5/30.
//

#ifndef DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_DOUDIZHU_GAME_H_
#define DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_DOUDIZHU_GAME_H_
#include <memory>
#include <random>
#include <utility>

#include "doudizhu_move.h"
#include "utils.h"

namespace doudizhu_learning_env {
class DoudizhuGame {
  public:
    // Acceptable parameters:
    //
    // "seed": Pseudo-random number generator seed. (default 42)
    // "allow_repeated_kickers": Whether it is legal to have
    // repeated kickers in planes, e.g. 333344, 33344455. (default true)
    // "allow_space_shuttle": Whether it is legal to play space shuttles, e.g.
    // 3333444456. (default false)

    explicit DoudizhuGame(const GameParameters &parameters);

    const std::vector<DoudizhuMove> &AllMoves() const { return moves_; }

    const std::vector<DoudizhuMove> &AllChanceOutcomes() const {
      return chance_outcomes_;
    }

    int MaxMoves() const { return MaxAuctionMoves() + MaxPlayMoves(); }

    int MaxChanceOutcomes() const { return MaxDealMoves(); }

    int MaxScore() const { return kMaxUtility; }

    int MinScore() const { return kMinUtility; }

    int MaxGameLength() { return kMaxAuctionLength + kNumCards * kNumPlayers; }

    GameParameters Parameters() const { return parameters_; }

    DoudizhuMove GetMove(const int uid) const { return moves_[uid]; }

    DoudizhuMove GetChanceOutcome(const int uid) const { return chance_outcomes_[uid]; }

    int GetMoveUid(const DoudizhuMove &move) const;

    int GetChanceOutcomeUid(const DoudizhuMove &chance) const;

    DoudizhuMove PickRandomChance(
      const std::pair<std::vector<DoudizhuMove>, std::vector<double> >
      &chance_outcomes) const;

    bool operator==(const DoudizhuGame &other_game) const {
      return parameters_ == other_game.parameters_;
    }

    bool AllowRepeatedKickers() const { return allow_repeated_kickers_; }

    bool AllowSpaceShuttle() const { return allow_space_shuttle_; }

  private:
    GameParameters parameters_;
    int seed_ = -1;
    bool allow_repeated_kickers_;
    bool allow_space_shuttle_;
    mutable std::mt19937 rng_;
    // Save moves here for faster construction.
    // [length, move]
    std::unordered_map<int, std::vector<DoudizhuMove> >
    plane_with_solo_per_length_{};
    std::unordered_map<int, std::vector<DoudizhuMove> >
    plane_with_pair_per_length_{};
    std::vector<DoudizhuMove> trio_with_solos_{};
    std::vector<DoudizhuMove> trio_with_pairs_{};
    std::vector<DoudizhuMove> quad_with_solos_{};
    std::vector<DoudizhuMove> quad_with_pairs_{};
    std::unordered_map<int, std::vector<DoudizhuMove> >
    space_shuttle_per_length_{};
    std::unordered_map<int, std::vector<DoudizhuMove> >
    space_shuttle_with_solo_per_length_{};
    std::unordered_map<int, std::vector<DoudizhuMove> >
    space_shuttle_with_pair_per_length_{};
    std::vector<DoudizhuMove> chance_outcomes_{};
    std::vector<DoudizhuMove> moves_{};

    int MaxDealMoves() const { return kNumCards; }

    int MaxAuctionMoves() const { return kNumBids + 1; }

    int MaxPlayMoves() const {
      int num_moves = MaxPassMoves()
          + MaxSoloMoves()
          + MaxPairMoves()
          + MaxTrioMoves()
          + MaxBombMoves()
          + MaxTrioWithSoloMoves()
          + MaxTrioWithPairMoves()
          + MaxChainOfSoloMoves()
          + MaxChainOfPairMoves()
          + MaxChainOfTrioMoves()
          + MaxPlaneWithSoloMoves()
          + MaxPlaneWithPairMoves()
          + MaxQuadWithSoloMoves()
          + MaxQuadWithPairMoves()
          + MaxRocketMoves();
      if (allow_space_shuttle_) {
        num_moves += MaxSpaceShuttleMoves()
            + MaxSpaceShuttleWithSoloMoves()
            + MaxSpaceShuttleWithPairMoves();
      }
      return num_moves;
    }

    int MaxPassMoves() const {
      return kNumPasses;
    }

    int MaxSoloMoves() const { return kNumSolos; }

    int MaxPairMoves() const { return kNumPairs; }

    int MaxTrioMoves() const { return kNumTrios; }

    int MaxBombMoves() const { return kNumBombs; }

    int MaxTrioWithSoloMoves() const {
      return kNumTrioWithSolos;
    }

    int MaxTrioWithPairMoves() const {
      return kNumTrioWithPairs;
    }

    int MaxChainOfSoloMoves() const {
      return kNumChainOfSolos;
    }

    int MaxChainOfPairMoves() const {
      return kNumChainOfPairs;
    }

    int MaxChainOfTrioMoves() const {
      return kNumChainOfTrios;
    }

    int MaxPlaneWithSoloMoves() const {
      return GetNumPlaneWithSolo(allow_repeated_kickers_);
    }

    int MaxPlaneWithPairMoves() const {
      return kNumPlaneWithPairs;
    }

    int MaxQuadWithSoloMoves() const {
      return GetNumQuadWithSolo(allow_repeated_kickers_);
    }

    int MaxQuadWithPairMoves() const {
      return kNumQuadWithPairs;
    }

    int MaxRocketMoves() const {
      return kNumRockets;
    }

    int MaxSpaceShuttleMoves() const {
      return GetNumSpaceShuttle();
    }

    int MaxSpaceShuttleWithSoloMoves() const {
      return GetNumSpaceShuttleWithSolo();
    }

    int MaxSpaceShuttleWithPairMoves() const {
      return GetNumSpaceShuttleWithPair();
    }

    DoudizhuMove ConstructChanceOutcome(int uid) const;
    DoudizhuMove ConstructMove(int uid) const;
    std::pair<int, int> GetChainOfSoloLengthAndStartRank(int uid) const;
    std::pair<int, int> GetChainOfPairLengthAndStartRank(int uid) const;
    std::pair<int, int> GetChainOfTrioLengthAndStartRank(int uid) const;
};

const GameParameters default_params = {};
const auto default_game = std::make_shared<DoudizhuGame>(default_params);
} // namespace doudizhu_learning_env
#endif // DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_DOUDIZHU_GAME_H_
