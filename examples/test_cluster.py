import sys
import smh
import numpy as np

data = smh.smh.smh_load(sys.argv[2])
mined = data.mine(4, 250)
if sys.argv[1] == "mhlink":
    clusters = mined.cluster_mhlink()
else:
    from sklearn.cluster import MiniBatchKMeans
    mbkm = MiniBatchKMeans(n_clusters=3, batch_size=10)
    clusters = mined.cluster_sklearn(mbkm)

clusters.show()
