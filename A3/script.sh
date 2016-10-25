echo "student's exec timing"
time ./matrixmultiply 1 5000 10000
time ./matrixmultiply 2 5000 10000
time ./matrixmultiply 4 5000 10000
time ./matrixmultiply 8 5000 10000
time ./matrixmultiply 16 5000 10000

echo "given exec timing"
time ./givenmatrixmultiply 1 5000 10000
time ./givenmatrixmultiply 2 5000 10000
time ./givenmatrixmultiply 4 5000 10000
time ./givenmatrixmultiply 8 5000 10000
time ./givenmatrixmultiply 16 5000 10000

echo "bad inputs"
./matrixmultiply
./matrixmultiply cant parse this
./matrixmultiply -1 2 3
./matrixmultiply 4 3 2 invalidfile infalidvile
./matrixmultiply 4 3 1 3x4.mat 4x4.mat toomanyarguments

# time ./matrixmultiply 3 4 4 3x4.mat 4x4.mat
# time ./givenmatrixmultiply 3 4 4 3x4.mat 4x4.mat
#
# time ./matrixmultiply 4 4 4 4x4.mat 4x4.mat
# time ./givenmatrixmultiply 4 4 4 4x4.mat 4x4.mat
#
# time ./matrixmultiply 4 3 3 4x3.mat 3x3.mat
# time ./givenmatrixmultiply 4 3 3 4x3.mat 3x3.mat
