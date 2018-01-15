CC=gcc
CFLAGS?=-std=c89 -g -Wall -Wno-unused-function -fPIC -pedantic
LDFLAGS?=-shared
LIBS=

SRCDIR=$(shell pwd)
OBJDIR=$(SRCDIR)/../../obj/plugins
BINDIR=$(SRCDIR)/../../bin/plugins

TARGETS=$(BINDIR)/hello.so $(BINDIR)/program101.so

.PHONY: all clean install uninstall
all: $(TARGETS)

$(BINDIR)/hello.so: $(OBJDIR)/hello.c.o
	@echo CCLD $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LIBS)
	@$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LIBS)

$(BINDIR)/program101.so: $(OBJDIR)/program101.c.o
	@echo CCLD $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LIBS)
	@$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LIBS)

$(OBJDIR)/%.c.o: $(SRCDIR)/%.c
	@echo CC $(CFLAGS) -c $< -o $@
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Cleaning..."
	@rm -f $(OBJDIR)/*.c.o $(TARGETS)
	@rmdir $(OBJDIR) $(BINDIR)
	@echo "Done."

install:
	@echo "TODO: Add install code here."

uninstall:
	@echo "TODO: Add uninstall code here."

