# DeepCool CH560 Digital display daemon

This daemon displays CPU/GPU utilization and temperature 
using case's builtin display

Currently, DeepCool provides corresponding software only for windows, and
it is implemented using Electron (o_0)
So this daemon intended to implement similar functionality for GNU/Linux 
operating systems. Probably also Mac and Windows compatibility will be added later. 

# Usage

# Development

## Libraries
* [hidapi-hidraw](https://github.com/libusb/hidapi)
* [lm-sensors](https://github.com/lm-sensors/lm-sensors)

# Contribute

Any help is appreciated

# Credits

Daemon based on skeleton provided by https://github.com/jirihnidek/daemon

## Build Requirements

C compiler (originally clang is used, but should be compatible with GCC as well)

## Build

```make```

To build release version 
```make RELEASE=1```
