all: ch560-daemon

TARGET=ch560-daemon
CC=clang
CXX=clang
COBJS=
CPPOBJS=src/syslog.o src/config.o src/daemon.o src/usage.o src/demonize.o src/display.o src/util.o src/sensors.o src/runtime_params.o
OBJS=$(COBJS) $(CPPOBJS)

ifeq ($(RELEASE), 1)
	CFLAGS=-O3 -Os -Wall -c `pkg-config hidapi-hidraw --cflags`
else
	CFLAGS=-O0 -DDEBUG -Wall -c -ggdb `pkg-config hidapi-hidraw --cflags`
endif
LDFLAGS=`pkg-config hidapi-hidraw --libs`
LIBS=-lsensors -lstdc++

ch560-daemon: pch $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) $(LIBS) -o $(TARGET)

$(COBJS): %.o: %.c
	$(CC) $(CFLAGS) $< -o $@

$(CPPOBJS): %.o: %.cpp
	$(CXX) -include-pch src/common.pch $(CFLAGS) $< -o $@

pch:
	$(CXX) -Xclang -emit-pch -o src/common.pch src/common.hpp

clean:
	rm -f $(OBJS) $(TARGET)
	rm -f src/common.pch
	rm -f ch560-daemon

.PHONY: clean