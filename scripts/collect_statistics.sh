#!/bin/bash
mkdir -p ../statistics
mkdir -p ../statistics/timer
mkdir -p ../statistics/w_timer
mkdir -p ../statistics/node_timer
mkdir -p ../statistics/visited_timer
mkdir -p ../statistics/i_timer
for density in 2 4 8
do
    ../build/bfs-sequential-timer ../examples/10000-0$density.dat 0 5 $nw 64 >> ../statistics/timer/seq-10000-0$density-$nw-$i.out
    ../build/bfs-sequential-i-timer ../examples/10000-0$density.dat 0 5 $nw 64 >> ../statistics/i_timer/10000-0$density-$nw-$i.out
    for i in {1..10}
    do
        for nw in 2 4 8 16 32
        do
            ../build/bfs-pthread-timer ../examples/10000-0$density.dat 0 5 $nw 64 >> ../statistics/timer/par-10000-0$density-$nw-$i.out
            ../build/bfs-pthread-w-timer ../examples/10000-0$density.dat 0 5 $nw 64 >> ../statistics/w_timer/10000-0$density-$nw-$i.out
            ../build/bfs-pthread-node-timer ../examples/10000-0$density.dat 0 5 $nw 64 >> ../statistics/node_timer/10000-0$density-$nw-$i.out
            ../build/bfs-pthread-visited-timer ../examples/10000-0$density.dat 0 5 $nw 64 >> ../statistics/visited_timer/10000-0$density-$nw-$i.out
        done
    done
done