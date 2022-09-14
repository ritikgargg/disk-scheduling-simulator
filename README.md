# Disk Scheduling Simulator

#### 1. What does this program do?

In this program, we simulate the read, write and seek operations of a disk, using disk parameters like the rotational speed, the average seek time, and the sector size, along with knowledge about the physical properties of the disk. For this purpose, a uniform distribution of requests is generated, each request specifying the read/write address and the number of requested sectors. Finally, different disk scheduling algorithms(random, FIFO, SSTF, SCAN, CSCAN) are employed to service the requests, and the throughput, the response time, and the variance of response times are reported for each.

#### 2. A description of how this program works (i.e. its logic)

The required inputs for the execution of the program are supplied as the command-line arguments. These include the rotational speed of the disk, the average seek-time, and the sector size. The other details about the physical characteristics of the disk as the total number of platters, read-write heads, cylinders, and sectors are already known. 

To begin with, we randomly generate the current position of RW-heads. We then calculate the rotational delay incurred in traversing through one sector and the seek time to move between consecutive cylinders.

Thereafter, we randomly generate a distribution of requests, where each request consists of the read-write address(platter, cylinder, sector) and the number of requested sectors. These requests are stored in `request_queue[]`, upper bounded by `total_requests`.

Next, we invoke the `disk_scheduler()` function, which is responsible for invoking the correct disk scheduling algorithm, based on the user's choice. These scheduling algorithms include the random disk scheduling(simulated by `random_scheduling()` function), the FIFO(First In, First Out) disk scheduling(simulated by `fifo_scheduling()` function), the SSTF(Shortest Seek Time First) disk scheduling(simulated by `sstf_scheduling()` function), the SCAN(or elevator) disk scheduling(simulated by `scan_scheduling()` function), and the CSCAN(Circular SCAN) disk scheduling(simulated by `cscan_scheduling()` function). 

In random scheduling, the requests are serviced by following random selection.

In FIFO scheduling, the requests are addressed in sequential order(the order they arrive in the disk queue).

In SSFT scheduling, the requests having shortest seek time are executed first.

In SCAN scheduling, the disk arm traverses in a particular direction,servicing all the requests coming in its path until there are no more requests in that direction. It reverses its direction and again services the request arriving in its path.  

In CSCAN scheduling, the disk arm traverses in a particular direction,servicing all the requests coming in its path until there are no more requests in that direction. The arm is then returned to the opposite end of the disk and the scan begins again.

Depending on the scheduling algorithm, the corresponding function is invoked, which orders and services the request accordingly. During this, the response times for each of the requests are determined and stored in `response_times[]`.

Finally, the stats viz. the throughput, the average response time, the minimum response time, the maximum response time, and the standard deviation of the response times are calculated and reported for the selected algorithm.


#### 3. How to compile and run this program

&nbsp;&nbsp;&nbsp;&nbsp;To compile the program:

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`gcc main.c -lm`

&nbsp;&nbsp;&nbsp;&nbsp;To execute the program:
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`./a.out r N T_s algo_choice`

&nbsp;&nbsp;&nbsp;&nbsp;where,
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`r` = Rotational speed of disk in revolutions per minute.
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`N` = Sector size in bytes.
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;T~s~ = Average seek time in ms.
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`algo_choice` = A number from 1 to 5, denoting one of the following disk scheduling algorithms:

1. Random
2. FIFO
3. SSTF
4. SCAN
5. C-SCAN


**Commands for a sample run**

```   
gcc main.c -lm
./a.out 7500 512 4 2
```