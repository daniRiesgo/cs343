if $1="MC"
do
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
done

make clean
make vote TYPE=$1

echo "Test results for type=$1"

echo "default values"
./vote 6 3 2
echo "only 1 voter"
./vote 1 1 2
echo "greater number of tasks than processors"
./vote 125 25 2
echo "some very big input"
./vote 3006 501 2
