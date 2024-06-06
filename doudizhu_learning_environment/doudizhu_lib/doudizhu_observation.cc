//
// Created by qzz on 2024/6/6.
//

#include "doudizhu_observation.h"
namespace doudizhu_learning_env {

int PlayerToOffset(int pid, int observer_pid) {
  const int direct_offset = pid - observer_pid;
  return direct_offset < 0 ? direct_offset + kNumPlayers : direct_offset;
}

void ChangeHistoryItemToObserverRelative(const int observer_player,
                                         DoudizhuHistoryItem *item) {
  if (item->move.MoveType() == DoudizhuMove::kDeal) {
    CHECK_TRUE(item->player == kChancePlayerId
                   && (item->deal_to_player > 0 || item->deal_to_player == kLeftOverPlayer));
    if (item->deal_to_player > 0) {
      item->deal_to_player =
          (item->deal_to_player - observer_player + kNumPlayers) % kNumPlayers;
    }
  } else {
    CHECK_GE(item->player, 0);
    item->player = (item->player - observer_player + kNumPlayers) % kNumPlayers;
  }
}
DoudizhuObservation::DoudizhuObservation(const DoudizhuState &state, int observing_player)
    : cur_player_offset_(PlayerToOffset(state.CurrentPlayer(), observing_player)),
      observing_player_(observing_player),
      dizhu_(state.Dizhu()),
      cards_left_over_(state.CardsLeftOver()),
      current_phase_(state.CurrentPhase()),
      winning_bid_(state.WinningBid()),
      num_bombs_played_(state.NumBombsPlayed()),
      legal_moves_(state.LegalMoves(observing_player)) {
  hands_.reserve(kNumPlayers);
  hands_.push_back(state.Hands()[observing_player_]);
  for (int offset = 1; offset < kNumPlayers; ++offset) {
    hands_.push_back(state.Hands()[(observing_player_ + offset) % kNumPlayers]);
  }

  const auto &history = state.MoveHistory();
  for (int i = kNumCards; i < static_cast<int>(history.size()); ++i) {
    auto item = history[i];
    ChangeHistoryItemToObserverRelative(observing_player_, &item);
    switch (item.move.MoveType()) {
      case DoudizhuMove::kAuction:auction_history_.push_back(item);
        break;
      case DoudizhuMove::kPlay:play_history_.push_back(item);
        break;
      default:FatalError("Should not reach here.");
    }
  }
}
std::string DoudizhuObservation::ToString() const {
  std::string rv{};
  rv += "Observing player: " + std::to_string(observing_player_) + "\n";
  rv += "Hand: " + hands_[0].ToString() + "\n";
  if (!auction_history_.empty()) {
    rv += "Auction history:\n";
    for (const auto &auction_move : auction_history_) {
      rv += auction_move.ToString() + "\n";
    }
  }
  if (!play_history_.empty()) {
    rv += "Three dizhu cards: ";
    for (const auto &card : cards_left_over_) {
      rv += card.ToString();
    }
    rv += "\n";
    rv += "Play history:\n";
    for (const auto &play_move : play_history_) {
      rv += play_move.ToString() + "\n";
    }
  }
  // Show how many cards left.
  rv += "Number of cards held by players:\n";
  for (int offset = 0; offset < kNumPlayers; ++offset) {
    rv += "Player " + std::to_string((observing_player_ + offset) % kNumPlayers) + ": "
        + std::to_string(hands_[offset].Size()) + "\n";
  }
  return rv;
}
}