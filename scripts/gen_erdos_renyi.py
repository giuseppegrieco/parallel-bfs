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

import networkx as nx

from utils import save_dag
from utils import uniform_labels
from utils import parser_base
from utils import probability_type

parser = parser_base('It generates random DAG using Erdos-Renyi model')
parser.add_argument(
    'p', 
    help='probability of attaching one edge', 
    type=probability_type
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

start_time = time.time()

# Generates an er graph of `numNodes` nodes
G = nx.fast_gnp_random_graph(numNodes, p, directed=True, seed=seed)

save_dag(G, numNodes, uniform_labels(numNodes, labelMin, labelMax), outputFile)
