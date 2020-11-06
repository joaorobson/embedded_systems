from menu import Menu
from server import Server
import threading
import curses
import signal
import sys

CS_PORT = 10010
DS_PORT = 10110 
CS_HOST = "192.168.0.53"
DS_HOST = "192.168.0.52"

global server
server = Server(CS_HOST, CS_PORT, DS_HOST, DS_PORT)

def signal_handler(signal, frame):
    server.close()
    sys.exit(0)

if __name__ == "__main__":
    signal.signal(signal.SIGINT, signal_handler)
    menu_thread = threading.Thread(target=curses.wrapper, args=[Menu, server]) 
    server_thread = threading.Thread(target=server.receive)
    server_thread.start()
    menu_thread.start()
