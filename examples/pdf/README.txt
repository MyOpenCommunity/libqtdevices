Building has been tested with the following software versions:

expat 2.0.1
freetype 2.4.3
fontconfig 2.8.0
libpng 1.2.29
poppler 0.14.4
zlib 1.2.5

Any later version should work, the only caveat is that the libpng
major/minor versions (eg. 1.2) must match the ones of the libpng that was
used to build Qt.

The directory examples/pdf/poppler/scripts contains some helper
scripts to automate cross-compilation of Poppler/Qt and its
dependencies; the scripts assume that:

- archive files (in .tar.gz format) have been downloaded to
  examples/pdf/poppler/sources
- the version numbers at the top op examples/pdf/poppler/scripts/Makefile
  match the downloaded source archives
- the ARM cross-compiler is in the PATH
- Qt embedded is installed in /lib/qt4lib (ARM) and /lib/qt4lib.x86 (x86)
  (note: this must be the same Qt used to build Poppler/Qt, hence it's
   not possible to use distribution-provided Poppler)

If that's true, running:

$ make rebuild
$ make rebuild-x86
$ maek clean

should build Poppler (x86) and Poppler and all its dependencies (ARM).

Installation prefix is /usr/bticino-exmaples (hence the libraries are
installed in /usr/bticino-exmaples/lib, which must be added to
LD_LIBRARY_PATH).  It's also possible to run the example against an
uninstalled library by setting LD_LIBRARY_PATH.

For x86, the development packages for libpng and fontconfig must be
installed for the build to complete.

When using the provided scripts just changing BUILD_PDF to "yes" in
common.pri should suffice for the example to build.  If You build
Poppler manually, change INCLUDES and LIBS accordingly.

Use this configuration item:

      <item>
        <square>0</square>
        <id>55556</id>
        <cid>16002</cid>
        <itemID>unique id</itemID>
        <descr>insert description</descr>
      </item>

to display the link in the Multimedia section.
