#!/usr/bin/env python
# -*- coding: utf-8
# ----------------------------------------------------------------------
# Library to generate clusters report
# ----------------------------------------------------------------------
# Ivan Vladimir Meza-Ruiz/ ivanvladimir at turing.iimas.unam.mx
# 2013/IIMAS/UNAM
# ----------------------------------------------------------------------
# clusters.py is free software: you can redistribute it and/or modify
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

import os.path
import codecs
import sys
import re

import numpy as np
from pylab import *
from rstutil import *

if os.path.exists('src/pyclusters'):
    sys.path.append('src/pyclusters')
from Utils import len_clusters 


def analysedir(dir,type,sections):
    if '/' in type:
        type_=os.path.basename(type)
    else:
        type_=type

    dir=dir+'/'+type
    txt=[]

    for isec,(sec,regs) in enumerate(sections):
        txt.append(rst_title(sec,u'',u'-'))
        maxs=[]
        avgs=[]
        labels=[]
        lens=[]
        
        for la,reg in regs:
            maxs.append([])
            labels.append(la)
            lens.append([])
            avgs.append([])
            re_file=re.compile(reg)
            listing=[x for x in os.listdir(dir) if re_file.match(x)]
            for file in listing:
                filename="{0}/{1}".format(dir,file)
                data=np.array(len_clusters(filename))
                if len(data)==0:
                    print filename
                    continue
                maxs[-1].append(np.amax(data))
                lens[-1].append(len(data))
                avgs[-1].append(np.average(data))
        fn1=rst_plot(maxs,labels,u'boxplot','max'+type_,prefix="sec{0}_".format(isec),stitle='Maximum size of cluster')
        fn2=rst_plot(lens,labels,u'boxplot','len'+type_,prefix="sec{0}_".format(isec),stitle='Total number of clusters')
        fn3=rst_plot(avgs,labels,u'boxplot','avgs'+type_,prefix="sec{0}_".format(isec),stitle='Average size of cluster')
        txt.extend([
            rst_images([(fn1,{'scale':u'80%','align':'center'})]),
            rst_images([(fn2,{'scale':u'80%','align':'center'})]),
            rst_images([(fn3,{'scale':u'80%','align':'center'})])
                ])
                 
        

    return u"".join(txt)
    

def rst_plot(data,xs,typpe,label,prefix="",stitle="",baseline=None):
    if not os.path.exists('imgs'):
        os.mkdir('imgs')
    filename=u'{0}{1}.{2}'.format(prefix,label,typpe).encode('ascii','ignore')

    filename=filename.strip()
    filename=filename.replace(' ','_')
    filename=filename.replace('{','')
    filename=filename.replace('}','')
    filename=filename.replace('$','')
    filename=filename.replace('\\','')
    filename='imgs/{0}.png'.format(filename)
    print filename
    figure()
    if typpe.startswith('boxplot'):
        title(stitle)
        boxplot(data)
        xticks(range(1,len(xs)+1),xs)
    savefig(filename)
    return filename
