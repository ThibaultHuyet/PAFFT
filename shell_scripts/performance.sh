end=$((SECONDS+5))

echo ${stringarray[1]}
echo ${stringarray[2]}
echo ${stringarray[3]}
echo ${stringarray[10]}

while [ $SECONDS -lt $end ];
do
	var=$(ps -aux | grep "./number")
	stringarray=($var)

	echo ${stringarray[1]}
	echo ${stringarray[2]}
	echo ${stringarray[3]}
	echo ${stringarray[10]}
done