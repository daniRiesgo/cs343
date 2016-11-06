nano time_results
for k in "STACK" "VECTOR2" "VECTOR1" "DARRAY"
do
    make new KIND=$k
    echo "Version: $k\n\n" > time_results
    for t in 1 2 4
    do
        echo "With $t task:\n" > time_results
        for i in {1..4}
        do
            time -f "%Uu %Ss %E" ./new $t 10000000 > time_results
        done
    done
    echo "\n\n" > time_results
done
