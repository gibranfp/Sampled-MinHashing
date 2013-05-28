#!/usr/bin/env python
# -*- coding: utf-8
# ----------------------------------------------------------------------
# Visualizes generated clusters
# ----------------------------------------------------------------------
# Ivan Vladimir Meza-Ruiz/ ivanvladimir at turing.iimas.unam.mx
# 2012/IIMAS/UNAM
# ----------------------------------------------------------------------
# clusters.py is free software: you can redistribute it and/or modify
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
from collections import Counter, defaultdict
from itertools import chain
from sets import Set
import re
import random
re_file=re.compile('(?P<corpus>[^.]+)\.(?P<ifs>[^-]+)-(?P<val>[^-]+)-(?P<win1>[^-]+)-(?P<win2>[^.]+).*')
from multiprocessing import Pool


# Local imports
from Utils import read_clusters2, read_docs2, docs2if, docs2probs
from distance import jacard,overlap

import scipy.sparse as sparse
import numpy as np
import matplotlib.pyplot as plt
from math import log


def rand_jsd((ixkdocs_,docs,dprobs,dlogprobs)):
    docs_=[docs[ii] for ii in ixkdocs]
    dprobs_=[dprobs[ii] for ii in ixkdocs]
    dlogprobs_=[dlogprobs[ii] for ii in ixkdocs]
    k_probs,k_logprobs=docs2probs(
            [reduce(operator.add,docs_,Counter())])
    jsd_actual=np.average(np.array([jsd(doc,dlogprobs_[ii],k_probs[0],k_logprobs[0]) for ii,doc in enumerate(dprobs_)]))
    return jsd_actual



def extractstats((ikluster,k,docs,idocs,dprobs,dlogprobs,threshold)):
    ixdocs=np.unique(idocs[k.data].data)
    if len(ixdocs)==0:
        return None
    docs_=docs[ixdocs]
    similarity = np.array([overlap(k,doc)  for doc in docs_])
    ixkdocs=np.nonzero(similarity>threshold)[0]
    print >> sys.stderr,ikluster, ' Clusters Analised of size ', len(k.data), ' with a coverage of ', len(ixkdocs)
    if len(ixkdocs)==0:
        return None

    dprobs_=dprobs[ixdocs]
    dlogprobs_=dlogprobs[ixdocs]
    docs_=docs_[ixkdocs]
    dprobs_=dprobs_[ixkdocs]
    dlogprobs_=dlogprobs_[ixkdocs]
    cols= docs_.sum(axis=0)
    total=cols.sum()
    probs=cols/total
    logprobs=np.log(cols/total)

    logprobs=np.nan_to_num(logprobs)
    k_probs=sparse.csr_matrix((probs),shape=(1,docs.shape[1]))
    k_logprobs=sparse.csr_matrix((logprobs),shape=(1,docs.shape[1]))
    jsd_actual=np.average(np.array([jsd(ii,doc,dlogprobs_[ii],k_probs,k_logprobs) for ii,doc in enumerate(dprobs_)]))
    return k.shape[0],ixkdocs,jsd_actual


def jsd(ii,doc,logdoc,clu,logclu):
    m_= (doc+clu)/2
    m_.data=np.log(m_.data)
    res=dkl(doc,logdoc,m_)/2+dkl(clu,logclu,m_)/2
    return res

def dkl(p,p_log,m_log):
    val=p.multiply(p_log-m_log)
    return val.sum()


# MAIN
if __name__ == "__main__":
    usage="""%prog docsfile clustersfile

        Analyse a clustering given a set of documents

        docsfile      : File with documents specified
        clusterfile   : File with clusters specified
"""

    version="%prog 0.1"

    # Command line options
    p = optparse.OptionParser(usage=usage,version=version)
    p.add_option("-k", "--k",type="int",
        action="store", dest="min_size", default=3,
        help="Minimum size of clusters")
    p.add_option("-t", "--threshold",type="float",
        action="store", dest="threshold", default=0.70,
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
    p.add_option("", "--processors",
        action="store", dest="processors", type="int",default=1,
        help="Number of processors [2]")
    p.add_option("-v", "--verbose",
        action="store_true", dest="verbose", default=False,
        help="Verbose mode")
    opts, args = p.parse_args()
 

    info={}

    # Setting verbose
    if opts.verbose:
        def verbose(*args):
            print "".join([str(a) for a in args])
    else:   
        verbose = lambda *a: None 

    if not len(args)>1:
        p.error('Wrong number of arguments')


    if opts.processors>1:
        # Loads the multriprocessors
        print >> sys.stderr, 'Usando multiple processors'
        try:
            pool=Pool(processes=opts.processors)
        except AtributteError:
            pool=Pool(processes=1)



    verbose("Loading documents file",args[0])
    docs=read_docs2(args[0])
    verbose("Calculating probabilities")
    dprobs,dlogprobs=docs2probs(docs)
    verbose("Creating inverse file")
    idocs=docs2if(docs)

    for cluster in args[1:]:
        basename=os.path.basename(cluster)
        #m=re_file.match(basename)
        #for k,v in m.groupdict().iteritems():
        #    info[k]=v
 
        verbose("Loading clustering ",cluster)
        print >> sys.stderr, "Loading clustering ",cluster
        clusters=read_clusters2(cluster,format=opts.format,min=opts.min_size)
        info['Original total cluster']=clusters.shape[0]
        verbose("Original number of clusters ",clusters.shape[0])
        print >> sys.stderr, "Original number of clusters ",clusters.shape[0]
        if clusters.shape[0]==0:
            continue
        if clusters.shape[0]>30000:
            continue
        info['Original Filename']=cluster
        info['Starting point']=opts.start
        if opts.max > 0:
            clusters=clusters[opts.start:opts.start+opts.max]
            info['Total cluster analysed']=opts.max
        else:
            clusters=clusters[opts.start:]
            info['Total cluster analysed']=clusters.shape[0]
        threshold=opts.threshold
        clusters=[(ii,k,docs,idocs,dprobs,dlogprobs,threshold) for ii,k in enumerate(clusters) if k.shape[0]]
        if len(clusters)==0:
            continue
        verbose("Total cluster larger than ",opts.min_size,': ',len(clusters))
        print >> sys.stderr,  "Total cluster larger than ",opts.min_size,': ',len(clusters)
        

        visual=False
        if opts.show or opts.save:
            visual=True

        stats=[]

        info['Threshold']=opts.threshold

        # Just analyse the documents with terms in common
        if opts.processors==1:
            mapres=map(extractstats,clusters)
        else:
            print >> sys.stderr, '- Using multiple processors'
            print >> sys.stderr, len(clusters)
            mapres=pool.map(extractstats,clusters)
        stats=filter(None,mapres)

        #return len(k), ixkdocs,jsd_actual
        docs_recovered=[]
        for s, ixs, v in stats:
            docs_recovered.append(ixs)

        if len(docs_recovered)>0:
            dd=np.concatenate(docs_recovered)
            docs_recovered=np.unique(dd)
        else:
            docs_recovered=[]
    

        #idocs_rand=dict([(s, 
        #    [[random.choice(range(len(docs))) 
        #                for y in range(s)] 
        #                for x in range(50)]) 
        #                for s,vals in sizes.iteritems()])

        #jsd_rand={}
        #for s,ks in docs_rand.iteritems():
        #    jsd_rand[s]=np.average([ rand_jsd((ixkdocs,docs,dprobs,dlogprobs))
        #        for ixkdocs in ks])

        for x in stats:
            print '>>',len(x[1]),x[2]

        #coh=sum([len(x[1])*(jsd_rand[len(x[1])]-x[2]) for x in stats])
        coh=sum([len(x[1])*x[2] for x in stats])
        den=sum([len(x[1]) for x in stats])
        if den==0.0:
            coh=0.0
        else:
            coh=coh/den
        if len(stats)>0:
            coh_=sum([x[2] for x in stats])/len(stats)
        else:
            coh_=0.0

        info['Coherence']=coh
        info['Coverture']=len(docs_recovered)

        txt="""\

    Cluster {0}:
        Size       : {1}
        Analysed   : {4}
        Coverture  : {2}
        Coherence  : {3}
        Coherence*  : {5}
    """

        print txt.format(
            cluster,
            len(clusters),
            len(docs_recovered),
            coh,
            len(stats),
            coh_)
#
#
