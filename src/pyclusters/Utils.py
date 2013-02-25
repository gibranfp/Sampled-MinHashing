#!/usr/bin/env python
# -*- coding: utf-8
# ----------------------------------------------------------------------
# Utilities to read cluster files
# ----------------------------------------------------------------------
# Ivan Vladimir Meza-Ruiz/ ivanvladimir at turing.iimas.unam.mx
# 2012/IIMAS/UNAM
# ----------------------------------------------------------------------
# authorid.py is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
# -------------------------------------------------------------------------

from collections import Counter
import sys

def read_clusters(filename,out=sys.stdout,err=sys.stderr):
    skipped=False
    clusters=[]
    with open(filename) as cf:
        for n,line in enumerate(cf):
            if skipped:
                data=[int(id) for id in line.strip().split()]
                if not len(data)-1 == data[0]:
                    print >> err, "Error in cluster",n
                    data=data[:1]
                
                clusters.append(Counter(data[1:]))
            else:
                skipped=True
    return clusters


def tf(w):
    bits=w.split(':')
    return int(bits[0]),int(bits[1])

def read_docs(filename,out=sys.stdout,err=sys.stderr):
    docs=[]
    with open(filename) as cf:
        for n,line in enumerate(cf):
            data=line.strip().split()
            size = int(data[0])
            data = dict([tf(w) for w in data[1:]])
            if not len(data) == size:
                print >> err, "Error in document",n
                data={}
            docs.append(Counter(data))
    return docs

