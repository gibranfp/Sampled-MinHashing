#!/usr/bin/env python
# -*- coding: utf-8
# ----------------------------------------------------------------------
# Sampled MinHash python interface
# ----------------------------------------------------------------------
# Ivan V. Meza
# 2015/IIMAS, México
# ----------------------------------------------------------------------

import pickle
import sys
import numpy as np
sys.path.append("../build/python")
import smh
import scipy.spatial.distance as distance
import pylab as pl
import random
from mpl_toolkits.mplot3d import Axes3D

# MAIN program
if __name__ == "__main__":
    import argparse
    p = argparse.ArgumentParser("Predictis")
    p.add_argument("model",default=None,
        action="store", help="Model")
    p.add_argument("file",default=None,
        action="store", help="file to predict")
    p.add_argument("classes",default=None,
        action="store", help="classes file with classes to  mine")
 
    opts = p.parse_args()
    opts.show=False

    print "Loading model:",opts.model
    model_img,classes_img=pickle.load(open(opts.model,'rb'))
    model_img=[img.reshape((784,)) for img in model_img]
    model_img_=[]
    for model in model_img:
        model[model<0.1]=0.0
        model_img_.append(model)
    model_img=model_img_
      
    samples=smh.smh_load(opts.file)
    print "Loading samples to predict:",samples.size()
    
    ids_=[]
    for line in open(opts.classes):
        ids_.append(float(line))
 
    ids=range(samples.size())

    random.shuffle(ids)
    ids=ids[:10]


    for id,sample_ in enumerate(samples.ldb):
        if not id in ids:
            continue
        print "GOAL:", ids_[id]
        sample=np.zeros(784)
        sample[[pix.item for pix in sample_]]=1
        pred=np.zeros(10)
        class_=[]
        if opts.show:
            pl.figure()
            pl.imshow(sample.reshape((28,28)),vmax=1.0)
            pl.show()
        for img,classes in zip(model_img,classes_img):
            sim=np.sum(sample*img)*1.0/sum(img)
            pred_=np.zeros(10)
            for c,p in classes.iteritems():
                pred_[c]=p*sim
                pred[c]+=p*sim
            print ">>>",sim,np.sum(sample*img),'/',sum(img),classes
            print "(",np.argmax(pred),")",np.max(pred),pred

            if opts.show:
                fig=pl.figure()
                ax = fig.gca(projection='3d')
                X = np.arange( 28)
                Y = np.arange( 28)
                X, Y = np.meshgrid(X, Y)
                ax.plot_surface(X,Y,(sample*img).reshape((28,28)))
                pl.show()

        print pred
        print "---------->>>>",np.argmax(pred),np.max(pred)






