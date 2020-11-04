#!/usr/bin/env python3

import socket
import random
HOST = '127.0.0.1'  # The server's hostname or IP address
PORT = 10010 # The port used by the server

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))
while True:
    s.sendall(bytes(str(random.randint(0,100)%99).encode('utf-8')))
    data = s.recv(1024)

    print('Received', repr(data))
