#!/usr/bin/env python
# -*- coding: utf-8
# ----------------------------------------------------------------------
# Visualizes generated clusters
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

# System libraries
import optparse
import sys
import os
import os.path

# Local imports
from Utils import read_clusters, read_docs
from distance import cosine,overlap

import numpy as np
from math import log

# MAIN
if __name__ == "__main__":
    usage="""%prog clusterfile docsfile

        Visualize clusters

        clusterfile   : File with clusters specified
        docsfile      : File with documents specified
"""

    version="%prog 0.1"

    # Command line options
    p = optparse.OptionParser(usage=usage,version=version)
    p.add_option("-t", "--threshold",type="float",
        action="store", dest="threshold", default=0.0,
        help="Threshold for clusters")
    p.add_option("", "--start",type="int",
        action="store", dest="start", default=0,
        help="Starting cluster")
    p.add_option("", "--max",type="int",
        action="store", dest="max", default=0,
        help="Maximum number of cluster")
    p.add_option("-v", "--verbose",
        action="store_true", dest="verbose", default=False,
        help="Verbose mode")
    opts, args = p.parse_args()

    # Setting verbose
    if opts.verbose:
        def verbose(*args):
            print "".join([str(a) for a in args])
    else:   
        verbose = lambda *a: None 

    verbose("Loading clusters file",args[0])
    clusters=read_clusters(args[0])

    if opts.max > 0:
        clusters=clusters[opts.start:opts.start+opts.max]
    else:
        clusters=clusters[opts.start:]

    verbose("Loading documents file",args[1])
    docs=read_docs(args[1])

    stats=[]
    threshold=opts.threshold

    for k in clusters:
        data=[]
        for doc in docs:
            data.append(overlap(k,doc))
        sim=np.array(data)
        sim=np.log(sim[sim>threshold])
        perplexity=np.power(-np.sum(sim),2)
        stats.append((
            len(k),
            len(sim),
            perplexity
        ))

    txt="""\

Cluster {0}:
    Size       : {1}
    Total docs : {2}
    Perplexity : {3}
"""

    for i,stats in enumerate(stats):
        print txt.format(opts.start+i,*stats)

    
