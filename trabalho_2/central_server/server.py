import socket
import json

class Server():
    def __init__(self, cs_host, cs_port, ds_host, ds_port):
        self.cs_host = cs_host # Central server host
        self.cs_port = cs_port # Central server port
        self.ds_host = ds_host # Distr server host
        self.ds_port = ds_port # Distr server port

        self.receive_socket_ = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.send_socket_ = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        self.receive_socket_.bind((cs_host, cs_port))

        self.run_server = True
        self.data = "{}"
        self.json_data = json.loads(self.data)
        
        self.connect_to_dist_server()
 
    def connect_to_dist_server(self):
        self.connected_to_ds = False
        print('Esperando servidor distribuído...')
        while not self.connected_to_ds:
            try:
                self.send_socket_.connect((self.ds_host, self.ds_port))
                self.connected_to_ds = True
            except Exception as e:
                pass #Do nothing, just try to connect again 

    def receive(self):       
        self.receive_socket_.listen()
        conn, addr = self.receive_socket_.accept()

        while self.run_server:
            self.data = conn.recv(1024)
            self.json_data = json.loads(self.data)
            if not self.data:
                self.receive_socket_.close()
                exit(0) 
            conn.sendall(self.data)

    def send(self, data):
        self.send_socket_.sendall(bytes(data.encode('utf-8')))
        self.send_socket_.recv(1024)

    def close(self):
        self.receive_socket_.shutdown(socket.SHUT_RDWR)
        self.receive_socket_.close()
        self.send_socket_.shutdown(socket.SHUT_RDWR)
        self.send_socket_.close()

