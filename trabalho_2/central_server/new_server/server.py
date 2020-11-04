import socket

data = "{}"
run_server = True

class Server():
    def __init__(self, host, port):
        self.socket_ = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket_.bind((host, port))

    def receive(self):       
        global data
        self.socket_.listen()
        conn, addr = self.socket_.accept()
        while run_server:
            data = conn.recv(1024)
            if not data:
                exit(0) 
            conn.sendall(data)
