CC = gcc
CFLAGS ?= -O2 -Wall -Wextra
CPPFLAGS += -Iinclude
ifeq ($(OS),Windows_NT)
TARGET = rdpd.exe
LDLIBS += -lws2_32
else
TARGET = rdpd
endif
.PHONY: all clean test
all: $(TARGET)
$(TARGET): src/main.c src/server.c include/rdpd.h
	$(CC) $(CPPFLAGS) $(CFLAGS) src/main.c src/server.c -o $@ $(LDLIBS)
clean:
	$(RM) rdpd rdpd.exe
test: all
	python tests/smoke.py
