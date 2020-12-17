import RPi.GPIO as GPIO
from constants import CENTRAL_TOPIC
import json

class InputOutputDevices:

    input_sensors = [
        ('Sala', 25),
        ('Cozinha', 26),
        ('Porta Cozinha', 5),
        ('Janela Cozinha', 6),
        ('Porta Sala', 12),
        ('Janela Sala', 16)
    ]
    output_devices = [
        ('Lâmpada Cozinha', 17),
        ('Lâmpada Sala', 18)
    ]
    initial_state = {
        'Sala': 0,
        'Cozinha': 0,
        'Porta Cozinha': 0,
        'Janela Cozinha': 0,
        'Porta Sala': 0,
        'Janela Sala': 0,
        'Lâmpada Cozinha': 0,
        'Lâmpada Sala': 0
    }

    def __init__(self, state, mqtt):
        GPIO.setmode(GPIO.BCM)
        GPIO.setup([x[-1] for x in self.input_sensors], GPIO.IN)
        GPIO.setup([x[-1] for x in self.output_devices], GPIO.OUT, initial=GPIO.LOW)
        self.mqtt = mqtt 
        self.last_activate_alarm = False
        self.full_state = state
        self.devices_state = state.get('devices', self.initial_state)

    def read_input_sensors_values(self):
        for label, pin in self.input_sensors:
            self.devices_state[label] = GPIO.input(pin)
    
    def read_output_devices_values(self):
        for label, pin in self.output_devices:
            self.devices_state[label] = GPIO.input(pin)

    def read_sensors_values(self):
        self.read_input_sensors_values()
        self.read_output_devices_values()

    def switch_output_device_state(self, device_name):
        if device_name in self.devices_state:
            pin = [dev[1] for dev in self.output_devices if dev[0] == device_name][0]
            current_state = self.devices_state[device_name]
            GPIO.output(pin, 1 - current_state)
            self.devices_state[device_name] = 1 - current_state
                        
    def set_output_devices_value(self, pin, value):
        GPIO.output(pin, value)
