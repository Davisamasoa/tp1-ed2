CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -O2
TARGET  = exe

empty:=
space:= $(empty) $(empty)

ASI_DIR := acesso sequencial indexado
GER_DIR := gerador bin
PB_DIR  := pesquisa binaria
AVB_DIR := arvore B
ABE_DIR := arvore B*

ASI_DIR_ESC := $(subst $(space),\ ,$(ASI_DIR))
GER_DIR_ESC := $(subst $(space),\ ,$(GER_DIR))
PB_DIR_ESC  := $(subst $(space),\ ,$(PB_DIR))
AVB_DIR_ESC := $(subst $(space),\ ,$(AVB_DIR))
# o make trata * como curinga (e "arvore B*" casaria tambem com "arvore B");
# [*] casa so com o caractere * em si
ABE_DIR_ESC := $(subst *,[*],$(subst $(space),\ ,$(ABE_DIR)))

CFLAGS += -I"$(ASI_DIR)" -I"$(GER_DIR)" -I"$(PB_DIR)" -I"$(AVB_DIR)" -I"$(ABE_DIR)"

OBJS = main.o $(ASI_DIR_ESC)/asi.o $(GER_DIR_ESC)/gerador.o $(PB_DIR_ESC)/pB.o $(AVB_DIR_ESC)/avB.o $(ABE_DIR_ESC)/ab[*].o

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) main.o "$(ASI_DIR)/asi.o" "$(GER_DIR)/gerador.o" "$(PB_DIR)/pB.o" "$(AVB_DIR)/avB.o" "$(ABE_DIR)/ab*.o"

main.o: main.c $(ASI_DIR_ESC)/asi.h $(GER_DIR_ESC)/gerador.h $(PB_DIR_ESC)/pB.h $(AVB_DIR_ESC)/avB.h registro.h
	$(CC) $(CFLAGS) -c main.c -o main.o

$(ASI_DIR_ESC)/asi.o: $(ASI_DIR_ESC)/asi.c $(ASI_DIR_ESC)/asi.h registro.h
	$(CC) $(CFLAGS) -c "$(ASI_DIR)/asi.c" -o "$(ASI_DIR)/asi.o"

$(GER_DIR_ESC)/gerador.o: $(GER_DIR_ESC)/gerador.c $(GER_DIR_ESC)/gerador.h registro.h
	$(CC) $(CFLAGS) -c "$(GER_DIR)/gerador.c" -o "$(GER_DIR)/gerador.o"

$(PB_DIR_ESC)/pB.o: $(PB_DIR_ESC)/pB.c $(PB_DIR_ESC)/pB.h registro.h
	$(CC) $(CFLAGS) -c "$(PB_DIR)/pB.c" -o "$(PB_DIR)/pB.o"

$(AVB_DIR_ESC)/avB.o: $(AVB_DIR_ESC)/avB.c $(AVB_DIR_ESC)/avB.h registro.h
	$(CC) $(CFLAGS) -c "$(AVB_DIR)/avB.c" -o "$(AVB_DIR)/avB.o"

$(ABE_DIR_ESC)/ab[*].o: $(ABE_DIR_ESC)/ab[*].c $(ABE_DIR_ESC)/ab[*].h registro.h
	$(CC) $(CFLAGS) -c "$(ABE_DIR)/ab*.c" -o "$(ABE_DIR)/ab*.o"

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
