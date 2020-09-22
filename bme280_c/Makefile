DEVICE := /dev/i2c-1

all: compile run

compile: 
	gcc read_bme280_data.c bme280.c -I . -lwiringPi -o bme280

run:
	./bme280 ${DEVICE}

clean:
	rm -v bme280
	> sensor_data_avgs.csv 
