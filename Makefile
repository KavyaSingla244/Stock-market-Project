TARGET=stock_simulator

CC=gcc

CFLAGS= -g -Wall

SRCS=main.c user.c orderbook.c


HDRS=user.h orderbook.h colors.h

$(TARGET): $(SRCS) $(HDRS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

clean :
	rm -f $(TARGET)

