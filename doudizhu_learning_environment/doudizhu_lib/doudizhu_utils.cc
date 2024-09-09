//
// Created by qzz on 2024/5/30.
//

#include "doudizhu_utils.h"

namespace doudizhu_learning_env {
int RankCharToRank(const char &rank_char) {
  switch (rank_char) {
    case '3':
      return 0;
    case '4':
      return 1;
    case '5':
      return 2;
    case '6':
      return 3;
    case '7':
      return 4;
    case '8':
      return 5;
    case '9':
      return 6;
    case 'T':
    case 't':
      return 7;
    case 'J':
    case 'j':
      return 8;
    case 'Q':
    case 'q':
      return 9;
    case 'K':
    case 'k':
      return 10;
    case 'A':
    case 'a':
      return 11;
    case '2':
      return 12;
    case 'B':
    case 'b':
      return kBlackJoker;
    case 'R':
    case 'r':
      return kRedJoker;
    default: {
      return -1;
    }
  }
}
Suit SuitCharToSuit(const char &suit_char) {
  switch (std::toupper(suit_char)) {
    case 'C':
      return Suit::kClubsSuit;
    case 'D':
      return Suit::kDiamondsSuit;
    case 'H':
      return Suit::kHeartsSuit;
    case 'S':
      return Suit::kSpadesSuit;
    default: {
      return Suit::kInvalidSuit;
    }
  }
}
} // namespace doudizhu_learning_env
