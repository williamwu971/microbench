#include "microbench.h"

static __uint128_t g_lehmer64_state;

static void init_seed(void) {
    srand(time(NULL));
    g_lehmer64_state = rand();
}

static uint64_t lehmer64() {
    g_lehmer64_state *= 0xda942042e4dd58b5;
    return g_lehmer64_state >> 64;
}

/* data_size in Bytes, time in us, returns bandwith in MB/s */
static double bandwith(long data_size, long time) {
    return (((double) data_size) / 1024. / 1024.) / (((double) time) / 1000000.);
}

static inline void clflush(char *data, int len, int front, int back) {
    volatile char *ptr = (char *) ((unsigned long) data & ~(CACHE_LINE_SIZE - 1));
    if (front)
        asm volatile("sfence":: :"memory");
    for (; ptr < data + len; ptr += CACHE_LINE_SIZE) {
#ifdef CLFLUSH
        asm volatile("clflush %0" : "+m" (*(volatile char *)ptr));
#elif CLFLUSH_OPT
        asm volatile(".byte 0x66; clflush %0" : "+m" (*(volatile char *)(ptr)));
#elif CLWB
        //        asm volatile(".byte 0x66; xsaveopt %0" : "+m" (*(volatile char *) (ptr)));
            asm volatile ("clwb (%0)" :: "r"(ptr))
#endif
    }
    if (back)
        asm volatile("sfence":: :"memory");
}

void start_perf() {


    char buf[10240];
    char *chaser = buf;

    chaser += sprintf(chaser, "sudo /home/blepers/linux-huge/tools/perf/perf stat -e ");
    int num_of_sticks = 8;

    for (int stick = 0; stick < num_of_sticks; stick++) {

        chaser += sprintf(chaser, "uncore_imc_%d/unc_m_pmm_rpq_inserts/,", stick);
        chaser += sprintf(chaser, "uncore_imc_%d/unc_m_pmm_wpq_inserts/", stick);

        if (stick != num_of_sticks - 1) {
            chaser += sprintf(chaser, ",");
        }

//        for (int i = 0; i < 10; i++) {
////            chaser += sprintf(chaser, "uncore_imc_%d/event=0xe%d,umask=0x0/", stick, i);
//            chaser += sprintf(chaser, "uncore_imc_%d/event=0xe%d,umask=0x0/", stick, i);
//
//            if (stick != num_of_sticks - 1 || i != 9) {
//                chaser += sprintf(chaser, ",");
//            }
//        }

    }


    chaser += sprintf(chaser, " > perf.data 2>&1 &");


    system(buf);
}

void stop_perf() {

    system("sudo killall -s INT -w perf");
}


int main(int argc, char **argv) {


    if (argc < 2) return 1;

    cpu_set_t cpuset;
    pthread_t thread = pthread_self();

    /* Set affinity mask to include CPUs 0 to 7 */

    CPU_ZERO(&cpuset);
    CPU_SET(7, &cpuset);

    pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);

    init_seed();

    long granularity = 256;      // granularity of accesses
    long nb_accesses = 30000000;   // nb ops
    char *path = argv[1];   // benched file

    /* Open file */
    int fd = open(path, O_RDWR | O_CREAT | O_DIRECT, 0777);
    if (fd == -1)
        die("Cannot open %s\n", path);

    /* Find size */
    struct stat sb;
    fstat(fd, &sb);
    printf("# Size of file being benched: %luMB\n", sb.st_size / 1024 / 1024);

    /* Mmap file */
    char *map = pmem_map_file(path, 0, 0, 0777, NULL, NULL);
    if (!pmem_is_pmem(map, sb.st_size))
        die("File is not in pmem?!");
//    char *map = mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SYNC | MAP_SHARED_VALIDATE, fd, 0);
//    memset(map, 0, sb.st_size);

    /* Allocate data to copy to the file */
//    char *page_data = aligned_alloc(PAGE_SIZE, granularity);
//    memset(page_data, lehmer64(), granularity);

    /*for(int i = 0; i < nb_accesses; i++) {
       memcpy(map[location], xxx, size);
    }*/


    uint64_t *locs = malloc(nb_accesses * sizeof(uint64_t));
    uint64_t start = lehmer64() % (sb.st_size - (nb_accesses + 1) * granularity) / granularity * granularity;
    if (argc != 2) {
        puts("rand");
        for (size_t i = 0; i < nb_accesses; i++) {
            locs[i] = lehmer64() % (sb.st_size - granularity) / granularity * granularity;
        }
    } else {
        puts("seq");
        for (size_t i = 0; i < nb_accesses; i++) {
            locs[i] = (start + i * granularity) % (sb.st_size - granularity) / granularity * granularity;
        }
    }

    //  1,920,000,000
    //    117,697,700
//    start_perf();

    puts("begin");

    uint64_t sum = 0;
    int set = (int) lehmer64();

    /* Benchmark N memcpy */
    declare_timer;
    start_timer
    {

        for (size_t i = 0; i < nb_accesses; i++) {
//            uint64_t loc = lehmer64() % (sb.st_size - granularity);
//            uint64_t loc = start + i * granularity;

            /**
             * todo: questionable flush here
             */

//            pmem_memcpy_persist(&map[locs[i]], page_data, granularity);
            pmem_memset_persist(&map[locs[i]], set, granularity);
//            memcpy(&map[locs[i]], page_data, granularity);
//            asm volatile ("clwb (%0)"::"r"(&map[locs[i]]));
//            asm volatile ("sfence":: : "memory");
//            sum += map[locs[i]];
        }
    }stop_timer("Doing %ld memcpy of %ld bytes (%f MB/s) sum %lu", nb_accesses, granularity,
                bandwith(nb_accesses * granularity, elapsed), sum);

//    stop_perf();

    return 0;
}
