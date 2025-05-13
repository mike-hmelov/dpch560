all: ch560-daemon

TARGET=ch560-daemon
CXX=g++
OBJS=src/config.o \
	src/daemon.o \
	src/demonize.o \
	src/display.o \
	src/runtime_params.o \
	src/sensors.o \
	src/syslog.o \
	src/usage.o \
	src/util.o

ifeq ($(RELEASE), 1)
	CFLAGS=-O3 -Os -Wall -c `pkg-config hidapi-hidraw --cflags`
else
	CFLAGS=-O0 -DDEBUG -Wall -c -ggdb `pkg-config hidapi-hidraw --cflags`
endif

LIBS=-lsensors -lhidapi-hidraw

ch560-daemon: pch $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

$(OBJS): %.o: %.cpp
	$(CXX) $(CFLAGS) $< -o $@

pch:
	$(CXX) -o src/common.hpp.gch -c src/common.hpp

clean:
	rm -f $(OBJS) $(TARGET)
	rm -f src/common.hpp.gch
	rm -f ch560-daemon

.PHONY: clean