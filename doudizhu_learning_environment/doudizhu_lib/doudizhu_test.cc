//
// Created by qzz on 2024/5/31.
//
#include <iostream>
#include <fstream>
#include <filesystem>
#include "doudizhu_game.h"
#include "doudizhu_state.h"

using namespace doudizhu_learning_env;
void CardTest() {
  std::vector<DoudizhuCard> cards;

  // 52 normal cards.
  for (int rank = 0; rank < kNumCardsPerSuit; ++rank) {
    for (const Suit suit : kAllSuits) {
      const DoudizhuCard card{rank, suit};
      CHECK_TRUE(card.IsValid());
      cards.push_back(card);
    }
  }

  // 2Jokers
  const DoudizhuCard black_joker{kBlackJoker, kInvalidSuit};
  const DoudizhuCard red_joker{kRedJoker, kInvalidSuit};
  CHECK_TRUE(black_joker.IsValid());
  CHECK_TRUE(black_joker.IsJoker());
  CHECK_TRUE(red_joker.IsValid());
  CHECK_TRUE(red_joker.IsJoker());

  cards.push_back(black_joker);
  cards.push_back(red_joker);

  const std::array<std::string, kNumCards> all_cards_str = {
      "C3", "D3", "H3", "S3", "C4", "D4", "H4", "S4",
      "C5", "D5", "H5", "S5", "C6", "D6", "H6", "S6",
      "C7", "D7", "H7", "S7", "C8", "D8", "H8", "S8",
      "C9", "D9", "H9", "S9", "CT", "DT", "HT", "ST",
      "CJ", "DJ", "HJ", "SJ", "CQ", "DQ", "HQ", "SQ",
      "CK", "DK", "HK", "SK", "CA", "DA", "HA", "SA",
      "C2", "D2", "H2", "S2", "BJ", "RJ"
  };

  for (int i = 0; i < kNumCards; ++i) {
    CHECK_EQ(cards[i].ToString(), all_cards_str[i]);
  }

  std::cout << "Passed card test." << std::endl;
}

void MoveTest() {
  // Invalid.
  {
    const DoudizhuMove move{};
    CHECK_EQ(move.ToString(), "I");
  }

  // Deal
  for (const Suit suit : kAllSuits) {
    for (int rank = 0; rank < kNumCardsPerSuit; ++rank) {
      const DoudizhuMove move(DoudizhuCard{rank, suit});
      std::string expected = "(Deal ";
      expected.push_back(kSuitChar[suit]);
      expected.push_back(kRankChar[rank]);
      expected.push_back(')');
//      std::cout << move.ToString() << std::endl;
      CHECK_EQ(move.ToString(), expected);
    }
  }

  {
    DoudizhuMove deal_red_joker(DoudizhuCard{kRedJoker, kInvalidSuit});
    CHECK_EQ(deal_red_joker.ToString(), "(Deal RJ)");
    DoudizhuMove deal_black_joker(DoudizhuCard{kBlackJoker, kInvalidSuit});
    CHECK_EQ(deal_black_joker.ToString(), "(Deal BJ)");
  }

  // Auction
  const DoudizhuMove auction_pass{DoudizhuMove::AuctionType::kPass};
  CHECK_EQ(auction_pass.ToString(), "(Pass)");
  for (const auto auction_type : {DoudizhuMove::AuctionType::kOne,
                                  DoudizhuMove::AuctionType::kTwo,
                                  DoudizhuMove::AuctionType::kThree}) {
    const DoudizhuMove move{auction_type};
//    std::cout << move.ToString() <<std::endl;
    CHECK_EQ(move.ToString(), "(Bid " + std::to_string(static_cast<int>(auction_type)) + ")");
  }

  // Play
  // Solo/Pair/Trio/Bomb.
  for (int rank = 0; rank < kNumCardsPerSuit; ++rank) {
    const DoudizhuMove solo_move{
        /*play_type=*/DoudizhuMove::PlayType::kSolo,
        /*single_rank=*/{rank, 1},
        /*chain=*/{},
        /*trio_comb=*/{},
        /*quad_comb=*/{},
        /*plane=*/{},
        /*kickers=*/{}
    };

    std::string expected_solo_str = "(Play ";
    expected_solo_str.push_back(kRankChar[rank]);
    expected_solo_str.push_back(')');
//    std::cout << solo_move.ToString() << std::endl;
    CHECK_EQ(solo_move.ToString(), expected_solo_str);

    const DoudizhuMove pair_move{
        /*play_type=*/DoudizhuMove::PlayType::kPair,
        /*single_rank=*/{rank, 2},
        /*chain=*/{},
        /*trio_comb=*/{},
        /*quad_comb=*/{},
        /*plane=*/{},
        /*kickers=*/{}
    };
    std::string expected_pair_str = "(Play ";
    expected_pair_str.push_back(kRankChar[rank]);
    expected_pair_str.push_back(kRankChar[rank]);
    expected_pair_str.push_back(')');
    CHECK_EQ(pair_move.ToString(), expected_pair_str);

    const DoudizhuMove trio_move{
        /*play_type=*/DoudizhuMove::PlayType::kTrio,
        /*single_rank=*/{rank, 3},
        /*chain=*/{},
        /*trio_comb=*/{},
        /*quad_comb=*/{},
        /*plane=*/{},
        /*kickers=*/{}
    };
    std::string expected_trio_str = "(Play ";
    expected_trio_str.push_back(kRankChar[rank]);
    expected_trio_str.push_back(kRankChar[rank]);
    expected_trio_str.push_back(kRankChar[rank]);
    expected_trio_str.push_back(')');
    CHECK_EQ(trio_move.ToString(), expected_trio_str);

    const DoudizhuMove bomb_move{
        /*play_type=*/DoudizhuMove::PlayType::kBomb,
        /*single_rank=*/{rank, 4},
        /*chain=*/{},
        /*trio_comb=*/{},
        /*quad_comb=*/{},
        /*plane=*/{},
        /*kickers=*/{}
    };
    std::string expected_bomb_str = "(Play ";
    expected_bomb_str.push_back(kRankChar[rank]);
    expected_bomb_str.push_back(kRankChar[rank]);
    expected_bomb_str.push_back(kRankChar[rank]);
    expected_bomb_str.push_back(kRankChar[rank]);
    expected_bomb_str.push_back(')');
    CHECK_EQ(bomb_move.ToString(), expected_bomb_str);
  }

  // Trio with solo.
  for (int rank = 0; rank < kNumCardsPerSuit; ++rank) {
    for (int kicker = 0; kicker < kNumRanks; ++kicker) {
      if (kicker == rank) {
        continue;
      }
      const DoudizhuMove move{
          /*play_type=*/DoudizhuMove::PlayType::kTrioWithSolo,
          /*single_rank=*/{},
          /*chain=*/{},
          /*trio_comb=*/{kSolo, rank},
          /*quad_comb=*/{},
          /*plane=*/{},
          /*kickers=*/{kicker}
      };
      std::string expected{"(Play "};
      for (int i = 0; i < kTrioLength; ++i) {
        expected.push_back(kRankChar[rank]);
      }
      expected.push_back(kRankChar[kicker]);
      expected.push_back(')');
//      std::cout << move.ToString() <<std::endl;
      CHECK_EQ(move.ToString(), expected);
    }
  }

  // Trio with pair.
  for (int rank = 0; rank < kNumCardsPerSuit; ++rank) {
    for (int kicker = 0; kicker < kNumCardsPerSuit; ++kicker) {
      if (kicker == rank) {
        continue;
      }
      const DoudizhuMove move{
          /*play_type=*/DoudizhuMove::PlayType::kTrioWithPair,
          /*single_rank=*/{},
          /*chain=*/{},
          /*trio_comb=*/{kPair, rank},
          /*quad_comb=*/{},
          /*plane=*/{},
          /*kickers=*/{kicker, kicker}
      };
      std::string expected{"(Play "};
      for (int i = 0; i < kTrioLength; ++i) {
        expected.push_back(kRankChar[rank]);
      }
      expected.push_back(kRankChar[kicker]);
      expected.push_back(kRankChar[kicker]);
      expected.push_back(')');
//      std::cout << move.ToString() <<std::endl;
      CHECK_EQ(move.ToString(), expected);
    }
  }

  // Chain of solo.
  for (int length = kChainOfSoloMinLength; length <= kChainOfSoloMaxLength; ++length) {
    for (int start_rank = 0; start_rank <= 11 - length + 1; ++start_rank) {
      const DoudizhuMove move{
          /*play_type=*/DoudizhuMove::PlayType::kChainOfSolo,
          /*single_rank=*/{},
          /*chain=*/{doudizhu_learning_env::ChainType::kSolo, length, start_rank},
          /*trio_comb=*/{},
          /*quad_comb=*/{},
          /*plane=*/{},
          /*kickers=*/{}
      };
      std::string expected{"(Play "};
      for (int i = 0; i < length; ++i) {
        expected.push_back(kRankChar[start_rank + i]);
      }
      expected.push_back(')');
//      std::cout << move.ToString() << std::endl;
      CHECK_EQ(move.ToString(), expected);
    }
  }

  // Chain of pair.
  for (int length = kChainOfPairMinLength; length <= kChainOfPairMaxLength; ++length) {
    for (int start_rank = 0; start_rank <= 11 - length + 1; ++start_rank) {
      const DoudizhuMove move{
          /*play_type=*/DoudizhuMove::PlayType::kChainOfPair,
          /*single_rank=*/{},
          /*chain=*/{doudizhu_learning_env::ChainType::kPair, length, start_rank},
          /*trio_comb=*/{},
          /*quad_comb=*/{},
          /*plane=*/{},
          /*kickers=*/{}
      };
      std::string expected{"(Play "};
      for (int i = 0; i < length; ++i) {
        expected.push_back(kRankChar[start_rank + i]);
        expected.push_back(kRankChar[start_rank + i]);
      }
      expected.push_back(')');
//      std::cout << move.ToString() << std::endl;
      CHECK_EQ(move.ToString(), expected);
    }
  }

  // Chain of trio.
  for (int length = kChainOfTrioMinLength; length <= kChainOfTrioMaxLength; ++length) {
    for (int start_rank = 0; start_rank <= 11 - length + 1; ++start_rank) {
      const DoudizhuMove move{
          /*play_type=*/DoudizhuMove::PlayType::kChainOfTrio,
          /*single_rank=*/{},
          /*chain=*/{doudizhu_learning_env::ChainType::kTrio, length, start_rank},
          /*trio_comb=*/{},
          /*quad_comb=*/{},
          /*plane=*/{},
          /*kickers=*/{}
      };
      std::string expected{"(Play "};
      for (int i = 0; i < length; ++i) {
        expected.push_back(kRankChar[start_rank + i]);
        expected.push_back(kRankChar[start_rank + i]);
        expected.push_back(kRankChar[start_rank + i]);
      }
      expected.push_back(')');
//      std::cout << move.ToString() << std::endl;
      CHECK_EQ(move.ToString(), expected);
    }
  }

  // Plane with solo.
  int num_plane_with_solo = 0;
  for (int length = kPlaneWithSoloMinLength; length <= kPlaneWithSoloMaxLength; ++length) {
    for (int start_rank = 0; start_rank <= 11 - length + 1; ++start_rank) {
      // Get possible kickers.
      Plane plane{kSolo, length, start_rank};
      const auto possible_kickers = GetPossibleKickers(plane);
      for (const auto &kickers : possible_kickers) {
        const DoudizhuMove move{
            /*play_type=*/DoudizhuMove::PlayType::kPlaneWithSolo,
            /*single_rank=*/{},
            /*chain=*/{},
            /*trio_comb=*/{},
            /*quad_comb=*/{},
            /*plane=*/plane,
            /*kickers=*/kickers
        };
        ++num_plane_with_solo;

        std::string expected{"(Play "};
        for (int r = start_rank; r < start_rank + length; ++r) {
          for (int i = 0; i < kTrioLength; ++i) {
            expected.push_back(kRankChar[r]);
          }
        }
        for (const int k : kickers) {
          expected.push_back(kRankChar[k]);
        }
        expected.push_back(')');
//        std::cout << move.ToString() << std::endl;
        CHECK_EQ(expected, move.ToString());
      }
    }
  }
  CHECK_EQ(num_plane_with_solo, kNumPlaneWithSolos);
//  std::cout << "num plane with solo: " << num_plane_with_solo << std::endl;

  // Plane with solo.
  int num_plane_with_pair = 0;
  for (int length = kPlaneWithPairMinLength; length <= kPlaneWithPairMaxLength; ++length) {
    for (int start_rank = 0; start_rank <= 11 - length + 1; ++start_rank) {
      // Get possible kickers.
      Plane plane{kPair, length, start_rank};
      const auto possible_kickers = GetPossibleKickers(plane);
      for (const auto &kickers : possible_kickers) {
        const DoudizhuMove move{
            /*play_type=*/DoudizhuMove::PlayType::kPlaneWithPair,
            /*single_rank=*/{},
            /*chain=*/{},
            /*trio_comb=*/{},
            /*quad_comb=*/{},
            /*plane=*/plane,
            /*kickers=*/kickers
        };
        ++num_plane_with_pair;

        std::string expected{"(Play "};
        for (int r = start_rank; r < start_rank + length; ++r) {
          for (int i = 0; i < kTrioLength; ++i) {
            expected.push_back(kRankChar[r]);
          }
        }
        for (const int k : kickers) {
          expected.push_back(kRankChar[k]);
        }
        expected.push_back(')');
//        std::cout << move.ToString() << std::endl;
        CHECK_EQ(expected, move.ToString());
      }
    }
  }
  CHECK_EQ(num_plane_with_pair, kNumPlaneWithPairs);
//  std::cout << "num plane with pair: " << num_plane_with_pair << std::endl;

  // Quad with solo.
  int num_quad_with_solo = 0;
  for (int rank = 0; rank < kNumCardsPerSuit; ++rank) {
    QuadComb quad_comb{kSolo, rank};
    const auto kickers = GetPossibleKickers(quad_comb);
    for (const auto &ks : kickers) {
      const DoudizhuMove move{
          /*play_type=*/DoudizhuMove::PlayType::kQuadWithSolo,
          /*single_rank=*/{},
          /*chain=*/{},
          /*trio_comb=*/{},
          /*quad_comb=*/quad_comb,
          /*plane=*/{},
          /*kickers=*/ks
      };
      ++num_quad_with_solo;
      std::string expected{"(Play "};
      for (int i = 0; i < kQuadLength; ++i) {
        expected.push_back(kRankChar[rank]);
      }
      for (const int k : ks) {
        expected.push_back(kRankChar[k]);
      }
      expected.push_back(')');
//      std::cout << move.ToString() << std::endl;
      CHECK_EQ(move.ToString(), expected);
    }

  }
  CHECK_EQ(num_quad_with_solo, kNumQuadWithSolos);

  // Quad with solo.
  int num_quad_with_pair = 0;
  for (int rank = 0; rank < kNumCardsPerSuit; ++rank) {
    QuadComb quad_comb{kPair, rank};
    const auto kickers = GetPossibleKickers(quad_comb);
    for (const auto &ks : kickers) {
      const DoudizhuMove move{
          /*play_type=*/DoudizhuMove::PlayType::kQuadWithPair,
          /*single_rank=*/{},
          /*chain=*/{},
          /*trio_comb=*/{},
          /*quad_comb=*/quad_comb,
          /*plane=*/{},
          /*kickers=*/ks
      };
      ++num_quad_with_pair;
      std::string expected{"(Play "};
      for (int i = 0; i < kQuadLength; ++i) {
        expected.push_back(kRankChar[rank]);
      }
      for (const int k : ks) {
        expected.push_back(kRankChar[k]);
      }
      expected.push_back(')');
//      std::cout << move.ToString() << std::endl;
      CHECK_EQ(move.ToString(), expected);
    }

  }
  CHECK_EQ(num_quad_with_pair, kNumQuadWithPairs);

  // Rocket
  {
    const DoudizhuMove move{
        /*play_type=*/DoudizhuMove::PlayType::kRocket,
        /*single_rank=*/{},
        /*chain=*/{},
        /*trio_comb=*/{},
        /*quad_comb=*/{},
        /*plane=*/{},
        /*kickers=*/{}
    };
    CHECK_EQ(move.ToString(), "(Play BR)");
  }

  // Pass
  {
    const DoudizhuMove move{
        /*play_type=*/DoudizhuMove::PlayType::kPass,
        /*single_rank=*/{},
        /*chain=*/{},
        /*trio_comb=*/{},
        /*quad_comb=*/{},
        /*plane=*/{},
        /*kickers=*/{}
    };
    CHECK_EQ(move.ToString(), "(Pass)");
  }

  std::cout << "Passed move test." << std::endl;
}

std::vector<std::string> StrSplit(const std::string &str, const std::string &delim) {
  std::vector<std::string> tokens;
  size_t start = 0;
  size_t end = 0;
  while ((end = str.find(delim, start)) != std::string::npos) {
    tokens.push_back(str.substr(start, end - start));
    start = end + delim.length();
  }
  tokens.push_back(str.substr(start));
  return tokens;
}

std::unordered_map<char, int> GetStringCounter(const std::string &str) {
  std::unordered_map<char, int> counter{};
  for (const char &c : str) {

    ++counter[c];
  }
  return counter;
}

bool MoveStringEqual(const std::string &lhs, const std::string &rhs) {
  return GetStringCounter(lhs) == GetStringCounter(rhs);
}

void GameTest() {
  DoudizhuGame game{{}};
  const auto &all_moves = game.AllMoves();
  CHECK_EQ(all_moves.size(), game.MaxMoves());

  // Load RL card actions.
  std::ifstream ifs("../../../doudizhu_learning_environment/doudizhu_lib/action_space.txt");
  if (ifs.is_open()) {
    std::string line;
    while (std::getline(ifs, line)) {
//      std::cout << line << std::endl;
    }
    const auto expected_move_strings = StrSplit(line, " ");
//    std::cout << expected_move_strings.size() << std::endl;
    CHECK_EQ(expected_move_strings.size(), kNumDistinctPlayMoves);
    for (int i = 0; i < kNumDistinctPlayMoves - 1; ++i) {
      std::string cur_move_str = all_moves[i + kNumBids + 1].ToString();
      cur_move_str = cur_move_str.substr(6, cur_move_str.length() - 6 - 1);
      if (!MoveStringEqual(cur_move_str, expected_move_strings[i])) {
        std::cout << cur_move_str << ", ";
        std::cout << expected_move_strings[i] << std::endl;
      }

//      CHECK_TRUE(MoveStringEqual(cur_move_str, expected_move_strings[i]));
    }
    ifs.close();
  } else {
    std::cerr << "Failed to open file" << std::endl;
  }

  // Test if GetChanceOutcome() is consistent with GetChanceOutcomeUid()
  const auto& all_chance_outcomes = game.AllChanceOutcomes();
  for (int i = 0; i < all_chance_outcomes.size(); ++i) {
    const int uid = game.GetChanceOutcomeUid(all_chance_outcomes[i]);
    CHECK_EQ(uid, i);
  }

  // Test if GetMove() is consistent with GetMoveUid()
  for (int i = 0; i < all_moves.size(); ++i) {
    const int uid = game.GetMoveUid(all_moves[i]);
    CHECK_EQ(uid, i);
  }

  std::cout << "Passed game test." << std::endl;
}

void RandomSimTest(bool verbose, int seed) {
  std::mt19937 rng;
  rng.seed(seed);
  GameParameters params = {
      {"seed", std::to_string(seed)}
  };
  std::shared_ptr<DoudizhuGame> game = std::make_shared<DoudizhuGame>(params);
  DoudizhuState state(game);
  while (!state.IsTerminal()) {
    // Chance node.
    if (state.CurrentPlayer() == kChancePlayerId) {
      auto chance_outcomes = state.ChanceOutcomes();
      std::discrete_distribution<std::mt19937::result_type> dist(
          chance_outcomes.second.begin(), chance_outcomes.second.end());
      auto move = chance_outcomes.first[dist(rng)];
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
    auto move = legal_moves[dist(rng)];
    if (verbose) {
      std::cout << "Current player: " << state.CurrentPlayer() << "\n";
      std::cout << state.ToString() << "\n\n";
      std::cout << "Legal moves:";
      for (int i = 0; i < legal_moves.size(); ++i) {
        std::cout << " " << legal_moves[i].ToString();
      }
      std::cout << "\n";
      std::cout << "Sampled move: " << move.ToString() << "\n\n";
    }
    state.ApplyMove(move);
  }
  if (verbose) {
    std::cout << "Game done, terminal state:\n" << state.ToString() << "\n\n";
  }

  std::cout << "Passed random sim test." << std::endl;
}

int main() {
  CardTest();
  MoveTest();
  GameTest();
  RandomSimTest(false, 42);
  return 0;
}