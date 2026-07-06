# Dynamic runtime configured Makefile - generated via mconfig
CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=gnu99 
LDFLAGS = -lcrypt
TARGET = froot
Q = @

.PHONY: all clean

all: $(TARGET) $(CONFIG_TOOL)

$(TARGET): main.o
	$(Q)$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	$(Q)echo " CC  $^"

%.o: %.c config.h local.h
	$(Q)$(CC) $(CFLAGS) -c $< -o $@
	$(Q)echo " LANGUAGE "

clean:
	$(Q)rm -f $(TARGET) *.o config.h
	$(Q)echo "Clean..."
