CC=gcc
CFLAGS?=-std=c89 -g -Wall -Wno-unused-function
LDFLAGS?=
LIBS=-ldl

SRCDIR=$(shell pwd)
OBJDIR=$(SRCDIR)/../obj
BINDIR=$(SRCDIR)/../bin

OBJECTS=$(OBJDIR)/main.c.o $(OBJDIR)/plugin_manager.c.o $(OBJDIR)/plugin_discovery.c.o
TARGET=$(BINDIR)/PluginManager

.PHONY: all clean install uninstall
all: $(TARGET)

$(TARGET): $(OBJECTS)
	@echo CCLD $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LIBS)
	@$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LIBS)

$(OBJDIR)/%.c.o: $(SRCDIR)/%.c
	@echo CC $(CFLAGS) -c $< -o $@
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Cleaning objects and executables..."
	@rm -f $(OBJDIR)/*.c.o $(TARGET)
	@rmdir $(OBJDIR) $(BINDIR)
	@echo "Cleaned."

install:
	@echo "TODO: Add install code here"

uninstall:
	@echo "TODO: Add uninstall code here"

