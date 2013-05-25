#!/usr/bin/env python
# -*- coding: utf-8
# ----------------------------------------------------------------------
# Views for webApp for visualizing clusters
# ----------------------------------------------------------------------
# Ivan Vladimir Meza-Ruiz/ ivanvladimir at turing.iimas.unam.mx
# 2013/IIMAS/UNAM
# ----------------------------------------------------------------------
# homepage.py is free software: you can redistribute it and/or modify
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

from flask import render_template, request
from webapp import app

import os
import os.path
import yaml

voca=[]
with open('data/nips/vocab.txt') as nips:
    for line in nips:
        voca.append(line.strip())
        
@app.route('/')
@app.route('/index')
def index():
    return render_template('index.html')

# List the available clusters
@app.route('/list/<corpus>/<type>')
def list(corpus,type):
    clusdir='clusters/'+corpus+'/'+type+'/'
    listing=[]
    for x in os.listdir(clusdir): 
        if not x.endswith('.txt'):
            continue
        base=x[:-4]
        info=None
        if os.path.exists(clusdir+base+'.info'):
            with open(clusdir+base+'.info') as fi:
                info=yaml.load(fi)
        listing.append((base,info))
    listing.sort()
    return render_template('list.html',listing=listing,corpus=corpus,type=type)

@app.route('/explore/<corpus>/<type>/<filename>')
def explore(corpus,type,filename):
    clusdir='clusters/'+corpus+'/'+type+'/'
    info=None
    if os.path.exists(clusdir+filename+'.info'):
        with open(clusdir+filename+".info") as fi:
            info=yaml.load(fi)
    with open(clusdir+filename+".txt") as kf:
        content=[]
        for iline, line in enumerate(kf):
            line=line.strip().split()
            if ':' in line[2]:
                content.append((None,line[0],line[1],iline))
            else:
                content.append((float(line[2]),line[0],line[1],iline))
        content.sort()
    return render_template('explore.html',filename=filename,info=info,content=content,corpus=corpus,type=type)


@app.route('/wordclouds/<corpus>/<type>/<filename>')
def wordclouds(filename):
    clusdir='clusters/'+corpus+'/'+type+'/'
    data=request.args
    info=[]
    info=None
    iids=[int(k[1]) for k in data.items()]
    if os.path.exists(clusdir+filename+'.info'):
        with open("clusters/"+filename+".info") as fi:
            info=yaml.load(fi)
    content=[]
    lines=[]
    with open(clusdir+filename+".txt") as kf:
        for iline, tline in enumerate(kf):
            if not iline in iids:
                continue
            lines.append((iline,tline.strip().split()))
    contents=[]
    fc=True
    for iid,line in lines:
        num=line[0]
        size=line[1]
        content=[]
        if ':' in line[2]:
            score=None
            bits=line[2:202]
            for bit in bits:
                bits2=bit.split(':')
                content.append((float(bits2[1]),voca[int(bits2[0])]))
        else:
            score=line[2]
            bits=line[3:203]
            for bit in bits:
                bits2=bit.split(':')
                content.append((float(bits2[1]),voca[int(bits2[0])]))
        content.sort()
        content.reverse()
        if fc:
            max=content[0][0]
            fc=False
        content=[(round(x*32/max),y) for x,y in content]
        contents.append((size,num,iid,content))

    return render_template('wordclouds.html',
            info=info,
            contents=contents,
            corpus=corpus,
            type=type,
            iids=iids)

@app.route('/showimage/<fun>/<corpus>/<type>/<filename>')
def showimage(fun,corpus,type,filename):
    clusdir='clusters/'+corpus+'/'+type
    if fun.startswith('cover'):
        image='clusters/'+corpus+'/'+type+'/'+filename[:-5]+"_hist_doc.png"
    else:
        image='clusters/'+corpus+'/'+type+'/'+filename[:-5]+"_hist_word.png"
    info=[]
    info=None
    if os.path.exists(clusdir+filename+'.info'):
        with open(clusdir+filename+".info") as fi:
            info=yaml.load(fi)
    return render_template('showimage.html',
            info=info,
            image=image)



@app.route('/wordcloud/<corpus>/<type>/<filename>/<int:iid>')
def wordcloud(corpus,type,filename,iid):
    clusdir='clusters/'+corpus+'/'+type+'/'
    info=[]
    info=None
    if os.path.exists(clusdir+filename+'.info'):
        with open(clusdir+filename+".info") as fi:
            info=yaml.load(fi)
    content=[]
    line=None
    with open(clusdir+filename+".txt") as kf:
        for iline, tline in enumerate(kf):
            if not iline == iid:
                continue
            line=tline.strip().split()
            break
    num=line[0]
    size=line[1]
    content=[]
    if ':' in line[2]:
        score=None
        bits=line[2:202]
        for bit in bits:
            bits2=bit.split(':')
            content.append((float(bits2[1]),voca[int(bits2[0])]))
    else:
        score=line[2]
        bits=line[3:203]
        for bit in bits:
            bits2=bit.split(':')
            content.append((float(bits2[1]),voca[int(bits2[0])]))
    content.sort()
    content.reverse()
    max=content[0][0]
    content=[(round(x*32/max),y) for x,y in content]

    return render_template('wordcloud.html',
            info=info,
            content=content,
            iid=iid,
            size=size,
            num=num,
            score=score)


@app.route('/vocab/<corpus>/<type>/<filename>/<int:iid>')
def vocab(corpus,type,filename,iid):
    info=[]
    info=None
    clusdir='clusters/'+corpus+'/'+type+'/'
    if os.path.exists(clusdir+filename+'.info'):
        with open(clusdir+filename+".info") as fi:
            info=yaml.load(fi)
    content=[]
    line=None
    with open(clusdir+filename+".txt") as kf:
        for iline, tline in enumerate(kf):
            if not iline == iid:
                continue
            line=tline.strip().split()
            break
    num=line[0]
    size=line[1]
    content=[]
    if ':' in line[2]:
        score=None
        bits=line[2:]
        for bit in bits:
            bits2=bit.split(':')
            content.append((float(bits2[1]),voca[int(bits2[0])]))
    else:
        score=line[2]
        bits=line[3:]
        for bit in bits:
            bits2=bit.split(':')
            content.append((float(bits2[1]),voca[int(bits2[0])]))
    content.sort()
    content.reverse()
    return render_template('vocab.html',iid=iid,info=info,content=content,num=num,size=size,score=score)

