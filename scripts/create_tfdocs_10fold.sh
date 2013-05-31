#!/bin/bash 

REUTERS1=/corpora/protected/topic_selection/Reuters_english_vol1
REUTERS2=/corpora/protected/topic_selection/Reuters_english_vol2
ODIR=/local_home/ivanvladimir/reuters/

python2.7 src/reuters/docs2tfdocs.py --split train 80 --split test 20 --stop-words data/english.stop -p 6 -v --odir ${ODIR} ${REUTERS1} ${REUTERS2}


