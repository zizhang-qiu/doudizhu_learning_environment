//
// Created by qzz on 2024/6/15.
//

#include "douzero_encoder.h"
namespace doudizhu_learning_env {
std::array<std::array<int, kNumRanks>, kNumPlayers> GetPlayedCardsPerRankByPlayer(const DoudizhuObservation &obs) {
  std::array<std::array<int, kNumRanks>, kNumPlayers> res{};
  const auto &play_history = obs.PlayHistory();
  for (const auto &item : play_history) {
    const auto ranks = item.move.ToRanks();
    for (const int rank : ranks) {
      res[item.player][rank]++;
    }
  }
  return res;
}

std::vector<int> EncodeCards(const std::array<int, kNumRanks> &cards_per_rank) {
  std::vector<int> encoding(kHandFeatureSize, 0);
  for (int rank = 0; rank < kNumRanks; ++rank) {
    const int num_cards_this_rank = cards_per_rank[rank];
    if (rank >= kBlackJoker) {
      if (num_cards_this_rank == 1) {
        encoding[kHandFeatureSize - 1 - (kNumRanks - rank) + 1] = 1;
      }
      continue;
    }
    for (int i = 0; i < num_cards_this_rank; ++i) {
      encoding[rank * kNumSuits + i] = 1;
    }
  }
  return encoding;
}

std::vector<int> EncodeMove(const DoudizhuMove &move) {
  const auto ranks = move.ToRanks();
  std::array<int, kNumRanks> cards_per_rank{};
  for (const int rank : ranks) {
    cards_per_rank[rank]++;
  }
  const auto encoding = EncodeCards(cards_per_rank);
  return encoding;
}

std::vector<int> EncodeMove(const DoudizhuHistoryItem &item) {
  const auto move = item.move;
  return EncodeMove(move);
}

Feature EncodeMyHand(const DoudizhuObservation &obs) {
  const auto my_hand = obs.Hands()[0];
  const auto cards_per_rank = my_hand.CardsPerRank();
  const auto encoding = EncodeCards(cards_per_rank);
  return {/*encoding=*/encoding, /*dims=*/{kHandFeatureSize,}, /*desc=*/"Feature of hand."};
}
Feature EncodeOtherHands(const DoudizhuObservation &obs) {
  const auto cards_left_per_rank = obs.CardsLeftPerRank();
  const auto encoding = EncodeCards(cards_left_per_rank);
  return {/*encoding=*/encoding, /*dims=*/{kHandFeatureSize,}, /*desc=*/"Feature for cards in other players' hands."};
}
Feature EncodeLastMove(const DoudizhuObservation &obs) {
  if (obs.PlayHistory().empty()) {
    return {/*encoding=*/std::vector<int>(kNumCards, 0),
        /*dims=*/{kHandFeatureSize,},
        /*desc=*/"Feature for last move played."};
  }
  DoudizhuMove last_move;
  last_move = obs.PlayHistory().back().move;
  if (last_move.GetPlayType() == DoudizhuMove::PlayType::kPass) {
    if (obs.PlayHistory().size() >= 2) {
      last_move = obs.PlayHistory()[obs.PlayHistory().size() - 2].move;
    }
  }
  const auto encoding = EncodeMove(last_move);
  return {/*encoding=*/encoding,
      /*dims=*/{kMoveFeatureSize,},
      /*desc=*/"Feature for last move played."};
}
Feature EncodePlayerPlayedCards(const DoudizhuObservation &obs, int relative_player) {
  const auto played_cards_per_rank_by_player = GetPlayedCardsPerRankByPlayer(obs);
  const auto encoding = EncodeCards(played_cards_per_rank_by_player[relative_player]);
  return {/*encoding=*/encoding, /*dims=*/{kHandFeatureSize}};
}
Feature EncodePlayerNumCardsLeft(const DoudizhuObservation &obs, int relative_player) {
  const bool is_dizhu = obs.Dizhu() == relative_player;
  const int feature_size = is_dizhu ? kNumDizhuCardsLeftFeatureSize : kNumFarmerCardsLeftFeatureSize;
  const int num_player_cards_left = static_cast<int>(obs.Hands()[relative_player].Size());
  std::vector<int> encoding(feature_size, 0);
  encoding[num_player_cards_left - 1] = 1;
  return {/*encoding=*/encoding, /*dims=*/{feature_size,}};
}
Feature EncodeNumBombs(const DoudizhuObservation &obs) {
  const int num_bombs_played = obs.NumBombsPlayed();
  std::vector<int> encoding(kNumBombsFeatureSize, 0);
  encoding[num_bombs_played] = 1;
  return {/*encoding=*/encoding, /*dims=*/{kNumBombsFeatureSize,}, /*desc=*/"Feature of number of bombs played."};
}

Feature EncodeRecentMoves(const DoudizhuObservation &obs, int num_moves) {
  const auto &play_history = obs.PlayHistory();
  std::vector<DoudizhuMove> move_history;
  move_history.reserve(num_moves);
  // Padding.
  if (play_history.size() < num_moves) {
    const int num_fill = num_moves - static_cast<int>(play_history.size());
    for (int i = 0; i < num_fill; ++i) {
      move_history.emplace_back();
    }
  }

  const size_t start_index = play_history.size() > num_moves ? play_history.size() - num_moves : 0;
  for (size_t i = start_index; i < play_history.size(); ++i) {
    move_history.push_back(play_history[i].move);
  }
  CHECK_EQ(move_history.size(), num_moves);
  std::vector<int> encoding(kMoveFeatureSize * num_moves, 0);
  for (int i = 0; i < move_history.size(); ++i) {
    const auto move_encoding = EncodeMove(move_history[i]);
    std::copy(move_encoding.begin(), move_encoding.end(), encoding.begin() + i * kMoveFeatureSize);
  }
  return {/*encoding=*/encoding, /*dims=*/{num_moves / kNumPlayers, kMoveFeatureSize * kNumPlayers}};
}
Feature EncodeLegalMoves(const DoudizhuObservation &obs) {
  const auto &legal_moves = obs.LegalMoves();
  const int num_legal_moves = static_cast<int>(legal_moves.size());
  std::vector<int> encoding(kMoveFeatureSize * num_legal_moves, 0);
  for (int i = 0; i < num_legal_moves; ++i) {
    const auto move_encoding = EncodeMove(legal_moves[i]);
    std::copy(move_encoding.begin(), move_encoding.end(), encoding.begin() + i * kMoveFeatureSize);
  }
  return {/*encoding=*/encoding, /*dims=*/{num_legal_moves, kMoveFeatureSize}, /*desc=*/"Feature of legal moves."};
}
Feature EncodeRecentDizhuMove(const DoudizhuObservation &obs) {
  const auto &play_history = obs.PlayHistory();
  const auto it = std::find_if(play_history.rbegin(), play_history.rend(), [&](const DoudizhuHistoryItem &item) {
    return item.player == obs.Dizhu();
  });
  if (it != play_history.rend()) {
    const auto encoding = EncodeMove(*it);
    return {/*encoding=*/encoding, /*dims=*/{kMoveFeatureSize,}, /*desc=*/"Feature of the most recent dizhu move"};
  }
  return {/*encoding=*/std::vector<int>(kMoveFeatureSize, 0), /*dims=*/{kMoveFeatureSize,}, /*desc=*/
                       "Feature of the most recent dizhu move"};
}
Feature EncodeRecentAnotherFarmerMove(const DoudizhuObservation &obs) {
  const auto &play_history = obs.PlayHistory();
  const auto it = std::find_if(play_history.rbegin(), play_history.rend(), [&](const DoudizhuHistoryItem &item) {
    return item.player != obs.Dizhu()
        && item.player != 0;
  });
  if (it != play_history.rend()) {
    const auto encoding = EncodeMove(*it);
    return {/*encoding=*/encoding, /*dims=*/{kMoveFeatureSize,}, /*desc=*/"Feature of the most recent dizhu move"};
  }
  return {/*encoding=*/std::vector<int>(kMoveFeatureSize, 0), /*dims=*/{kMoveFeatureSize,}, /*desc=*/
                       "Feature of the most recent dizhu move"};
}

Features DouzeroEncoder::Encode(const DoudizhuObservation &obs) {
  Features features{};
  // Moves.
  const auto moves_feature = EncodeLegalMoves(obs);
  features.insert({"legal_moves", moves_feature});

  // My hand.
  const auto my_hand_feature = EncodeMyHand(obs);
  features.insert({"my_hand", my_hand_feature});

  // Union of other players' hand.
  const auto other_hands_feature = EncodeOtherHands(obs);
  features.insert({"other_hands", other_hands_feature});

  // The most recent move.
  const auto most_recent_move_feature = EncodeLastMove(obs);
  features.insert({"most_recent_move", most_recent_move_feature});

  // One-hot vector representing the number bombs in the current state.
  const auto num_bombs_feature = EncodeNumBombs(obs);
  features.insert({"num_bombs", num_bombs_feature});

  // Concatenated matrix of the most recent 15 moves.
  const auto recent_moves_feature = EncodeRecentMoves(obs, /*num_moves=*/15);
  features.insert({"recent_moves", recent_moves_feature});

  const bool is_dizhu = obs.Dizhu() == 0;
  if (is_dizhu) {
    // Played cards of the first farmer.
    const auto first_farmer_played_cards_feature = EncodePlayerPlayedCards(obs, /*relative_player=*/1);
    features.insert({"first_farmer_played_cards", first_farmer_played_cards_feature});

    // Played cards of the second farmer.
    const auto second_farmer_played_cards_feature = EncodePlayerPlayedCards(obs, /*relative_player=*/2);
    features.insert({"second_farmer_played_cards", second_farmer_played_cards_feature});

    // One-hot vector representing the number cards left of the first farmer.
    const auto first_farmer_num_cards_left_feature = EncodePlayerNumCardsLeft(obs, /*relative_player=*/1);
    features.insert({"first_farmer_num_cards_left", first_farmer_num_cards_left_feature});

    // One-hot vector representing the number cards left of the second farmer.
    const auto second_farmer_num_cards_left_feature = EncodePlayerNumCardsLeft(obs, /*relative_player=*/2);
    features.insert({"second_farmer_num_cards_left", second_farmer_num_cards_left_feature});

  } else {
    // Card matrix of the most recent move performed by the dizhu.
    const auto dizhu_most_recent_move_feature = EncodeRecentDizhuMove(obs);
    features.insert({"dizhu_most_recent_move", dizhu_most_recent_move_feature});

    // Card matrix of the most recent move performed by the other farmer.
    const auto another_farmer_most_recent_move_feature = EncodeRecentAnotherFarmerMove(obs);
    features.insert({"another_farmer_most_recent_move", another_farmer_most_recent_move_feature});

    // Played cards of the Dizhu.
    const auto dizhu_played_cards_feature = EncodePlayerPlayedCards(obs, /*relative_player=*/obs.Dizhu());
    features.insert({"dizhu_played_cards", dizhu_played_cards_feature});

    // Played cards of another farmer.
    const int another_farmer_relative = kNumPlayers - obs.Dizhu();
    const auto
        another_farmer_played_cards_feature = EncodePlayerPlayedCards(obs, /*relative_player=*/another_farmer_relative);
    features.insert({"another_farmer_played_cards", another_farmer_played_cards_feature});

    // One-hot vector representing the number cards left of the Dizhu.
    const auto dizhu_num_cards_left_feature = EncodePlayerNumCardsLeft(obs, obs.Dizhu());
    features.insert({"dizhu_num_cards_left", dizhu_num_cards_left_feature});

    // One-hot vector representing the number cards left of the other farmer.
    const auto another_farmer_num_cards_left_feature = EncodePlayerNumCardsLeft(obs, another_farmer_relative);
    features.insert({"another_farmer_num_cards_left", another_farmer_num_cards_left_feature});
  }

  return features;
}
}