#!/usr/bin/env python
#-*- coding: utf-8 -*-

import os

def fixQhp(filename):
    f = open(filename, 'r')
    out_lines = []
    line = f.readline()

    while line.strip() != "<toc>":
       out_lines.append(line)
       line = f.readline()

    out_lines.append(line)

    out_lines.append("      <section title=\"BTouch\" ref=\"index.html\">\n")
    out_lines.append("      </section>\n")

    while line.strip() != "</toc>":
       line = f.readline()

    out_lines.append(line)
    line = f.readline()

    while line.strip() != "<files>":
       out_lines.append(line)
       line = f.readline()

    out_lines.append(line)
    out_lines.append("      <file>myhome.jpg</file>\n")
    line = f.readline()

    while line.strip() != "":
       out_lines.append(line)
       line = f.readline()


    f.close()
    f = open(filename, 'w+')
    f.write(''.join(out_lines))
    f.close()

if __name__ == '__main__':
    os.system('doxygen btouch.cfg')
    fixQhp('../doc/html/index.qhp')
    os.system('cp myhome.jpg ../doc/html/')
    os.system('qhelpgenerator ../doc/html/index.qhp -o ../doc/btouch.qch')
    os.system('rm ../doc/html/index.qhp')

