CC = gcc
CFLAGS =
LIBS = -lpthread -lbcm2835
INCLUDE = -Iinclude/

SOURCE = src/main.c
SOURCE += src/angle.c
SOURCE += src/pwm_50.c
SOURCE += src/PID.c

all:
	gcc $(SOURCE) $(CFLAGS) $(LIBS) $(INCLUDE) -o build/main
