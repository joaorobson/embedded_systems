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

    def __init__(self, state, emitter):
        GPIO.setmode(GPIO.BCM)
        GPIO.setup([x[-1] for x in self.input_sensors], GPIO.IN)
        GPIO.setup([x[-1] for x in self.output_devices], GPIO.OUT, initial=GPIO.LOW)
        self.emitter = emitter
        self.last_activate_alarm = False
        self.full_state = state
        self.devices_state = state.get('devices', self.initial_state)

    def read_input_sensors_values(self):
        for label, pin in self.input_sensors:
            self.devices_state[label] = GPIO.input(pin)

    def check_activate_alarm(self):
        activate_alarm = any([self.devices_state[x] for x in self.input_sensors])
        if activate_alarm and self.last_activate_alarm != activate_alarm:
            self.full_state['alarm'] = activate_alarm
            self.emitter.send_data("fse/150154003/central", json.dumps(self.full_state))
            self.last_activate_alarm = activate_alarm
        elif not activate_alarm and self.last_activate_alarm != activate_alarm:
            self.full_state['alarm'] = activate_alarm
            self.last_activate_alarm = activate_alarm

    def set_output_devices_value(self, pin, value):
        GPIO.output(pin, value)
