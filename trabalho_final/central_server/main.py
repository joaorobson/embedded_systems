import json
import signal
from threading import Thread, Condition, Lock
from bme280_sensor import BME280
from gpio import InputOutputDevices
from mqtt import MQTT
from constants import CENTRAL_TOPIC
import sys

run_main = True

run_bme280 = False
lock_bme280 = Lock()
condition_bme280 = Condition(lock_bme280)

run_send_data = False
lock_send_data = Lock()
condition_send_data = Condition(lock_send_data)

run_read_security_sensors = False
lock_read_security_sensors = Lock()
condition_read_security_sensors = Condition(lock_read_security_sensors)

number_of_executions = 0

def read_security_sensors(devices, state):
    lock_read_security_sensors.acquire()
    global run_read_security_sensors
    while not run_read_security_sensors and run_main:
        condition_read_security_sensors.wait()
        devices.read_sensors_values()
        run_read_security_sensors = False
    lock_read_security_sensors.release()

def read_bme280_sensor(sensor_bme280, state):
    lock_bme280.acquire()
    global run_bme280
    while not run_bme280 and run_main:
        condition_bme280.wait()
        sensor_bme280.read_data()
        run_bme280 = False
    lock_bme280.release()

def send_data(mqtt, state):
    lock_send_data.acquire()
    global run_send_data
    while not run_send_data and run_main:
        condition_send_data.wait()
        mqtt.send_data(CENTRAL_TOPIC, json.dumps(state))
        run_send_data = False
    lock_send_data.release()

def signal_handler(signal, frame):
    global run_main
    run_main = False

def alarm_handler(signum, frame):
    global number_of_executions
    number_of_executions += 1

    lock_read_security_sensors.acquire()
    global run_read_security_sensors
    if not run_read_security_sensors:
        run_read_security_sensors = True
        condition_read_security_sensors.notify()
    lock_read_security_sensors.release()
    
    # reads bme280 and send data every 1 second
    if number_of_executions == 5:
        lock_bme280.acquire()
        global run_bme280
        if not run_bme280:
            run_bme280 = True
            condition_bme280.notify()
        lock_bme280.release()

        lock_send_data.acquire()
        global run_send_data
        if not run_send_data:
            run_send_data = True
            condition_send_data.notify()
        lock_send_data.release()
        number_of_executions = 0

if __name__ == '__main__':
    signal.signal(signal.SIGINT, signal_handler)

    state = {'bme280': {}, 'devices': {}, 'alarm': False}
    sensor_bme280 = BME280(state)
    mqtt = MQTT()
    devices = InputOutputDevices(state, mqtt)
    
    mqtt.set_state(state, devices)

    check_security_sensors = Thread(target=read_security_sensors, args=(devices, state,))
    read_temperature_humidity = Thread(target=read_bme280_sensor, args=(sensor_bme280, state,))
    send_data_thread = Thread(target=send_data, args=(mqtt, state,))

    signal.signal(signal.SIGALRM, alarm_handler)
    signal.setitimer(signal.ITIMER_REAL, 0.2, 0.2)

    check_security_sensors.start()
    read_temperature_humidity.start()
    send_data_thread.start()
