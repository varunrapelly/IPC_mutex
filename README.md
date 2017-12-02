# IPC_mutex
Example of two different process synchronization using Mutex.

time_server.c - Process updates current time in shared memory. 
timer_client.c - Process reads time updated in shared memory. 
Both the processes use shared mutex, before accessing shared memory (time)
