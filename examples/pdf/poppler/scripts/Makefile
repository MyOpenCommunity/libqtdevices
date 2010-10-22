#!/usr/bin/make -f

# put the source archives under ../sources
# tested with the versions below; it's important that the
# libpng version (1.2) matches the one used to build Qt
ZLIB=1.2.5
EXPAT=2.0.1
FONTCONFIG=2.8.0
FREETYPE=2.4.3
LIBPNG=1.2.29
POPPLER=0.14.4

SOURCES=$(PWD)/../sources
SCRIPTS=$(PWD)

help:
	@echo
	@echo "Usage: make [clean|rebuild|rebuild-x86]"
	@echo

clean:
	rm -rf ../work

zlib:
	cd ../work/zlib-$(ZLIB) && $(SCRIPTS)/build-zlib.sh

expat:
	cd ../work/expat-$(EXPAT) && $(SCRIPTS)/build-expat.sh

fontconfig:
	cd ../work/fontconfig-$(FONTCONFIG) && $(SCRIPTS)/build-fontconfig.sh

freetype:
	cd ../work/freetype-$(FREETYPE) && $(SCRIPTS)/build-freetype.sh

libpng:
	cd ../work/libpng-$(LIBPNG) && $(SCRIPTS)/build-libpng.sh

poppler:
	cd ../work/poppler-$(POPPLER) && $(SCRIPTS)/build-poppler.sh

poppler-x86:
	cd ../work/poppler-$(POPPLER) && $(SCRIPTS)/build-poppler-x86.sh

unpack:
	mkdir -p ../work
	( cd ../work && tar xzf $(SOURCES)/zlib-$(ZLIB).tar.gz )
	( cd ../work && tar xzf $(SOURCES)/expat-$(EXPAT).tar.gz )
	( cd ../work && tar xzf $(SOURCES)/fontconfig-$(FONTCONFIG).tar.gz )
	( cd ../work && tar xzf $(SOURCES)/freetype-$(FREETYPE).tar.gz )
	( cd ../work && tar xzf $(SOURCES)/libpng-$(LIBPNG).tar.gz )
	( cd ../work && tar xzf $(SOURCES)/poppler-$(POPPLER).tar.gz )

unpack-x86:
	mkdir -p ../work
	( cd ../work && tar xzf $(SOURCES)/poppler-$(POPPLER).tar.gz )

build: zlib libpng expat freetype fontconfig poppler

build-x86: poppler-x86

rebuild: clean unpack build
rebuild-x86: clean unpack-x86 build-x86
