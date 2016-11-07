for type in "MC" "SEM" "BAR"
do
    sh test.sh ${type} > out${type}.txt >> out${type}.txt
done
