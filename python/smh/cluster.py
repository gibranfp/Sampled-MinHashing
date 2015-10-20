#!/usr/bin/env python
# -*- coding: utf-8
#
# Gibran Fuentes Pineda <gibranfp@unam.mx>
# IIMAS, UNAM
# 2015
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
Functions to cluster the output of Sampled Min-Hashing
"""
from sklearn.cluster import KMeans
from sklearn.cluster import MiniBatchKMeans
from sklearn.cluster import DBSCAN
from sklearn.cluster import SpectralClustering
from sklearn.cluster import AgglomerativeClustering
from sklearn.preprocessing import Normalizer


def cluster_kmeans(data, number_of_clusters = 100):
    km = KMeans(n_clusters=number_of_clusters, njobs=-1)
    km.fit(data)
    return km.cluster_centers_
    
def cluster_mbkm(data, number_of_clusters = 100):
    mbkm = MiniBatchKMeans(n_clusters=number_of_clusters, batch_size=1000)
    mbkm.fit(data)
    
    return mbkm.cluster_centers_

def cluster_spectral(data, number_of_clusters = 100, aff='cosine'): 
    sc = SpectralClustering(eps=mindist, min_samples=nsamples)
    sc.fit(data)
    return

def cluster_dbscan(data, mindist=0.5, nsamples=10):
    ds = DBSCAN(eps=mindist, min_samples=nsamples)
    ds.fit(data)
    
    return ds.components_
