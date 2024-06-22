//
// Created by qzz on 2024/6/15.
//

#ifndef DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_DOUZERO_ENCODER_H_
#define DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_DOUZERO_ENCODER_H_
#include "doudizhu_observation.h"
#include "observation_encoder.h"
namespace doudizhu_learning_env {

inline constexpr int kHandFeatureSize = kNumCards;
inline constexpr int kMoveFeatureSize = kNumCards;
inline constexpr int kNumFarmerCardsLeftFeatureSize = kNumFarmerCards;
inline constexpr int kNumDizhuCardsLeftFeatureSize = kNumDizhuCards;
inline constexpr int kNumBombsFeatureSize = 15;

std::array<std::array<int, kNumRanks>, kNumPlayers> GetPlayedCardsPerRankByPlayer(const DoudizhuObservation &obs);

Feature EncodeMyHand(const DoudizhuObservation &obs);
Feature EncodeOtherHands(const DoudizhuObservation &obs);
Feature EncodeLastMove(const DoudizhuObservation &obs);
Feature EncodePlayerPlayedCards(const DoudizhuObservation &obs, int relative_player);
Feature EncodePlayerNumCardsLeft(const DoudizhuObservation &obs, int relative_player);
Feature EncodeNumBombs(const DoudizhuObservation &obs);
Feature EncodeRecentMoves(const DoudizhuObservation &obs, int num_moves);
Feature EncodeRecentDizhuMove(const DoudizhuObservation &obs);
Feature EncodeRecentAnotherFarmerMove(const DoudizhuObservation &obs);
Feature EncodeLegalMoves(const DoudizhuObservation &obs);

class DouzeroEncoder : public ObservationEncoder {

 public:
  ~DouzeroEncoder() override = default;
  Features Encode(const DoudizhuObservation &obs) const override;
};
}
#endif //DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_DOUZERO_ENCODER_H_
