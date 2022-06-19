
echo "C.cat /dev/local/metronom" 
echo "Expected: [metronome: 120 beats/min, time signature 2/4, secs-per-interval: 0.25, nanoSecs: 250000000]"
echo "\nOutput:"
cat /dev/local/metronome

sleep 2

echo "\n"


echo "E.echo set 100 2 4 >/dev/local/metrnonome\n"
echo "Expected: metronome regmgr changes settings to: 100 bpm in 2/4 time; run-time behaviour of " 
echo set 100 2 4 > /dev/local/metronome
echo "\nOutput:"
cat /dev/local/metronome
sleep 3

echo "\n"

echo "F.cat /dev/local/metronome\n"
echo "Expected: [metronome: 100 beats/min, time signature 2/4, secs-per- interval: 0.30, nanoSecs: 300000000] "
cat /dev/local/metronome
echo "\nOutput:"
sleep 3
echo "\n"

echo "G.echo set 200 5 4 >/dev/local/metrnonome\n"
echo set 200 5 4 > /dev/local/metronome
echo "\nOutput:"
sleep 3
echo "\n"

echo "H.cat /dev/local/metronome\n"
echo "Expected: [metronome: 200 beats/min, time signature 5/4, secs-per- interval: 0.15, nanoSecs: 150000000] "
cat /dev/local/metronome
echo "\nOutput:"
sleep 3
echo "\n"

echo "I.echo stop > /dev/local/metronome\n"
echo stop > /dev/local/metronome 
echo "Expected: metronome stops running; metronome resmgr is still running as a process: pidin | grep 
metronome."
sleep 3
echo "\n"

echo "J.start > /dev/local/metronome\n "
echo start > /dev/local/metronome
sleep 3
echo "\n"

echo "K.cat /dev/local/metronome\n"
echo cat /dev/local/metronome
sleep 3
echo "\n"

echo "L.echo stop > /dev/local/metronome\n"
echo stop > /dev/local/metronome
sleep 3
echo "\n"

echo "M.echo stop > /dev/local/metronome\n"
echo stop > /dev/local/metronome
sleep 3
echo "\n"

echo "N.echo start > /dev/local/metronome\n"
echo start > /dev/local/metronome
sleep 3
echo "\n"

echo "O.echo start > /dev/local/metronome\n"
echo start > /dev/local/metronome
sleep 3
echo "\n"

echo "P.echo cat /dev/local/metronome\n"
echo cat /dev/local/metronome
sleep 3
echo "\n"

echo "Q.echo pause 3 > /dev/local/metronome\n"
echo pause 3 > /dev/local/metronome
sleep 3
echo "\n"

echo "R.echo pause 10 > /dev/local/metronome\n"
echo pause 10 > /dev/local/metronome
sleep 3
echo "\n"

echo "S.echo bogus > /dev/local/metronome\n"
echo bogus > /dev/local/metronome
sleep 3
echo "\n"

echo "T.echo set 120 2 4 > /dev/local/metronome\n"
echo set 120 2 4 > /dev/local/metronome
sleep 3
echo "\n"

echo "U.cat /dev/local/metronome\n"
echo cat /dev/local/metronome
sleep 3
echo "\n"

echo "X.echo quit > /dev/local/metronome && pidin | grep metronome"
echo quit > /dev/local/metronome && pidin | grep metronome
sleep 3
echo "\n"