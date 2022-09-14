#include "all_functions.h" 

int main(int argc, char* argv[]){
    if(argc != 5){
        printf("Usage: %s r N T_s algo_choice\n",argv[0]);
        printf("where, \n");
        printf("r = Rotational speed of disk in revolutions per minute.\n");
        printf("N = Sector size in bytes.\n");
        printf("T_s = Average seek time in ms.\n");
        printf("algo_choice = A number from 1 to 5, denoting one of the following disk scheduling algorithms ::\n ");
        printf("\t1. Random\n");
        printf("\t2. FIFO\n");
        printf("\t3. SSTF\n");
        printf("\t4. SCAN\n");
        printf("\t5. C-SCAN\n");
        exit(-1);
    }
    //srand(time(NULL));
    r = atoi(argv[1]);
    N = atoi(argv[2]);
    T_s = atoi(argv[3]);
    algo_choice = atoi(argv[4]);
    
    printf("-------------------------------------\n");
    printf("Parameters for simulation :: \n");
    printf("Rotational Speed: %d revolutions per minute\n", r);
    printf("Sector Size: %d bytes\n", N);
    printf("Average access time: %d ms\n", T_s);
    printf("Number of requests: %d\n", total_requests);
    printf("Disk Scheduling Algorithm: ");
    switch (algo_choice){
        case 1: printf("Random\n");
                break;
        case 2: printf("FIFO\n");
                break;
        case 3: printf("SSTF\n");
                break;
        case 4: printf("SCAN\n");
                break;
        case 5: printf("CSCAN\n");
                break;
    }
    printf("-------------------------------------\n");
    printf("\n");

    /* Initialising the current position of RW heads randomly */
    head_pos_cylinder = rand() % total_cylinders;   
    head_pos_sector = rand() % total_sectors;
    
    /*Calculating the time required to read through 1 sector */
    rotation_time_per_sector = (60 * 1000.0) / (r * total_sectors);
   
    /*Calculating the time required to seek through 1 track, using average seek time */
    seek_time_per_track = (3.0 * T_s)/(total_cylinders - 1);

    generate_requests();

    disk_scheduler();

    calculate_stats();
    for(int i = 0; i < total_requests; i++)
        free(request_queue[i]);
    return 0;
}