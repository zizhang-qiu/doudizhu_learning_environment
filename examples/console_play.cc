//
// Created by qzz on 2024/6/14.
//
#include <iostream>
#include "doudizhu_state.h"
#include "doudizhu_observation.h"

namespace dle = doudizhu_learning_env;

void PlayGame(const std::shared_ptr<dle::DoudizhuGame> &game,
              int player_position,
              std::mt19937 *rng) {
  dle::DoudizhuState state(game);
  while (!state.IsTerminal()) {
    // Chance node.
    if (state.IsChanceNode()) {
      state.ApplyRandomChance();
      continue;
    }

    const auto legal_moves = state.LegalMoves();
    // Other players' turn.
    if (state.CurrentPlayer() != player_position) {
      std::uniform_int_distribution<std::mt19937::result_type> dist(
          0, legal_moves.size() - 1);
      const auto &move = legal_moves[dist(*rng)];
      state.ApplyMove(move);
      continue;
    }
    // My turn.
    std::cout << "My turn to make a move, current observation:\n";
    const dle::DoudizhuObservation obs{state};
    std::cout << obs.ToString() << "\n";
    std::cout << "Legal moves:\n";
    for (const auto &move : legal_moves) {
      std::cout << move.ToString() << ", ";
    }
    std::cout << "\n";
    std::cout << "Please enter your move.\n";
    std::string move_str;
    std::cin >> move_str;
    const auto move = dle::GetMoveFromString(move_str,
                                             state.CurrentPhase() == dle::Phase::kAuction
                                             ? doudizhu_learning_env::DoudizhuMove::kAuction
                                             : doudizhu_learning_env::DoudizhuMove::kPlay);
    std::cout << "The move you choose is " << move.ToString() << std::endl;
    state.ApplyMove(move);
  }

  std::cout << state.ToString() << std::endl;
}

int main(int argc, char **argv) {
  std::mt19937 rng;
  int game_seed = -1;
  while (game_seed == -1) {
    game_seed = static_cast<int>(std::random_device()());
  }
  rng.seed(std::random_device()());
  const dle::GameParameters params = {
      {"seed", std::to_string(game_seed)}
  };
  const auto game = std::make_shared<dle::DoudizhuGame>(params);
  std::cout << "Created game with seed=" << game_seed << std::endl;
  PlayGame(game, 0, &rng);

  return 0;
}