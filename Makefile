all: ch560-daemon

TARGET=ch560-daemon
CC=clang
CXX=clang
COBJS=src/daemon.o src/config.o src/usage.o
CPPOBJS=
OBJS=$(COBJS) $(CPPOBJS)
CFLAGS=-Wall -c -ggdb
LDFLAGS=
#`fox-config --cflags` `pkg-config libusb-1.0 --cflags`
LIBS=
#-ludev -lrt -lpthread `fox-config --libs` `pkg-config libusb-1.0 --libs`


ch560-daemon: $(OBJS)
	$(CXX) $^ $(LDFLAGS) $(LIBS) -o $(TARGET)

$(COBJS): %.o: %.c
	$(CC) $(CFLAGS) $< -o $@

$(CPPOBJS): %.o: %.cpp
	$(CXX) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: clean