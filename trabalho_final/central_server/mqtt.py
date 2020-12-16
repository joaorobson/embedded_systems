import paho.mqtt.client as mqtt
import json
from constants import FRONTEND_TOPIC
from alarm_player import Player

class MQTT:

    def __init__(self):
        self.player = Player()
        self.client = mqtt.Client()
        self.client.on_connect = self.on_connect
        self.client.on_message = self.process_message

        self.client.connect("mqtt.eclipseprojects.io", 1883, 60)

        self.client.loop_start()

    def set_devices(self, devices):
        self.devices = devices

    def on_connect(self, client, userdata, flags, rc):
        self.client.subscribe(FRONTEND_TOPIC)

    def send_data(self, topic, state):
        self.client.publish(topic, state)

    def process_message(self, client, userdata, msg):
        try:
            msg = json.loads(msg.payload.decode())
        except Exception as e:
            print("Message is not in JSON format!", e)
            return -1 

        if "device_name" in msg:
            self.devices.switch_output_device_state(msg["device_name"])
        elif "alarm" in msg:
            self.player.decide_play_alarm(msg["alarm"])
