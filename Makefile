# Dynamic runtime configured Makefile - generated via mconfig
CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=gnu99 
LDFLAGS = -lcrypt
TARGET = froot

.PHONY: all clean

all: $(TARGET) $(CONFIG_TOOL)

$(TARGET): main.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c config.h local.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) *.o config.h
