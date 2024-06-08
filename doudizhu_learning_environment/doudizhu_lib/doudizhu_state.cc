//
// Created by qzz on 2024/6/5.
//

#include <iostream>
#include "doudizhu_state.h"
namespace doudizhu_learning_env {
DoudizhuState::DoudizhuState(const std::shared_ptr<DoudizhuGame> &parent_game)
    : deck_(),
      phase_(Phase::kDeal),
      current_player_(kInvalidPlayer),
      dizhu_(kInvalidPlayer),
      final_winner_(kInvalidPlayer),
      num_passes_(0),
      new_trick_begin_(false),
      cards_left_over_(),
      hands_(kNumPlayers),
      move_history_(),
      parent_game_(parent_game) {
  std::fill(played_cards_per_rank_.begin(), played_cards_per_rank_.end(), 0);
}

int DoudizhuState::PlayerToDeal() const {
  if (deck_.Empty()) {
    return kInvalidPlayer;
  }
  if (deck_.Size() <= kNumCardsLeftOver) {
    return kLeftOverPlayer;
  }
  return (kNumCards - deck_.Size()) % kNumPlayers;
}
bool DoudizhuState::DealIsLegal(const DoudizhuMove &move) const {
  if (phase_ != Phase::kDeal) {
    return false;
  }
  return deck_.CardInDeck(move.DealCard());
}
bool DoudizhuState::AuctionIsLegal(const DoudizhuMove &move) const {
  if (phase_ != Phase::kAuction) {
    return false;
  }
  if (move.Auction() == DoudizhuMove::AuctionType::kPass) {
    // Pass is always legal.
    return true;
  }
  // Otherwise, a higher bid is legal.
  return static_cast<int>(move.Auction()) > winning_bid_;
}
void DoudizhuState::ApplyDealMove(const DoudizhuMove &move) {
  if (current_player_ != kLeftOverPlayer) {
//    std::cout << "Player to deal: " << PlayerToDeal() << std::endl;
    hands_[PlayerToDeal()].AddCard(move.DealCard());
  } else {
    cards_left_over_.push_back(move.DealCard());
  }
  deck_.DealCard(CardIndex(move.DealCard()));
  if (deck_.Size() == kNumCardsLeftOver) {
    current_player_ = kLeftOverPlayer;
  }
  // Enter auction phase.
  if (deck_.Empty()) {
    phase_ = Phase::kAuction;
    current_player_ = 0;
  }
}
void DoudizhuState::ApplyAuctionMove(const DoudizhuMove &move) {
  if (move.Auction() == DoudizhuMove::AuctionType::kPass) {
    ++num_passes_;
  } else {
    num_passes_ = 0;
  }

  bool has_winner = false;
  if (move.Auction() == DoudizhuMove::AuctionType::kPass) {
    if (num_passes_ == kNumPlayers) {
      phase_ = Phase::kGameOver;
    } else if (num_passes_ == kNumPlayers - 1 && winning_bid_ > 0) {
      has_winner = true;
    }
  } else {
    dizhu_ = current_player_;
    winning_bid_ = static_cast<int>(move.Auction());
    if (winning_bid_ == kNumBids) {
      has_winner = true;
    }
  }

  if (has_winner) {
    for (const auto &card : cards_left_over_) {
      hands_[dizhu_].AddCard(card);
    }
    phase_ = Phase::kPlay;
    current_player_ = dizhu_;
    new_trick_begin_ = true;
    tricks_.push_back(Trick(dizhu_, {}));
    num_passes_ = 0;
  } else {
    current_player_ = (current_player_ + 1) % kNumPlayers;
  }
}
bool DoudizhuState::AfterPlayHand(int player, const DoudizhuMove &move) {
  const auto ranks = move.ToRanks();
  for (const int rank : ranks) {
    hands_[player].RemoveFromHand(rank, played_cards_per_rank_);
  }
  bool flag = true;
  for (int rank = 0; rank < kNumRanks; ++rank) {
    if (hands_[player].CardsPerRank()[rank] > 0) {
      flag = false;
      break;
    }
  }
  return flag;
}
void DoudizhuState::ApplyPlayMove(const DoudizhuMove &move) {
  ++num_played_;
  if (move.GetPlayType() == DoudizhuMove::PlayType::kPass) {
    ++num_passes_;
  } else {
    num_passes_ = 0;
  }

  if (move.GetPlayType() == DoudizhuMove::PlayType::kPass) {
    if (num_passes_ == kNumPlayers - 1) {
      current_player_ = CurrentTrick().Winner();
      ++trick_played_;
      num_passes_ = 0;
      tricks_.emplace_back();
      new_trick_begin_ = true;
    }else{
      current_player_ = (current_player_ + 1) % kNumPlayers;
    }
  } else {
    if (move.IsBomb()) { ++num_bombs_played_; }
    ++players_hands_played[current_player_];

    if (new_trick_begin_) {
      new_trick_begin_ = false;
    }

    CurrentTrick().Play(current_player_, move);

    const bool all_played = AfterPlayHand(current_player_, move);
    if (all_played) {
      final_winner_ = current_player_;
      ScoreUp();
      phase_ = Phase::kGameOver;
    }

    current_player_ = (current_player_ + 1) % kNumPlayers;
  }

}

void DoudizhuState::ApplyMove(const DoudizhuMove &move) {
  DoudizhuHistoryItem history(move);
  switch (move.MoveType()) {

    case DoudizhuMove::kInvalid:FatalError("Move is invalid.");
    case DoudizhuMove::kDeal:history.player = current_player_;
      history.deal_to_player = PlayerToDeal();
      history.deal_card = move.DealCard();
      ApplyDealMove(move);
      break;
    case DoudizhuMove::kAuction:history.player = current_player_;
      history.auction_type = move.Auction();
      ApplyAuctionMove(move);
      break;
    case DoudizhuMove::kPlay:history.player = current_player_;
      history.play_type = move.GetPlayType();
      history.single_rank = move.GetSingleRank();
      history.chain = move.GetChain();
      history.trio_comb = move.GetTrioComb();
      history.quad_comb = move.GetQuadComb();
      history.kickers = move.Kickers();
      ApplyPlayMove(move);
      break;
    default:FatalError("Should not reach here.");
  }
  move_history_.push_back(history);
}

int DoudizhuState::CurrentPlayer() const {
  if (phase_ == Phase::kDeal) {
    return kChancePlayerId;
  } else if (phase_ == Phase::kGameOver) {
    return kTerminalPlayerId;
  } else {
    return current_player_;
  }
}
void DoudizhuState::ScoreUp() {
  // No one becomes dizhu.
  if (dizhu_ == kInvalidPlayer) {
    return;
  }

  // if none of the farmers played, or the dizhu only played once
  // then it is spring!
  bool is_spring = false;
  is_spring |= (players_hands_played[dizhu_] == 1);
  is_spring |= ((!players_hands_played[(dizhu_ + 1) % 3]) &&
      (!players_hands_played[(dizhu_ + 2) % 3]));

  int score = winning_bid_;
  for (int i = 0; i < is_spring + num_bombs_played_; ++i) {
    score *= 2;
  }
  const int dizhu_sign = final_winner_ == dizhu_ ? 1 : -1;
  returns_[dizhu_] = dizhu_sign * 2 * score;
  returns_[(dizhu_ + 1) % 3] = -dizhu_sign * score;
  returns_[(dizhu_ + 2) % 3] = -dizhu_sign * score;
}
bool DoudizhuState::PlayIsLegal(const DoudizhuMove &move) const {
  if (phase_ != Phase::kPlay) {
    return false;
  }
  if (!HandCanMakeMove(hands_[current_player_], move))return false;
  // A player can make any move except pass.
  if (new_trick_begin_) {
    return move.GetPlayType() != DoudizhuMove::PlayType::kPass;
  }
  if (move.GetPlayType() == DoudizhuMove::PlayType::kPass) {
    // A player can't pass if he wins previous trick.
    if (new_trick_begin_) {
      return false;
    }
    return true;
  }

  if (move.IsBomb()) {
    // If the last move in current trick is not a bomb, a bomb is always legal.
    if (!CurrentTrick().WinningMove().IsBomb()) {
      return true;
    }
    // Otherwise, a bomb should have higher rank than previous one.
    return move.BombRank() > CurrentTrick().WinningMove().BombRank();
  }

  // If a move is not a bomb and has different type with previous move, it is illegal.
  if (move.GetPlayType() != CurrentTrick().WinningMove().GetPlayType()) {
    return false;
  }

  const auto current_trick_winning_move = CurrentTrick().WinningMove();
  switch (move.GetPlayType()) {
    case DoudizhuMove::PlayType::kSolo:
    case DoudizhuMove::PlayType::kPair:
    case DoudizhuMove::PlayType::kTrio:
      return move.GetSingleRank().rank > current_trick_winning_move.GetSingleRank().rank;
    case DoudizhuMove::PlayType::kTrioWithSolo:
    case DoudizhuMove::PlayType::kTrioWithPair:
      return move.GetTrioComb().trio_rank > current_trick_winning_move.GetTrioComb().trio_rank;
    case DoudizhuMove::PlayType::kChainOfSolo:
    case DoudizhuMove::PlayType::kChainOfPair:
    case DoudizhuMove::PlayType::kChainOfTrio: {
      // A chain should have same chain type and length with previous one.
      if (move.GetChain().chain_type != current_trick_winning_move.GetChain().chain_type) {
        return false;
      }
      if (move.GetChain().length != current_trick_winning_move.GetChain().length) {
        return false;
      }
      return move.GetChain().start_rank > current_trick_winning_move.GetChain().start_rank;
    }
    case DoudizhuMove::PlayType::kPlaneWithSolo:
    case DoudizhuMove::PlayType::kPlaneWithPair: {
      // A plane should have same kicker type and length with previous one.
      if (move.GetPlane().kicker_type != current_trick_winning_move.GetPlane().kicker_type
          || move.GetPlane().length != current_trick_winning_move.GetPlane().length) {
        return false;
      }
      return move.GetPlane().start_rank > current_trick_winning_move.GetPlane().start_rank;
    }
    case DoudizhuMove::PlayType::kQuadWithSolo:
    case DoudizhuMove::PlayType::kQuadWithPair: {
      // A quad comb should have same kicker type with previous one.
      if (move.GetQuadComb().kicker_type != current_trick_winning_move.GetQuadComb().kicker_type) {
        return false;
      }
      return move.GetQuadComb().quad_rank > current_trick_winning_move.GetQuadComb().quad_rank;
    }
    default:FatalError("PlayIsLegal(): Should not reach here.");
  }
}
bool DoudizhuState::MoveIsLegal(const DoudizhuMove &move) const {
  switch (move.MoveType()) {
    case DoudizhuMove::kInvalid:return false;
    case DoudizhuMove::kDeal:return DealIsLegal(move);
    case DoudizhuMove::kAuction:return AuctionIsLegal(move);
    case DoudizhuMove::kPlay:return PlayIsLegal(move);
    default:FatalError("Should not reach here.");
  }
}
std::vector<DoudizhuMove> DoudizhuState::LegalMoves(int player) const {
  CHECK_GE(player, 0);
  CHECK_LT(player, kNumPlayers);
  if (player != CurrentPlayer()) {
    // Turn-based game. Empty move list for other players.
    return {};
  }

  std::vector<DoudizhuMove> moves;
  const int max_move_uid = parent_game_->MaxMoves();
  for (int uid = 0; uid < max_move_uid; ++uid) {
    const DoudizhuMove move = parent_game_->GetMove(uid);
    if (MoveIsLegal(move)) {
      moves.push_back(move);
    }
  }
  return moves;
}
std::pair<std::vector<DoudizhuMove>, std::vector<double>> DoudizhuState::ChanceOutcomes() const {
  std::pair<std::vector<DoudizhuMove>, std::vector<double>> rv;
  int max_outcome_uid = parent_game_->MaxChanceOutcomes();
  for (int uid = 0; uid < max_outcome_uid; ++uid) {
    const DoudizhuMove move = parent_game_->GetChanceOutcome(uid);
    if (MoveIsLegal(move)) {
      rv.first.push_back(move);
      rv.second.push_back(1.0 / static_cast<double>(deck_.Size()));
    }
  }
  return rv;
}
void DoudizhuState::ApplyRandomChance() {
  const auto chance_outcomes = ChanceOutcomes();
  CHECK_FALSE(chance_outcomes.second.empty());
  const auto chance = parent_game_->PickRandomChance(chance_outcomes);
//  std::cout << chance.ToString() << std::endl;
  ApplyMove(chance);
}
std::string DoudizhuState::FormatAuction() const {
  CHECK_TRUE(deck_.Empty());
  std::string rv = "Bidding phase begin\n";
  for (int i = kNumCards; i < move_history_.size() - num_played_; ++i) {
    rv += move_history_[i].ToString();
    rv += "\n";
  }
  return rv;
}
std::string DoudizhuState::FormatPlay() const {
  CHECK_GT(num_played_, 0);
  std::string rv = "Playing phase begin \n";
  for (int i = static_cast<int>(move_history_.size()) - num_played_; i < move_history_.size(); ++i) {
    rv += move_history_[i].ToString();
    rv += "\n";
  }
  return rv;
}
std::string DoudizhuState::FormatResult() const {
  CHECK_TRUE(IsTerminal());
  std::string rv = "The results are: \n";
  for (int player = 0; player < kNumPlayers; ++player) {
    rv += "Player " + std::to_string(player) + " got " + std::to_string(returns_[player]);
    rv += "\n";
  }
  return rv;
}
std::vector<DoudizhuHand> DoudizhuState::OriginalDeal() const {
  CHECK_GE(move_history_.size(), kNumCards + 1);
  std::vector<DoudizhuHand> rv(kNumPlayers);
  for (int i = 0; i < kNumCards - kNumCardsLeftOver; ++i) {
    rv[i % kNumPlayers].AddCard(move_history_[i].deal_card);
  }
  for (const auto &card : cards_left_over_) {
    rv[dizhu_].AddCard(card);
  }
  return rv;
}
std::string DoudizhuState::FormatDeal() const {
  std::vector<DoudizhuHand> hands{};
  std::string rv;
  if (IsTerminal()) {
    hands = OriginalDeal();
  } else {
    hands = hands_;
  }
  rv += "Three dizhu cards: ";
  for (const auto &card : cards_left_over_) {
    rv += card.ToString() + " ";
  }
  rv += "\n";
  for (int pl = 0; pl < kNumPlayers; ++pl) {
    rv += "player " + std::to_string(pl) + ": ";
    rv += hands[pl].ToString();
    rv += "\n";
  }
  return rv;
}
std::string DoudizhuState::ToString() const {
  std::string rv = FormatDeal();

  if (move_history_.size() > kNumCards) {
    rv += FormatAuction();
  }
  if (num_played_ > 0) {
    rv += FormatPlay();
  }
  if (IsTerminal()) {
    rv += FormatResult();
  }
  return rv;
}

}