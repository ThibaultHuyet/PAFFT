end=$((SECONDS+5))

echo ${stringarray[1]}
echo ${stringarray[2]}
echo ${stringarray[3]}
echo ${stringarray[10]}

while [ $SECONDS -lt $end ];
do
	var=$(ps -aux | grep "./number")
	stringarray=($var)

	mosquitto_pub -t 'hello' -m "{'PID' : ${stringarray[1]}, 'CPU%' : ${stringarray[2]}, 'MEM%' : ${stringarray[3]}, 'Process Name' : ${stringarray[10]}}"
done