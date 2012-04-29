#!/bin/sh

make clean ; make

rm -f debug.op.1 debug.op.2
rm -f debug.diff.1 debug.diff.2

echo -e "\n"
echo -e "##################### DEBUG RUN 1 ##########################\n"
./sim 16 4 debug_traces/debug_trace_gcc1 > debug.op.1
diff -Naur debug_runs/in.16x4.debug_trace_gcc1 debug.op.1 > debug.diff.1
echo -e "cat debug.diff.1\n"
cat debug.diff.1

echo -e "\n"
echo -e "##################### DEBUG RUN 2 ##########################\n"
./sim 32 8 debug_traces/debug_trace_gcc1 > debug.op.2
diff -Naur debug_runs/in.32x8.debug_trace_gcc1 debug.op.2 > debug.diff.2
echo -e "cat debug.diff.2\n"
cat debug.diff.2

echo -e "\n"
rm -f debug.op.1 debug.op.2
rm -f debug.diff.1 debug.diff.2
echo -e "##################### DEBUG RUNS DONE ######################\n"

rm -f validation.op.1 validation.op.2
rm -f validation.diff.1 validation.diff.2

echo -e "\n"
echo -e "##################### VALIDATION RUN 1 ##########################\n"
./sim 64 8 validation_traces/val_trace_gcc1 > validation.op.1
diff -Naur validation_runs/val1.txt validation.op.1 > validation.diff.1
echo "cat validation.diff.1"
cat validation.diff.1

echo -e "\n"
echo -e "##################### VALIDATION RUN 2 ##########################\n"
./sim 16 3 validation_traces/val_trace_perl1 > validation.op.2
diff -Naur validation_runs/val2.txt validation.op.2 > validation.diff.2
echo "cat validation.diff.2"
cat validation.diff.2

echo -e "\n"
rm -f validation.op.1 validation.op.2
rm -f validation.diff.1 validation.diff.2
echo -e "##################### VALIDATION RUNS DONE ######################\n"

make clean ;

