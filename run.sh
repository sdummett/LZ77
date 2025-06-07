#!/bin/zsh

# Compress 
# rm my_test/test_bin_big.lz77; ./lz77 -s64 -l16 -c my_test/test_bin_big.bin -o my_test/test_bin_big.lz77

# Decompress
gcc *.c -o lz77
rm monprogramme64.exe.txt.lz77
./lz77 -c monprogramme64.exe.txt -o monprogramme64.exe.txt.lz77
rm monprogramme64.exe.txt.out
./lz77 -d monprogramme64.exe.txt.lz77 -o monprogramme64.exe.txt.out

diff -u <(hexdump -C monprogramme64.exe.txt) <(hexdump -C monprogramme64.exe.txt.out)

# gcc -g3 *.c -o lz77
# rm to_compress.txt.lz77
# valgrind --leak-check=full --error-exitcode=1 ./lz77 -c to_compress.txt -o to_compress.txt.lz77 -l32

# rm monprogramme64.exe.txt.lz77
# valgrind --leak-check=full --error-exitcode=1 ./lz77 -c monprogramme64.exe.txt -o monprogramme64.exe.txt.lz77 -l32
