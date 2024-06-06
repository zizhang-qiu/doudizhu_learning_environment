//
// Created by qzz on 2024/6/6.
//
#include <iostream>
#include "doudizhu_game.h"
#include "doudizhu_state.h"

namespace dle = doudizhu_learning_env;

constexpr const char *kGameParamArgPrefix = "--config.doudizhu.";

std::vector<double> SimulateGame(const dle::DoudizhuGame &game,
                                 bool verbose,
                                 std::mt19937 *rng) {
  dle::DoudizhuState state(std::make_shared<dle::DoudizhuGame>(game));
  while (!state.IsTerminal()) {
    // Chance node.
    if (state.CurrentPlayer() == dle::kChancePlayerId) {
      auto chance_outcomes = state.ChanceOutcomes();
      std::discrete_distribution<std::mt19937::result_type> dist(
          chance_outcomes.second.begin(), chance_outcomes.second.end());
      auto move = chance_outcomes.first[dist(*rng)];
      if (verbose) {
        std::cout << "Legal chance:";
        for (int i = 0; i < chance_outcomes.first.size(); ++i) {
          std::cout << " <" << chance_outcomes.first[i].ToString() << ", " << chance_outcomes.second[i] << ">";
        }
        std::cout << "\n";
        std::cout << "Sampled move: " << move.ToString() << "\n\n";
      }
      state.ApplyMove(move);
      continue;
    }
    const auto legal_moves = state.LegalMoves();
    std::uniform_int_distribution<std::mt19937::result_type> dist(
        0, legal_moves.size() - 1);
    const auto &move = legal_moves[dist(*rng)];
    if (verbose) {
      std::cout << "Current player: " << state.CurrentPlayer() << "\n";
      std::cout << state.ToString() << "\n\n";
      std::cout << "Legal moves:";
      for (const auto &legal_move : legal_moves) {
        std::cout << " " << legal_move.ToString();
      }
      std::cout << "\n";
      std::cout << "Sampled move: " << move.ToString() << "\n\n";
    }
    state.ApplyMove(move);
  }
  if (verbose) {
    std::cout << "Game done, terminal state:\n" << state.ToString() << "\n\n";
  }
  return state.Returns();
}

std::unordered_map<std::string, std::string> ParseArguments(int argc,
                                                            char **argv) {
  std::unordered_map<std::string, std::string> game_params;
  const auto prefix_len = strlen(kGameParamArgPrefix);
  for (int i = 1; i < argc; ++i) {
    std::string param = argv[i];
    if (param.compare(0, prefix_len, kGameParamArgPrefix) == 0 &&
        param.size() > prefix_len) {
      std::string value;
      param = param.substr(prefix_len, std::string::npos);
      auto value_pos = param.find('=');
      if (value_pos != std::string::npos) {
        value = param.substr(value_pos + 1, std::string::npos);
        param = param.substr(0, value_pos);
      }
      game_params[param] = value;
    }
  }
  return game_params;
}

int main(int argc, char **argv) {
  auto game_params = ParseArguments(argc, argv);
  auto game = dle::DoudizhuGame(game_params);
  std::mt19937 rng;
  rng.seed(std::random_device()());
  SimulateGame(game, true, &rng);
}
