This directory contains two very simple VxWorks demonstration programs.

"demo.c" is a simple function that prints its task id and name and its 
startup parameter.

"client.c" is the client side of a simple client/server program that runs
on VxWorks. 

To rebuild "demo.c" and "client.c" simply type:

	% make CPU=<CPU_TYPE>
	eg:
	% make CPU=SPARC

This will install the VxWorks objects "demo.o" and "client.o" in the
$WIND_BASE/target/lib/objSPARCgnutest/ directory.

To rebuilt the server programs that runs on Unix do:

	% cc server.c -o server

NOTE: server.c is not available on WIN32 platform.
