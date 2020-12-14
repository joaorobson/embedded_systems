import smbus2
import bme280

class BME280:
    i2c_port = 1
    address = 0x76

    def __init__(self, state):
        self.state = state.get("bme280", {})
        self.bus = smbus2.SMBus(self.i2c_port)
        self.calibration = bme280.load_calibration_params(self.bus, self.address)

    def read_data(self):
        data = bme280.sample(self.bus, self.address, self.calibration)
        self.state['humidity'] = round(data.humidity, 2)
        self.state['temperature'] = round(data.temperature, 2)
