//
// Created by qzz on 2024/5/30.
//

#include "doudizhu_utils.h"
namespace doudizhu_learning_env {

int RankCharToRank(const char &rank_char) {
  switch (rank_char) {
    case '3':return 0;
    case '4':return 1;
    case '5':return 2;
    case '6':return 3;
    case '7':return 4;
    case '8':return 5;
    case '9':return 6;
    case 'T':return 7;
    case 'J':return 8;
    case 'Q':return 9;
    case 'K':return 10;
    case 'A':return 11;
    case '2':return 12;
    case 'B':return kBlackJoker;
    case 'R':return kRedJoker;
    default: {
      std::string msg = "Can't convert rank char ";
      msg += rank_char;
      msg += " to rank.";
      FatalError(msg);
    }
  }
}
Suit SuitCharToSuit(const char &suit_char) {
  switch (std::toupper(suit_char)) {
    case 'C':
      return kClubsSuit;
    case 'D':
      return kDiamondsSuit;
    case 'H':
      return kHeartsSuit;
    case 'S':
      return kSpadesSuit;
    default:{
      std::string msg = "Can't convert suit char ";
      msg += suit_char;
      msg += " to suit.";
      FatalError(msg);
    }
  }
}
}

