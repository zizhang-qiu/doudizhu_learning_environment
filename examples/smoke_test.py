"""
@author: qzz
@contact:q873264077@gmail.com
@version: 1.0.0
@file: smoke_test.py
@time: 2024/6/6 22:33
"""
import os
import pickle
import sys
import unittest


def append_sys_path():
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    sys.path.append(os.path.join(root, "build", "doudizhu_learning_environment"))


append_sys_path()

try:
    import pydoudizhu
except Exception as e:
    print(e)
    sys.exit(1)


class SmokeTest(unittest.TestCase):

    def test_doudizhu_card(self):
        invalid_card = pydoudizhu.DoudizhuCard()
        # is_valid()
        self.assertFalse(invalid_card.is_valid())

        # __eq__()
        suit = pydoudizhu.Suit.CLUBS_SUIT
        rank = 0
        card = pydoudizhu.DoudizhuCard(rank, suit)
        self.assertEqual(card, pydoudizhu.DoudizhuCard(rank, suit))

        # __repr__()
        self.assertEqual(repr(card), "C3")

        # is_joker()
        self.assertFalse(card.is_joker())

        # card_rank(), card_suit()
        self.assertEqual(card.card_rank(), 0)
        self.assertEqual(card.card_suit(), pydoudizhu.Suit.CLUBS_SUIT)

        # Pickle.
        with open("temp_card", "wb") as fp:
            pickle.dump(card, fp)

        with open("temp_card", "rb") as fp:
            loaded_card = pickle.load(fp)

        self.assertEqual(card, loaded_card)

        os.remove("temp_card")

    def test_doudizhu_hand(self):
        hand = pydoudizhu.DoudizhuHand()

        # add_card()
        hand.add_card(0)
        hand.add_card(pydoudizhu.BLACK_JOKER)
        hand.add_card(pydoudizhu.DoudizhuCard(1, pydoudizhu.Suit.CLUBS_SUIT))

        # cards_per_rank()
        self.assertEqual(hand.cards_per_rank(), [1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0])

        # size()
        self.assertEqual(hand.size(), 3)

        # can_add()
        self.assertTrue(hand.can_add(rank=2))
        self.assertFalse(hand.can_add(pydoudizhu.BLACK_JOKER))

        # can_remove()
        self.assertTrue(hand.can_remove(rank=0))
        self.assertFalse(hand.can_remove(rank=3))

        # remove_from_hand()
        hand.remove_from_hand(0)
        self.assertEqual(hand.size(), 2)

        # Pickle
        with open("temp_hand", "wb") as fp:
            pickle.dump(hand, fp)

        with open("temp_hand", "rb") as fp:
            loaded_hand = pickle.load(fp)

        self.assertEqual(hand, loaded_hand)

        os.remove("temp_hand")

    def test_doudizhu_move(self):
        invalid_move = pydoudizhu.DoudizhuMove()
        self.assertFalse(invalid_move.is_valid())

        # Solo
        solo_move = pydoudizhu.DoudizhuMove(
            play_type=pydoudizhu.PlayType.SOLO,
            single_rank=pydoudizhu.SingleRank(r=0, n=1),
            chain=pydoudizhu.Chain(),
            trio_comb=pydoudizhu.TrioComb(),
            quad_comb=pydoudizhu.QuadComb(),
            plane=pydoudizhu.Plane(),
            kickers=[]
        )
        self.assertEqual(repr(solo_move), "(Play 3)")

        # Pair
        pair_move = pydoudizhu.DoudizhuMove(pydoudizhu.SingleRank(r=1, n=2))
        self.assertEqual(repr(pair_move), "(Play 44)")
        self.assertEqual(pair_move.get_play_type(), pydoudizhu.PlayType.PAIR)

        # Trio
        trio_move = pydoudizhu.DoudizhuMove(pydoudizhu.SingleRank(r=2, n=3))
        self.assertEqual(repr(trio_move), "(Play 555)")
        self.assertEqual(trio_move.get_play_type(), pydoudizhu.PlayType.TRIO)

        # Bomb
        bomb_move = pydoudizhu.DoudizhuMove(pydoudizhu.SingleRank(r=3, n=4))
        self.assertEqual(repr(bomb_move), "(Play 6666)")
        self.assertTrue(bomb_move.is_bomb())
        self.assertEqual(bomb_move.bomb_rank(), 3)
        self.assertEqual(bomb_move.get_play_type(), pydoudizhu.PlayType.BOMB)

        # Chain of solo
        chain_of_solo_move = pydoudizhu.DoudizhuMove(
            pydoudizhu.Chain(chain_type=pydoudizhu.ChainType.SOLO, length=5, start_rank=0))
        self.assertEqual(repr(chain_of_solo_move), "(Play 34567)")
        self.assertEqual(chain_of_solo_move.get_play_type(), pydoudizhu.PlayType.CHAIN_OF_SOLO)

        # Chain of pair
        chain_of_pair_move = pydoudizhu.DoudizhuMove(
            pydoudizhu.Chain(chain_type=pydoudizhu.ChainType.PAIR, length=3, start_rank=0))
        self.assertEqual(repr(chain_of_pair_move), "(Play 334455)")
        self.assertEqual(chain_of_pair_move.get_play_type(), pydoudizhu.PlayType.CHAIN_OF_PAIR)

        # Chain of trio
        chain_of_trio_move = pydoudizhu.DoudizhuMove(
            pydoudizhu.Chain(chain_type=pydoudizhu.ChainType.TRIO, length=3, start_rank=1))
        self.assertEqual(repr(chain_of_trio_move), "(Play 444555666)")
        self.assertEqual(chain_of_trio_move.get_play_type(), pydoudizhu.PlayType.CHAIN_OF_TRIO)

        # Trio with solo
        trio_with_solo_move = pydoudizhu.DoudizhuMove(
            pydoudizhu.TrioComb(kt=pydoudizhu.KickerType.SOLO, tr=4),
            [0]
        )
        self.assertEqual(repr(trio_with_solo_move), "(Play 7773)")
        self.assertEqual(trio_with_solo_move.get_play_type(), pydoudizhu.PlayType.TRIO_WITH_SOLO)

        # Trio with pair
        trio_with_pair_move = pydoudizhu.DoudizhuMove(
            pydoudizhu.TrioComb(kt=pydoudizhu.KickerType.PAIR, tr=5),
            [1, 1]
        )
        self.assertEqual(repr(trio_with_pair_move), "(Play 88844)")
        self.assertEqual(trio_with_pair_move.get_play_type(), pydoudizhu.PlayType.TRIO_WITH_PAIR)

        # Quad with solo
        quad_with_solo_move = pydoudizhu.DoudizhuMove(
            pydoudizhu.QuadComb(kt=pydoudizhu.KickerType.SOLO, qr=2),
            [0, 1]
        )
        self.assertEqual(repr(quad_with_solo_move), "(Play 555534)")
        self.assertEqual(quad_with_solo_move.get_play_type(), pydoudizhu.PlayType.QUAD_WITH_SOLO)

        # Quad with pair
        quad_with_pair_move = pydoudizhu.DoudizhuMove(
            pydoudizhu.QuadComb(kt=pydoudizhu.KickerType.PAIR, qr=11),
            [9, 9, 10, 10]
        )
        self.assertEqual(repr(quad_with_pair_move), "(Play AAAAQQKK)")
        self.assertEqual(quad_with_pair_move.get_play_type(), pydoudizhu.PlayType.QUAD_WITH_PAIR)

        # Plane with solo
        plane_with_solo_move = pydoudizhu.DoudizhuMove(
            pydoudizhu.Plane(kt=pydoudizhu.KickerType.SOLO, l=2, sr=0),
            kickers=[2, 3]
        )
        self.assertEqual(repr(plane_with_solo_move), "(Play 33344456)")
        self.assertEqual(plane_with_solo_move.get_play_type(), pydoudizhu.PlayType.PLANE_WITH_SOLO)

        # Plane with solo
        plane_with_pair_move = pydoudizhu.DoudizhuMove(
            pydoudizhu.Plane(kt=pydoudizhu.KickerType.PAIR, l=3, sr=0),
            kickers=[4, 4, 5, 5, 6, 6]
        )
        self.assertEqual(repr(plane_with_pair_move), "(Play 333444555778899)")
        self.assertEqual(plane_with_pair_move.get_play_type(), pydoudizhu.PlayType.PLANE_WITH_PAIR)

        # Rocket
        rocket_move = pydoudizhu.DoudizhuMove(
            pydoudizhu.PlayType.ROCKET
        )
        self.assertTrue(rocket_move.is_bomb())
        self.assertEqual(repr(rocket_move), "(Play BR)")
        self.assertEqual(rocket_move.bomb_rank(), 13)

        # Pickle
        for move in [
            solo_move,
            pair_move,
            trio_move,
            bomb_move,
            chain_of_solo_move,
            chain_of_pair_move,
            chain_of_trio_move,
            trio_with_solo_move,
            trio_with_pair_move,
            quad_with_solo_move,
            quad_with_pair_move,
            plane_with_solo_move,
            plane_with_pair_move,
            rocket_move]:
            with open("temp_move", "wb") as fp:
                pickle.dump(move, fp)

            with open("temp_move", "rb") as fp:
                loaded_move = pickle.load(fp)

            self.assertEqual(move, loaded_move)

            os.remove("temp_move")

    def test_doudizhu_deck(self):
        deck = pydoudizhu.DoudizhuDeck()
        self.assertEqual(deck.size(), pydoudizhu.NUM_CARDS)

        card = deck.deal_card(rank=0, suit=pydoudizhu.Suit.CLUBS_SUIT)
        self.assertEqual(deck.size(), pydoudizhu.NUM_CARDS - 1)
        self.assertEqual(card, pydoudizhu.DoudizhuCard(0, pydoudizhu.Suit.CLUBS_SUIT))

        self.assertTrue(deck.card_in_deck(rank=0, suit=pydoudizhu.Suit.DIAMONDS_SUIT))
        self.assertFalse(deck.card_in_deck(rank=0, suit=pydoudizhu.Suit.CLUBS_SUIT))

        with open("temp_deck", "wb") as fp:
            pickle.dump(deck, fp)

        with open("temp_deck", "rb") as fp:
            loaded_deck = pickle.load(fp)

        self.assertEqual(deck, loaded_deck)

        os.remove("temp_deck")

    def test_doudizhu_game(self):
        params = {}
        game = pydoudizhu.DoudizhuGame(params)
        self.assertEqual(len(game.all_moves()), 27472 + 4)
        self.assertEqual(len(game.all_chance_outcomes()), pydoudizhu.NUM_CARDS)
        self.assertEqual(game.parameters(), params)
        with open("temp_game", "wb") as fp:
            pickle.dump(game, fp)

        with open("temp_game", "rb") as fp:
            loaded_game = pickle.load(fp)

        self.assertEqual(game, loaded_game)

        os.remove("temp_game")





if __name__ == '__main__':
    unittest.main()
