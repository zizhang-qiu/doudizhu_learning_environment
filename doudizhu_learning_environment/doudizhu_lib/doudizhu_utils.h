//
// Created by qzz on 2024/5/30.
//

#ifndef DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_DOUDIZHU_UTILS_H_
#define DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_DOUDIZHU_UTILS_H_
#include "utils.h"
namespace doudizhu_learning_env {

inline constexpr int kInvalidPlayer = -1;
inline constexpr int kLeftOverPlayer = -2;
inline constexpr int kChancePlayerId = -3;
inline constexpr int kTerminalPlayerId = -4;

inline constexpr int kNumPlayers = 3;
inline constexpr int kNumCards = 54;

inline constexpr int kNumBids = 3;
inline constexpr int kNumCardsPerSuit = 13;

inline constexpr int kPairLength = 2;
inline constexpr int kTrioLength = 3;
inline constexpr int kQuadLength = 4;

// player 0, 1 passes, 2 bids 1, then 0 passes, 1 bids 2, 2 passes, 0 bids 3, 1
// & 2 passes
inline constexpr int kMaxAuctionLength = 9;

// the maximum/minimum utility is achieved if the players play all 13 bombs
// alternatively and dizhu bid maximum bids
inline constexpr int kMaxUtility = kNumBids * 16384;
inline constexpr int kMinUtility = -kNumBids * 8192;

// 13 normal cards + 2 jokers
inline constexpr int kNumRanks = kNumCardsPerSuit + 2;

inline constexpr int kNumCardsLeftOver = 3;
inline constexpr int kChainAndPlaneMaxRank = 11; // Ace.

constexpr int NumberOfChainOrPlane(int length) {
  return kChainAndPlaneMaxRank - length + 1 - 0 + 1;
}

inline constexpr int kNumSuits = 4;
inline constexpr int kNumNormalCards = kNumCardsPerSuit * kNumSuits;

inline constexpr int kNumFarmerCards = (kNumCards - 3) / 3;
inline constexpr int kNumDizhuCards = kNumFarmerCards + kNumCardsLeftOver;

inline constexpr int kChainOfSoloMinLength = 5; // 34567
inline constexpr int kChainOfSoloMaxLength = 12; // 3456789TJQKA
inline constexpr int kChainOfPairMinLength = 3; // 334455
inline constexpr int kChainOfPairMaxLength = 10; // 33445566778899TTJJQQ
inline constexpr int kChainOfTrioMinLength = 2; // 333444
inline constexpr int kChainOfTrioMaxLength = 6; // 333444555666777888

inline constexpr int kPlaneWithSoloMinLength = 2; // 333444xx
inline constexpr int kPlaneWithSoloMaxLength = 5;
inline constexpr int kPlaneWithPairMinLength = 2; // 333444xxyy
inline constexpr int kPlaneWithPairMaxLength = 4; // 333444xxyy

inline constexpr int kNumSolos = kNumRanks;
inline constexpr int kNumPairs = kNumCardsPerSuit;
inline constexpr int kNumTrios = kNumCardsPerSuit;
inline constexpr int kNumTrioWithSolos = kNumCardsPerSuit * (kNumRanks - 1);
inline constexpr int kNumTrioWithPairs = kNumCardsPerSuit * (kNumCardsPerSuit - 1);

constexpr int GetNumChainOfSolos() {
  int num = 0;
  for (int l = kChainOfSoloMinLength; l <= kChainOfSoloMaxLength; ++l) {
    num += NumberOfChainOrPlane(l);
  }
  return num;
}

constexpr int GetNumChainOfPairs() {
  int num = 0;
  for (int l = kChainOfPairMinLength; l <= kChainOfPairMaxLength; ++l) {
    num += NumberOfChainOrPlane(l);
  }
  return num;
}

constexpr int GetNumChainOfTrios() {
  int num = 0;
  for (int l = kChainOfTrioMinLength; l <= kChainOfTrioMaxLength; ++l) {
    num += NumberOfChainOrPlane(l);
  }
  return num;
}

inline constexpr int kNumChainOfSolos = GetNumChainOfSolos();
inline constexpr int kNumChainOfPairs = GetNumChainOfPairs(); // 33~qq is the maximum length.
inline constexpr int kNumChainOfTrios = GetNumChainOfTrios(); // 333~888 is the maximum length.
inline constexpr int kNumLength2PlaneWithSolos = 11 * (kNumCardsPerSuit - 2 // 2 same cards.
    + Combination(kNumRanks - 2, 2) - 1); // 2 different cards except Rocket.
inline constexpr int kNumLength3PlaneWithSolos = 10 * (kNumCardsPerSuit - 3 // 3 same cards.
    + ((kNumCardsPerSuit - 3) * Combination(kNumRanks - 4, 1)) // 2 same cards and a different card.
    + Combination(kNumRanks - 3, 3) - (kNumCardsPerSuit - 3)) // 3 different cards.
    - (1 * 2 + 2
        * 8); // Remove combinations like 333444555666 for consistency since they're not treated as plane with solos in RLCard.

inline constexpr int kNumLength4PlaneWithSolos = 9 * (
    ((kNumCardsPerSuit - 4) * Combination(kNumRanks - 5, 1)) // 3 same cards and a different card.
        + ((kNumCardsPerSuit - 4) * (Combination(kNumRanks - 5, 2) - 1)) // 2 same cards and 2 different cards.
        + Combination(kNumCardsPerSuit - 4, 2) // 2 pairs
        + (Combination(kNumRanks - 4, 4) - Combination(kNumCardsPerSuit - 4, 2))) // 4 different cards
    - (2 * 1 * (kNumRanks - 5) + 7 * 2 * (kNumRanks
        - 5)); // Remove combinations like 333444555666777x for consistency since they're not treated as plane with solos in RLCard.

inline constexpr int kNumLength5PlaneWithSolos = 8 * (
    ((kNumCardsPerSuit - 5) * (kNumCardsPerSuit - 6)) // 3 same cards and a pair.
        + ((kNumCardsPerSuit - 5) * (Combination(kNumRanks - 6, 2) - 1)) // 3 same cards and 2 different cards.
        + (Combination(kNumCardsPerSuit - 5, 2) * (kNumRanks - 7)) // 2 pairs and a different card.
        + ((kNumCardsPerSuit - 5)
            * (Combination(kNumRanks - 6, 3) - (kNumCardsPerSuit - 6))) // A pair and 3 different cards.
        + (Combination(kNumRanks - 5, 5) - Combination(kNumCardsPerSuit - 5, 3)))   // 5 different cards
    - (2 * 1 * (kNumCardsPerSuit - 6 + Combination(kNumRanks - 6, 2) - 1)
        + 6 * 2 * (kNumCardsPerSuit - 6 + Combination(kNumRanks - 6, 2)
            - 1)); // Remove combinations like 333444555666777888xx for consistency since they're not treated as plane with solos in RLCard.

inline constexpr int kNumPlaneWithSolos =
    kNumLength2PlaneWithSolos + kNumLength3PlaneWithSolos + kNumLength4PlaneWithSolos + kNumLength5PlaneWithSolos;

inline constexpr int kNumLength2PlaneWithPairs = 11 * Combination(kNumCardsPerSuit - 2, 2);
inline constexpr int kNumLength3PlaneWithPairs = 10 * Combination(kNumCardsPerSuit - 3, 3);
inline constexpr int kNumLength4PlaneWithPairs = 9 * Combination(kNumCardsPerSuit - 4, 4);
inline constexpr int
    kNumPlaneWithPairs = kNumLength2PlaneWithPairs + kNumLength3PlaneWithPairs + kNumLength4PlaneWithPairs;

inline constexpr int kNumQuadWithPairs = kNumCardsPerSuit * Combination(kNumCardsPerSuit - 1, 2);
inline constexpr int kNumQuadWithSolos = kNumCardsPerSuit * (
    kNumCardsPerSuit - 1 // A pair
        + Combination(kNumRanks - 1, 2) - 1 // 2 different cards.
);

inline constexpr int kNumBombs = kNumCardsPerSuit;
inline constexpr int kNumRockets = 1; // BR.
inline constexpr int kNumPasses = 1;

inline constexpr int kNumDistinctPlayMoves = kNumSolos
    + kNumPairs
    + kNumTrios
    + kNumTrioWithSolos
    + kNumTrioWithPairs
    + kNumChainOfSolos
    + kNumChainOfPairs
    + kNumChainOfTrios
    + kNumPlaneWithSolos
    + kNumPlaneWithPairs
    + kNumQuadWithSolos
    + kNumQuadWithPairs
    + kNumBombs
    + kNumRockets
    + kNumPasses;

inline constexpr int kNumDistinctMoves =
    kNumCards + kNumBids + 1 + kNumDistinctPlayMoves;

constexpr char kSuitChar[] = "CDHS";
constexpr char kRankChar[] = "3456789TJQKA2BR";

}
#endif //DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_DOUDIZHU_UTILS_H_
