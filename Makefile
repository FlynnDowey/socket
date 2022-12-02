# Makefile for building embedded application.
# by Brian Fraser

# Edit this file to compile extra C files into their own programs.
SOURCES_INTERFACE_BBG = doorbell.c gpio.c general.c mainBBG.c photo.c
TARGET_INTERFACAE_BBG = mainBBG

SOURCES_COM_BBG =  client.c clientDriver.c
TARGET_COM_BBG = clientDriver

SOURCES_INTERFACE_PI = mainPi.c general.c photo.c
TARGET_INTERFACAE_PI = mainPi

SOURCES_COM_PI =  server.c serverDriver.c photo.c
TARGET_COM_PI = serverDriver

PUBDIR_PI = $(HOME)/cmpt433/public/project/PI
PUBDIR_BBG = $(HOME)/cmpt433/public/project/BBG
# PUBDIR = .
OUTDIR_PI = $(PUBDIR_PI)
OUTDIR_BBG = $(PUBDIR_BBG)

CROSS_TOOL = arm-linux-gnueabihf-
# CROSS_TOOL = 

CC_CPP = $(CROSS_TOOL)g++
CC_C = $(CROSS_TOOL)gcc
CC_H = gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L  
# -pg for supporting gprof profiling.
# CFLAGS += -pg

all: 
	$(CC_C) $(CFLAGS) $(SOURCES_INTERFACE_BBG) -o $(OUTDIR_BBG)/$(TARGET_INTERFACAE_BBG) -lpthread
	$(CC_C) $(CFLAGS) $(SOURCES_COM_BBG) -o $(OUTDIR_BBG)/$(TARGET_COM_BBG) -lpthread
	$(CC_C) $(CFLAGS) $(SOURCES_INTERFACE_PI) -o $(OUTDIR_PI)/$(TARGET_INTERFACAE_PI) -lpthread
	$(CC_C) $(CFLAGS) $(SOURCES_COM_PI) -o $(OUTDIR_PI)/$(TARGET_COM_PI) -lpthread

clean:
	rm -f $(OUTDIR_BBG)/$(TARGET_INTERFACAE_BBG)
	rm -f $(OUTDIR_BBG)/$(TARGET_COM_BBG)
	rm -f $(OUTDIR_PI)/$(TARGET_INTERFACAE_PI)
	rm -f $(OUTDIR_PI)/$(TARGET_PI)

