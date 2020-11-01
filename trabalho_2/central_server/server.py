#!/usr/bin/env python3
import curses
import socket
from time import sleep
import threading
import sys
import signal
import os

global stdscr
stdscr = curses.initscr()
stdscr.nodelay(1) # set getch() non-blocking
stdscr.timeout(100)
curses.noecho()
data = ""
HOST = '127.0.0.1'  # Standard loopback interface address (localhost)
PORT = 10010 # Port to listen on (non-privileged ports are > 1023)
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((HOST, PORT))


def signal_handler(signal, frame):
    curses.endwin()
    sys.exit(0)

def show():
    line = 1
    buf = ""
    n = ""
    global data
    try:
        while True:
            stdscr.clear()
            stdscr.addstr(0,0,str(data))
            stdscr.addstr(1,0,"buf: {}".format(buf))
            stdscr.addstr(2,0,"n: {}".format(n))
            stdscr.refresh()

            line += 1
            c = stdscr.getch()
            if c != curses.ERR:
                if c == 10:
                    n = buf
                    buf = "" 
                else:
                    buf += chr(c)
                          
    except KeyboardInterrupt:
        curses.endwin()
def receive():
    global data
    s.listen()
    conn, addr = s.accept()
    while True:
        data = conn.recv(1024)
        conn.sendall(data)

if __name__ == "__main__":
    signal.signal(signal.SIGINT, signal_handler)

    x = threading.Thread(target=show)
    y = threading.Thread(target=receive)
    x.start()
    y.start()
