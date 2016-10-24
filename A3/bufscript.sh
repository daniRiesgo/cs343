# Check bad inputs
echo ""
echo "Should warn of bad cons:"
echo ""
./buffer -1
echo ""
echo "Should warn of bad prods:"
echo ""
./buffer 1 -2
echo ""
echo "Should warn of bad cons:"
echo ""
./buffer 0
echo ""
echo "Should warn of bad produce:"
echo ""
./buffer 1 2 0
echo ""
echo "Should warn of bad produce:"
echo ""
./buffer 5 3 -1
echo ""
echo "Should warn of bad produce:"
echo ""
./buffer 5 3 2147483650
echo ""
echo "Should warn of bad bufsize:"
echo ""
./buffer 5 3 10 0
echo ""
echo "Should warn of bad bufsize:"
echo ""
./buffer 5 3 10 -1
echo ""
echo "Should warn of bad bufsize:"
echo ""
./buffer 5 3 10 100000000000
echo ""
echo "Should warn of bad delay:"
echo ""
./buffer 5 3 10 10 -1
echo ""
echo "Should warn of bad prods:"
echo ""
./buffer 5 43222222222222
echo "Should warn of bad prods+cons:"
echo ""
./buffer 2147483640 40
echo ""

# Check corner cases
echo "Total should be 55"
echo ""
./buffer 1 1
echo ""
echo "Total should be 165"
echo ""
./buffer
echo ""
echo "Total should be 15150"
echo ""
./buffer 5 3 100
echo ""
echo "Total should be 165"
echo ""
./buffer 10000 3 10
echo ""
echo "Total should be 60000"
echo ""
./buffer 1 10000 3 30000
