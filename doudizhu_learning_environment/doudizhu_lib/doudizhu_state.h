//
// Created by qzz on 2024/6/5.
//

#ifndef DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_DOUDIZHU_STATE_H_
#define DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_DOUDIZHU_STATE_H_
#include <utility>

#include "doudizhu_deck.h"
#include "doudizhu_move.h"
#include "doudizhu_history_item.h"
#include "doudizhu_game.h"

namespace doudizhu_learning_env {
class Trick {
 public:
  Trick(int leader, const DoudizhuMove &move) : winning_player_(leader),
                                                leader_(leader), winning_move_(move) {}

  Trick() : Trick(kInvalidPlayer, {}) {}
  void Play(int player, const DoudizhuMove &move) {
    winning_player_ = player;
    winning_move_ = move;
  }
  DoudizhuMove WinningMove() const {
    return winning_move_;
  }
  int Winner() const { return winning_player_; }
  int Leader() const { return leader_; }
 private:
  DoudizhuMove winning_move_;
  const int leader_;
  int winning_player_;
};

enum class Phase {
  kDeal,
  kAuction,
  kPlay,
  kGameOver
};

class DoudizhuState {
 public:
  explicit DoudizhuState(const std::shared_ptr<DoudizhuGame> &parent_game);
  bool IsChanceNode() const { return phase_ == Phase::kDeal; }
  bool IsTerminal() const { return phase_ == Phase::kGameOver; }
  int CurrentPlayer() const;
  Phase CurrentPhase() const { return phase_; }
  std::string ToString() const;
  bool MoveIsLegal(const DoudizhuMove &move) const;
  void ApplyMove(const DoudizhuMove &move);
  std::vector<DoudizhuMove> LegalMoves(int player) const;
  std::vector<DoudizhuMove> LegalMoves() const { return LegalMoves(CurrentPlayer()); }
  std::pair<std::vector<DoudizhuMove>, std::vector<double>> ChanceOutcomes() const;
  void ApplyRandomChance();
  const std::vector<DoudizhuHand> &Hands() const { return hands_; }
  const DoudizhuDeck &Deck() const { return deck_; }
  std::shared_ptr<DoudizhuGame> ParentGame() const { return parent_game_; }
  const std::vector<DoudizhuHistoryItem> &MoveHistory() const { return move_history_; }
  const std::array<uint8_t, kNumRanks> &PlayedCardsPerRank() const { return played_cards_per_rank_; }
  int Dizhu() const { return dizhu_; }
  int WinningBid() const { return winning_bid_; }
  int NumBombsPlayed() const { return num_bombs_played_; }
  const std::vector<DoudizhuCard> &CardsLeftOver() const { return cards_left_over_; }
  std::vector<double> Returns() const { return returns_; }
  bool operator==(const DoudizhuState &other_state) const { return ToString() == other_state.ToString(); }
 private:
  DoudizhuDeck deck_;
  Phase phase_;
  int current_player_;
  int dizhu_;
  int final_winner_;
  int num_passes_;
  bool new_trick_begin_;
  // Three landlord cards.
  std::vector<DoudizhuCard> cards_left_over_;
  std::vector<DoudizhuHand> hands_;
  std::array<uint8_t, kNumRanks> played_cards_per_rank_;
  std::vector<DoudizhuHistoryItem> move_history_;
  std::shared_ptr<DoudizhuGame> parent_game_ = nullptr;
  int winning_bid_ = 0;
  int num_bombs_played_ = 0;
  int num_played_ = 0;
  int trick_played_ = 0;
  std::vector<Trick> tricks_{};
  std::array<int, kNumPlayers> players_hands_played{};
  std::vector<double> returns_ = std::vector<double>(kNumPlayers);;

  int PlayerToDeal() const;
  Trick &CurrentTrick() { return tricks_[trick_played_]; }
  const Trick &CurrentTrick() const { return tricks_[trick_played_]; }
  bool DealIsLegal(const DoudizhuMove &move) const;
  bool AuctionIsLegal(const DoudizhuMove &move) const;
  bool PlayIsLegal(const DoudizhuMove &move) const;
  void ApplyDealMove(const DoudizhuMove &move);
  void ApplyAuctionMove(const DoudizhuMove &move);
  bool AfterPlayHand(int player, const DoudizhuMove &move);
  void ApplyPlayMove(const DoudizhuMove &move);
  void ScoreUp();
  std::string FormatAuction() const;
  std::string FormatPlay() const;
  std::string FormatResult() const;
  std::vector<DoudizhuHand> OriginalDeal() const;
  std::string FormatDeal() const;
};
}
#endif //DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_DOUDIZHU_STATE_H_
