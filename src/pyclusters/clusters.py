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
import operator
from collections import Counter


# Local imports
from Utils import read_clusters, read_docs
from distance import cosine,overlap

import numpy as np
import matplotlib.pyplot as plt
from math import log

# MAIN
if __name__ == "__main__":
    usage="""%prog docsfile clustersfile

        Visualize clusters

        docsfile      : File with documents specified
        clusterfile   : File with clusters specified
"""

    version="%prog 0.1"

    # Command line options
    p = optparse.OptionParser(usage=usage,version=version)
    p.add_option("-k", "--k",type="int",
        action="store", dest="min_size", default=0.0,
        help="Minimum size of clusters")
    p.add_option("-t", "--threshold",type="float",
        action="store", dest="threshold", default=0.0,
        help="Threshold for clusters")
    p.add_option("", "--start",type="int",
        action="store", dest="start", default=0,
        help="Starting cluster")
    p.add_option("", "--max",type="int",
        action="store", dest="max", default=0,
        help="Maximum number of cluster")
    p.add_option("", "--format",type="int",
        action="store", dest="format", default=0,
        help="(0 - Gibran|1 - Documents)")
    p.add_option("", "--show-figures",
        action="store_true", dest="show",
        help="Maximum number of cluster")
    p.add_option("", "--save-figures",
        action="store_true", dest="save",
        help="Maximum number of cluster")
    p.add_option("", "--odir",default="./",
        action="store", dest="odir",
        help="Output directory [current]")
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

    if not len(args)>1:
        p.error('Wrong number of arguments')

    verbose("Loading documents file",args[0])
    docs=read_docs(args[0])


    for cluster in args[1:]:
        verbose("Loading clusters file",cluster)
        clusters=read_clusters(cluster,format=opts.format)

        if opts.max > 0:
            clusters=clusters[opts.start:opts.start+opts.max]
        else:
            clusters=clusters[opts.start:]

        nclusters=[]
        visual=False
        if opts.show or opts.save:
            visual=True

        stats=[]
        threshold=opts.threshold

        kn=0
        for k in clusters:
            if len(k)<=opts.min_size:
                continue
            similarity_ = [overlap(k,doc)  for doc in docs]
            kn+=1
            similarity=np.array(similarity_)
            ixkdocs=np.nonzero(similarity>threshold)
            kdocs=[Counter(dict([(ix_,docs[ix][ix_]) for ix_ in
                set(docs[ix].keys()).intersection(set(k.keys()))])) for ix in np.nditer(ixkdocs)]
            kwcounts=reduce(operator.add, kdocs, Counter([]))
            total_c=sum(kwcounts.values())
            kwcounts=[(ix,1.0*c/total_c) for ix,c in  kwcounts.most_common()]
            similarity=similarity[similarity>threshold]
            #print ixsim_
            #sim=np.log(sim_)
            #perplexity=np.power(-np.sum(sim),2)
            nclusters.append((len(similarity),kwcounts))
            stats.append((
                len(k),
                len(similarity),
                0.0 #perplexity
            ))

        txt="""\

    Cluster {0}:
        Size       : {1}
        Total docs : {2}
        Perplexity : {3}
    """

        basename=os.path.basename(cluster)

        nclusters.sort(key=lambda k: k[0])
        with open ("{0}/{1}_kbest_wbest.txt".format(opts.odir,basename),'w') as kfile:
            for h,k in nclusters:
                print >> kfile, len(k), " ".join(["{0}:{1:0.4f}".format(ix,val) for ix,val in k]) 


        if visual:
            lens=np.array([s[0] for s in stats])
            min = np.min(lens)
            max = np.max(lens)
            mu = np.mean(lens)
            median = np.median(lens)
            sigma = np.std(lens)
            textstr = '$min=%d$,\n$max=%d$,\n$\mu=%.2f$\n$\mathrm{median}=%.2f$\n$\sigma=%.2f$'%(min,max,mu,
                    median, sigma)

            y,binEdges=np.histogram(lens,bins=100)
            bincenters = 0.5*(binEdges[1:]+binEdges[:-1])
            fig=plt.figure()
            p1 = fig.add_subplot(111)
            plt.title('Histogram for {1} cluster size\n (threshold {0})'.format(opts.threshold,kn))
            plt.xlabel('Cluster size (words)')
            plt.ylabel('Number of clusters')
            p1.plot(bincenters,y,'-')
            p1.text(0.95, 0.95, textstr,
                    transform=p1.transAxes,verticalalignment='top',horizontalalignment='right')

            if opts.show:
                fig.show()
            if opts.save:
                fig.savefig("{0}/{1}_hist_word.png".format(opts.odir,basename))

            lens=np.array([s[1] for s in stats])
            min = np.min(lens)
            max = np.max(lens)
            mu = np.mean(lens)
            median = np.median(lens)
            sigma = np.std(lens)
            textstr = '$min=%d$,\n$max=%d$,\n$\mu=%.2f$\n$\mathrm{median}=%.2f$\n$\sigma=%.2f$'%(min,max,mu,
                    median, sigma)


            y,binEdges=np.histogram(lens,bins=100)
            bincenters = 0.5*(binEdges[1:]+binEdges[:-1])
            fig=plt.figure()
            p2 = fig.add_subplot(111)
            plt.title('Histogram for {1} cluster coverage\n (threshold {0})'.format(opts.threshold,kn))
            plt.xlabel('Cluster hits (documents {0})'.format(len(docs)))
            plt.ylabel('Number of clusters')
            p2.plot(bincenters,y,'-')
            p2.text(0.95, 0.95, textstr,
                    transform=p2.transAxes,verticalalignment='top',horizontalalignment='right')


            if opts.show:
                fig.show()
            if opts.save:
                fig.savefig("{0}/{1}_hist_doc.png".format(opts.odir,basename))



        #for i,stats in enumerate(stats):
        #    print txt.format(opts.start+i,*stats)

        
