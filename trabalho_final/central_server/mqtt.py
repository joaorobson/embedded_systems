import paho.mqtt.client as mqtt
import json
from alarm_player import Player
from constants import GENERAL_TOPIC, FRONTEND_TOPIC
from datetime import datetime
import csv
from handle_csv import CSV

class MQTT:

    def __init__(self):
        self.player = Player()
        self.client = mqtt.Client()
        self.csv_handler = CSV()
        self.client.on_connect = self.on_connect
        self.client.on_message = self.process_message

        self.client.connect("mqtt.eclipseprojects.io", 1883, 60)

        self.client.loop_start()


    def set_state(self, state, devices_handler):
        self.state = state 
        self.devices_handler = devices_handler

    def on_connect(self, client, userdata, flags, rc):
        self.client.subscribe(GENERAL_TOPIC)

    def send_data(self, topic, state):
        self.client.publish(topic, state)
        self.csv_handler.write_on_csv(topic, state)

    def process_message(self, client, userdata, msg):
        try:
            msg_data = json.loads(msg.payload.decode())
        except Exception as e:
            print("Message is not in JSON format!", e)
            return -1 
        
        if msg.topic == FRONTEND_TOPIC:
            if "device_name" in msg_data:
                self.devices_handler.switch_output_device_state(msg_data["device_name"])
            elif "alarm" in msg_data:
                self.player.decide_play_alarm(msg_data["alarm"])
                self.state['alarm'] = msg_data["alarm"]

        if GENERAL_TOPIC[:-1] in msg.topic:
             self.csv_handler.write_on_csv(msg.topic, msg_data)
