CC     = gcc
CFLAGS = -Wall -O2
TARGET = ltfm

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) -o $(TARGET) main.c

clean:
	rm -f $(TARGET)