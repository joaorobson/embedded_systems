from menu import Menu
from server import Server
import threading
import curses
import signal
import sys

PORT = 10010
HOST = "127.0.0.1"

def signal_handler(signal, frame):
    sys.exit(0)
    curses.endwin()

if __name__ == "__main__":
    signal.signal(signal.SIGINT, signal_handler)
    server = Server(HOST, PORT)
    x = threading.Thread(target=curses.wrapper, args=[Menu]) 
    y = threading.Thread(target=server.receive)
    x.start()
    y.start()
