CC=gcc
CFLAGS?=-std=c89 -Wall -Wno-unused-function
LDFLAGS?=
LIBS=

SRCDIR=$(shell pwd)
OBJDIR=$(SRCDIR)
BINDIR=$(SRCDIR)

OBJECTS=main.c.o
TARGET=plugins

.PHONY: all clean install uninstall
all: $(TARGET)

$(TARGET): $(OBJECTS)
	@echo $(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LIBS)
	@$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LIBS)

%.c.o: %.c
	@echo $(CC) $(CFLAGS) -c $< -o $@
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Cleaning objects and executables..."
	@rm -f $(OBJDIR)/*.c.o $(BINDIR)/$(TARGET)
	@echo "Cleaned."

install:
	@echo "TODO: Add install code here"

uninstall:
	@echo "TODO: Add uninstall code here"

