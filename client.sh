#!/bin/bash

echo "This script will run the client program\nAgainst threaded (ths) signal (polls) and epoll (eps) servers\n\n"

echo "start the first server now\n\n"
read -p "Press any key to continue..." -n1

./mec 192.168.0.12 54321 1 1000 > eps1x1k.pthread_exit
