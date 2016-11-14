echo 'Generating executables'

touch output.txt
make
make KIND=PAD

echo 'Timing results without PAD defined'

time -f "%Uu %Ss %E" ./cPAD 10000000 
time -f "%Uu %Ss %E" ./cPAD 10000000

echo 'Timing results with PAD defined'

time -f "%Uu %Ss %E" ./cNPAD 10000000
time -f "%Uu %Ss %E" ./cNPAD 10000000
