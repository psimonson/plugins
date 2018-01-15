CC=gcc
CFLAGS=-std=c89 -g -Wall -Wno-unused-function -pedantic
#LDFLAGS?=
#LIBS?=
VERSION=0.02

.PHONY: all PluginManager clean install uninstall dist
all: PluginManager

PluginManager:
	@echo "Building project..."
	@mkdir -p bin/plugins
	@mkdir -p obj/plugins
	@cd src/plugins; make
	@cd src; make
	@echo "Done building."

clean:
	@echo "Cleaning project..."
	cd src/plugins; make clean
	cd src; make clean
	@echo "Cleaned project."

install:
	@echo "Installing project..."
	@cd src/plugins; make install
	@cd src; make install
	@echo "Done with install!"

uninstall:
	@echo "Uninstalling project..."
	@cd src/plugins; make uninstall
	@cd src; make uninstall
	@echo "Done with uninstall!"

dist:
	@echo "Making distribution..."
	@tar cvjpf ../plugins-$(VERSION).tar.bz2 ../plugins
	@echo "Done."

