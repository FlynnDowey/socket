# Makefile for building embedded application.
# by Brian Fraser

# Edit this file to compile extra C files into their own programs.

SOURCES_INTERFACE = mainPi.c general.c photo.c
TARGET_INTERFACE = mainPi

SOURCES_COM =  server.c serverDriver.c photo.c
TARGET_COM = serverDriver

PUBDIR = $(HOME)/cmpt433/public/project/PI

OUTDIR = $(PUBDIR)

CROSS_TOOL = arm-linux-gnueabihf-


CC_CPP = $(CROSS_TOOL)g++
CC_C = $(CROSS_TOOL)gcc

CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L  


all: copy_fifo
	$(CC_C) $(CFLAGS) $(SOURCES_INTERFACE) -o $(OUTDIR)/$(TARGET_INTERFACE) -lpthread
	$(CC_C) $(CFLAGS) $(SOURCES_COM) -o $(OUTDIR)/$(TARGET_COM) -lpthread

clean:
	rm -f $(OUTDIR)/$(TARGET_INTERFACE)
	rm -f $(OUTDIR)/$(TARGET_COM)

copy_fifo:
	cp makeFifo_1.sh $(OUTDIR)
	cp makeFifo_2.sh $(OUTDIR)
