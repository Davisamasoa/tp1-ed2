CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -O2
TARGET  = exe

empty:=
space:= $(empty) $(empty)

ASI_DIR := acesso sequencial indexado
GER_DIR := gerador bin
AV_DIR  := pesquisa binaria

ASI_DIR_ESC := $(subst $(space),\ ,$(ASI_DIR))
GER_DIR_ESC := $(subst $(space),\ ,$(GER_DIR))
AV_DIR_ESC  := $(subst $(space),\ ,$(AV_DIR))

CFLAGS += -I"$(ASI_DIR)" -I"$(GER_DIR)" -I"$(AV_DIR)"

OBJS = main.o $(ASI_DIR_ESC)/asi.o $(GER_DIR_ESC)/gerador.o $(AV_DIR_ESC)/av.o

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) main.o "$(ASI_DIR)/asi.o" "$(GER_DIR)/gerador.o" "$(AV_DIR)/av.o"

main.o: main.c $(ASI_DIR_ESC)/asi.h $(GER_DIR_ESC)/gerador.h $(AV_DIR_ESC)/av.h
	$(CC) $(CFLAGS) -c main.c -o main.o

$(ASI_DIR_ESC)/asi.o: $(ASI_DIR_ESC)/asi.c $(ASI_DIR_ESC)/asi.h
	$(CC) $(CFLAGS) -c "$(ASI_DIR)/asi.c" -o "$(ASI_DIR)/asi.o"

$(GER_DIR_ESC)/gerador.o: $(GER_DIR_ESC)/gerador.c $(GER_DIR_ESC)/gerador.h $(ASI_DIR_ESC)/asi.h
	$(CC) $(CFLAGS) -c "$(GER_DIR)/gerador.c" -o "$(GER_DIR)/gerador.o"

$(AV_DIR_ESC)/av.o: $(AV_DIR_ESC)/av.c $(AV_DIR_ESC)/av.h $(ASI_DIR_ESC)/asi.h
	$(CC) $(CFLAGS) -c "$(AV_DIR)/av.c" -o "$(AV_DIR)/av.o"

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
