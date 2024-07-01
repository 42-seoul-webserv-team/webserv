#!/bin/bash

i=0;
while true;
do
	start_time=$(date +%s);
	curl -s -o /dev/null http://localhost:8080
	if [ $? -ne 0 ]; then
		echo "fail : $i";
		break;
	fi
	end_time=$(date +%s);
	excution_time=$((end_time - start_time));
	echo "success[$i] || time[$excution_time] : $start_time ~ $end_time";
	i=$((i + 1));
	if [ $i -eq $TEST ]; then
		break
	fi
done
