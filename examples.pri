######################################################################
# Manual mantained!
#
# This file contains all the examples for the BTouch interface.

# Screensaver
SOURCES += examples/logoscreensaver/logoscreensaver.cpp
HEADERS += examples/logoscreensaver/logoscreensaver.h

# Banner
SOURCES += examples/tcpbanner/banner/tcpdimmer.cpp \
	examples/tcpbanner/banner/tcpstatebanner.cpp \
	examples/tcpbanner/banner/tcpbannerpage.cpp
HEADERS += examples/tcpbanner/banner/tcpdimmer.h \
	examples/tcpbanner/banner/tcpstatebanner.h \
	examples/tcpbanner/banner/tcpbannerpage.h

# Pdf (see examples/pdf/README.txt)
BUILD_PDF = no

contains(BUILD_PDF, no) {
	message(Skipping PDF display example.)
} else {
	isEmpty(TEST_ARCH) {
		POPPLER_DIR=install-x86/usr/bticino-examples-x86
	} else {
		POPPLER_DIR=install/usr/bticino-examples
	}

	DEFINES += PDF_EXAMPLE
	INCLUDEPATH += ../examples/pdf/poppler/$${POPPLER_DIR}/include
	SOURCES += examples/pdf/pages/pdfpage.cpp
	HEADERS += examples/pdf/pages/pdfpage.h
	LIBS += -L../examples/pdf/poppler/$${POPPLER_DIR}/lib -lpoppler-qt4 -lpoppler -lfontconfig -lfreetype -lexpat -lpng -lz
}
