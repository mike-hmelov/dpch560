all: ch560-daemon

TARGET=ch560-daemon
CC=clang
CXX=clang
COBJS=src/daemon.o src/config.o src/usage.o src/demonize.o
CPPOBJS=
OBJS=$(COBJS) $(CPPOBJS)

ifeq ($(RELEASE), 1)
	CFLAGS=-O3 -Os -Wall -c `pkg-config hidapi-hidraw --cflags`
else
	CFLAGS=-O0 -DDEBUG -Wall -c -ggdb `pkg-config hidapi-hidraw --cflags`
endif
LDFLAGS=`pkg-config hidapi-hidraw --libs`
LIBS=

ch560-daemon: $(OBJS)
	$(CXX) $^ $(LDFLAGS) $(LIBS) -o $(TARGET)

$(COBJS): %.o: %.c
	$(CC) $(CFLAGS) $< -o $@

$(CPPOBJS): %.o: %.cpp
	$(CXX) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: clean