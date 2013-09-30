#!/bin/bash 

REUTERS1=/corpora/protected/topic_selection/Reuters_english_vol1
REUTERS2=/corpora/protected/topic_selection/Reuters_english_vol2

#ODIR1=/local_home/ivanvladimir/reuters/f10
#ODIR2=/local_home/ivanvladimir/reuters/f20
#ODIR3=/local_home/ivanvladimir/reuters/f30
#ODIR4=/local_home/ivanvladimir/reuters/f40
#ODIR5=/local_home/ivanvladimir/reuters/f50
ODIR6=/local_home/ivanvladimir/reuters/f60
ODIR7=/local_home/ivanvladimir/reuters/f70
ODIR8=/local_home/ivanvladimir/reuters/f80
ODIR9=/local_home/ivanvladimir/reuters/f90

#mkdir $ODIR1
#mkdir $ODIR2
#mkdir $ODIR3
#mkdir $ODIR4
#mkdir $ODIR5
mkdir $ODIR6
mkdir $ODIR7
mkdir $ODIR8
mkdir $ODIR9

#python2.7 src/reuters/docs2tfdocs.py --split train 10 --split test 90 --stop-words data/english.stop -p 6 -v --odir ${ODIR1} ${REUTERS1} ${REUTERS2}
#python2.7 src/reuters/docs2tfdocs.py --split train 20 --split test 80 --stop-words data/english.stop -p 6 -v --odir ${ODIR2} ${REUTERS1} ${REUTERS2}
#python2.7 src/reuters/docs2tfdocs.py --split train 30 --split test 70 --stop-words data/english.stop -p 6 -v --odir ${ODIR3} ${REUTERS1} ${REUTERS2}
#python2.7 src/reuters/docs2tfdocs.py --split train 40 --split test 60 --stop-words data/english.stop -p 6 -v --odir ${ODIR4} ${REUTERS1} ${REUTERS2}
#python2.7 src/reuters/docs2tfdocs.py --split train 50 --split test 50 --stop-words data/english.stop -p 6 -v --odir ${ODIR5} ${REUTERS1} ${REUTERS2}

python2.7 src/reuters/docs2tfdocs.py --split train 60 --split test 40 --stop-words data/english.stop -p 6 -v --odir ${ODIR6} ${REUTERS1} ${REUTERS2}
python2.7 src/reuters/docs2tfdocs.py --split train 70 --split test 30 --stop-words data/english.stop -p 6 -v --odir ${ODIR7} ${REUTERS1} ${REUTERS2}
python2.7 src/reuters/docs2tfdocs.py --split train 80 --split test 20 --stop-words data/english.stop -p 6 -v --odir ${ODIR8} ${REUTERS1} ${REUTERS2}
python2.7 src/reuters/docs2tfdocs.py --split train 90 --split test 10 --stop-words data/english.stop -p 6 -v --odir ${ODIR9} ${REUTERS1} ${REUTERS2}

