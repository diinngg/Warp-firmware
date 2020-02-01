#!/usr/bin/env python3


import os
import pickle
import random
import sys


def get_board_id(board):
    board_id = 0
    for i in range(9):
        board_id += board[i] * 2**(2*i)
    return board_id


def is_valid(board, boards):
    # equivalent boards
    board_id = get_board_id(board)
    if board_id in boards:
        return 0
    min_board_id = board_id
    # rotations
    board_id = get_board_id([board[i] for i in [2, 5, 8, 1, 4, 7, 0, 3, 6]])
    if board_id in boards:
        return 0
    min_board_id = min(board_id, min_board_id)
    board_id = get_board_id([board[i] for i in [8, 7, 6, 5, 4, 3, 2, 1, 0]])
    if board_id in boards:
        return 0
    min_board_id = min(board_id, min_board_id)
    board_id = get_board_id([board[i] for i in [6, 3, 0, 7, 4, 1, 8, 5, 2]])
    if board_id in boards:
        return 0
    min_board_id = min(board_id, min_board_id)
    # reflections
    board_id = get_board_id([board[i] for i in [2, 1, 0, 5, 4, 3, 8, 7, 6]])
    if board_id in boards:
        return 0
    min_board_id = min(board_id, min_board_id)
    board_id = get_board_id([board[i] for i in [6, 7, 8, 3, 4, 5, 0, 1, 2]])
    if board_id in boards:
        return 0
    min_board_id = min(board_id, min_board_id)
    board_id = get_board_id([board[i] for i in [8, 5, 2, 7, 4, 1, 6, 3, 0]])
    if board_id in boards:
        return 0
    min_board_id = min(board_id, min_board_id)
    board_id = get_board_id([board[i] for i in [0, 3, 6, 1, 4, 7, 2, 5, 8]])
    if board_id in boards:
        return 0
    min_board_id = min(board_id, min_board_id)

    # won boards
    if (board[0] != 0) and ((board[0] == board[1] == board[2]) or
                            (board[0] == board[3] == board[6]) or
                            (board[0] == board[4] == board[8])):
        return 0
    elif (board[2] != 0) and ((board[2] == board[5] == board[8]) or
                              (board[2] == board[4] == board[6])):
        return 0
    elif (board[7] != 0) and ((board[6] == board[7] == board[8]) or
                              (board[1] == board[4] == board[7])):
        return 0
    elif (board[3] != 0) and ((board[3] == board[4] == board[5])):
        return 0

    return min_board_id


def move(board, move_num, boards):
    """Recursively make all possible moves"""
    if move_num < 7:
        for i in range(9):
            if not board[i]:
                board[i] = (move_num % 2) + 1
                board_id = is_valid(board, boards)
                if board_id:
                    boards[board_id] = move_num + 1
                    print(len(boards))
                move(board, move_num + 1, boards)
                board[i] = 0


def do_move():
    """Find all possible boards"""
    board = [0, 0, 0, 0, 0, 0, 0, 0, 0]
    boards = {0: 0}
    move(board, 0, boards)
    return boards


if __name__ == '__main__':
    os.chdir(sys.path[0])
    # find all possible boards, takes a while so comment out if done before
    with open('boards.pickle', 'wb') as f:
        pickle.dump(do_move(), f, protocol=pickle.HIGHEST_PROTOCOL)
    # load boards
    with open('boards.pickle', 'rb') as f:
        boards = pickle.load(f)

    # list boards by number of moves made
    boards_by_move_num = [[] for _ in range(10)]
    for board_id, move_num in boards.items():
        boards_by_move_num[move_num].append(board_id)

    print()

    # generate boards arrays
    out_type = 'uint32_t'
    for i in range(len(boards_by_move_num)):
        if not len(boards_by_move_num[i]):
            continue
        boards_by_move_num[i].sort()
        
        print(f'const {out_type:s} boards{i:d}[] = '
              f'{{ 0x{len(boards_by_move_num[i]):X}, ', end = '')
        for board in boards_by_move_num[i][0:-1]:
            print(f'0x{board:X}, ', end = '')
        print(f'0x{boards_by_move_num[i][-1]:X} }};')
    print(f'const {out_type:s} *boards[] = {{ boards0, boards1, boards2, boards3, boards4, boards5, boards6, boards7 }};')

    print()

    # generate moves arrays
    out_type = 'uint8_t'
    sum = 0
    for i in range(len(boards_by_move_num)):
        if not len(boards_by_move_num[i]):
            continue
        
        spaces = 9 - i
        s = '0x'
        for j in range(spaces):
            s += '7'
            if j % 2 and j < spaces - 1:
                s += ', 0x'
        if spaces % 2:
            s += '0'
        s = ', '.join([s]*len(boards_by_move_num[i]))
        print(f'{out_type:s} moves{i:d}[] = {{ {s:s} }};')
    print(f'const {out_type:s} *moves[] = {{ moves0, moves1, moves2, moves3, moves4, moves5, moves6, moves7 }};')

    print()

    # generate random array
    num_rand = 127
    bits = 8
    out_type = 'uint8_t'
    rand = [random.getrandbits(bits) for _ in range(num_rand)]
    print(f'const {out_type:s} rand_nums[] = {{ ', end='')
    for i in range(len(rand) - 1):
        print(f'0x{rand[i]:X}, ', end='')
    print(f'0x{rand[-1]:X} }};')
    print(f'const uint32_t rand_len = {num_rand};')

    print()
    