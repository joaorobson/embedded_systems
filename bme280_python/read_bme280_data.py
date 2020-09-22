import smbus2
import bme280
import RPi_I2C_driver
from time import *

porta_i2c = 1
endereco = 0x76
bus = smbus2.SMBus(porta_i2c)

calibracao_paramentros = bme280.load_calibration_params(bus, endereco)
mylcd = RPi_I2C_driver.lcd()

while True:
    dado = bme280.sample(bus, endereco, calibracao_paramentros)
    mylcd.lcd_display_string("P:{:.2f}".format((dado.pressure)), 1)
    mylcd.lcd_display_string("T:{:.2f} ".format(dado.temperature) + "U:{:.2f}".format(dado.humidity), 2)
    sleep(1)
