CC = gcc
CFLAGS ?= -O2 -Wall -Wextra
CPPFLAGS += -Iinclude
ifeq ($(OS),Windows_NT)
TARGET = rdpd.exe
EXEEXT = .exe
LDLIBS += -lws2_32
else
TARGET = rdpd
EXEEXT =
endif
SOURCES = src/main.c src/server.c src/protocol.c src/mcs.c src/mcs_response.c src/gcc.c src/mcs_domain.c src/mcs_data.c src/client_info.c src/activation.c src/finalization.c src/framebuffer.c src/desktop.c src/gfx.c src/wm.c src/widget.c src/icon.c
HEADERS = include/widget.h include/icon.h include/desktop.h include/wm.h include/gfx.h include/framebuffer.h include/activation.h include/finalization.h include/rdpd.h include/protocol.h include/mcs.h include/mcs_response.h include/gcc.h include/mcs_domain.h include/mcs_data.h include/client_info.h
.PHONY: all clean test
all: $(TARGET)
$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) -o $@ $(LDLIBS)
clean:
	$(RM) rdpd rdpd.exe test_gfx test_gfx.exe test_wm test_wm.exe test_composition test_composition.exe test_controls test_controls.exe test_widgets test_widgets.exe
test: all
	python tests/smoke.py
	python tests/protocol.py
	python tests/test_rdpclient.py
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_gfx.c src/gfx.c -o test_gfx$(EXEEXT)
	./test_gfx$(EXEEXT)
	$(RM) test_gfx$(EXEEXT)
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_wm.c src/wm.c src/gfx.c -o test_wm$(EXEEXT)
	./test_wm$(EXEEXT)
	$(RM) test_wm$(EXEEXT)
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_composition.c src/wm.c src/gfx.c -o test_composition$(EXEEXT)
	./test_composition$(EXEEXT)
	$(RM) test_composition$(EXEEXT)
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_controls.c src/wm.c src/gfx.c -o test_controls$(EXEEXT)
	./test_controls$(EXEEXT)
	$(RM) test_controls$(EXEEXT)
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_widgets.c src/widget.c src/icon.c src/gfx.c -o test_widgets$(EXEEXT)
	./test_widgets$(EXEEXT)
	$(RM) test_widgets$(EXEEXT)
