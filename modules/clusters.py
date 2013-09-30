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
from Utils import len_clusters,stats


def analysedirtime(dir_,types,sections):
    time={}
    mem={}
    t_time={}
    t_mem={}

    for type in types:
        if '/' in type:
            type_=os.path.basename(type)
        else:
            type_=type

        dir=dir_+'/'+type
        txt=[]
        mem[type]={}
        time[type]={}
        for isec,(sec,regs) in enumerate(sections):
            txt.append(rst_title(sec,u'',u'-'))
            for la,reg in regs:
                time[type][la]=[]
                mem[type][la]=[]
                re_file=re.compile(reg)
                listing=[x for x in os.listdir(dir) if re_file.match(x)]
                for file in listing:
                    filename="{0}/{1}".format(dir,file)
                    sts=stats(filename)
                    time[type][la].append(float(sts['User time (seconds)']))
                    mem[type][la].append(float(sts['Maximum resident set size (kbytes)']))


    lists=[]
    for type,la_vals in time.iteritems():
        list=[]
        lists.append((type,list))
        for la,vals in la_vals.iteritems():
            row=[]
            row.append(la)
            row.append(np.min(vals))
            row.append(np.mean(vals))
            row.append(np.max(vals))
            list.append(row)

    for type,la_vals in mem.iteritems():
        list=[]
        lists.append((type,list))
        for la,vals in la_vals.iteritems():
            row=[]
            row.append(la)
            row.append(np.min(vals))
            row.append(np.mean(vals))
            row.append(np.max(vals))
            list.append(row)


    txt.append(rst_title('Time',u'',u'>'))
    for type,list in lists:
        txt.append(rst_text(type))
        txt.append(rst_table(list))



    return u"".join(txt)






def analysedir(dir,type,sections):
    if '/' in type:
        type_=os.path.basename(type)
    else:
        type_=type

    dir=dir+'/'+type
    txt=[]

    for isec,(sec,regs,title,xl,yl) in enumerate(sections):
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
        fn1=rst_plot(maxs,labels,u'boxplot','max'+type_,prefix="sec{0}_".format(isec),stitle=title,xl=xl,yl=yl)
        fn2=rst_plot(lens,labels,u'boxplot','len'+type_,prefix="sec{0}_".format(isec),stitle=title,xl=xl,yl=yl)
        fn3=rst_plot(avgs,labels,u'boxplot','avgs'+type_,prefix="sec{0}_".format(isec),stitle=title,xl=xl,yl=yl)
        txt.extend([
            rst_images([(fn1,{'scale':u'80%','align':'center'})]),
            rst_images([(fn2,{'scale':u'80%','align':'center'})]),
            rst_images([(fn3,{'scale':u'80%','align':'center'})])
                ])
                 
        

    return u"".join(txt)
    

def rst_plot(data,xs,typpe,label,prefix="",stitle="",baseline=None,xl=None,yl=None):
    if not os.path.exists('imgs'):
        os.mkdir('imgs')
    filename=u'{0}{1}_{2}'.format(prefix,label,typpe).encode('ascii','ignore')

    fig_width_pt = 198.74  # Get this from LaTeX using \showthe\columnwidth
    inches_per_pt = 1.0/72.27               # Convert pt to inch
    golden_mean = (sqrt(5)-1.0)/2.0         # Aesthetic ratio
    fig_width = fig_width_pt*inches_per_pt  # width in inches
    fig_height = fig_width*golden_mean      # height in inches
    fig_size =  [fig_width,fig_height]
    params = {'backend': 'ps',
            'axes.labelsize': 6,
            'text.fontsize': 6,
            'legend.fontsize': 6,
            'xtick.labelsize': 6,
            'ytick.labelsize': 6,
            'text.usetex': True,
            'figure.figsize': fig_size}
    rcParams.update(params)


    filename=filename.strip()
    filename=filename.replace(' ','_')
    filename=filename.replace('{','')
    filename=filename.replace('}','')
    filename=filename.replace('$','')
    filename=filename.replace('\\','')
    filename='imgs/{0}.eps'.format(filename)
    print filename
    figure()
    axes([0.2,0.2,0.90-0.2,0.82-0.2])
    if typpe.startswith('boxplot'):
        title(stitle)
        ylabel(yl)
        xlabel(xl)
        boxplot(data)
        xticks(range(1,len(xs)+1),xs)
        locs,labels = yticks()
        yticks(locs,["{0}k".format(l/1000) for l in  locs])

    savefig(filename)
    return filename
