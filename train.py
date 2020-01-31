#!/usr/bin/env python3


import fnmatch
import os
import pickle
import random
import sys
import telnetlib
import time
from collections import Counter

import serial
from matplotlib import pyplot as plt


# whether to use telnet or uart
use_telnet = True
# telnet port
port = 19021
# uart baudrate
baudrate = 115200

# whether this program moves first
move_first = False
# max number of training games
max_games = 100000
# time delay (ms) between games
game_delay = 0
# how often (in games) to print current stats
print_interval = 100


class Comm():
    def __init__(self, telnet, port=None, baudrate=9600, timeout=1):
        self.use_telnet = telnet
        self.port = port
        self.baudrate = baudrate
        self.timeout = timeout
        if self.use_telnet:
            self.t = telnetlib.Telnet(host='localhost', port=self.port)
        else:
            if port is None:
                tty_dir = '/dev/'
                tty_match = 'ttyUSB*'
                ttys = [os.path.join(tty_dir, f) for f in os.listdir(tty_dir) 
                            if fnmatch.fnmatch(f, tty_match)]
                for tty in ttys:
                    print(f'Trying {tty:s}')
                    try:
                        with serial.Serial(tty, self.baudrate, 
                                           self.timeout) as t:
                            t.write(b'I')
                            if t.read().decode('ascii') == 'Y':
                                self.port = tty
                                break
                    except:
                        pass
                else:
                    raise RuntimeError('Could not find system tty')
            self.t = serial.Serial(self.port, self.baudrate, 
                                   timeout=self.timeout)
            self.t.open()
    
    def write(self, s):
        self.t.write(s.encode('ascii'))

    def read_until(self, s):
        if self.use_telnet:
            return self.t.read_until(s.encode('ascii'), 
                                     self.timeout).decode('ascii')
        else:
            return self.t.read_until(s.encode('ascii')).decode('ascii')

    def close(self):
        self.t.close()


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
    record_stats = {c: [] for c in 'WDLEG'}

    t = Comm(use_telnet, port if use_telnet else None, 
             baudrate=baudrate, timeout=1)
    try:
        print(t.read_until('???'))
        t.write('R')

        time_start = time.time()

        for i in range(int(max_games)):
            t.write('F')
            board = [0, 0, 0, 0, 0, 0, 0, 0, 0]
            if move_first:
                box = pick_box(board, 0)
                board[box] = 1
                t.write(str(box))
            while True:
                c = t.read_until('\n')
                if not c:
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
                t.write(str(box))
            if not (len(record) % print_interval):
                time_end = time.time()
                count = Counter(record[-print_interval:])
                win_percent = 100 * count['W'] / print_interval
                draw_percent = 100 * count['D'] / print_interval
                loss_percent = 100 * count['L'] / print_interval
                error_percent = 100 * count['E'] / print_interval
                games_per_sec = print_interval / (time_end - time_start)
                print(f'games: {i + 1:d}  '
                      f'win: {win_percent:.2f}%  '
                      f'draw: {draw_percent:.2f}%  '
                      f'loss: {loss_percent:.2f}%  '
                      f'error: {error_percent:.2f}%  '
                      f'games/sec: {games_per_sec:.2f}  '
                      f'\r', file=sys.stderr, end='')
                record_stats['W'].append(win_percent)
                record_stats['D'].append(draw_percent)
                record_stats['L'].append(loss_percent)
                record_stats['E'].append(error_percent)
                record_stats['G'].append(games_per_sec)
                time_start = time.time()
            time.sleep(game_delay)
    except:
        raise
    finally:
        t.write('R')
        t.close()
        os.chdir(sys.path[0])
        with open('record.pickle', 'wb') as f:
            pickle.dump(record, f, protocol=pickle.HIGHEST_PROTOCOL)
        x = [print_interval * (i + 1) for i in range(len(record_stats['W']))]
        plt.plot(x, record_stats['W'], 'g', label='win %')
        plt.plot(x, record_stats['D'], 'y', label='draw %')
        plt.plot(x, record_stats['L'], 'r', label='loss %')
        plt.plot(x, record_stats['E'], 'm', label='error %')
        plt.plot(x, record_stats['G'], 'b', label='games/sec')
        plt.legend()
        plt.grid()
        plt.xlabel('games')
        if x:
            plt.xlim(x[0], x[-1])
        plt.ylabel('')
        plt.ylim(0, 100)
        plt.show()
