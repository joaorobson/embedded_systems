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
            stdscr.addstr(0,0,"Temperatura: " + str(json_data.get("Temp", "")) + "ºC")
            stdscr.addstr(1,0,"Umidade: " + str(json_data.get("Hum", "")) + "%")
            stdscr.addstr(2,0,"Lâmp. 1: " + str(json_data.get("Lamp1", "")))
            stdscr.addstr(3,0,"Lâmp. 2: " + str(json_data.get("Lamp2", "")))
            stdscr.addstr(4,0,"Lâmp. 3: " + str(json_data.get("Lamp3", "")))
            stdscr.addstr(5,0,"Lâmp. 4: " + str(json_data.get("Lamp4", "")))
            stdscr.addstr(6,0,"Ar Cond. 1: " + str(json_data.get("Lamp4", "")))
            stdscr.addstr(7,0,"Ar Cond. 2: " + str(json_data.get("Lamp4", "")))
            stdscr.addstr(8,0,"Sens. Pres. 1: " + str(json_data.get("PresSens1", "")))
            stdscr.addstr(9,0,"Sens. Pres. 2: " + str(json_data.get("PresSens2", "")))
            stdscr.addstr(9,0,"Sens. Abert. 1: " + str(json_data.get("OpenSens1", "")))
            stdscr.addstr(9,0,"Sens. Abert. 2: " + str(json_data.get("OpenSens2", "")))
            stdscr.addstr(9,0,"Sens. Abert. 3: " + str(json_data.get("OpenSens3", "")))
            stdscr.addstr(9,0,"Sens. Abert. 4: " + str(json_data.get("OpenSens4", "")))
            stdscr.addstr(9,0,"Sens. Abert. 5: " + str(json_data.get("OpenSens5", "")))
            stdscr.addstr(9,0,"Sens. Abert. 6: " + str(json_data.get("OpenSens6", "")))
            stdscr.addstr(10,0,"buf: {}".format(buf))
            stdscr.addstr(11,0,"n: {}".format(n))
            stdscr.refresh()

            line += 1
            c = stdscr.getch()
            if c != curses.ERR:
                if c == 10:
                    n = buf
                    buf = "" 
                else:
                    buf += chr(c)
                          
    except:
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
