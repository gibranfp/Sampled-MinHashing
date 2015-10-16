#!/usr/bin/env python
# -*- coding: utf-8
# ----------------------------------------------------------------------
# Sampled MinHash python interface
# ----------------------------------------------------------------------
# Ivan V. Meza
# 2015/IIMAS, México
# ----------------------------------------------------------------------

import itertools
import numpy as np
from scipy.sparse import csr_matrix
import smh_api as sa

def smh_load(filename):
    ldb = sa.listdb_load_from_file(filename)
    return SMH(ldb=ldb)

def csr_to_listdb(csr):
    ldb = sa.listdb_create(csr.shape[0], csr.shape[1])
    coo = csr.tocoo()    
    for i,j,v in itertools.izip(coo.row, coo.col, coo.data):
        ldb.push(int(i), int(j), int(round(v * 100000000)))

    return SMH(ldb=ldb)

def ndarray_to_listdb(arr):
    ldb = sa.listdb_create(arr.shape[0], arr.shape[1])
    for i,row in enumerate(arr):
        for j,item in enumerate(row):
            if item != 0:
                ldb.push(int(i), int(j), int(round(item * 100000000)))
    return SMH(ldb=ldb)

def centers_from_labels(data, labels):
    number_of_classes = np.max(labels) + 1
    centers = np.zeros((number_of_classes, data.shape[1]))
    for i in range(number_of_classes):
        centers[i] = data[np.where(labels==i)].mean(axis=0)

    return ndarray_to_listdb(centers)

class SMH:
    def __init__(self,size=0,dim=0,ldb=None):
        self._inverted=False
        self._original=None
        if ldb:
            self.ldb=ldb
        else:
            self.ldb = sa.listdb_init(size,dim)

    def save(self,filename):
        sa.listdb_save_to_file(filename,self.ldb)

    def show(self):
        sa.listdb_print(self.ldb)

    def mine(self,tuple_size,num_tuples,table_size=2**19):
        ldb=sa.sampledmh_mine(self.ldb,tuple_size,num_tuples,table_size)
        return SMH(ldb=ldb)

    def cluster_mhlink(self, num_tuples=3, tuple_size=255, table_size=2**19, thres=0.7):
        ldb=sa.mhlink_cluster(self.ldb, table_size, num_tuples, tuple_size, sa.list_overlap, 0.7)
        return SMH(ldb=ldb)

    def cluster_sklearn(self, algorithm):
        csr = self.tocsr()
        algorithm.fit(csr)
        return centers_from_labels(csr, algorithm.labels_)

    def invert(self):
        ldb=sa.sampledmh_mine(self.ldb)
        ldb._inverted=True
        ldb._original=self
        return SMH(ldb=ldb)

    def cutoff(self,min=5,max=None):
        if min:
            sa.listdb_delete_smallest(self.ldb,min)
        if max:
            sa.listdb_delete_largest(self.ldb,max)

        ldb= sa.sampledmh_mine(self.ldb,tuple_size,num_tuples,table_size)
        return SMH(ldb=ldb)

    def size(self):
        return self.ldb.size

    def dim(self):
        return self.ldb.dim

    def tocsr(self):
        number_of_items = 0
        for l in self.ldb:
            number_of_items += l.size

        rows = []
        cols = []
        arr = []
        for r, l in enumerate(self.ldb):
            for i in l:
                rows.append(r)
                cols.append(i.item)
                arr.append(float(i.freq))
                
        return csr_matrix((arr, (rows, cols)))


# MAIN program
if __name__ == "__main__":
    import argparse
    p = argparse.ArgumentParser("Mines")
    p.add_argument("-r","--tuple_size",default=4,type=int,
        action="store", dest='r',help="Size of the tupple")
    p.add_argument("-l","--number_tuples",default=10,type=int,
        action="store", dest='l',help="Number of the tupple")
    p.add_argument("--output",default=None,type=str,
        action="store", dest='output',help="Filename to save mined model")
    p.add_argument("--min",default=1000,type=int,
        action="store", dest='min',help="Minimum number of items")
    p.add_argument("file",default=None,
        action="store", help="File to mine")

    opts = p.parse_args()

    s=smh_load(opts.file)
    print "Size of loaded file:",s.size()
    m=s.mine(opts.r,opts.l)
    print "Size of original mined topics:",m.size()
    m.cutoff(min=opts.min)
    print "Size of cutted off mined topics:",m.size()
    if opts.output:
        print "Saving resulting model to",opts.output
        m.save(opts.output)
