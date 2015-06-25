#!/usr/bin/env python
# -*- coding: utf-8
# ----------------------------------------------------------------------
# Sampled MinHash python interface
# ----------------------------------------------------------------------
# Ivan V. Meza
# 2015/IIMAS, México
# ----------------------------------------------------------------------

import smh_api as sa

def smh_load(filename):
    ldb = sa.listdb_load_from_file(filename)
    return SMH(ldb=ldb)

class SMH:
    def __init__(self,size=0,dim=0,ldb=None):
        if ldb:
            self.ldb=ldb
        else:
            self.ldb = sa.listdb_init(size,dim)

    def save(self,filename):
        sa.listdb_save_to_file(filename,self.ldb)

    def show(self):
        sa.listdb_print(self.ldb)


# MAIN program
if __name__ == "__main__":
    import argparse
    p = argparse.ArgumentParser("Mines")
    p.add_argument("file",default=None,
        action="store", help="File for mining")

    opts = p.parse_args()

    s=smh_load(opts.file)
    s.show()



