import socket
import os
import json
import csv
from datetime import datetime
import subprocess
from subprocess import Popen, DEVNULL, STDOUT


FILENAME = "log.csv"

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
        self.play_alarm_sound = None
 
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
            if not self.data:
                self.receive_socket_.close()
                exit(0) 
            self.json_data = json.loads(self.data)

    def send(self, data):
        self.send_socket_.send(bytes(data.encode('utf-8')))

    def close(self):
        self.receive_socket_.shutdown(socket.SHUT_RDWR)
        self.receive_socket_.close()
        self.send_socket_.shutdown(socket.SHUT_RDWR)
        self.send_socket_.close()

    def data_to_register(self, data):
        if "Alert" in data:
            if data["Alert"]:
                return "Alarme ativado"
            return "Alarme desativado"
        elif "Device" in data:
            device, state = data["Device"]
            devices_names = {"Lamp1": "Lâmpada da cozinha",
                             "Lamp2": "Lâmpada da sala",
                             "Lamp3": "Lâmpada do quarto 1",
                             "Lamp4": "Lâmpada do quarto 2",
                             "AirC1": "Ar Cond. do quarto 1",
                             "AirC2": "Ar Cond. do quarto 2"}
            device_name = devices_names[device]
            return device_name + " " + self.get_state_message(state, device)

    def get_state_message(self, state, key):
        if key.startswith("Lamp"):
            if state == 1:
                return "Ligada"
            return "Desligada"

        elif key.startswith("AirC"):
            if state == 1:
                return "Ligado"
            return "Desligado"

        elif key.startswith("PresSens"):
            if state == 1:
                return "Ativado"
            return "Desativado"

        elif key.startswith("OpenSens"):
            if state == 1:
                return "Aberta"
            return "Fechada"

    def control_alarm(self, alarm_state):
        if alarm_state:
            self.play_alarm_sound = Popen(["omxplayer", "nuclear.mp3"],
                                          stdin=subprocess.PIPE,
                                          stdout=DEVNULL,
                                          stderr=STDOUT,
                                          bufsize=0)
        else:
            self.play_alarm_sound.stdin.write(b'q')

    def save_log(self, data):
        with open(FILENAME, 'a') as f:
            writer = csv.writer(f)
            # Check if csv file has a header
            if os.path.getsize(FILENAME) == 0:
                fields = ["Data/Hora", "Registro"]
                writer.writerow(fields)

            date_hour = datetime.now().strftime("%d/%m/%Y %H:%M:%S")
            register = self.data_to_register(data)

            writer.writerow([date_hour, register])
