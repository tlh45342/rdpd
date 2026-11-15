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
$(TARGET): src/main.c src/server.c src/protocol.c src/mcs.c src/mcs_response.c src/gcc.c src/mcs_domain.c include/rdpd.h include/protocol.h include/mcs.h include/mcs_response.h include/gcc.h include/mcs_domain.h
	$(CC) $(CPPFLAGS) $(CFLAGS) src/main.c src/server.c src/protocol.c src/mcs.c src/mcs_response.c src/gcc.c src/mcs_domain.c -o $@ $(LDLIBS)
clean:
	$(RM) rdpd rdpd.exe
test: all
	python tests/smoke.py
	python tests/protocol.py
