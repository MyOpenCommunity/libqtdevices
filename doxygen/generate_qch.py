#!/usr/bin/env python
#-*- coding: utf-8 -*-

import os

def replaceToc(filename):
    f = open(filename, 'r')
    out_lines = []
    line = f.readline()

    while line.strip() != "<toc>":
       out_lines.append(line)
       line = f.readline()

    out_lines.append("    <toc>\n")
    out_lines.append("      <section title=\"BTouch\" ref=\"index.html\">\n")
    out_lines.append("      </section>\n")
    out_lines.append("    </toc>\n")

    while line.strip() != "</toc>":
       line = f.readline()
   
    line = f.readline()
    while line.strip() != "":
       out_lines.append(line)
       line = f.readline()

    f.close()
    f = open(filename, 'w+')
    f.write(''.join(out_lines))
    f.close()

if __name__ == '__main__':
    replaceToc('../doc/html/index.qhp')
    os.system('qhelpgenerator ../doc/html/index.qhp -o ../doc/btouch.qch')
