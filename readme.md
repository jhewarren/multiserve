Comp 8005 Computer Systems Technology February 2018
Network and Security Applications Development
Assignment #2

## Objective
To compare the scalability and performance of the select-, multi-threaded-, and epoll-based client-server implementations.

## Requirements
Design and implement 3 separate servers
 - Multi Threaded (traditional)
 - Select (level-triggered) multiplexed
 - epoll (edge-triggered) asynchronous
all servers must 
 - handle multiple connections
 - transfer specified data to connecgted client
extended echo server
echo client
 - send variable length strings
 - send configurable quantity
 - maintain connection
 - varying duration
increase load of each server type
track when performance degrades
track number of connections 
track turnaround time (performance) as load increases
load vs performance
maintain stats on both server and client
multiple instances on multiple machines
