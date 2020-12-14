import signal
from threading import Thread, Condition, Lock
from bme280_sensor import BME280
from gpio import InputOutputDevices

run_bme280 = False
lock_bme280 = Lock()
condition_bme280 = Condition(lock_bme280)

run_send_data = False
lock_send_data = Lock()

run_read_security_sensors = False
lock_read_security_sensors = Lock()
condition_read_security_sensors = Condition(lock_read_security_sensors)

number_of_executions = 0

def read_security_sensors(params, state):
    lock_read_security_sensors.acquire()
    global run_read_security_sensors
    while not run_read_security_sensors:
        condition_read_security_sensors.wait()
        params.read_input_sensors_values()
        print(state['devices'])
        run_read_security_sensors = False
    lock_read_security_sensors.release()

def read_bme280_sensor(params, state):
    lock_bme280.acquire()
    global run_bme280
    while not run_bme280:
        condition_bme280.wait()
        params.read_data()
        print(state['bme280'])
        run_bme280 = False
    lock_bme280.release()

def alarm_handler(signum, frame):
    global number_of_executions
    number_of_executions += 1

    lock_read_security_sensors.acquire()
    global run_read_security_sensors
    if not run_read_security_sensors:
        run_read_security_sensors = True
        condition_read_security_sensors.notify()
    lock_read_security_sensors.release()

    # reads bme280 every 1 second
    if number_of_executions == 5:
        lock_bme280.acquire()
        global run_bme280
        if not run_bme280:
            run_bme280 = True
            condition_bme280.notify()
        lock_bme280.release()
        number_of_executions = 0

if __name__ == '__main__':
    signal.signal(signal.SIGALRM, alarm_handler)
    signal.setitimer(signal.ITIMER_REAL, 0.2, 0.2)

    state = {'bme280': {}, 'devices': {}}
    sensor_bme280 = BME280(state)
    devices = InputOutputDevices(state)

    check_security_sensors = Thread(target=read_security_sensors, args=(devices, state,))
    read_temperature_humidity = Thread(target=read_bme280_sensor, args=(sensor_bme280, state,))

    check_security_sensors.start()
    read_temperature_humidity.start()
