#!/bin/bash

host_ip=$(ip a s eth0 | awk '/inet / {print$2}' | cut -d/ -f1)

echo "Starting test server at: " $host_ip
./test_server $host_ip 50051
