# Patrick Henry Drive MP3 Audio Server
This project is a multithreaded polling server that implements persistent connections according to the HTTP/1.1 protocol and is based on my code from sysstatd a.k.a. project 4 of CS 3214 (Computer Systems) completed during the Spring 2017 semester at Virginia Tech. This server program is special to me because it performed very highly among those of my classmates which helped me receive a high grade for this project [a percentage of the grade for the project was based on our benchmarks and how they stacked up against other teams].

## Epoll vs Poll
The main factor contributing to high performance is the use of `epoll(4)` as opposed to `poll(2)` because it can scale to a large number of watched file descriptors. This is suitable for the Raspberry Pi as epoll is specific to linux systems. In servers that use poll, all file descriptors are scanned every time a ready event occurs resulting in O(n) complexity; however, epoll leverages the linux kernel to avoid linear search and achieve O(1) complexity. Although some people say the differences between poll and epoll are negligeble, the complexity advantages of using epoll become clear with increasing server load - especially as the number of open file descriptors exceeds ~1000. While this server will never come close to a fraction 

## Supported Functionality
#### /runloop
When this URL is visited, a POSIX thread is started and run in parallel for a given amount of time in an effort to increase the load average of the system.
#### /loadavg
When this URL is visited, the server provides the system's load average which is reported by the kernel to loadavg of the /proc file system.

## Personal Notes for Raspberry Pi
Temporary USB storage location:
```
/mnt/pidrive1/
```
Server code location:
```
/srv/
```
