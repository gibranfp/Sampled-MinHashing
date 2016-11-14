#!/usr/bin/env python
# -*- coding: utf-8
# ----------------------------------------------------------------------
# Sampled MinHash python interface
# ----------------------------------------------------------------------
# Ivan V. Meza
# 2015/IIMAS, México
# ----------------------------------------------------------------------
"""
Wrapper classes and function for working with Sampled Min-Hashing
"""

import itertools
import numpy as np
from scipy.sparse import csr_matrix
import smh_api as sa

def smh_init(seed):
    sa.mh_rng_init(seed)

def smh_load(filename):
    """
    Loads a ListDB array from a given file
    """
    ldb = sa.listdb_load_from_file(filename)
    return SMH(ldb=ldb)

def csr_to_listdb(csr):
    """
    Converts a Compressed Sparse Row (CSR) matrix to a ListDB structure
    """
    if issubclass(csr.dtype.type,np.integer):
        factor=1
    else:
        factor= 100000000
 
    ldb = sa.listdb_create(csr.shape[0], csr.shape[1])
    coo = csr.tocoo()    
    for i,j,v in itertools.izip(coo.row, coo.col, coo.data):
        ldb.push(int(i), int(j), int(round(v * factor)))

    return SMH(ldb=ldb)

def ndarray_to_listdb(arr):
    """
    Converts a numpy multidimensional array to a ListDB structure
    """
    if issubclass(arr.dtype.type,np.integer):
        factor=1
    else:
        factor= 100000000
    ldb = sa.listdb_create(arr.shape[0], arr.shape[1])
    for i,row in enumerate(arr):
        for j,item in enumerate(row):
            if item != 0:
                ldb.push(int(i), int(j), int(round(item * factor)))
    return SMH(ldb=ldb)

def centers_from_labels(data, labels):
    """
    Computes the center points of a set of clusters from a list of labels
    """
    number_of_classes = np.max(labels) + 1
    centers = np.zeros((number_of_classes, data.shape[1]))
    for i in range(number_of_classes):
        centers[i] = data[np.where(labels==i)].mean(axis=0)

    return ndarray_to_listdb(centers)

class Weights:
    """
    Interface class for handling item weights
    """
    def __init__(self,path):
        self.weights=sa.weights_load_from_file(path)

    def save(self,path):
        sa.weights_save_to_file(path,self.weights)

class SMH:
    """
    Interface class to Sampled Min-Hashing
    """
    def __init__(self,size=0,dim=0,ldb=None):
        """
        Initializes class with ListDB structure
        """
        self._inverted=False
        self._original=None
        if ldb:
            self.ldb=ldb
        else:
            self.ldb = sa.listdb_create(size,dim)

    #def __getitem__(self, index):
    #   """
    #   Initializes class with ListDB structure
    #   """
    #   row, col = index
    #   return self.ldb[row][col]
 
   
    def push(self,topic):
        """
        Appends list to a ListDB structure
        """
        sa.listdb_push(self.ldb,topic)

    def save(self,filename):
        """
        Saves ListDB structure to file
        """
        sa.listdb_save_to_file(filename,self.ldb)

    def show(self):
        """
        Prints ListDB structure
        """
        sa.listdb_print(self.ldb)

    def mine(self,tuple_size,num_tuples,weights=None,expand=False,table_size=2**19):
        """
        Mines co-occurring items from a database of lists using Sampled Min-Hashing
        """
        if not weights and not expand5H:
            ldb=sa.sampledmh_mine(self.ldb,tuple_size,num_tuples,table_size)
        elif expand and not weights:
            max_freq=sa.mh_get_cumulative_frequency(self.ldb,expand.ldb)
            ldb_=sa.mh_expand_listdb(self.ldb,max_freq)
            ldb=sa.sampledmh_mine(ldb_,tuple_size,num_tuples,table_size)
        elif not expand and weights:
            ldb=sa.sampledmh_mine_weighted(self.ldb,tuple_size,num_tuples,table_size,weights.weights)
        elif expand and weights:
            max_freq=sa.mh_get_cumulative_frequency(self.ldb,expand.ldb)
            ldb_=sa.mh_expand_listdb(self.ldb, max_freq)
            weights_=sa.mh_expand_weights(expand.ldb.size, max_freq, weights.weights)
            ldb=sa.sampledmh_mine_weighted(ldb_,tuple_size,num_tuples,table_size,weights_)
            
        return SMH(ldb=ldb)

    def prune(self,ifindex,min_size=3,min_hits=3,overlap=0.7,cooc_th=0.7):
        """
        Prunes a database of mined lists based on the inverted file
        """
        sa.sampledmh_prune(ifindex.ldb, self.ldb, min_size, min_hits, overlap, cooc_th)

    def cluster_mhlink(self, num_tuples=255, tuple_size=3, table_size=2**20, thres=0.7,
                       min_cluster_size=3):
        """
        Clusters a database of mined lists using agglomerative clustering based on Min-Hashing
        """
        models=sa.mhlink_cluster(self.ldb, tuple_size, num_tuples, table_size,
                                 sa.list_overlap, thres, min_cluster_size)

        sa.listdb_apply_to_all(models, sa.list_sort_by_frequency_back)
                
        return SMH(ldb=models)

    def cluster_sklearn(self, algorithm):
        """
        Clusters a database of mined lists using the clustering methods available in scikit-learn
        """
        csr = self.tocsr()
        algorithm.fit(csr.toarray())
        if hasattr(algorithm, 'cluster_centers_'):
            ldb = ndarray_to_listdb(algorithm.cluster_centers_)
        else:
            ldb = centers_from_labels(csr, algorithm.labels_)

        sa.listdb_apply_to_all(ldb.ldb, sa.list_sort_by_frequency_back)
        
        return ldb

    def invert(self):
        ldb=sa.sampledmh_mine(self.ldb)
        ldb._inverted=True
        ldb._original=self
        return SMH(ldb=ldb)

    def cutoff(self,min=5,max=None):
        """
        Removes the largest and smallest lists in the ListDB
        """
        if min:
            sa.listdb_delete_smallest(self.ldb,min)
        if max:
            sa.listdb_delete_largest(self.ldb,max)

    def size(self):
        """
        Returns the size of the ListDB structure
        """
        return self.ldb.size

    def dim(self):
        """
        Returns the dimensionality of the ListDB structure
        """
        return self.ldb.dim

    def destroy(self):
        """
        Destroys the ListDB structure
        """
        sa.listdb_destroy(self.ldb)

    def tocsr(self):
        """
        Returns the ListDB structure as a Compressed Sparse Row (CSR) matrix
        """
        indptr = [0]
        indices = []
        data = []
        for l in self.ldb:
            for i in l:
                indices.append(i.item)
                data.append(i.freq)
            indptr.append(len(indices))

        return csr_matrix((data, indices, indptr), dtype=np.uint32)

    def toarray(self):
        """
        Converts a listdb structure to a numpy multidimensional array
        """
        arr = np.zeros((self.size(), self.dim()))
        for i, l in enumerate(self.ldb):
            for j in l:
                arr[i, j.item] = j.freq

        return arr

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
