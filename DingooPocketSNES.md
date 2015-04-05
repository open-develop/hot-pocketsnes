# Native Dingoo #

This project includes all native Dingoo versions/features with the exception of:

  * v.2.3.1 / v.2.3.2 - PKS support
  * v.2.3.1 - default clock speed of 400Mhz (default out of box speed will always be the original clock speed, users can override this and save their own default). v.2.3.2 appeared to revert the default clock speed
  * v.2.3.1 / v.2.3.2 New cflags for optimization - source code does not appear to be available. Guess are using -O2 instead of -O3 or possibly a profiling session


# Open Dingux #

There is a port for OD, the code for this is available from this project but it is not part of the main line as the port broke all the other platforms. See source code. New features present in OD that would be useful to merge into main line are:

  * support for 50Hz PAL games
  * support for mono (or stereo) sound