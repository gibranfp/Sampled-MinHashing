#!/usr/bin/env python
# -*- coding: utf-8
# ----------------------------------------------------------------------
# Coherence
# ----------------------------------------------------------------------
# Ivan Vladimir Meza-Ruiz/ ivanvladimir at turing.iimas.unam.mx
# 2013/IIMAS/UNAM
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
import re
re_file=re.compile('(?P<corpus>[^.]+)\.(?P<ifs>[^-]+)-(?P<val>[^-]+)-(?P<win1>[^-]+)-(?P<win2>[^.]+).*')
from multiprocessing import Pool

from Utils import read_clusters2, read_docs2, docs2if

import numpy as np


def coherence((docsw1,docsw2)):
    num=float(len(np.intersect1d(docsw1,docsw2))+1)
    den=float(len(docsw2))
    if den==0:
        return 0.0
    return np.log(num/den)


def overlap((k,d)):
    inter= np.intersect1d(k,d.indices)
    num = inter.shape[0]*1.0
    den = min(len(k),len(d.indices))
    
    if den==0:
        return 1.0
    else:
        return num/den



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
    p.add_option("", "--processors",
        action="store", dest="processors", type="int",default=8,
        help="Number of processors [8]")
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
        try:
            pool=Pool(processes=opts.processors)
            print >> sys.stderr, 'Using multiple processors'
        except AtributteError:
            pool=Pool(processes=1)
            print >> sys.stderr, 'Using one processor'



    verbose("Loading documents file ",args[0])
    docs=read_docs2(args[0])
    verbose("Creating inverted file ")
    idocs=docs2if(docs)

    for topic in args[1:]:
        basename=os.path.basename(topic)
        #m=re_file.match(basename)
        #for k,v in m.groupdict().iteritems():
        #    info[k]=v
 
        verbose("Loading topic ",topic)
        print >> sys.stderr, "Loading topicing ",topic
        topics=read_clusters2(topic,format=opts.format,min=opts.min_size)
        info['Original total topic']=topics.shape[0]
        verbose("Original number of topics ",topics.shape[0])
        print >> sys.stderr, "Original number of topics ",topics.shape[0]
        if topics.shape[0]==0:
            continue
        if topics.shape[0]>30000:
            continue
        info['Original Filename']=topic
        info['Starting point']=opts.start
        if opts.max > 0:
            topics=topics[opts.start:opts.start+opts.max]
            info['Total topic analysed']=opts.max
        else:
            topics=topics[opts.start:]
            info['Total topic analysed']=topics.shape[0]
        verbose("Total topic larger than ",opts.min_size,': ',topics.shape[0])
        total_co=0.0
        for t in range(topics.shape[0]):
            verbose('Analysing topic ',t)
            V=topics[t,:].data
            verbose('Size of topics ',V.shape)
            res=0.0

            verbose("Identifying documents in topic")
            topicdoc = [(V,doc)  for doc in docs]
            # Just analyse the documents with terms in common
            if opts.processors==1:
                mapres=map(overlap,topicdoc)
            else:
                mapres=pool.map(overlap,topicdoc)


            similarity = np.array(mapres)
            ixkdocs=np.nonzero(similarity>=0.01)[0]
            verbose("Creating inverse file over ",len(ixkdocs)," documents")

            for m in range(1,len(V)):
                vals=[]
                for l in range(0,m):
                    try:
                        m_=np.intersect1d(idocs[V[m]].indices,ixkdocs)
                        l_=np.intersect1d(idocs[V[l]].indices,ixkdocs)
                        vals.append((m_,l_))
                    except IndexError:
                        pass

                # Just analyse the documents with terms in common
                if opts.processors==1:
                    mapres=map(coherence,vals)
                else:
                    print '.',
                    mapres=pool.map(coherence,vals)
                res+=sum(mapres)

            verbose('Coherence of topic ',res)
            total_co+=res
        verbose('Total coherence ',total_co)




        
