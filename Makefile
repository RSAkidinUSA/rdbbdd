CC = gcc
CFLAGS += -Wall -Werror
SRC	= $(wildcard *.c)
OBJ = $(patsubst %.c,%.o, $(SRC))
APP = rdbbdd

.PHONY: clean

all: CFLAGS += -g
all: build

O1: CFLAGS += -O1
O1:	build

O2: CFLAGS += -O2
O2:	build

O3: CFLAGS += -O3
O3:	build

build: $(OBJ)
	$(CC) -o $(APP) $(OBJ)

clean:
	rm -f $(OBJ) $(APP)
