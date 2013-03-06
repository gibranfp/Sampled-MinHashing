#!/usr/bin/env python
# -*- coding: utf-8
#
# Gibran Fuentes Pineda 
# <gibranfp@turing.iimas.unam.mx>
# IIMAS, UNAM
# 2013
#
# -------------------------------------------------------------------------
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
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
# -------------------------------------------------------------------------
"""
Converts from lists of ids to terms.
"""
import sys
import argparse
import ifutils

def main():
    # command line parser
    try:
        parser = argparse.ArgumentParser()
        parser = argparse.ArgumentParser(
            description="Converts from lists of ids to terms.")
        parser.add_argument("idfile", type=str, 
                            help="file with id lists")
        parser.add_argument("vocab", type=str, 
                            help="file with vocabulary")
        parser.add_argument("termfile", type=str, 
                            help="file with terms")
        args = parser.parse_args()
        if args.idfile and args.vocab and args.termfile:
            idlists = ifutils.read_idlists(args.idfile)
            vocab = ifutils.read_vocab(args.vocab)
            termlists = ifutils.id2term(idlists,vocab)
            ifutils.write_termlists(args.termfile,termlists)
    except SystemExit:
        print "for help use --help"
        sys.exit(2)

if __name__ == "__main__":
    main()
