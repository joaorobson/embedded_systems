#!/usr/bin/env python3
import curses
import socket
from time import sleep
import threading
import sys
import signal
import os
import json

global stdscr
stdscr = curses.initscr()
stdscr.nodelay(1) # set getch() non-blocking
stdscr.timeout(100)
curses.noecho()
data = "{}"
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
            if not data:
                curses.endwin()
                exit(0) 
            json_data = json.loads(data)
            stdscr.clear()
            stdscr.addstr(0,0,"Temperatura: " + str(json_data.get("Temp", "")))
            stdscr.addstr(1,0,"Umidade: " + str(json_data.get("Hum", "")))
            stdscr.addstr(2,0,"Lâmp. 1: " + str(json_data.get("Lamp1", "")))
            stdscr.addstr(3,0,"buf: {}".format(buf))
            stdscr.addstr(4,0,"n: {}".format(n))
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
        exit(0)

def receive():
    global data
    s.listen()
    conn, addr = s.accept()
    try:
        while True:
            data = conn.recv(1024)
            if not data:
                exit(0) 
            conn.sendall(data)

    except KeyboardInterrupt:
        curses.endwin()
        exit(0)

if __name__ == "__main__":
    signal.signal(signal.SIGINT, signal_handler)

    x = threading.Thread(target=show)
    y = threading.Thread(target=receive)
    x.start()
    y.start()
