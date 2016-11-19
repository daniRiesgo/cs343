make clean
for type in "EXT" "INT" "INTB" "AUTO" "TASK"
do
    sh test.sh ${type} > out${type}.txt 2> out${type}.txt
done
make clean
