for type in "MC" "SEM" "BAR"
do
    sh test.sh ${type} > out${type}.txt 2> out${type}.txt
done
