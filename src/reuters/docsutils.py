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
import zipfile
import re
import xml.etree.ElementTree as ET
import logging
from collections import Counter

punct=re.compile("[a-z']+")


def extractidxs(filename,log=True):
    """
    Extracts indices of files
    """
    zf= zipfile.ZipFile(filename,'r')
    if log:
        logger = logging.getLogger("extractidxs")
        logger.info('Analyzing zip file {0}'.format(filename))
    files=[]
    for info in zf.infolist():
        if not (info.filename.endswith('.xml') or  info.filename.endswith('.XML')) :
            continue
        files.append(info.filename)
        if log:
            logger.info('Finalizing analyzing zip file {0}'.format(filename))
    return files



def vocazip((filename,sw,idxs,names),log=True):
    """
    Unzips an XML file and creates a total counter, and document counter
    """
    zf= zipfile.ZipFile(filename,'r')
    if log:
        logger = logging.getLogger("vocazip")
        logger.info('Analyzing zip file {0}'.format(filename))
    cwords={}
    cdocs={}
    for name in names:
        # Counter for words
        cwords[name]=Counter()
        # Counter for docs
        cdocs[name]=Counter()
    for info in zf.infolist():
        if not (info.filename.endswith('.xml') or  info.filename.endswith('.XML')) :
            continue
        if log:
            logger = logging.getLogger("vocazip")
            logger.info('Analazing the file {0}'.format(info.filename))
        try:
            root = ET.fromstring(zf.read(info.filename))
            for child in root.findall('text'):
                for pp in child:
                    vocastr(pp.text,cwords[idxs[info.filename]],sw)
                    vocastr(pp.tail,cwords[idxs[info.filename]],sw)
                vocastr(child.text,cwords[idxs[info.filename]],sw)
                vocastr(child.tail,cwords[idxs[info.filename]],sw)
            cdocs[idxs[info.filename]].update(set(cwords[idxs[info.filename]]))
        except ET.ParseError:
            print "Error in XML file:",info.filename 
            if log:
                logger.error('Error in file file {0}'.format(info.filename))
        if log:
            logger.info('Finalizing analyzing zip file {0}'.format(filename))
    print ".",
    return cwords,cdocs


def docszip((n,filename,voca,path,sw,splits,splitnames),log=True):
    """
    Creates the documents for XML file
    """
    corpusfile={}
    idxfile={}
    for name in splitnames:
        corpusfile[name]=open("{0}{1}.corpus.{2}".format(path,name,n),"w")
        idxfile[name]=open("{0}{1}.idx.{2}".format(path,name,n),"w")
    zf= zipfile.ZipFile(filename,'r')
    docs_=0
    zips_=0
 
    if log:
        logger = logging.getLogger("docszip")
        logger.info('Analyzing zip file {0}'.format(filename))
    for info in zf.infolist():
        zips_+=1
        if not (info.filename.endswith('.xml') or  info.filename.endswith('.XML')) :
            continue
        if log:
            logger.info('Analazing the file {0}'.format(info.filename))
        try:
            root = ET.fromstring(zf.read(info.filename))
            for child in root.findall('text'):
                # Counter for words
                cwords=Counter()
                for pp in child:
                    vocastr(pp.text,cwords,sw,voca)
                    vocastr(pp.tail,cwords,sw,voca)
                vocastr(child.text,cwords,sw,voca)
                vocastr(child.tail,cwords,sw,voca)
                cnts=["{0}:{1}".format(voca[w],x) for w,x in cwords.most_common() if voca.has_key(w)]
                print >> corpusfile[splits[info.filename]], "{0} {1}".format(len(cwords)," ".join(cnts))
                print >> idxfile[splits[info.filename]],"{0}:{1}".format(filename,info.filename)
                docs_+=1
        except ET.ParseError:
            print "Error in XML file:",info.filename 
            if log:
                logger.error('Error in file file {0}'.format(info.filename))
     
    if log:
        logger.info('Finalizing analyzing zip file {0}'.format(filename))
    for name in splitnames:
        corpusfile[name].close()
        idxfile[name].close()
    return docs_,zips_


def vocastr(str,voca=Counter([]),sw=[],validvoca=None):
    """Counts the words in a str"""
    str=str.strip()
    if len(str) > 0:
        words=[w for w in punct.findall(str.lower()) if len(w)>0 and not w in
                sw and (not validvoca or validvoca.has_key(w))]
        voca.update(words)


