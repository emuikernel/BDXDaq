all:
	(cd server; make OSTYPE=$(OSTYPE))
	(cd client; make OSTYPE=$(OSTYPE))
	(cd relay; make OSTYPE=$(OSTYPE))
	(cd tools; make OSTYPE=$(OSTYPE))

clean:
	(cd server; make clean)
	(cd client; make clean)
	(cd relay; make clean)
	(cd tools; make clean)
