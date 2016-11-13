echo 'Generating executables'

u++ -g -Wall -O2 -multi -nodebug -DPAD -c q1.cc -o countersPad
u++ -g -Wall -O2 -multi -nodebug -c q1.cc -o countersNoPad
touch output.txt

echo 'Timing results without PAD defined'

time -f "%Uu %Ss %E" ./countersNoPad 10000000 >> output.txt
time -f "%Uu %Ss %E" ./countersNoPad 10000000 >> output.txt

echo 'Timing results with PAD defined'

time -f "%Uu %Ss %E" ./countersPad 10000000 >> output.txt
time -f "%Uu %Ss %E" ./countersPad 10000000 >> output.txt
