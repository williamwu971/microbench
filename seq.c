//
// Created by xiaoxiang on 11/30/22.
//

#include "microbench.h"

#define GNL 256
#define NUM_THREADS 16

struct args {
    char *loc;
    size_t len;
    char buf[GNL];
};

int seq = 1;

void *thread(void *arg) {

    struct args a = *(struct args *) arg;

    int *indexes = malloc(sizeof(int) * (a.len / GNL));
    for (int i = 0; i < a.len / GNL; i++) {
        if (seq) {
            indexes[i] = i * GNL;
        } else {
            indexes[i] = (int) (rand() % (a.len - GNL)) / GNL * GNL;
        }
    }

    uint64_t not_aligned = 0;

    declare_timer
    start_timer

    for (int i = 0; i < a.len / GNL; i++) {

        if (a.loc + indexes[i] % 256) {
            not_aligned++;
        }


        pmem_memcpy_persist(a.loc + indexes[i], a.buf, GNL);
//        pmem_persist(a.loc, GNL);
    }stop_timer("not A: %lu", not_aligned);


    return (void *) elapsed;
}

int main(int argc, char **argv) {

    if (argc < 2)return 1;
    if (argc > 2) {
        seq = 0;
        puts("rand");
    } else {
        puts("seq");
    }

    size_t mapped_len;
    int is_pmem;
    char *map = pmem_map_file(argv[1], 0, 0, 0, &mapped_len, &is_pmem);
    if (!is_pmem)
        die("File is not in pmem?!");

    /* Find size */
    printf("# Size of file being benched: %luMB\n", mapped_len / 1024 / 1024);

    memset(map, 0, mapped_len);
    srand(time(NULL));


    pthread_t *threads = malloc(sizeof(pthread_t) * NUM_THREADS);
    struct args *threads_args = malloc(sizeof(struct args) * NUM_THREADS);

    for (int i = 0; i < NUM_THREADS; i++) {
        threads_args[i].loc = map + (mapped_len / NUM_THREADS * i);
        threads_args[i].len = mapped_len / NUM_THREADS;

        for (int j = 0; j < GNL; j++) {
            threads_args[i].buf[j] = rand();
        }
    }

    puts("begin");

    char sys_command[1024];
    sprintf(sys_command,
            "/home/blepers/linux-huge/tools/perf/perf stat -e uncore_imc/event=0xe7/ -e uncore_imc/event=0xe3/ &");
    system(sys_command);


    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(threads + i, NULL, thread, threads_args + i);
    }

    uint64_t elapsed = 0;

    for (int i = 0; i < NUM_THREADS; i++) {
        uint64_t e;
        pthread_join(threads[i], (void **) &e);
        elapsed += e;
    }

    system("killall -s INT perf");


    printf("e: %.2fms\n", (double) elapsed / NUM_THREADS / 1000.);

    return 0;
}