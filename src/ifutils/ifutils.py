#!/usr/bin/env python
# -*- coding: utf-8
# --------------------------------------------------------------------
# Gibran Fuentes Pineda <gibranfp at turing.iimas.unam.mx>
# IIMAS, UNAM
# 2013
# --------------------------------------------------------------------
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
# --------------------------------------------------------------------
"""
Functions to work with text corpora, vocabularies and inverted files.
"""
import sys

def read_vocab(fname):
    """Reads vocabulary in NIPS format."""
    with open(fname,'r') as fvoc:
        txtbuff = fvoc.readlines()
        vocab = []
        for line in txtbuff:    
            term,termid,freq = line.split(' = ')
            vocab.append(term)
    return vocab

def read_idlists(fname):
    """Reads a file of id lists."""
    with open(fname,'r') as fids:
        txtbuff = fids.readlines()
        idlists = []
        for line in txtbuff[1:]:    
            idbuff = line.split()
            ids = []
            for i in idbuff[1:]:
               ids.append(int(i))
            idlists.append(ids)
    return idlists

def write_idlists(fname,idlists,dbsize):
    """Writes a database of id lists in a file."""
    with open(fname,'w') as dbfile:
        dbfile.write(str(dbsize) + '\n')
        dbfile.writelines(str(len(i)) + ' ' + ' '.join(str(j) for j in i) +
                          '\n' for i in idlists)

def write_termlists(fname,termlists):
    """Writes a database of term lists in a file."""
    with open(fname,'w') as termfile:
        termfile.writelines(' '.join(j for j in i) +
                            '\n' for i in termlists)

def id2term(idlists,vocab):
    """Converts lists of ids to lists of terms."""
    termlists = []
    for ids in idlists:    
        terms = []
        for i in ids:
            terms.append(vocab[int(i)])
        termlists.append(terms)
    return termlists

def make_ifs(fname,vocsize):
    """Reads corpus and computes its inverted file."""
    with open(fname,'r') as f:
        corpus = f.readlines()
    ifs = [[] for i in range(int(vocsize))]
    for docid,line in enumerate(corpus):    
        doc = line.split()
        for term in doc[1:]:
            termid,freq = term.split(":")
            ifs[int(termid)].append(docid)
    return ifs

def query_ifs(ifs,querylist):
    """Queries the inverted file and returns a list of document ids."""
    doclist = []
    for ids in querylist:
        doclist.append(ifs[ids])
    return doclist
