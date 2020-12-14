import paho.mqtt.client as mqtt

class Emitter:

    def __init__(self):
        self.client = mqtt.Client()
        self.client.connect("mqtt.eclipseprojects.io", 1883, 60)
        self.client.loop_start()

    def send_data(self, topic, state):
        self.client.publish(topic, state)
