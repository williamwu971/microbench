//
// Created by xiaoxiang on 11/30/22.
//

#include "microbench.h"

int GNL = 0;
#define NUM_THREADS 19

int log_start_perf() {

    remove("/mnt/sdb/xiaoxiang/pcm.txt");

    int res = system("sudo /mnt/sdb/xiaoxiang/pcm/build/bin/pcm-memory -all >/dev/null 2>&1 &");
    sleep(1);

    return res;
}

int log_stop_perf() {

    int res = system("sudo pkill --signal SIGHUP -f pcm-memory");
    sleep(1);

    return res;
}

void log_print_pmem_bandwidth(double elapsed) {

    uint64_t read = 0;
    uint64_t write = 0;

    int scanned_channel = 0;

    while (scanned_channel < 12) {
        FILE *file = fopen("/mnt/sdb/xiaoxiang/pcm.txt", "r");
        read = 0;
        write = 0;

        char buffer[256];
        int is_first_line = 1;
        while (fgets(buffer, 256, file) != NULL) {
            if (is_first_line) {
                is_first_line = 0;
                continue;
            }
            uint64_t skt, channel, pmmReads, pmmWrites, elapsedTime;
            sscanf(buffer, "%lu %lu %lu %lu %lu", &skt, &channel, &pmmReads, &pmmWrites, &elapsedTime);
            scanned_channel++;
            read += pmmReads;
            write += pmmWrites;
        }
    }


    double read_gb = (double) read / 1024.0f / 1024.0f / 1024.0f;
    double write_gb = (double) write / 1024.0f / 1024.0f / 1024.0f;

    double read_bw = read_gb / elapsed;
    double write_bw = write_gb / elapsed;


    printf("\n");

    printf("read: ");
    printf("%.2fgb ", read_gb);
    printf("%.2fgb/s ", read_bw);

    printf("write: ");
    printf("%.2fgb ", write_gb);
    printf("%.2fgb/s ", write_bw);

    printf("elapsed: %.2f ", elapsed);

    printf("\n");

    FILE *f = fopen("seq.csv", "a");
    fprintf(f, "%dB,%.2f,%.2f,%.2f,%.2f,%.2f,\n", GNL, read_gb, read_bw, write_gb, write_bw, elapsed);

}

void *thread(void *arg) {

    int granularity = GNL;
    char *ptr = (char *) arg;

    void *buf = malloc(granularity);
    memset(buf, 0xdeadbeef, granularity);

    for (int i = 0; i < 1073741824; i += granularity, ptr += granularity) {

        memcpy(ptr, buf, granularity);
        pmem_persist(ptr, granularity);
    }


    return NULL;
}

int main(int argc, char **argv) {

    if (argc != 2)return 1;
    GNL = atoi(argv[1]);

    size_t mapped_len;
    int is_pmem;
    char *map = pmem_map_file("/pmem0/microbench", 20401094656,
                              PMEM_FILE_CREATE | PMEM_FILE_EXCL,
                              00666, &mapped_len, &is_pmem);
    if (!is_pmem)
        die("File is not in pmem?!");

    /* Find size */
    printf("#+-+- Size of file being benched: %luMB\n", mapped_len / 1024 / 1024);

    memset(map, 0, mapped_len);
    for (size_t i = 0; i < mapped_len; i += 4096) {
        map[i] = 0;
    }


    pthread_t *threads = malloc(sizeof(pthread_t) * NUM_THREADS);

    puts("begin");
    log_start_perf();

    declare_timer
    start_timer

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(threads + i, NULL, thread, map + i * 1073741824);
    }


    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    stop_timer();

    log_stop_perf();
    log_print_pmem_bandwidth((double) elapsed / 1000000.0);

    return 0;
}