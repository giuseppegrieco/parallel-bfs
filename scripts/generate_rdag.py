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
import time
import random
import argparse

import networkx as nx

def probability_type(val):
    x = float(val)
    if x >= 0 and x <= 1:
        return x
    raise ValueError(x)

parser = argparse.ArgumentParser('It generates random DAG using Erdos-Renyi model')
parser.add_argument(
    'numNodes', 
    help='number of nodes to create',
    type=int
)
parser.add_argument(
    'p', 
    help='probability of attaching one edge', 
    type=probability_type
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
    'outputFile', 
    help='the path to the output files'
)
parser.add_argument(
    '--seed', 
    help='seed for the generation of the graph',
    type=int,
    default=None
)
args = parser.parse_args()
p = args.p
numNodes = args.numNodes
outputFile = args.outputFile
labelMin = args.labelMin
labelMax = args.labelMax
seed = args.seed
if seed != None:
    seed = random.seed(seed)

# Generates an er graph of `numNodes` nodes
G = nx.fast_gnp_random_graph(numNodes, p, directed=True, seed=seed)

labels = []
for i in range(0, numNodes):
    labels.append(str(int(random.uniform(labelMin, labelMax))))

f = open(outputFile, 'w')
f.write(str(numNodes) + '\n')
for i in range(0, numNodes):
    for j in range(i, numNodes):
        if G.has_edge(i, j):
            f.write(str(i) + '[' + labels[i] + '],' + str(j) + '[' + labels[j] + ']\n')
