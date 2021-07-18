mkdir -p ../results
mkdir -p ../results/timer
mkdir -p ../results/w_timer
mkdir -p ../results/node_timer
mkdir -p ../results/visited_timer
mkdir -p ../results/i_timer
for density in 2 4 8
do
    ../build/bfs-sequential ../examples/10000-0$density.dat 0 5 $nw 64 >> ../results/sequential-10000-0$density-$nw-$i.out
    for i in {1..10}
    do
        for nw in 2 4 8 16 32
        do
            ../build/bfs-pthread ../examples/10000-0$density.dat 0 5 $nw 64 >> ../results/pthread-10000-0$density-$nw-$i.out
            ../build/bfs-fastflow ../examples/10000-0$density.dat 0 5 $nw 64 >> ../results/fastflow-10000-0$density-$nw-$i.out
        done
    done
done