#!/usr/bin/env python
# -*- coding: utf-8
# ----------------------------------------------------------------------
# Sampled MinHash demo with images
# ----------------------------------------------------------------------
# Ivan V. Meza
# 2015/IIMAS, México
# ----------------------------------------------------------------------

import numpy as np
import pylab as pl
from mpl_toolkits.mplot3d import Axes3D

from smh import smh

# MAIN program
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
    p.add_argument("--show",default=False,
            action="store_true", dest='show',help="Show bases found")
    p.add_argument("idx",default=None,
        action="store", help="idx file with images to mine")
    p.add_argument("classes",default=None,
        action="store", help="classes file with classes to  mine")
    p.add_argument("--xdim",default=28,type=int,
        action="store", dest="xdim",help="Dimension of x axis of images")
    p.add_argument("--ydim",default=28,type=int,
        action="store", dest="ydim",help="Dimension of y axis of images")

    opts = p.parse_args()
    size_dim=opts.xdim*opts.ydim

    print "Loading file idx:",opts.idx
    s=smh.smh_load(opts.idx)
    print "Loading file with classes", opts.classes
    classes=[]
    for line in open(opts.classes):
        classes.append(float(line))
    print "Mining topics..."
    m=s.mine(opts.r,opts.l)
    print "Size of original mined topics:",m.size()
    m.cutoff(min=opts.min)
    print "Size of cutted off mined topics:",m.size()

    model_img=[]
    classes_img=[]
    for ii,l in enumerate(m.ldb):
        avgimgs=np.zeros(size_dim)
        n=0
        classes_img.append({})
        for i in l:
            img=np.zeros(size_dim)
            img[[pix.item for pix in s.ldb[i.item]]]=1
            avgimgs+=img
            n+=1
            try:
                classes_img[-1][classes[i.item]]+=1.0
            except KeyError:
                classes_img[-1][classes[i.item]]=1.0
        #avgimgs=avgimgs/s.size()
        avgimgs=avgimgs/n
        maxi=0
        for key in classes_img[-1].keys():
            classes_img[-1][key]=classes_img[-1][key]/l.size
            if classes_img[-1][key]/l.size > maxi:
                maxi=classes_img[-1][key]/l.size 

        avgimgs=avgimgs.reshape((opts.xdim,opts.ydim))
        model_img.append(avgimgs)
        print "For element",ii
        for k,i in classes_img[-1].iteritems():
                print "{0} -> {1:1.3f}".format(k,i)
        if opts.show:
            fig=pl.figure()
            ax = fig.gca(projection='3d')
            X = np.arange( 28)
            Y = np.arange( 28)
            X, Y = np.meshgrid(X, Y)
            ax.plot_surface(X,Y,avgimgs.reshape((28,28)),rstride=1,
                    cstride=1, cmap='hot')
            pl.show()
 
    if opts.output:
        import pickle
        print "Saving resulting model to",opts.output
        pickle.dump((model_img,classes_img),open(opts.output,'wb'))




