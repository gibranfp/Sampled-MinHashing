#!/usr/bin/env python
# -*- coding: utf-8
# ----------------------------------------------------------------------
# Sampled MinHash python interface
# ----------------------------------------------------------------------
# Ivan V. Meza
# 2015/IIMAS, México
# ----------------------------------------------------------------------

import smh_api as sa
import numpy as np
import sys
import pylab as pl

def smh_load(filename):
    ldb = sa.listdb_load_from_file(filename)
    return SMH(ldb=ldb)

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

    def size(self):
        return self.ldb.size

    def dim(self):
        return self.ldb.dim


# MAIN program
if __name__ == "__main__":
    import argparse
    p = argparse.ArgumentParser("Mines")
    p.add_argument("-r","--tuple_size",default=4,
        action="store", dest='r',help="Size of the tupple")
    p.add_argument("-l","--number_tuples",default=10,
        action="store", dest='l',help="Number of the tupple")
    p.add_argument("file",default=None,
        action="store", help="File for mining")

    opts = p.parse_args()

    s=smh_load(opts.file)
    print "Size of loaded file:",s.size()
    m=s.mine(opts.r,opts.l)
    print "Size of original mined topics:",m.size()
    m.cutoff(min=1000)
    print "Size of cutted off mined topics:",m.size()

    for l in m.ldb:
        avgimgs=np.zeros(784)
        n=0
        for i in l:
            img=np.zeros(784)
            img[[pix.item for pix in s.ldb[i.item]]]=1
            avgimgs+=img
            n+=1
        avgimgs=avgimgs/60000
        avgimgs=avgimgs.reshape((28,28))
        pl.figure()
        pl.imshow(avgimgs)
        pl.show()
    #m.show()



