"""
@author: qzz
@contact:q873264077@gmail.com
@version: 1.0.0
@file: temp.py
@time: 2024/6/8 22:14
"""
import os
import pickle
import random
import sys
from typing import List, Dict

import numpy as np
from tqdm import trange


def append_sys_path():
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    sys.path.append(os.path.join(root, "build", "doudizhu_learning_environment"))


append_sys_path()

try:
    import pydoudizhu
except Exception as e:
    print(e)
    sys.exit(1)


def get_array_feature(features: Dict, key: str) -> np.ndarray:
    encoding = features[key].encoding
    dims = features[key].dims
    f = np.array(encoding)
    f.resize(dims)
    return f


def get_array_feature_batched(features: Dict, key: str, num_repeat: int) -> np.ndarray:
    encoding = features[key].encoding
    dims = features[key].dims
    f = np.array(encoding)
    f.resize(dims)
    f_batch = np.repeat(f[np.newaxis, :], num_repeat, axis=0)
    return f_batch


def rank_to_douzero_representation(rank: int):
    if rank == pydoudizhu.BLACK_JOKER:
        return 20
    if rank == pydoudizhu.RED_JOKER:
        return 30
    if rank == pydoudizhu.BLACK_JOKER - 1:
        return 17
    return rank + 3


def move_to_douzero_representation(ranks: List[int]):
    res = []
    for rank in ranks:
        res.append(rank_to_douzero_representation(rank))
    res.sort()
    return res


def cards_per_ranks_to_ranks(cards_per_ranks: List[int]) -> List[int]:
    ranks = []
    for rank in range(pydoudizhu.NUM_RANKS):
        for i in range(cards_per_ranks[rank]):
            ranks.append(rank)
    ranks.sort()
    return ranks


def get_cards_play_data(state):
    assert state.current_phase() == pydoudizhu.Phase.PLAY
    dizhu = state.dizhu()
    assert dizhu >= 0
    hands = state.hands()
    dizhu_hand = hands[dizhu]
    dizhu_cards = cards_per_ranks_to_ranks(dizhu_hand.cards_per_rank())
    dizhu_cards = [rank_to_douzero_representation(rank) for rank in dizhu_cards]
    landlord_up = (dizhu - 1 + 3) % 3
    landlord_up_cards = cards_per_ranks_to_ranks(hands[landlord_up].cards_per_rank())
    landlord_up_cards = [rank_to_douzero_representation(rank) for rank in landlord_up_cards]
    landlord_down = (dizhu + 1) % 3
    landlord_down_cards = cards_per_ranks_to_ranks(hands[landlord_down].cards_per_rank())
    landlord_down_cards = [rank_to_douzero_representation(rank) for rank in landlord_down_cards]
    cards_left_over = state.cards_left_over()
    three_landlord_cards = []
    for card in cards_left_over:
        three_landlord_cards.append(rank_to_douzero_representation(card.card_rank()))

    three_landlord_cards.sort()
    return {
        'landlord': dizhu_cards,
        'landlord_up': landlord_up_cards,
        'landlord_down': landlord_down_cards,
        'three_landlord_cards': three_landlord_cards
    }


def collect_trajectory_and_features():
    state = pydoudizhu.DoudizhuState(pydoudizhu.default_game)
    while state.current_phase() == pydoudizhu.Phase.DEAL:
        state.apply_random_chance()

    # while state.current_phase() == pydoudizhu.Phase.AUCTION:
    #     legal_moves = state.legal_moves()
    #     random_move = random.choice(legal_moves)
    #     state.apply_move(random_move)
    state.apply_move(pydoudizhu.DoudizhuMove(pydoudizhu.AuctionType.THREE))

    cards_play_data = get_cards_play_data(state)
    encoder = pydoudizhu.DouzeroEncoder()
    save_features: List[Dict[str, np.ndarray]] = []
    while not state.is_terminal():
        obs = pydoudizhu.DoudizhuObservation(state)
        # print(obs)
        legal_moves = state.legal_moves()
        num_legal_moves = len(obs.legal_moves())
        features = encoder.encode(obs)
        modified_features = {}
        # for k, v in features.items():
        #     # print(k)
        #     f = np.array(v.encoding)
        #     f.resize(v.dims)
        #     modified_features[k] = f
        if obs.dizhu() == 0:
            x_no_action = np.hstack((
                get_array_feature(features, "my_hand"),
                get_array_feature(features, "other_hands"),
                get_array_feature(features, "most_recent_move"),
                get_array_feature(features, "second_farmer_played_cards"),
                get_array_feature(features, "first_farmer_played_cards"),
                get_array_feature(features, "second_farmer_num_cards_left"),
                get_array_feature(features, "first_farmer_num_cards_left"),
                get_array_feature(features, "num_bombs"),
            ))
            # print(x_no_action.shape)
            modified_features["x_no_action"] = x_no_action
            x_batch = np.hstack((
                get_array_feature_batched(features, "my_hand", num_legal_moves),
                get_array_feature_batched(features, "other_hands", num_legal_moves),
                get_array_feature_batched(features, "most_recent_move", num_legal_moves),
                get_array_feature_batched(features, "second_farmer_played_cards", num_legal_moves),
                get_array_feature_batched(features, "first_farmer_played_cards", num_legal_moves),
                get_array_feature_batched(features, "second_farmer_num_cards_left", num_legal_moves),
                get_array_feature_batched(features, "first_farmer_num_cards_left", num_legal_moves),
                get_array_feature_batched(features, "num_bombs", num_legal_moves),
                get_array_feature(features, "legal_moves"),
            ))
            # print(x_batch.shape)
            modified_features["x_batch"] = x_batch
            modified_features["position"] = "landlord"
        else:
            x_no_action = np.hstack((
                get_array_feature(features, "my_hand"),
                get_array_feature(features, "other_hands"),
                get_array_feature(features, "dizhu_played_cards"),
                get_array_feature(features, "another_farmer_played_cards"),
                get_array_feature(features, "most_recent_move"),
                get_array_feature(features, "dizhu_most_recent_move"),
                get_array_feature(features, "another_farmer_most_recent_move"),
                get_array_feature(features, "dizhu_num_cards_left"),
                get_array_feature(features, "another_farmer_num_cards_left"),
                get_array_feature(features, "num_bombs"),
            ))
            # print(x_no_action.shape)
            modified_features["x_no_action"] = x_no_action
            x_batch = np.hstack((
                get_array_feature_batched(features, "my_hand", num_legal_moves),
                get_array_feature_batched(features, "other_hands", num_legal_moves),
                get_array_feature_batched(features, "dizhu_played_cards", num_legal_moves),
                get_array_feature_batched(features, "another_farmer_played_cards", num_legal_moves),
                get_array_feature_batched(features, "most_recent_move", num_legal_moves),
                get_array_feature_batched(features, "dizhu_most_recent_move", num_legal_moves),
                get_array_feature_batched(features, "another_farmer_most_recent_move", num_legal_moves),
                get_array_feature_batched(features, "dizhu_num_cards_left", num_legal_moves),
                get_array_feature_batched(features, "another_farmer_num_cards_left", num_legal_moves),
                get_array_feature_batched(features, "num_bombs", num_legal_moves),
                get_array_feature(features, "legal_moves"),
            ))
            # print(x_batch.shape)
            modified_features["x_batch"] = x_batch
            if obs.dizhu() == 1:
                modified_features["position"] = "landlord_up"
            else:
                modified_features["position"] = "landlord_down"

        z = np.array(features["recent_moves"].encoding)
        z.resize(features["recent_moves"].dims)
        modified_features["z"] = z
        print("z.shape: ", z.shape)

        z_batch = np.repeat(
            z[np.newaxis, :, :],
            num_legal_moves, axis=0)
        print("z_batch.shape: ", z_batch.shape)

        modified_features["z_batch"] = z_batch

        # print(z.shape)
        # print(z_batch.shape)

        legal_actions = [move_to_douzero_representation(move.to_ranks()) for move in legal_moves]
        modified_features["legal_actions"] = legal_actions
        save_features.append(modified_features)

        # print(features)
        random_move = random.choice(legal_moves)
        state.apply_move(random_move)

    history = state.move_history()
    trajectory = []
    for item in history:
        if item.move.move_type() != pydoudizhu.MoveType.PLAY:
            continue
        ranks = item.move.to_ranks()
        ranks = [rank_to_douzero_representation(rank) for rank in ranks]
        trajectory.append(ranks)
    return cards_play_data, trajectory, save_features


# num_deals = 1
#
# data = []
# for i in trange(num_deals):
#     cards, traj, fs = collect_trajectory_and_features()
#     # print(cards, traj, fs, sep="\n")
#     data.append((cards, traj, fs))

# with open("data.pkl", "wb") as f:
#     pickle.dump(data, f, pickle.HIGHEST_PROTOCOL)
#
# print("done")

if __name__ == '__main__':
    from itertools import combinations
