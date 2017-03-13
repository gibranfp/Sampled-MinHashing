#!/usr/bin/env python
# -*- coding: utf-8
#
# Ivan V. Meza and
# Gibran Fuentes-Pineda <gibranfp@unam.mx>
# IIMAS, UNAM
# 2016
#
# -------------------------------------------------------------------------
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
# -------------------------------------------------------------------------
"""
Wrapper classes and functions for working with Sampled Min-Hashing
"""

import itertools
import numpy as np
import os
from scipy.sparse import csr_matrix
import smh_api as sa


def rng_init(seed):
    sa.mh_rng_init(seed)

def listdb_load(filename):
    """
    Loads a ListDB array from a given file
    """
    if os.path.isfile(filename):
        ldb = sa.listdb_load_from_file(filename)
        return ListDB(ldb = ldb)
    else:
        print filename, "does not exists"
        return None

def csr_to_listdb(csr):
    """
    Converts a Compressed Sparse Row (CSR) matrix to a ListDB structure
    """
    ldb = sa.listdb_create(csr.shape[0], csr.shape[1])
    coo = csr.tocoo()    
    for i,j,v in itertools.izip(coo.row, coo.col, coo.data):
        ldb.push(int(i), int(j), int(round(v)))

    return ListDB(ldb=ldb)

def ndarray_to_listdb(arr):
    """
    Converts a numpy multidimensional array to a ListDB structure
    """
    ldb = sa.listdb_create(arr.shape[0], arr.shape[1])
    for i,row in enumerate(arr):
        for j,item in enumerate(row):
            if item != 0:
                ldb.push(int(i), int(j), int(round(item)))
    return ListDB(ldb=ldb)

def array_to_listdb(X):
    """
    Converts array (CSR or ndarray) to a dabase of lists
    """
    if type(X) is csr_matrix:
        listdb = csr_to_listdb(X.T)
    elif type(X) is ndarray:
        listdb = ndarray_to_listdb(X.T)
    else:
        raise Exception('Invalid array type')

    return listdb

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

class ListDB:
    """
    Interface class to Sampled Min-Hashing
    """
    def __init__(self, size = 0, dim = 0, ldb = None):
        """
        Initializes class with ListDB structure
        """
        if ldb:
            self.ldb=ldb
        elif size == 0 and dim == 0:
            self.ldb = sa.ListDB()
            sa.listdb_init(self.ldb)
        else:
            self.ldb = sa.listdb_create(size, dim)

    def __del__(self):
        """
        Destroys ListDB structure
        """
        sa.listdb_destroy(self.ldb)

    def push(self, arraylist):
        """
        Appends list to a ListDB structure
        """
        sa.listdb_push(self.ldb, arraylist)

    def pop(self):
        """
        Pops a list from a ListDB structure
        """
        sa.listdb_pop(self.ldb)

    def load(self, filename):
        """
        Loads a ListDB structure from a file
        """
        self.ldb = sa.listdb_load_from_file(filename)

    def save(self, filename):
        """
        Saves ListDB structure to file
        """
        sa.listdb_save_to_file(filename, self.ldb)

    def show(self):
        """
        Prints ListDB structure
        """
        sa.listdb_print(self.ldb)

    def cutoff(self, minsize = None, maxsize = None):
        """
        Removes the largest and smallest lists in the ListDB
        """
        if minsize:
            sa.listdb_delete_smallest(self.ldb, minsize)
        if maxsize:
            sa.listdb_delete_largest(self.ldb, maxsize)

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

    def invert(self):
        """
        Invert database of lists
        """
        ifs = sa.ifindex_make_from_corpus(self.ldb)
        return ListDB(ldb = ifs)

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

class SMHDiscoverer:
    def __init__(self,
                 tuple_size = 3,
                 number_of_tuples = 255,
                 table_size = 2**19,
                 cooccurrence_threshold = None, 
                 min_set_size = 3,
                 cluster_number_of_tuples = 255,
                 cluster_tuple_size = 3,
                 cluster_table_size = 2**20,
                 overlap = 0.7,
                 min_cluster_size = 3):

        self.tuple_size_ = tuple_size

        if cooccurrence_threshold:
            self.cooccurrence_threshold = cooccurrence_threshold
            self.number_of_tuples = log(0.5) / log(1.0 - pow(cooccurrence_threshold, tuple_size))
        else:
            self.number_of_tuples = number_of_tuples

        self.table_size_ = table_size
        self.min_set_size_ = min_set_size
        self.cluster_number_of_tuples_ = cluster_number_of_tuples
        self.cluster_tuple_size_ = cluster_tuple_size
        self.cluster_table_size_ = cluster_table_size
        self.overlap_ = overlap
        self.min_cluster_size_ = min_cluster_size

    def mine(self,
             listdb,
             weights = None,
             expand = None):
        """
        samples nverted file to mine sets of highly co-occurring items
        """
        if not weights and not expand:
            mined = sa.sampledmh_mine(listdb.ldb,
                                      self.tuple_size_,
                                      self.number_of_tuples_,
                                      self.table_size_)
        elif expand and not weights:
            max_freq = sa.mh_get_cumulative_frequency(listdb.ldb, expand.ldb)
            ldb_ = sa.mh_expand_listdb(listdb.ldb, max_freq)
            mined = sa.sampledmh_mine(ldb_,
                                      self.tuple_size_,
                                      self.number_of_tuples_,
                                      self.table_size_)
            sa.listdb_destroy(ldb_)
            
        elif not expand and weights:
            mined = sa.sampledmh_mine_weighted(listdb.ldb,
                                               self.tuple_size_,
                                               self.number_of_tuples_,
                                               self.table_size_,
                                               weights.weights)
        elif expand and weights:
            max_freq = sa.mh_get_cumulative_frequency(listdb.ldb, expand.ldb)
            ldb_ = sa.mh_expand_listdb(listdb.ldb, max_freq)
            weights_ = sa.mh_expand_weights(expand.ldb.size, max_freq, weights.weights)
            mined = sa.sampledmh_mine_weighted(ldb_,
                                               self.tuple_size_,
                                               self.number_of_tuples,
                                               self.table_size_,
                                               weights_)
            sa.listdb_destroy(ldb_)

        sa.listdb_delete_smallest(mined, self.min_set_size_)

        return ListDB(ldb = mined)
        
    def cluster(self):
        """
        Clusters a database of mined lists using agglomerative clustering based on Min-Hashing
        """
        models = sa.mhlink_cluster(self.ldb,
                                   self.cluster_tuple_size,
                                   self.cluster_number__of_tuples,
                                   self.cluster_table_size,
                                   sa.list_overlap,
                                   self.overlap,
                                   self.min_cluster_size)

        sa.listdb_apply_to_all(models, sa.list_sort_by_frequency_back)
                
        return ListDB(ldb = models)

    def fit(self,
            listdb,
            weights = None,
            expand = None):
        """
        Discovers patterns from a database of lists
        """
        mined = self.mine(listdb, weights = weights, expand = expand)
        models = sa.mhlink_cluster(mined.ldb,
                                   self.cluster_tuple_size_,
                                   self.number_of_tuples_,
                                   self.cluster_table_size_,
                                   sa.list_overlap, self.overlap_,
                                   self.min_cluster_size_)
        sa.listdb_apply_to_all(models, sa.list_sort_by_frequency_back)

        mined.destroy()

        return ListDB(ldb = models)
        
