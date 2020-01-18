#!/usr/bin/env python3


import random


def pick_box(board, skill_level):
    if skill_level == 1:
        pass
    elif skill_level == 2:
        pass
    else:
        return random.choice([i for i, x in enumerate(board) if x == 0])


if __name__ == '__main__':
    listen_chars = '012345678DWLE'
    record = []

    try:
        while True:
            print('F', end='')
            board = [0, 0, 0, 0, 0, 0, 0, 0, 0]
            while True:
                c = input()[0]

                if c not in listen_chars:
                    continue
                elif c in listen_chars[0:9]:
                    board[int(c)] = 2
                elif c in listen_chars[9:]:
                    record.append(c)
                    break

                box = pick_box(board, 0)
                board[box] = 1
                print(str(box), end='')
    except:
        raise
    finally:
        print('R', end='')