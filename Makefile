SOURCES_HOST = testSocket.c client.c server.c
TARGET_HOST = socket

PUBDIR = $(HOME)/cmpt433/public/
OUTDIR = $(PUBDIR)

CC_C_HOST = gcc


CFLAGS = -g -std=c99 -Werror -Wall


all: $(SOURCES)
	$(CC_C_HOST) $(CFLAGS) $(SOURCES_HOST) -o $(OUTDIR)/$(TARGET_HOST) 
clean:
	rm -f $(OUTDIR)/$(TARGET_HOST)
