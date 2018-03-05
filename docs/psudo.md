**poll**
	create socket for listening for clients
	create poll struct
	add socket to poll

	wait for poll event
	if error in event
		remove client from data strctires
	if a file discriptor has data to read
		if it's a new connection
			make connection non blocking
			add connection to poll
		else it's an message
			echo back
	if event is we can write echo harder

**epoll**
	create socket for listening for clients
	create epoll struct
	add socket to epoll

	on all physical threads
		wait for epoll event
		if error in event
			remove client from data strctires
		if a file discriptor has data to read
			if it's a new connection
				make connection non blocking
				add connection to epoll
			else it's an message
				echo back
		if event is we can write echo harder

**threads**
	create socket for listening for clients
	infinite loop 
		pause untill a connection is accepted
		launch connection handler thread with client file discriptor

connection handler
	allocate buffer
	while recieving message from client
		send message back
		if recieve is not getting 0 input
			close socket
			close thread
