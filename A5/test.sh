if [ "$1" = "EXT" ]; then
    make
    echo "Check bad inputs"

    echo "V % G != 0 -> expected usage message"
    ./vote 5 4
    echo "V < G      -> expected usage message"
    ./vote 4 5
    echo "G % 2 == 0 -> expected usage message"
    ./vote 8 4
    echo "V <= 0     -> expected usage message"
    ./vote 0 3
    echo "G <= 0     -> expected usage message"
    ./vote 9 0
    echo "seed < 0   -> expected usage message"
    ./vote 9 -3 -23
fi;

make clean
make TYPE=$1

echo "Test results for type=$1"

echo "default values"
./vote 6 3 2
echo "only 1 voter"
./vote 1 1 2
echo "big number of tasks"
./vote 21 3 2
