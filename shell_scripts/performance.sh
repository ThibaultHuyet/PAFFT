end=$((SECONDS+5))

while [ $SECONDS -lt $end ];
do
        total=$(ps --no-headers -a -x -o pid,%cpu,%mem,command | awk '{cpu += $2; mem += $3} END {print cpu, mem}')
        dockerd=$(ps --no-headers -a -x -o pid,%cpu,%mem,command | grep dockerd)
        container=$(ps --no-headers -a -x -o pid,%cpu,%mem,command | grep docker-cont)
        program=$(ps --no-headers -a -x -o pid,%cpu,%mem,command | grep main)

        totalarray=($total)
        dockerdarray=($dockerd)
        containerarray=($container)
        programarray=($program)

        unix_time=$(date '+%s')

        mosquitto_pub -h '52.50.208.168' -t 'Nimbus/Top/2/Performance' -m '{"loc": "Nimbus/Top/2", "time":'"$unix_time"',"performance" : {"tot$
{"cpu" :'"${totalarray[0]}"', "mem" :'"${totalarray[1]}"'}, "dockerd" : {"cpu":
'"${dockerdarray[1]}"', "mem":'"${dockerdarray[2]}"'}, "container":{"cpu":
'"${containerarray[1]}"', "mem":'"${containerarray[2]}"'},
"program":{"cpu":'"${programarray[1]}"', "mem":'"${programarray[2]}"'}}}'

done






