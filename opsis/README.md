# Opsis firmware

This is a port of MicroPython to Opsis board from the TimVideos HDMI2USB
project.

Opsis is a FPGA based system that contains a softcore. Initial support is for
the or1k softcore.

## Building the firmware

```
cd opsis
export PATH=/path/to/or1k/bin
make
```

This creates a file in `opsis/build/firmware.bin` that can be loaded onto the
board. Note that the file is statically linked to the base of `firmware_ram`.

## Run in simulator

To run in the veriliator simulator:
1. Copy `firmware.bin` to `opsis-soc/firmware/firmware.bin` 
2. Remove the `make clean all` line from the opsis_sim target in the Makefile 
3. `make opsis_sim` to run the simulator
