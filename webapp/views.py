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

voca=[]
with open('data/nips/vocab.txt') as nips:
    for line in nips:
        voca.append(line.strip())
        
@app.route('/')
@app.route('/index')
def index():
    return render_template('index.html')

@app.route('/list')
def list():
    listing=[x for x in os.listdir('figs') if x.endswith('.txt')]
    return render_template('list.html',listing=listing)


@app.route('/explore/<filename>')
def explore(filename):
    with open("figs/"+filename) as kf:
        content=[]
        for line in kf:
            line=line.strip().split()
            if ':' in line[2]:
                content.append((line[0],line[1],None,"&".join(line[2:])))
            else:
                content.append((line[0],line[1],line[2],"&".join(line[3:])))
    return render_template('explore.html',filename=filename,content=content)

@app.route('/wordcloud/<num>/<size>')
@app.route('/wordcloud/<num>/<size>/<score>')
def wordcloud(num,size,score=None):
    info=[]
    for x in request.args:
        bits=x.split(':')
        info.append((float(bits[1]),voca[int(bits[0])]))
    info.sort()
    info.reverse()
    max=info[0][0]
    info=[(round(x*32/max),y) for x,y in info[:200]]

    return render_template('wordcloud.html',
            info=info,
            size=size,
            num=num,
            score=score)


@app.route('/vocab/<num>/<size>')
@app.route('/vocab/<num>/<size>/<score>')
def vocab(num,size,score=None):
    info=[]
    for x in request.args:
        bits=x.split(':')
        info.append((float(bits[1]),voca[int(bits[0])]))

    info.sort()
    info.reverse()
    return render_template('vocab.html',info=info,num=num,size=size,score=score)

