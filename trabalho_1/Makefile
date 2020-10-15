all: compile run

compile: 
	gcc main.c -o main -lwiringPi -lbcm2835 -lpthread

run:
	./main

clean:
	rm main
	> sensor_data_avgs.csv 
