
for k in "STACK" "VECTOR2" "VECTOR1" "DARRAY"
do
    make new KIND=$k
    for t in 1 2 4
    do
        # for i in {1..4}
        # do
            time -f "%Uu %Ss %E" ./new $t 10000000
        # done
    done
done
