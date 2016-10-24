# Check bad inputs
echo "Should warn of bad cons:\n"
./buffer -1
echo "Should warn of bad prods:\n"
./buffer 1 -2
echo "Should warn of bad cons:\n"
./buffer 0
echo "Should warn of bad produce:\n"
./buffer 1 2 0
echo "Should warn of bad produce:\n"
./buffer 5 3 -1
echo "Should warn of bad produce:\n"
./buffer 5 3 2147483650
echo "Should warn of bad bufsize:\n"
./buffer 5 3 10 0
echo "Should warn of bad bufsize:\n"
./buffer 5 3 10 -1
echo "Should warn of bad bufsize:\n"
./buffer 5 3 10 100000000000
echo "Should warn of bad delay:\n"
./buffer 5 3 10 10 -1
echo "Should warn of bad prods:\n"
./buffer 5 43222222222222
echo "Should warn of bad prods+cons:\n"
./buffer 2147483640 40

# Check corner cases
echo "Total should be 55\n"
./buffer 1 1
echo "Total should be 165\n"
./buffer
echo "Total should be 15150\n"
./buffer 5 3 100
echo "Total should be 165\n"
./buffer 10000 3 10
echo "Total should be 60000\n"
./buffer 1 10000 3 30000
