import signal
from threading import Thread, Condition, Lock

run_bme280 = False
lock_bme280 = Lock()
condition_bme280 = Condition(lock_bme280)

run_send_data = False
lock_send_data = Lock()

run_read_security_sensors = False
lock_read_security_sensors = Lock()
condition_read_security_sensors = Condition(lock_read_security_sensors)

number_of_executions = 0

def read_security_sensors(params):
    lock_read_security_sensors.acquire()
    global run_read_security_sensors
    while not run_read_security_sensors:
        condition_read_security_sensors.wait()
        print("READ SECURITY SENSOR")
        run_read_security_sensors = False
    lock_read_security_sensors.release()

def read_bme280_sensor(params):
    lock_bme280.acquire()
    global run_bme280
    while not run_bme280:
        condition_bme280.wait()
        print("READ BME280 SENSOR")
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

    check_security_sensors = Thread(target=read_security_sensors, args=('security sensor',))
    read_temperature_humidity = Thread(target=read_bme280_sensor, args=('bme280 sensor',))

    check_security_sensors.start()
    read_temperature_humidity.start()
