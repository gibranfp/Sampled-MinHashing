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
from sets import Set
import numpy as np

def len_clusters(filename,format=0):
    skipped=False

    def line_cluster1(line):
        line=line.split(':',1)[1].strip().split()
        return len(line)

    def line_cluster0(line):
        line=line.strip().split()
        return len(line)-1

    with open(filename) as cf:
        # 1st info
        # size words
        if format==0:
            clusters=map(line_cluster0,cf)
            return clusters[1:]
        # size: words
        elif format==1:
            clusters=map(line_cluster1,cf)
            return clusters



def read_clusters(filename,out=sys.stdout,err=sys.stderr,format=0,min=0):
    skipped=False
    clusters=[]
   

    def line_cluster1(line):
        line=line.split(':',1)[1].strip().split()
        if min>0 and len(line)<min:
            return None
        else:
            data=map(int, line)
            return Counter(data)

    def line_cluster0(line):
        line=line.strip().split()
        if min>0 and len(line)<min:
            return None
        else:
            data=map(int, line)
            data=data[1:]
            return Counter(data)

    with open(filename) as cf:
        # 1st info
        # size words
        if format==0:
            clusters=map(line_cluster0,cf)
            return clusters[1:]
        # size: words
        elif format==1:
            clusters=map(line_cluster1,cf)
            return clusters


def docs2probs(docs):
    docs_=[]
    logdocs_=[]
    for doc in docs:
        t=sum(doc.values())
        docs_.append(dict([(k,1.0*v/t) for k,v in doc.iteritems() ])) 
        logdocs_.append(dict([(k,np.log(1.0*v/t)) for k,v in doc.iteritems() ])) 
    return docs_,logdocs_


def docs2if(docs):
    if_={}
    for idoc,doc in enumerate(docs):
        for w,v in doc.iteritems():
            try:
                if_[w].append(idoc)
            except KeyError:
                if_[w]=[idoc]
    return dict([(k,Set(v)) for k,v in if_.iteritems()])
        

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

