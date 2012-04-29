#!/bin/sh

# For each benchmark, IPC values for following configuration are to be extracted -
# For N = 1, 2, 4 & 8, one curve is to be produced.
# For every value of N, the values of Scheduling Queues should be varied as belows -
# S = 8, 16, 32, 64, 128 & 256.

make clean ; make

for benchmark in gcc1 perl1 ; do
	rm -f graph_points.$benchmark
	for N in 1 2 4 8 ; do
		echo "N=$N ->" >> graph_points.$benchmark
		echo " "
		for S in 8 16 32 64 128 256 ; do
			echo "$benchmark: $N -> $S"
			./sim $S $N validation_traces/val_trace_$benchmark > op.$benchmark.$N.$S
			IPC=`cat op.$benchmark.$N.$S | grep "IPC" | awk '{print $3}'`
			echo "$S $IPC" >> graph_points.$benchmark
			rm -f op.$benchmark.$N.$S
		done;
		echo "------------------------------------" >> graph_points.$benchmark
	done;
done;

make clean ;

