#Makefile
#Jared Mulhausen
#9-28-19
CC = gcc
CFLAGS = -g -Wall
TARGET = project1

all: $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean:
	$(RM) $(TARGET)
