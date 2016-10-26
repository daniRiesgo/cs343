# Check bad inputs
echo "Should output usage message"
echo ""
./givenbuffer 1 2 3 4 5 6
./buffer 1 2 3 4 5 6
echo ""
echo "Should warn of bad cons:"
echo ""
./givenbuffer -1
./buffer -1
echo ""
echo "Should warn of bad prods:"
echo ""
./givenbuffer 1 -2
./buffer 1 -2
echo ""
echo "Should warn of bad cons:"
echo ""
./givenbuffer 0
./buffer 0
echo ""
echo "Should warn of bad produce:"
echo ""
./givenbuffer 1 2 0
./buffer 1 2 0
echo ""
echo "Should warn of bad produce:"
echo ""
./givenbuffer 5 3 -1
./buffer 5 3 -1
echo ""
echo "Should warn of bad produce:"
echo ""
./givenbuffer 5 3 2147483650
./buffer 5 3 2147483650
echo ""
echo "Should warn of bad bufsize:"
echo ""
./givenbuffer 5 3 10 0
./buffer 5 3 10 0
echo ""
echo "Should warn of bad bufsize:"
echo ""
./givenbuffer 5 3 10 -1
./buffer 5 3 10 -1
echo ""
echo "Should warn of bad bufsize:"
echo ""
./givenbuffer 5 3 10 100000000000
./buffer 5 3 10 100000000000
echo ""
echo "Should warn of bad delay:"
echo ""
./givenbuffer 5 3 10 10 -1
./buffer 5 3 10 10 -1
echo ""
echo "Should warn of bad prods:"
echo ""
./givenbuffer 5 43222222222222
./buffer 5 43222222222222
echo ""
echo "Should warn of bad prods+cons:"
echo ""
./givenbuffer 2147483640 40
./buffer 2147483640 40
echo ""

# Check corner cases
echo "Total should be 55"
echo ""
./givenbuffer 1 1
./buffer 1 1
echo ""
echo "Total should be 165"
echo ""
./givenbuffer
./buffer
echo ""
echo "Total should be 15150"
echo ""
./givenbuffer 5 3 100
./buffer 5 3 100
echo ""
echo "Total should be 15015000"
echo ""
./givenbuffer 20 30 100
./buffer 20 30 100
echo ""
echo "Total should be 60000"
echo ""
# ./givenbuffer 1 10000 3 30000
# ./buffer 1 10000 3 30000
