#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define total_requests 1000
#define total_platters 4
#define total_cylinders 25
#define total_sectors 20

/*Structure to store the parameters required to specify a request*/
struct node{
    int platter_no; /* To store the platter number */
    int cylinder_no;    /* To store the cylinder number */
    int sector_no;  /* To store the sector number */
    int no_of_req_sectors;  /* To store the number of requested sectors */
};


struct node* request_queue[total_requests]; /* Queue to store the requests */
double response_times[total_requests];   /* Queue to store the time taken to process a request, once it is scheduled by the disk scheduler */
int r;  /* Rotational speed of disk in revolutions per minute */
int T_s;    /* Average seek time in ms */
int N;  /* Sector size in bytes */
int algo_choice;    /* Integer to specify the choice of disk-scheduling algorithm */

/* The RW-heads move in unison*/
int head_pos_cylinder;  /* Stores the current cylinder position of RW-heads */
int head_pos_sector;    /* Stores the current sector position of RW-heads */

double rotation_time_per_sector;
double seek_time_per_track;
double waiting_time = 0;

/**
 * Find maximum between two numbers.
 */
double max(double num1, double num2){
    return (num1 > num2) ? num1 : num2;
}

/**
 * Find minimum between two numbers.
 */
double min(double num1, double num2){
    return (num1 > num2) ? num2 : num1;
}

/**
 * Comparator function to be used with qsort()
 */
int cmpfunc (const void *a, const void *b) {
   return (request_queue[*(int*)a]->cylinder_no - request_queue[*(int*)b]->cylinder_no);
}

/**
 * Function to calculate the time taken to serve the request.
 * @param cur_idx The index of the request to be processed, in the queue.
 * @return The time required to process the request.
 */
double service_time(int cur_idx){
    double seek_time = abs(request_queue[cur_idx]->cylinder_no - head_pos_cylinder) * seek_time_per_track;
    double rotational_delay = ((request_queue[cur_idx]->sector_no - head_pos_sector + total_sectors) % total_sectors) * rotation_time_per_sector;
    double transfer_time = request_queue[cur_idx]->no_of_req_sectors * rotation_time_per_sector;
    return (seek_time + rotational_delay + transfer_time);
}
/**
 * Function to simulate the operation on the disk.
 * @param cur_idx The index of the request to be processed, in the queue.
 */
void perform_read_write(int cur_idx){
    
    double time_to_serve = service_time(cur_idx);
    // printf("Current request being services is %d\n", cur_idx);
    // printf("RW head moves from (cylinder_no = %d, sector_no = %d) to (cylinder_no = %d, sector_no = %d)\n", head_pos_cylinder, head_pos_sector, request_queue[cur_idx]->cylinder_no, request_queue[cur_idx]->sector_no);
    
    head_pos_cylinder = request_queue[cur_idx]->cylinder_no;    /* Updating the current cylinder position of the RW-heads */
    head_pos_sector = request_queue[cur_idx]->sector_no;    /* Updating the current sector position of the RW-heads */

    response_times[cur_idx] = time_to_serve + waiting_time;
    waiting_time += time_to_serve;
}

/**
 * Function to generate a distribution of disk requests.
 */
void generate_requests(){
    /* The requests are upper limited by the constant total_requests and stored in the request_queue */
    for(int i = 0; i < total_requests; i++){
        struct node *newRequest = (struct node*)malloc(sizeof(struct node));
        newRequest->platter_no = rand() % total_platters;
        newRequest->cylinder_no = rand() % total_cylinders;
        newRequest->sector_no = rand() % total_sectors;
        newRequest->no_of_req_sectors = (rand() % total_sectors) + 1;
        request_queue[i] = newRequest;
        //printf("%d) <%d, %d, %d, %d>\n", i + 1, newRequest->platter_no, newRequest->cylinder_no, newRequest->sector_no, newRequest->no_of_req_sectors);
    }
}

/**
 * Function to simulate the random scheduling policy to service the disk requests.
 */
void random_scheduling(){
    int req_to_serve = total_requests;
    int ri, actual_ri;
    bool is_req_served[total_requests]; /* Boolean array to store the status of already served requests and requests remaining to be served */
    for(int i = 0; i < total_requests; i++){
        is_req_served[i] = false;
    }
    while (req_to_serve > 0)
    {
        ri = (rand() % req_to_serve) + 1;
        actual_ri = -1;
        while(ri != 0){
            actual_ri += 1;
            if(!is_req_served[actual_ri]){
                ri -= 1;
            }
        }
        perform_read_write(actual_ri);   /* Serve the request */
        is_req_served[actual_ri] = true;    /* Mark the request as served */
        req_to_serve -= 1;
    }
}

/**
 * Function to simulate the FIFO(First In, First Out) scheduling policy to service the disk requests.
 */
void fifo_scheduling(){
    for(int i = 0; i < total_requests; i++){
        perform_read_write(i);
    }
}

/**
 * Function to return the index of the request with minimum seek time from the current position of RW-heads.
 * @param is_req_served A boolean array which stores the status of already served requests and requests remaining to be served.
 * @return Index of the request with minimum seek time from head_pos_cylinder.
 */
int minSeekTime(bool is_req_served[]){
    int minIdx, minST = INT_MAX;
    for(int i = 0; i < total_requests; i++){
        if(!is_req_served[i]){
            if(minST > abs(request_queue[i]->cylinder_no - head_pos_cylinder)){
                minST = abs(request_queue[i]->cylinder_no - head_pos_cylinder);
                minIdx = i;
            }
        }
    }
    return minIdx;
}

/**
 * Function to simulate the SSTF(Shortest Seek Time First) scheduling policy to service the disk requests.
 */
void sstf_scheduling(){
    bool is_req_served[total_requests]; /* Boolean array to store the status of already served requests and requests remaining to be served */
    
    for(int i = 0; i < total_requests; i++){
        is_req_served[i] = false;   /* Initially all requests are unserviced */
    }

    for(int k = 0; k < total_requests; k++){
        int idx_to_serve = minSeekTime(is_req_served);  
        perform_read_write(idx_to_serve);    /* Serve the request */
        is_req_served[idx_to_serve] = true; /* Mark the request as served */
    }
}

/**
 * Function to simulate the SCAN(or elevator algorithm) scheduling policy to service the disk requests.
 */
void scan_scheduling(){
    int smaller_cylinder_req[total_requests];   /* Array to store the indices of requests with cylinder numbers lesser than the cylinder number of current head position */
    int greater_cylinder_req[total_requests];   /* Array to store the indices of requests with cylinder numbers greater than or equal to the cylinder number of current head position */
    int size_scr = 0, size_gcr = 0; /* Sizes of the smaller_cylinder_req[] and greater_cylinder_req[] respectively */

    for (int i = 0; i < total_requests; i++) {
        if (request_queue[i]->cylinder_no < head_pos_cylinder)
            smaller_cylinder_req[size_scr++] = i;
        else
            greater_cylinder_req[size_gcr++] = i;
    }

    /* Sorting the requests in corresponding sets, in ascending order of cylinder numbers */
    qsort(smaller_cylinder_req, size_scr, sizeof(int), cmpfunc);
    qsort(greater_cylinder_req, size_gcr, sizeof(int), cmpfunc);

    /* Process requests with track numbers greater than or equal to the current head position */
    for(int k = 0; k < size_gcr; k++){
        perform_read_write(greater_cylinder_req[k]);
    }

    /* Traverse back and process requests */
    for(int k = size_scr - 1; k >= 0; k--){
        perform_read_write(smaller_cylinder_req[k]);
    }
}

/**
 * Function to simulate the CSCAN(Circular SCAN) scheduling policy to service the disk requests.
 */
void cscan_scheduling(){
    int smaller_cylinder_req[total_requests];   /* Array to store the indices of requests with cylinder numbers lesser than the cylinder number of current head position */
    int greater_cylinder_req[total_requests];   /* Array to store the indices of requests with cylinder numbers greater than or equal to the cylinder number of current head position */
    int size_scr = 0, size_gcr = 0; /* Sizes of the smaller_cylinder_req[] and greater_cylinder_req[] respectively */

    for (int i = 0; i < total_requests; i++) {
        if (request_queue[i]->cylinder_no < head_pos_cylinder)
            smaller_cylinder_req[size_scr++] = i;
        else
            greater_cylinder_req[size_gcr++] = i;
    }
    
    /* Sorting the requests in corresponding sets, in ascending order of cylinder numbers */
    qsort(smaller_cylinder_req, size_scr, sizeof(int), cmpfunc);
    qsort(greater_cylinder_req, size_gcr, sizeof(int), cmpfunc);

    /* Process requests with track numbers greater than or equal to the current head position */
    for(int k = 0; k < size_gcr; k++){
        perform_read_write(greater_cylinder_req[k]);
    }

    /* Go to the other end of the disk and service the requests */
    for(int k = 0; k < size_scr; k++){
        perform_read_write(smaller_cylinder_req[k]);
    }
}

/**
 * Function to invoke the correct disk scheduling algorithm, based on user's choice. 
 */
void disk_scheduler(){
    
    switch (algo_choice){
        case 1: random_scheduling();
                break;
        case 2: fifo_scheduling();
                break;
        case 3: sstf_scheduling();
                break;
        case 4: scan_scheduling();
                break;
        case 5: cscan_scheduling();
                break;
    }
}

/**
 * Function to calculate the throughput and the average, minimum, maximum and standard deviation of the response times. 
 */
void calculate_stats(){
    double total_response_time = 0;
    double min_response_time = INT_MAX, max_response_time = 0; 
    for(int i = 0; i < total_requests; i++){
        total_response_time += response_times[i];
        min_response_time = min(min_response_time, response_times[i]);
        max_response_time = max(max_response_time, response_times[i]);
    }

    double mean_response_time = total_response_time/total_requests;
    double throughput = (total_requests * 1000)/waiting_time;

    /* Calculate sum squared differences with mean. */
    double sqDiff = 0;
    for (int i = 0; i < total_requests; i++)
        sqDiff += (response_times[i] - mean_response_time) * (response_times[i] - mean_response_time);

    double sd_response_time = sqrt(sqDiff/total_requests);

    printf("Throughput = %lf requests/s\n", throughput);
    printf("Average Response Time = %lf s\n", mean_response_time/1000);
    printf("Minimum Response Time = %lf s\n", min_response_time/1000);
    printf("Maximum Response Time = %lf s\n", max_response_time/1000);
    printf("Standard Deviation of Response Times = %lf s\n", sd_response_time/1000);
}