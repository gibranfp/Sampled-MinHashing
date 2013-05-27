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
import scipy.sparse as sparse
from scipy.sparse.sparsetools import csr_scale_rows 

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



def read_clusters2(filename,out=sys.stdout,err=sys.stderr,format=0,min=0):
    skipped=False
    clusters=[]
   
    def line_cluster1(line):
        line=line.split(':',1)[1].strip().split()
        if min>0 and len(line)<min:
            return None
        else:
            data=map(int, line)
            return np.array(data)

    def line_cluster0(line):
        line=line.strip().split()
        if min>0 and len(line)<min:
            return None
        else:
            data=map(int, line)
            data=data[1:]
            return np.array(data)

    with open(filename) as cf:
        # 1st info
        # size words
        if format==0:
            clusters=map(line_cluster0,cf)[1:]
        # size: words
        elif format==1:
            clusters=map(line_cluster1,cf)
    data=np.concatenate([np.array(cls) for cls in clusters])
    rows=np.concatenate([np.array([ii for c in cls ]) for ii,cls in enumerate(clusters)])
    cols=np.concatenate([np.array(cls) for cls in clusters])
    cls = sparse.csr_matrix((data,(rows,cols)))
    return cls


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



def normalize_pairs(pairs):
    """Normalized rows of the pairs matrix so that sum(row) == 1 (or 0 for
    empty rows).

    Note
    ----
        Does the modificiation in-place."""
    factor  = pairs.sum(axis=1)
    nnzeros = np.where(factor > 0)
    factor[nnzeros] = 1.0 / factor[nnzeros]
    factor = np.array(factor)[0]
    if not pairs.format == "csr":
        raise ValueError("csr only")
    csr_scale_rows(
        pairs.shape[0], 
        pairs.shape[1], 
        pairs.indptr, 
        pairs.indices,
        pairs.data, 
        factor)
    factor  = pairs.sum(axis=1)
    return pairs



def docs2probs(docs):
    probs=docs.copy()
    probs=normalize_pairs(probs)
    probs.eliminate_zeros()
    logprobs=sparse.csr_matrix((np.log(probs.data),probs.indices,probs.indptr))
    
    return probs,logprobs
    #    logdocs_.append(dict([(k,np.log(1.0*v/t)) for k,v in doc.iteritems() ])) 

def docs2if(docs):
    size,etc=docs.shape
    ind=docs.indices
    ptr=docs.indptr
    data=[]
    col=[]
    row=[]
    for i in range(size):
        row.append(np.array(ind[ptr[i]:ptr[i+1]]))
        col.append(np.array([i for x in row[-1]]))
        data.append(np.array([i for x in row[-1]]))
    row=np.concatenate(row)
    col=np.concatenate(col)
    data=np.concatenate(data)
    docs = sparse.csr_matrix((data,(row,col)))
    return docs

def tf(w):
    bits=w.split(':')
    return float(bits[0]),float(bits[1])

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

def line_docs2((ii,line)):
    data=line.strip().split()
    size = int(data[0])
    data_ = map(tf,data[1:])
    data = np.array([ y  for x,y in data_])
    row  = np.array([ ii for x   in data_])
    col  = np.array([ x  for x,y in data_])
 
    return data,row,col

def read_docs2(filename,out=sys.stdout,err=sys.stderr):
    with open(filename) as cf:
        docs=map(line_docs2,enumerate(cf))

    data,row,col=zip(*docs)
       
    data=np.concatenate(data)
    row =np.concatenate(row)
    col =np.concatenate(col)
    
    docs = sparse.csr_matrix((data,(row,col)))
    return docs


