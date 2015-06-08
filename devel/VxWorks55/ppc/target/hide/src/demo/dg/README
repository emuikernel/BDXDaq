This directory contains a simple datagram demonstration program.

To build the VxWorks object for that demo use:

	% make CPU=<CPU_TYPE>
	eg:
	% make CPU=SPARC

This will install the VxWorks objects "dgTest.o" in the
$WIND_BASE/target/lib/objSPARCgnutest/ directory.

To rebuilt the Unix programs dgSender and dgReceiver do:

	% cc -o dgSender dgMain.c dgTest.c
	% cc -o dgReceiver dgMain.c dgTest.c

NOTE: This demo is not available on WIN32 platform.
