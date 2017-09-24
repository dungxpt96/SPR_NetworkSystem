#
# Makefile that builds btest and other helper programs for the CS:APP data lab
# 
CC = gcc
CFLAGS = -O
LIBS = -lm

all: proxy sender receiver

proxy: proxy.c proxy.h spr.h
	$(CC) $(CFLAGS) $(LIBS) -o proxy proxy.c -lpthread

sender: sender.c sender.h spr.h
	$(CC) $(CFLAGS) $(LIBS) -o sender sender.c -lpthread

receiver: receiver.c receiver.h spr.h
	$(CC) $(CFLAGS) $(LIBS) -o receiver receiver.c -lpthread

clean:
	rm -f *.o proxy sender receiver *~


