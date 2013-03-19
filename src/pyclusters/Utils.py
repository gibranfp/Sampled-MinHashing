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

def line_cluster1(line):
    line=line.split(':',1)[1]
    data=map(int, line.strip().split())
    data=data[1:]
    return Counter(data)


def line_cluster0(line):
    data=map(int, line.strip().split())
    data=data[1:]
    return Counter(data)


def read_clusters(filename,out=sys.stdout,err=sys.stderr,format=0):
    skipped=False
    clusters=[]
    
    with open(filename) as cf:
        # 1st info
        # size words
        if format==0:
            clusters=map(line_cluster0,cf)
        # size: words
        elif format==1:
            clusters=map(line_cluster1,cf)

    return clusters[1:]


def tf(w):
    bits=w.split(':')
    return int(bits[0]),int(bits[1])

def line_docs(line):
    data=line.strip().split()
    size = int(data[0])
    doc = Counter(dict(map(tf,data[1:])))
    if not len(doc) == size:
        doc= Counter([])
    return doc

def read_docs(filename,out=sys.stdout,err=sys.stderr):
    docs=[]
    with open(filename) as cf:
        docs=map(line_docs,cf)
    return docs

