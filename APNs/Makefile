all: clean build

clean: 
	- rm apns.o
	- rm apns_request.dat
	- rm apns_response.dat

build: apns.o
	gcc -g -o apns apns.o

apns.o: apns.c apns.h
	gcc -c apns.c -o apns.o

