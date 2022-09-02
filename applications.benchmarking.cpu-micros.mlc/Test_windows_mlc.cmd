mlc_internal.exe
mlc_internal.exe --idle_latency
mlc_internal.exe --idle_latency -e

mlc_internal.exe --latency_matrix
mlc_internal.exe --latency_matrix -a

mlc_internal.exe --bandwidth_matrix
mlc_internal.exe --peak_bandwidth
mlc_internal.exe --c2c_latency

mlc_internal.exe --loaded_latency
mlc_internal.exe --loaded_latency -T -d0 -R
mlc_internal.exe --loaded_latency -T -d0 -W2
mlc_internal.exe --loaded_latency -T -d0 -W3
mlc_internal.exe --loaded_latency -T -d0 -W4
mlc_internal.exe --loaded_latency -T -d0 -W5
mlc_internal.exe --loaded_latency -T -d0 -W6
mlc_internal.exe --loaded_latency -T -d0 -W7
mlc_internal.exe --loaded_latency -T -d0 -W8
mlc_internal.exe --loaded_latency -T -d0 -W9
mlc_internal.exe --loaded_latency -T -d0 -W10
mlc_internal.exe --loaded_latency -T -d0 -W11
mlc_internal.exe --loaded_latency -T -d0 -W12

mlc_internal.exe --loaded_latency -T -d0 -R -Y
mlc_internal.exe --loaded_latency -T -d0 -W2 -Y
mlc_internal.exe --loaded_latency -T -d0 -W3 -Y
mlc_internal.exe --loaded_latency -T -d0 -W4 -Y
mlc_internal.exe --loaded_latency -T -d0 -W5 -Y
mlc_internal.exe --loaded_latency -T -d0 -W6 -Y
mlc_internal.exe --loaded_latency -T -d0 -W7 -Y
mlc_internal.exe --loaded_latency -T -d0 -W8 -Y
mlc_internal.exe --loaded_latency -T -d0 -W9 -Y
mlc_internal.exe --loaded_latency -T -d0 -W10 -Y
mlc_internal.exe --loaded_latency -T -d0 -W11 -Y
mlc_internal.exe --loaded_latency -T -d0 -W12 -Y

mlc_internal.exe --loaded_latency -T -d0 -U -R
mlc_internal.exe --loaded_latency -T -d0 -U -W2
mlc_internal.exe --loaded_latency -T -d0 -U -W5
mlc_internal.exe --loaded_latency -T -d0 -U -W6

mlc_internal.exe --loaded_latency -T -d0 -R -k1-2
mlc_internal.exe --loaded_latency -T -d0 -R -mf
mlc_internal.exe --loaded_latency -T -d0 -R -X

echo 1-12 W5 rand 4772185 dram 0 >L1.TXT
echo 49-60 W5 rand 4772185 dram 1 >>L1.TXT 
mlc_internal.exe --loaded_latency -oL1.TXT

echo 1-12 W5 rand 4772185 dram 1 >L1.TXT
echo 49-60 W5 rand 4772185 dram 0 >>L1.TXT
mlc_internal.exe --loaded_latency -oL1.TXT

echo 1-12 W8 seq 4772185 dram 0 >L1.TXT
echo 49-60 W8 seq 4772185 dram 1 >>L1.TXT
mlc_internal.exe --loaded_latency -oL1.TXT

echo 1-12 W8 seq 4772185 dram 1 >L1.TXT
echo 49-60 W8 seq 4772185 dram 0 >>L1.TXT
mlc_internal.exe --loaded_latency -oL1.TXT

echo 1-6 W6 rand 4772185 dram 0 >L1.TXT
echo 7-12 R rand 4772185 dram 0 >>L1.TXT
echo 24-34 R rand 4772185 dram 0 >>L1.TXT
echo 49-54 W6 rand 4772185 dram 1 >>L1.TXT
echo 55-60 R rand 4772185 dram 1 >>L1.TXT
echo 72-82 R rand 4772185 dram 1 >>L1.TXT
mlc_internal.exe --loaded_latency -oL1.TXT

echo 1-6 W6 rand 4772185 dram 1 >L1.TXT
echo 7-12 R rand 4772185 dram 1 >>L1.TXT
echo 24-34 R rand 4772185 dram 1 >>L1.TXT
echo 49-54 W6 rand 4772185 dram 0 >>L1.TXT
echo 55-60 R rand 4772185 dram 0 >>L1.TXT
echo 72-82 R rand 4772185 dram 0 >>L1.TXT
mlc_internal.exe --loaded_latency -oL1.TXT

echo 1-12 W5 seq 4772185 dram 0 >L1.TXT
echo 49-60 W5 seq 4772185 dram 1 >>L1.TXT
mlc_internal.exe --loaded_latency -oL1.TXT

echo 1-12 W5 seq 4772185 dram 1 >L1.TXT
echo 49-60 W5 seq 4772185 dram 0 >>L1.TXT
mlc_internal.exe --loaded_latency -oL1.TXT

echo 1-6 W6 seq 4772185 dram 0 >L1.TXT
echo 7-12 R seq 4772185 dram 0 >>L1.TXT
echo 24-34 R seq 4772185 dram 0 >>L1.TXT
echo 49-54 W6 seq 4772185 dram 1 >>L1.TXT
echo 55-60 R seq 4772185 dram 1 >>L1.TXT
echo 72-82 R seq 4772185 dram 1 >>L1.TXT
mlc_internal.exe --loaded_latency -oL1.TXT

echo 1-6 W6 seq 4772185 dram 1 >L1.TXT
echo 7-12 R seq 4772185 dram 1 >>L1.TXT
echo 24-34 R seq 4772185 dram 1 >>L1.TXT
echo 49-54 W6 seq 4772185 dram 0 >>L1.TXT
echo 55-60 R seq 4772185 dram 0 >>L1.TXT
echo 72-82 R seq 4772185 dram 0 >>L1.TXT
mlc_internal.exe --loaded_latency -oL1.TXT

echo 1-12 W5 rand 2000 pmem S:\ >L1.TXT
echo 49-60 W5 rand 2000  pmem T:\ >>L1.TXT
mlc_internal.exe --loaded_latency -oL1.TXT

echo 1-12 W5 rand 2330168 pmem S:\ >L1.TXT
echo 49-60 W5 rand 2330168 pmem T:\ >> L1.TXT
mlc_internal.exe --loaded_latency -oL1.TXT

echo 1-12 W5 rand 2097152 pmem S:\ >L1.TXT
echo 49-60 W5 rand 2097152 pmem T:\ >>L1.TXT
mlc_internal.exe --loaded_latency -oL1.TXT

echo 1-12 W5 rand 2097152K pmem S:\ >L1.TXT
echo 49-60 W5 rand 2097152K pmem T:\ >>L1.TXT
mlc_internal.exe --loaded_latency -oL1.TXT

echo 1-12 W5 rand 2048M pmem S:\ >L1.TXT
echo 49-60 W5 rand 2048M pmem T:\ >>L1.TXT
mlc_internal.exe --loaded_latency -oL1.TXT

echo 1-12 W5 rand 2G pmem S:\ >L1.TXT
echo 49-60 W5 rand 2G pmem T:\ >>L1.TXT
mlc_internal.exe --loaded_latency -oL1.TXT

echo 1-12 W5 rand 2000 pmem S:\ >L1.TXT
echo 49-60 W5 rand 2000 pmem T:\ >>L1.TXT
mlc_internal.exe --loaded_latency -oL1.TXT

echo 1-12 W5 rand 2M pmem S:\ >L1.TXT
echo 49-60 W5 rand 2M pmem T:\ >>L1.TXT
mlc_internal.exe --loaded_latency -oL1.TXT

echo 1-12 W5 rand 2275K pmem S:\ >L1.TXT
echo 49-60 W5 rand 2275K pmem T:\ >>L1.TXT
mlc_internal.exe --loaded_latency -oL1.TXT

echo 1-12 W5 rand 2329600  pmem S:\ >L1.TXT
echo 49-60 W5 rand 2329600  pmem T:\ >>L1.TXT
mlc_internal.exe --loaded_latency -oL1.TXT


