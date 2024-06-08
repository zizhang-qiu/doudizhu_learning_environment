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

        # pickle.
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

        # pickle
        with open("temp_hand", "wb") as fp:
            pickle.dump(hand, fp)

        with open("temp_hand", "rb") as fp:
            loaded_hand = pickle.load(fp)

        self.assertEqual(hand, hand)

        os.remove("temp_hand")


if __name__ == '__main__':
    unittest.main()
