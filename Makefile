SOURCES_HOST = serverDriver.c server.c
TARGET_HOST = serverDriver

SOURCES = clientDriver.c client.c
TARGET = clientDriver

S = mainPi.c
T = mainPi

SS = mainBBG.c
TT = mainBBG

PUBDIR = $(HOME)/cmpt433/public/project
OUTDIR = $(PUBDIR)

CC_C_HOST = arm-linux-gnueabihf-gcc


CFLAGS = -g -std=c99 -Werror 


all: 
	$(CC_C_HOST) $(CFLAGS) $(SOURCES_HOST) -o $(OUTDIR)/$(TARGET_HOST) 
	$(CC_C_HOST) $(CFLAGS) $(SOURCES) -o $(OUTDIR)/$(TARGET) 
	$(CC_C_HOST) $(CFLAGS) $(S) -o $(OUTDIR)/$(T)
	$(CC_C_HOST) $(CFLAGS) $(SS) -o $(OUTDIR)/$(TT)
clean:
	rm -f $(OUTDIR)/$(TARGET_HOST)
