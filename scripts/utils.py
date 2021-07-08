#!/usr/bin/python
# -*- coding: utf-8 -*-
# Copyright (c) 2016, Giuseppe Grieco <g.grieco1997@gmail.com>
#
# Permission to use, copy, modify, and/or distribute this software for any purpose
# with or without fee is hereby granted, provided that the above copyright notice
# and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
# REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
# FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT,
# OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
# DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
# ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
# SOFTWARE.

__author__ = 'giuseppegrieco'

import random
import argparse

# Get the upper part of the adj matrix of er graph and
# write the resulting graph in the output file
def save_dag(G, start, target, numNodes, labels, outputFile):
    f = open(outputFile, 'w')
    f.write(str(target) + '\n')
    f.write(str(start) + '\n')
    for i in range(0, numNodes):
        for j in range(i, numNodes):
            if G.has_edge(i, j):
                f.write(str(i) + '[' + labels[i] + '],' + str(j) + '[' + labels[j] + ']\n')

def uniform_labels(numNodes, labelMin, labelMax):
    labels = []
    for i in range(0, numNodes):
        labels.append(str(int(random.uniform(labelMin, labelMax))))
    return labels

def parser_base(description):
    parser = argparse.ArgumentParser(
        description=description
    )
    parser.add_argument(
        'numNodes', 
        help='number of nodes to create',
        type=int
    )
    parser.add_argument(
        'start', 
        help='starting node',
        type=int
    )
    parser.add_argument(
        'labelMin', 
        help='labels minimum value',
        type=int
    )
    parser.add_argument(
        'labelMax', 
        help='labels maximum value',
        type=int
    )
    parser.add_argument(
        'target', 
        help='label target',
        type=int
    )
    parser.add_argument(
        'outputFile', 
        help='the path to the output files'
    )
    parser.add_argument(
        '--seed', 
        help='seed for the generation of the graph',
        type=int,
        default=None
    )
    return parser

def probability_type(val):
    x = float(val)
    if x >= 0 and x <= 1:
        return x
    raise ValueError(x)