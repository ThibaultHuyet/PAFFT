end=$((SECONDS+5))

while [ $SECONDS -lt $end ];
do
	var=$(ps -aux | grep "./number")
	stringarray=($var)

	pid=${stringarray[1]}
	cpu=${stringarray[2]}
	mem=${stringarray[3]}
	pname=${stringarray[10]}
	
	mosquitto_pub -t 'hello' -m "$pid $cpu $mem $pname"
done