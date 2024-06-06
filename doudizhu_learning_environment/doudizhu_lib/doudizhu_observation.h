//
// Created by qzz on 2024/6/6.
//

#ifndef DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_DOUDIZHU_OBSERVATION_H_
#define DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_DOUDIZHU_OBSERVATION_H_
#include "doudizhu_state.h"
namespace doudizhu_learning_env {
int PlayerToOffset(int pid, int observer_pid);

class DoudizhuObservation {
 public:
  DoudizhuObservation(const DoudizhuState &state, int observing_player);
  explicit DoudizhuObservation(const DoudizhuState &state) : DoudizhuObservation(state, state.CurrentPlayer()) {}

  int ObservingPlayer() const { return observing_player_; }
  int CurPlayerOffset() const { return cur_player_offset_; }
  int Dizhu() const { return dizhu_; }
  Phase CurrentPhase() const { return current_phase_; }
  int WinningBid() const { return winning_bid_; }
  int NumBombsPlayed() const { return num_bombs_played_; }
  const std::vector<DoudizhuHand> &Hands() const { return hands_; }
  const std::vector<DoudizhuHistoryItem> &AuctionHistory() const { return auction_history_; }
  const std::vector<DoudizhuHistoryItem> &PlayHistory() const { return play_history_; }
  std::string ToString() const;
 private:
  int observing_player_;
  int cur_player_offset_; // offset of current_player_ from observing_player
  int dizhu_;
  std::vector<DoudizhuCard> cards_left_over_;
  Phase current_phase_;
  int winning_bid_;
  int num_bombs_played_;

  // hands_[0] contains observing player's hand.
  std::vector<DoudizhuHand> hands_;
  std::vector<DoudizhuMove> legal_moves_;
  std::vector<DoudizhuHistoryItem> auction_history_;
  std::vector<DoudizhuHistoryItem> play_history_;
};
}
#endif //DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_DOUDIZHU_OBSERVATION_H_
