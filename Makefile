CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -O2
TARGET  = exe

empty:=
space:= $(empty) $(empty)

ASI_DIR := acesso sequencial indexado
GER_DIR := gerador bin

ASI_DIR_ESC := $(subst $(space),\ ,$(ASI_DIR))
GER_DIR_ESC := $(subst $(space),\ ,$(GER_DIR))

CFLAGS += -I"$(ASI_DIR)" -I"$(GER_DIR)"

OBJS = main.o $(ASI_DIR_ESC)/asi.o $(GER_DIR_ESC)/gerador.o

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) main.o "$(ASI_DIR)/asi.o" "$(GER_DIR)/gerador.o"

main.o: main.c $(ASI_DIR_ESC)/asi.h $(GER_DIR_ESC)/gerador.h
	$(CC) $(CFLAGS) -c main.c -o main.o

$(ASI_DIR_ESC)/asi.o: $(ASI_DIR_ESC)/asi.c $(ASI_DIR_ESC)/asi.h
	$(CC) $(CFLAGS) -c "$(ASI_DIR)/asi.c" -o "$(ASI_DIR)/asi.o"

$(GER_DIR_ESC)/gerador.o: $(GER_DIR_ESC)/gerador.c $(GER_DIR_ESC)/gerador.h $(ASI_DIR_ESC)/asi.h
	$(CC) $(CFLAGS) -c "$(GER_DIR)/gerador.c" -o "$(GER_DIR)/gerador.o"

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
