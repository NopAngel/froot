CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=gnu99 
LDFLAGS = -lcrypt
TARGET = froot
Q = @

PREFIX ?= /usr/local
BINDIR = $(DESTDIR)$(PREFIX)/bin

.PHONY: all clean install uninstall

all: $(TARGET) $(CONFIG_TOOL)

$(TARGET): main.o
	$(Q)$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	$(Q)echo " CC  $^"

%.o: %.c config.h local.h
	$(Q)$(CC) $(CFLAGS) -c $< -o $@
	$(Q)echo " LANGUAGE "

# System installation rule
install: $(TARGET)
	@echo "Installing $(TARGET) to $(BINDIR)..."
	$(Q)mkdir -p $(BINDIR)
	$(Q)cp $(TARGET) $(BINDIR)/$(TARGET)
	@echo "Setting SUID permissions (root elevation) for security..."
	$(Q)chown root:root $(BINDIR)/$(TARGET) 2>/dev/null || echo "  warning   could not change owner to root. run 'sudo make install' to fix."
	$(Q)chmod 4755 $(BINDIR)/$(TARGET)
	@echo "Installation successful."

# System uninstallation rule
uninstall:
	@echo "Removing $(TARGET) from $(BINDIR)..."
	$(Q)rm -f $(BINDIR)/$(TARGET)
	@echo "Uninstallation complete."

clean:
	$(Q)rm -f $(TARGET) *.o config.h
	$(Q)echo "Clean..."
