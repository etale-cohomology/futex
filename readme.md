# futexes in C/Linux

Some examples of using [futexes](https://man7.org/linux/man-pages/man2/futex.2.html) in C/Linux to wait until an **arbitrary position** in a **shared memory buffer** reaches an **arbitrary 32-bit value**.  
Based on [Eli Bendersky example](https://eli.thegreenplace.net/2018/basics-of-futexes/).  

2 processes shared a `mmap()`-ed memory buffer (in `/dev/shm`) having the following binary layout:

```
[byte 0x00 .. byte 0x04)  futex
[byte 0x04 .. byte 0x08)  reserved
[byte 0x08 .. byte 0x10)  bdim
[byte 0x10 .. byte   -1]  data
```

`futex.c` is a stand-alone example.  
`srv.c` and `clt.c` are a client-server version of `futex.c`.  
In both programs, the `son`/`client` waits on input from `stdin`. The `father`/`server` waits/sleeps until the `son`/`client` sends data.  

Exit the program by typing `cmd exit` in the `son`/`client`.  

The main problem with this particular `@futex_wait()/futex_wake()` pair is that it's a BUSY LOOP (which I don't know how to avoid), which for me defeats a great part of the appeal of futexes in particular (and synchronization primitives in general), which is to wait without spinning.  

# TODOs

Implement bidirectional communication (ie. "return codes", or something like that: when the the son sends data, the father responds with something).  

# notes

@futex() is a syscall to do IPC/ITC.  
A @futex is a 32-bit val (32-bit aligned) whose address is passed to the @futex() syscall. All futex ops are ruled by this val.  
A futex can be used to atomatically read & write from/to a shared mem 32-bit buf.  
To share a futex between procs, use a smem address (eg. via @mmap() or @shmat()) to store the futex val (so the futex can have a diff virtual mem pos in different procs, but these these virtual mem pos's will map to the same physical mem pos).  
To share a futex between threads, just use a global var.  
Blocking w/ a futex is an atomic CAS op (so you don't need to call CAS directly?).  
A futex can be used to implement a mutex: eg. the futex val is 0 if it's unlocked and 1 if it's locked  
A futex can be used to implement mutexes, condition vars, semaphores, barriers, read/write locks  
In its  bare form, a futex is an aligned integer which is touched only by atomic assembler instructions.  This integer is four bytes long on all platforms.  Processes can share this integer using mmap(2), via shared memory segments, or because they share memory space, in which case the application is commonly called multithreaded.  

```
@atomic_compare_exchange_strong(data, expected, desired) atomically does:
	if(*data == *expected)
		*data = desired;
Returns 1 if the test yielded 1 and *data was changed.

mmap()/munmap()
shmget()/shmat()/shmdt()
shm_open()/shm_unlink()
sem_init()/sem_open()/sem_destroy() / sem_wait()/sem_post()
pthread_create()/pthread_destroy() / pthread_join()
pthread_mutex_init()/pthread_mutex_destroy() / pthread_mutex_lock() / pthread_mutex_unlock()
pthread_cond_init()/pthread_cond_destroy() / pthread_cond_wait() / pthread_cond_signal()
pthread_barrier_init()/pthread_barrier_destroy() / pthread_barrier_wait()

# c linux ipc

- best approach?  stackoverflow.com/questions/4836863
	0) shm_open()
	1) ftruncate()
	2) mmap()

- there are 3 APIs:
	0) SysV  ("old")
	1) POSIX ("new")
	2) kernel (best! since the other 2 should ultimately use this one at the lowest level)

- our experiments reveal that SHARED MEMORY provides the lowest latency and highest throughput, followed by kernel pipes and lastly, TCP/IP sockets
- one can expect shared memory to offer very low latencies
- the system does not provide any synchronization for accesses to shared memory; it ought to implemented by user programs using primitives such as semaphores
- IPC using smem is much better than using file locks: akkadia.org/drepper/futex.pdf
- https://pages.cs.wisc.edu/~adityav/Evaluation_of_Inter_Process_Communication_Mechanisms.pdf
- https://beej.us/guide/bgipc/pdf/bgipc_USLetter_2.pdf
- shared memory involves no syscalls!

## sharem memory

- posix: shm_open
- sysv:  shmget, shmat, shmctl

### posix
	- mmap(2)        map   the shared memory object into the virtual address space of the calling process
	- munmap(2)      unmap the shared memory object from the virtual address space of the calling process
	- shm_open(3)    create and open a new object, or open an existing object. this is analogous to open(2). the call returns a file descriptor for use by the other interfaces listed below
	- ftruncate(2)   set the size of the shared memory object. (a newly created shared memory object has a length of zero.)
	- shm_unlink(3)  remove a shared memory object name
	- close(2)       close the file descriptor allocated by shm_open(3) when it is no longer needed
	- fstat(2)       obtain a stat structure that describes the shared memory object. among the information returned by this call are the object's size (st_size), permissions (st_mode), owner (st_uid), and group (st_gid)
	- fchown(2)      to change the ownership of a shared memory object
	- fchmod(2)      to change the permissions of a shared memory object

## semaphores

- posix: sem_open, sem_wait, sem_post, sem_close, sem_unlink
- sysv:  semget, semop, semctl

## bench

  pipe
        128              512            1,024          4,096
      1,319 Mb/s       5,110 Mb/s       8,932 Mb/s    20,297 Mb/s
  1,288,233 msg/s  1,247,449 msg/s  1,090,370 msg/s  619,407 msg/s

  fifo
        128              512            1,024          4,096
      1,358 Mb/s       5,491 Mb/s       8,440 Mb/s    22,018 Mb/s
  1,326,016 msg/s  1,340,502 msg/s  1,030,215 msg/s  671,929 msg/s

  unnamed unix domain socket
        128              512            1,024          4,096
      1,117 Mb/s       4,401 Mb/s       8,869 Mb/s    17,207 Mb/s
  1,090,370 msg/s  1,074,548 msg/s  1,082,674 msg/s  525,121 msg/s

  named unix domain socket
     128                 512            1,024         4,096
     100 Mb/s            102 Mb/s         109 Mb/s       97 Mb/s
  97,600 msg/s        24,986 msg/s     13,289 msg/s   2,968 msg/s

  tcp
      128              512            1,024           4,096
      106 Mb/s         143 Mb/s         135 Mb/s        128 Mb/s
  103,508 msg/s     34,852 msg/s     16,529 msg/s     3,897 msg/s

## notes

https://github.com/detailyang/ipc_benchmark
https://users.cs.cf.ac.uk/Dave.Marshall/C/node27.html
http://cse.cuhk.edu.hk/~ericlo/teaching/os/lab/7-IPC2/sync-pro.html
https://docs.oracle.com/cd/E19120-01/open.solaris/817-4415/sockets-18552/index.html
https://blog.superpat.com/2010/07/14/semaphores-on-linux-sem_init-vs-sem_open/

# IPC / interprocess communication & networking
	- https://opensource.com/article/19/4/interprocess-communication-linux-storage
	- https://opensource.com/article/19/4/interprocess-communication-linux-networking
	- https://binarytides.com/packet-sniffer-code-c-linux/
	- https://binarytides.com/packet-sniffer-code-in-c-using-linux-sockets-bsd-part-2/
	- https://ibm.com/docs/en/i/7.3?topic=designs-example-nonblocking-io-select
	- c packet sniffer
	- c read tcp packets
	- c non blocking io
	- c select tutorial
	- c epoll tutorial
	- IPC: shared memory
	- IPC: sockets
	- c TLS implement
	- c non blocking shared memory
	- c non blocking sockets
	- PayPal says: use TLS 1.2 or higher... developer.paypal.com/api/rest/reference/info-security-guidelines/#usetls12

--------------------------------------------------------------------------------------------------------------------------------
Socket flow of events: Server that uses nonblocking I/O and select()
The following calls are used in the example:

The socket() API returns a socket descriptor, which represents an endpoint. The statement also identifies that the INET (Internet Protocol) address family with the TCP transport (SOCK_STREAM) is used for this socket.
The ioctl() API allows the local address to be reused when the server is restarted before the required wait time expires. In this example, it sets the socket to be nonblocking. All of the sockets for the incoming connections are also nonblocking because they inherit that state from the listening socket.
After the socket descriptor is created, the bind() gets a unique name for the socket.
The listen() allows the server to accept incoming client connections.
The server uses the accept() API to accept an incoming connection request. The accept() API call blocks indefinitely, waiting for the incoming connection to arrive.
The select() API allows the process to wait for an event to occur and to wake up the process when the event occurs. In this example, the select() API returns a number that represents the socket descriptors that are ready to be processed.
	-  0 Indicates that the process times out. In this example, the timeout is set for 3 minutes.
	- -1 Indicates that the process has failed.
	-  1 Indicates only one descriptor is ready to be processed. In this example, when a 1 is returned, the FD_ISSET and the subsequent socket calls complete only once.
	-  n Indicates that multiple descriptors are waiting to be processed. In this example, when an n is returned, the FD_ISSET and subsequent code loops and completes the requests in the order they are received by the server.
The accept() and recv() APIs are completed when the EWOULDBLOCK is returned.
The send() API echoes the data back to the client.
The close() API closes any open socket descriptors.

--------------------------------------------------------------------------------------------------------------------------------
In general, multi-threaded/multi-process apps need sync primitives more complicated than mutexes: condition vars, semaphores, barriers, read/write mutexes
All but the semaphore have in common that they need interval vars to hold state
Modifying such state requires a mutex
Implementing condition variables using futexes is very complicated

--------------------------------------------------------------------------------------------------------------------------------
git init
git remote add origin https://github.com/etale-cohomology/futex.git
gitc
git push -u origin master
```

# sources

man 2 futex  
man 7 futex  
https://eli.thegreenplace.net/2018/basics-of-futexes/  
https://github.com/eliben/code-for-blog/tree/master/2018/futex-basics  
https://kernel.org/doc/ols/2002/ols2002-pages-479-495.pdf  
https://akkadia.org/drepper/futex.pdf  
http://locklessinc.com/articles/mutex_cv_futex/  
