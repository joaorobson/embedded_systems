import RPi.GPIO as GPIO

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

    def __init__(self, state):
        GPIO.setmode(GPIO.BOARD)
        GPIO.setup([x[-1] for x in self.input_sensors], GPIO.IN)
        GPIO.setup([x[-1] for x in self.output_devices], GPIO.OUT, initial=GPIO.LOW)
        self.state = state.get('devices', {})

    def read_input_sensors_values(self):
        for label, pin in self.input_sensors:
            self.state[label] = GPIO.input(pin)

    def set_output_devices_value(self, pin, value):
        GPIO.output(pin, value)
