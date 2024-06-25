//
// Created by qzz on 2024/5/30.
//

#ifndef DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_DOUDIZHU_GAME_H_
#define DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_DOUDIZHU_GAME_H_
#include <utility>
#include <random>
#include <memory>

#include "doudizhu_move.h"
#include "utils.h"
namespace doudizhu_learning_env {

class DoudizhuGame {
 public:
  explicit DoudizhuGame(const GameParameters &parameters);

  const std::vector<DoudizhuMove> &AllMoves() const {
    return moves_;
  }

  const std::vector<DoudizhuMove> &AllChanceOutcomes() const {
    return chance_outcomes_;
  }

  int MaxMoves() const {
    return MaxAuctionMoves() + MaxPlayMoves();
  }

  int MaxChanceOutcomes() const {
    return MaxDealMoves();
  }

  int MaxScore() const {
    return kMaxUtility;
  }

  int MinScore() const {
    return kMinUtility;
  }

  int MaxGameLength() {
    return kMaxAuctionLength + kNumCards * kNumPlayers;
  }

  GameParameters Parameters() const { return parameters_; }

  DoudizhuMove GetMove(int uid) const { return moves_[uid]; }

  DoudizhuMove GetChanceOutcome(int uid) const { return chance_outcomes_[uid]; }

  int GetMoveUid(const DoudizhuMove& move) const;

  int GetChanceOutcomeUid(const DoudizhuMove& chance) const;

  DoudizhuMove PickRandomChance(const std::pair<std::vector<DoudizhuMove>,
                                                std::vector<double>> &chance_outcomes) const;

  bool operator==(const DoudizhuGame &other_game) {
    return parameters_ == other_game.parameters_;
  }

 private:
  GameParameters parameters_;
  int seed_ = -1;
  mutable std::mt19937 rng_;
  // Save moves here for faster construction.
  // [length, move]
  std::unordered_map<int, std::vector<DoudizhuMove>> plane_with_solo_per_length_{};
  std::unordered_map<int, std::vector<DoudizhuMove>> plane_with_pair_per_length_{};
  std::vector<DoudizhuMove> trio_with_solos_{};
  std::vector<DoudizhuMove> trio_with_pairs_{};
  std::vector<DoudizhuMove> quad_with_solos_{};
  std::vector<DoudizhuMove> quad_with_pairs_{};
  std::vector<DoudizhuMove> chance_outcomes_{};
  std::vector<DoudizhuMove> moves_{};

  int MaxDealMoves() const {
    return kNumCards;
  }

  int MaxAuctionMoves() const {
    return kNumBids + 1;
  }

  int MaxPlayMoves() const {
    return kNumDistinctPlayMoves;
  }

  DoudizhuMove ConstructChanceOutcome(int uid) const;
  DoudizhuMove ConstructMove(int uid) const;
  std::pair<int, int> GetChainOfSoloLengthAndStartRank(int uid) const;
  std::pair<int, int> GetChainOfPairLengthAndStartRank(int uid) const;
  std::pair<int, int> GetChainOfTrioLengthAndStartRank(int uid) const;
};

const GameParameters default_params = {};
const auto default_game = std::make_shared<DoudizhuGame>(default_params);
}
#endif //DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_DOUDIZHU_GAME_H_
