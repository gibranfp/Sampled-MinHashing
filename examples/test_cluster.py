import sys
import smh
import numpy as np

data = smh.smh.smh_load(sys.argv[2])
mined = data.mine(4, 250)
if sys.argv[1] == "mhlink":
    clusters = mined.cluster_mhlink()
elif sys.argv[1] == "mbkm":
    from sklearn.cluster import MiniBatchKMeans
    mbkm = MiniBatchKMeans(n_clusters=3, batch_size=10)
    clusters = mined.cluster_sklearn(mbkm)
elif sys.argv[1] == "kmeans":
    from sklearn.cluster import KMeans
    km = KMeans(n_clusters=3)
    clusters = mined.cluster_sklearn(km)
elif sys.argv[1] == "spectral":
    from sklearn.cluster import SpectralClustering
    spc = SpectralClustering(n_clusters=3, affinity='cosine')
    clusters = mined.cluster_sklearn(spc)

clusters.show()
