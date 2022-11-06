SOURCES_HOST = serverDriver.c server.c
TARGET_HOST = server

SOURCES = clientDriver.c client.c
TARGET = client

S = mainPi.c
T = main

PUBDIR = $(HOME)/cmpt433/public/myApps
OUTDIR = $(PUBDIR)

CC_C_HOST = gcc


CFLAGS = -g -std=c99 -Werror 


all: 
	$(CC_C_HOST) $(CFLAGS) $(SOURCES_HOST) -o $(OUTDIR)/$(TARGET_HOST) 
	$(CC_C_HOST) $(CFLAGS) $(SOURCES) -o $(OUTDIR)/$(TARGET) 
	$(CC_C_HOST) $(CFLAGS) $(S) -o $(OUTDIR)/$(T)

clean:
	rm -f $(OUTDIR)/$(TARGET_HOST)
