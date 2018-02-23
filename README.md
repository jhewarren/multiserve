# 8005 Assignment 2

## Outline
Due: February 26, 1200 hrs. You may work in groups of two.

Objective: To compare the scalability and performance of the select-, multi-threaded-, and epoll- based client-server implementations.

## Assignment

Design and implement three separate servers:

1. A multi-threaded, traditional server
2. A select (level-triggered) multiplexed server
3. An epoll (edge-triggered) asynchronous server

all servers must
- handle multiple connections
- transfer specified data to connecgted client

echo client
- send variable length strings
- send configurable quantity of messages
- maintain connection
- **resultant** varying duration

Logging
- track when performance degrades
- track number of connections
- track turnaround time (performance) as load increases
- load vs performance
- maintain stats on both server and client
- multiple instances on multiple machines

Note that you will need to have the client maintain the connection for varying time durations (depending on how much data and iterations). The idea is to keep increasing the load on the server until its performance degrades quite significantly. In other words we want to measure how many (scalability) connections the server can handle, and how fast (performance) it can deliver the data back to the clients.

## Constraints

- The server will maintain a list of all connected clients (host names) and store the list together with the number of requests generated by each client and the amount of data transferred to each client.

- Each client will also maintain a record of how many requests it made to the server, the amount of data sent to server, and the amount of time it took for the server to respond (i.e.,
echo the data back).

- You are required to summarize all your data and findings in a properly formatted technical report. Make extensive use of tables and graphs to support your findings and conclusions.

# Design
## Client
 - need to allow input of server IP, data length, run duration/quantity
 - need to run multiple clients on one box
 - [like to optimize performance by threading clients]
* initial design will be to call clients from bash shell
command line will always be consistent {program} IP client-number payload-length clients msg-quantity [delay]
need to track with wireshark
### script ;
    for i = 1 to nclients;
        cpid[i]=launch client in bg
        list cpid [i]
    wait for input/error
    for i=1 to nclients
        killpid[i]

### client
// receives: server-ip server-port sequence payload-size
// saves: IP(of server), client-number, requests sent, data sent, lowest RTT, highest RTT, avgRTT
capture start time
end time = start time + duration
open sockets; in/out
build message info
while
	sendtime=now()
	send packet()
	listen
	receive packet()
	rcvtime=now()
	count++
	latest = sendtime-rcvtime
	average += latest
	if latest < min then min=latest
	if latest > max then max=latest
end while
capture end time 
avg /= count
log client-number, end time - start time, (= total running time), requests-sent, payload*count (=data-sent), min, max, avg