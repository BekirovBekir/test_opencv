.PHONY: all clean
CC = arm-linux-gnueabihf-gcc -O0 -g3 -Wall -MD
LIB = -lpthread
INCLUDE = -L/usr/arm-linux-gnueabihf/lib  -L/usr/arm-linux-gnueabihf/bin -I/usr/arm-linux-gnueabihf/include
VPATH = ../
SRC = GPIO.c ComPort.c FrameBuffer.c ModemSARA.c TempSensors.c main.c

all:power_test

#power_test: GPIO.o ComPort.o FrameBuffer.o ModemSARA.o TempSensors.o main.o
#	$(CC) GPIO.o ComPort.o FrameBuffer.o ModemSARA.o TempSensors.o main.o -o power_test $(LIB) $(INCLUDE)

#GPIO.o: ../GPIO.c
#	$(CC) ../GPIO.c $(LIB) -c $(INCLUDE)

#ComPort.o: ../ComPort.c
#	$(CC) ../ComPort.c $(LIB) -c $(INCLUDE)

#FrameBuffer.o: ../FrameBuffer.c
#	$(CC) ../FrameBuffer.c $(LIB) -c $(INCLUDE)


#ModemSARA.o: ../ModemSARA.c
#	$(CC) ../ModemSARA.c $(LIB) -c $(INCLUDE)

#TempSensors.o: ../TempSensors.c
#	$(CC) ../TempSensors.c $(LIB) -c $(INCLUDE)

#main.o: ../main.c
#	$(CC) ../main.c $(LIB) -c $(INCLUDE)



#power_test: $(patsubst ../%.c, %.o, $(wildcard ../*.c))
power_test: $(SRC:.c=.o)
	$(CC) $^ -o $@ $(LIB)

%.o: %.c
	$(CC) -c $< $(LIB) $(addprefix, -I, ../)

include $(wildcard *.d) 

clean:
	rm -rf *.o *.d power_test

 
