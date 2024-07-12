CC = gcc
CFLAGS = -I/usr/local/cuda/include
LDFLAGS = -L/usr/local/cuda/lib64 -lOpenCL

all: device_query

device_query: main.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

run: device_query
	./device_query
clean: 
	rm -f device_query