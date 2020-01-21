#!/usr/bin/env python3


import random
import sys
import telnetlib
import time

from collections import Counter
from matplotlib import pyplot as plt


port = 19021

move_first = False

max_games = 1e6

game_delay = 0

print_interval = 100


def pick_box(board, skill_level):
    if skill_level == 1:
        pass
    elif skill_level == 2:
        pass
    else:
        try:
            return random.choice([i for i, x in enumerate(board) if x == 0])
        except IndexError:
            return -1


if __name__ == '__main__':
    listen_chars = '012345678WDLE'
    record = []
    record_stats = {c: [] for c in 'WDLE'}

    try:
        t = telnetlib.Telnet(host='localhost', port=port)
        print(t.read_until(b'???', 1).decode('ascii'))
        t.write(b'R')

        for i in range(int(max_games)):
            t.write(b'F')
            board = [0, 0, 0, 0, 0, 0, 0, 0, 0]
            if move_first:
                box = pick_box(board, 0)
                board[box] = 1
                t.write(str(box).encode('ascii'))
            while True:
                c = t.read_until(b'\n', 1).decode('ascii')
                if not c:
                    print(t.read_eager().decode('ascii'))
                    raise Exception
                if c[0] not in listen_chars:
                    continue
                elif c[0] in listen_chars[0:9]:
                    board[int(c[0])] = 2
                elif c[0] in listen_chars[9:]:
                    record.append(c[0])
                    break
                else:
                    raise Exception

                box = pick_box(board, 0)
                if box == -1:
                    continue
                board[box] = 1
                t.write(str(box).encode('ascii'))
            if not (len(record) % print_interval):
                count = Counter(record[-print_interval:])
                win_percent = 100 * count['W'] / print_interval
                draw_percent = 100 * count['D'] / print_interval
                loss_percent = 100 * count['L'] / print_interval
                error_percent = 100 * count['E'] / print_interval
                print(f'games: {i + 1:d}  '
                      f'win: {win_percent:.2f}%  '
                      f'draw: {draw_percent:.2f}%  '
                      f'loss: {loss_percent:.2f}%  '
                      f'error: {error_percent:.2f}%  '
                      f'\r', file=sys.stderr, end='')
                record_stats['W'].append(win_percent)
                record_stats['D'].append(draw_percent)
                record_stats['L'].append(loss_percent)
                record_stats['E'].append(error_percent)
            time.sleep(game_delay)
    except:
        raise
    finally:
        t.write(b'R')
        t.close()
        x = [print_interval * i for i in range(len(record_stats['W']))]
        plt.plot(x, record_stats['W'], 'g', label='win %')
        plt.plot(x, record_stats['D'], 'y', label='draw %')
        plt.plot(x, record_stats['L'], 'r', label='loss %')
        plt.plot(x, record_stats['E'], 'm', label='error %')
        plt.legend()
        plt.grid()
        plt.xlabel('games')
        plt.ylabel('%')
        plt.ylim(0, 100)
        plt.show()