end=$((SECONDS+5))

this_script=$$

while [ $SECONDS -lt $end ];
do
        total=$(ps --no-headers -a -x -o pid,%cpu,%mem,command | awk '{cpu += $2; mem += $3} END {print cpu, mem}')
        dockerd=$(ps --no-headers -a -x -o pid,%cpu,%mem,command | grep dockerd)
        container=$(ps --no-headers -a -x -o pid,%cpu,%mem,command | grep docker-cont)
        program=$(ps --no-headers -a -x -o pid,%cpu,%mem,command | grep main)
	shell=$(ps --no-headers -a -x -o pid,%cpu,%mem,command | grep "$this_Script")

        totalarray=($total)
        dockerdarray=($dockerd)
        containerarray=($container)
        programarray=($program)
	shellarray=($shell)

        unix_time=$(date '+%s')

        mosquitto_pub -h 'localhost' -t 'local' -m '{"loc": "Nimbus/Top/2", "time":'"$unix_time"',"performance" : {"tot$
{"cpu" :'"${totalarray[0]}"', "mem" :'"${totalarray[1]}"'}, "shell":{"cpu":'"${shellarray[1]}"', "mem":'"${shellarray[2]}"'}"dockerd" : {"cpu":
'"${dockerdarray[1]}"', "mem":'"${dockerdarray[2]}"'}, "container":{"cpu":
'"${containerarray[1]}"', "mem":'"${containerarray[2]}"'},
"program":{"cpu":'"${programarray[1]}"', "mem":'"${programarray[2]}"'}}}'

done
