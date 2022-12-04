# Makefile for building embedded application.
# by Brian Fraser

# Edit this file to compile extra C files into their own programs.
SOURCES_INTERFACE = doorbell.c gpio.c general.c mainBBG.c photo.c 
TARGET_INTERFACE = mainBBG

SOURCES_COM =  client.c clientDriver.c photo.c
TARGET_COM = clientDriver

PUBDIR = /mnt/remote/project/BBG

OUTDIR = $(PUBDIR)

CROSS_TOOL = arm-linux-gnueabihf-

CC_C = $(CROSS_TOOL)gcc

CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L  


all: copy_fifo
	#$(CC_C) $(CFLAGS) $(SOURCES_INTERFACE) -o $(OUTDIR)/$(TARGET_INTERFACE) -lpthread
	$(CC_C) $(CFLAGS) $(SOURCES_COM) -o $(OUTDIR)/$(TARGET_COM) -lpthread
	$(CC_C) -L /mnt/remote/face -Wall -o mainBBG $(SOURCES_INTERFACE) -lface `pkg-config --cflags --libs opencv4`
	export LD_LIBRARY_PATH=/mnt/remote/face/:$LD_LIBRARY_PATH

clean:
	rm -rf $(OUTDIR)/$(TARGET_COM)
	rm -rf $(OUTDIR)/$(TARGET_INTERFACE)


