#!/bin/bash

####################
# Lab 1 Exercise 5
# Name: Yeap Yi Sheng James
# Student No: A0218234L
# Lab Group: B09
####################

# Fill the below up
hostname=$HOSTNAME
machine_hardware=$(uname -s -p)
max_user_process_count=$(ulimit -u)
user_process_count=$(ps -u | wc -l)
user_with_most_processes=$(ps -eo user | sort | uniq -c | sort -r | head -n 1 | awk '{print $2}')
mem_free_percentage=$(free | grep Mem | awk '{print $4/$2 * 100.0}')
swap_free_percentage=$(free | grep Swap | awk '{print $4/$2 * 100.0}')

echo "Hostname: $hostname"
echo "Machine Hardware: $machine_hardware"
echo "Max User Processes: $max_user_process_count"
echo "User Processes: $user_process_count"
echo "User With Most Processes: $user_with_most_processes"
echo "Memory Free (%): $mem_free_percentage"
echo "Swap Free (%): $swap_free_percentage"
