
CFLAGS = -I../libvme
LIBS = -L../libvme -lvme

ti_pulser: ti_pulser.cc
	g++  $(CFLAGS) $< -o $@ $(LIBS)


clean:
	rm -f ti_pulser
 
