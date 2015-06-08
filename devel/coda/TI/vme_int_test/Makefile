
CFLAGS = -I../libvme
LIBS = -L../libvme -lvme


vme_int_test: vme_int_test.cc
	g++ $(CFLAGS) $< -o $@ $(LIBS)

clean:
	rm -f vme_int_test
