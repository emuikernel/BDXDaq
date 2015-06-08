#
# Makefile for images/
#

all:
	@echo "Nothing to compile"


install:	$(CODA)/common/images/bitmap \
			$(CODA)/common/images/bitmaps \
			$(CODA)/common/images/gif

$(CODA)/common/images/bitmap: ./bitmap/*
	@echo "Install bitmap"
	-mkdir -p $(CODA)/common/images
	@cp -rf ./bitmap $(CODA)/common/images

$(CODA)/common/images/bitmaps: ./bitmaps/*
	@echo "Install bitmaps"
	-mkdir -p $(CODA)/common/images
	@cp -rf ./bitmaps $(CODA)/common/images

$(CODA)/common/images/gif: ./gif/*
	@echo "Install gif"
	-mkdir -p $(CODA)/common/images
	@cp -rf ./gif $(CODA)/common/images

clean:
	@echo "Nothing to clean"
