CC = gcc
CFLAGS = -m32 -fPIC -shared -Wall -O2
LDFLAGS = -ldl
TARGET = libunity.so
SRC = unity_libretro.c

all:
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET)

