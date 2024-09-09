"""
@author: qzz
@contact:q873264077@gmail.com
@version: 1.0.0
@file: random_sim.py.py
@time: 2024/7/25 17:22
"""
import os
import random
import sys


def append_sys_path():
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    sys.path.append(os.path.join(root, "build", "doudizhu_learning_environment"))


append_sys_path()

import pydoudizhu

if __name__ == '__main__':
    params = {}
    game = pydoudizhu.DoudizhuGame(params)
    for i in range(100):
        state = pydoudizhu.DoudizhuState(game)

        while state.is_chance_node():
            state.apply_random_chance()

        while not state.is_terminal():
            # print(state)
            legal_moves = state.legal_moves()
            # print(legal_moves)
            legal_move_uids = [game.get_move_uid(move) for move in legal_moves]
            print(legal_move_uids)
            random_move = random.choice(legal_moves)
            state.apply_move(random_move)

