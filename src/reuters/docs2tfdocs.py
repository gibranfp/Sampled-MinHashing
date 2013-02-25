#!/usr/bin/env python
# -*- coding: utf-8
# ----------------------------------------------------------------------
# Transforms docs into itf documents
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
import logging
import random
random.seed()

from multiprocessing import Pool
from docsutils import vocazip, docszip, extractidxs
from collections import Counter
from itertools import imap,chain


# MAIN
if __name__ == "__main__":
    usage="""%prog [options] dir1 [dirs]

        Create inverse file documents

        dir   : Directory with reuters format
"""

    version="%prog 0.1"

    # Command line options
    p = optparse.OptionParser(usage=usage,version=version)
    p.add_option("", "--idx",default=None,
            action="store", dest="idx",
            help="Idx file to reproduce [None]")
    p.add_option("", "--split",default=[],nargs=2,
            action="append", dest="splits",
            help="Splits to produce and percentage [None]")
    p.add_option("", "--voca",default="train",
            action="store", dest="voca",
            help="Name of main vocabulary bases on splits [train]")
    p.add_option("", "--corpus",default="reuters",
            action="store", dest="corpus",
            help="Name for corpus file [reuters]")
    p.add_option("", "--odir",default=None,
            action="store", dest="odir",
            help="Output dir for documents [None]")
    p.add_option("", "--stop-words",default=None,metavar="FILE",
            action="store", dest="stopwords",
            help="stopwords file [None]")
    p.add_option("-l", "--logging",default="docs2itfdocs.log",
            action="store", dest="logfile",
            help="log [docs2itfdocs.log]")
    p.add_option("-p", "--processors",
        action="store", dest="processors", type="int",default=2,
        help="Number of processors [2]")
    p.add_option("", "--cutoff",
        action="store", dest="cutoff", type="int",default=5,
        help="Cut off for frecuencies [5]")
    p.add_option("-s", "--serial",
        action="store_true", dest="serial", default=False,
        help="Runs in sereial mode")
    p.add_option("-v", "--verbose",
        action="store_true", dest="verbose", default=False,
        help="Verbose mode")
    opts, args = p.parse_args()


    # Load stopwords
    if opts.stopwords:
        sws=[]
        for line in open(opts.stopwords):
            line=line.strip()
            if line.startswith("#"):
                continue
            sws+=[w.strip() for w in line.split(',')]
        sws=set(sws)

    # Change directory
    if opts.odir:
        os.chdir(opts.odir)

    # Setting logging
    logging.basicConfig(filename=opts.logfile, level=logging.INFO)
        

    # Collects zip files
    listing=[]
    for dirname in args:
        dirpath=dirname+"/{0}"
        listing += [ dirpath.format(zipf)  for zipf in os.listdir(dirname) if
                zipf.endswith('.zip') ]

    # Loads the multriprocessors
    try:
        pool=Pool(processes=opts.processors)
    except AtributteError:
        pool=Pool(processes=1)



    # Load names of files to extract
    if opts.idx:
        idxs=[]
        with open(opts.idx) as idxf:
            for line in idxf:
                idxs.append(line.strip())
    else:
        # Extracts de vocabulary
        if opts.serial:
            mapres=map(extractidxs,listing)
        else:
            logging.info("Using {0} processors".format(opts.processors))
            if opts.verbose:
                print "Total processors",opts.processors

            mapres=pool.map(extractidxs,listing)
        idxs=list(chain(*mapres))


    # Creating splits
    if len(opts.splits)>0:
        if not sum([float(y) for x,y in opts.splits ]) == 100.0:
            p.error("Split options defined but it does not adds to 100%")
        random.shuffle(idxs)
        splits={}
        dist=[]
        splitnames=["."+x for x,y in opts.splits]
        if opts.voca:
            mainvoca="."+opts.voca
        else:
            mainvoca=splitnames[0]
        for name,size in opts.splits:
            dist+=["."+name for x in range(int(size))]

        random.shuffle(dist)
        for filename in idxs:
            splits[filename]=random.choice(dist)
            pass

        for name,sise in opts.splits:
            logging.info("Splits for {0} : {1:3.2f}%".format(name,100*sum([1 for x  in splits.values() if x.startswith(name)  ])/len(splits)))

    else:
        splitnames=[""]
        mainvoca=""
        splits=dict([(name,"") for name in idxs])

    if not mainvoca in splitnames:
        p.error("The voca label is not defined in the splits")

    # Extracts de vocabulary
    if opts.serial:
        mapres=map(vocazip,[(x,sws,splits,splitnames) for x in listing])
    else:
        logging.info("Using {0} processors".format(opts.processors))
        if opts.verbose:
            print "Total processors",opts.processors

        mapres=pool.map(vocazip,[(x,sws,splits,splitnames) for x in listing])

    cwords={}
    cdocs={}

    voca={}
    # Consolidating the results
    for name in splitnames:
        cwords[name]=Counter([])
        cdocs[name]=Counter([])
        for cws_,cds_ in mapres:
            try:
                cwords[name].update(cws_[name])
                cdocs[name].update(cds_[name])
            except KeyError:
                pass

        voca_=[(y,x) for (x,y) in enumerate([w for w,c in cwords[name].most_common() if
          c>opts.cutoff])]
        voca[name]=dict(voca_)

        # Saving voca
        if opts.odir:
            vocafile=open("{0}/{1}{2}.voca".format(opts.odir,opts.corpus,name),"w")
        else:
            vocafile=open("{0}{1}.voca".format(opts.corpus,name),"w")


        # Printing voca files
        for word,id in voca_:
            print >> vocafile, "{0} = {1} = {2} {3}".format(word,id,cwords[name][word],cdocs[name][word])
        
    if opts.odir:
        pathcorpus="{0}/{1}".format(opts.odir,opts.corpus)
    else:
        pathcorpus="{0}".format(opts.corpus)

    # Prints documents
    if opts.serial:
        mapres=imap(docszip,[(n,x,voca[mainvoca],pathcorpus,sws,splits,splitnames) for n,x in enumerate(listing)])
    else:
        logging.info("Using {0} processors".format(opts.processors))
        if opts.verbose:
            print "Total processors",opts.processors
        mapres=pool.map(docszip,[(n,x,voca[mainvoca],pathcorpus,sws,splits,splitnames) for n,x in enumerate(listing)])

    
    docs,zips=zip(*mapres)
    docs_=sum(docs)
    zips_=sum(zips)

    for name in splitnames:
        ucorpusfile="{0}{1}.corpus".format(pathcorpus,name)
        with open(ucorpusfile, 'w') as outfile:
            for n,x in enumerate(listing):
                corpusfile="{0}{1}.corpus.{2}".format(pathcorpus,name,n)
                with open(corpusfile) as infile:
                    for line in infile:
                        outfile.write(line)
                os.remove(corpusfile)


        uidxfile="{0}{1}.idx".format(pathcorpus,name)
        with open(uidxfile, 'w') as outfile:
            for n,x in enumerate(listing):
                idxfile="{0}{1}.idx.{2}".format(pathcorpus,name,n)
                with open(idxfile) as infile:
                    for line in infile:
                        outfile.write(line)
                os.remove(idxfile)

    print "Total documents",docs_
    print "Total zips",zips_
    print "Total splits",len(splitnames)
    print "Total directories",len(args)
    print "Total vocabulary",len(voca_)

    vocafile.close() 
        

        


       
        

