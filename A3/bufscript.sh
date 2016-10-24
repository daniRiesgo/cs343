# Check bad inputs
echo "Should warn of bad cons:"
./buffer -1
echo "Should warn of bad prods:"
./buffer 1 -2
echo "Should warn of bad cons:"
./buffer 0
echo "Should warn of bad produce:"
./buffer 1 2 0
echo "Should warn of bad produce:"
./buffer 5 3 -1
echo "Should warn of bad produce:"
./buffer 5 3 2147483650
echo "Should warn of bad bufsize:"
./buffer 5 3 10 0
echo "Should warn of bad bufsize:"
./buffer 5 3 10 -1
echo "Should warn of bad bufsize:"
./buffer 5 3 10 100000000000
echo "Should warn of bad delay:"
./buffer 5 3 10 10 -1
echo "Should warn of bad prods:"
./buffer 5 43222222222222
echo "Should warn of bad prods+cons:"
./buffer 2147483640 40

# Check corner cases
echo "Total should be 55"
./buffer 1 1
echo "Total should be 165"
./buffer
echo "Total should be 15150"
./buffer 5 3 100
echo "Total should be 165"
./buffer 10000 3 10
echo "Total should be 60000"
./buffer 1 10000 3 30000
