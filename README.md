
MinHash-Clustering
==================

MinHash-based clustering for binary dyadic data (e.g. bag of words).

Utilities
---------

* Scripts for Reuters corpus


### Reuters corpus


In order to create the term frecuencie documents run the script:

    sh scripts/create_ftdocs.sh


For more control use the script:
    
    src/reuters/docs2tfdocs.py
    

More options:

    Options:
      --version             show program's version number and exit
      -h, --help            show this help message and exit
      --idx=IDX             Idx file to reproduce [None]
      --split=SPLITS        Splits to produce and percentage [None]
      --voca=VOCA           Name of main vocabulary bases on splits [train]
      --corpus=CORPUS       Name for corpus file [reuters]
      --odir=ODIR           Output dir for documents [None]
      --stop-words=FILE     stopwords file [None]
      -l LOGFILE, --logging=LOGFILE
                            log [docs2itfdocs.log]
      -p PROCESSORS, --processors=PROCESSORS
                            Number of processors [2]
      --cutoff=CUTOFF       Cut off for frecuencies [5]
      -s, --serial          Runs in sereial mode
      -v, --verbose         Verbose mode 



Stopwords from:

    http://jmlr.csail.mit.edu/papers/volume5/lewis04a/a11-smart-stop-list/english.stop    
