#!/bin/bash

python2.7 ./src/pyclusters/clusters.py\
        --odir clusters/nips/cws\
        --save-figures
        $*\
        data/nips/nips.corpus\
        /local_home/ivanvladimir/results_nips2013/cws/* 
