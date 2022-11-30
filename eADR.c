//
// Created by xiaoxiang on 11/30/22.
//

#include "microbench.h"

#define GNL 256
#define NUM_THREADS 19

struct args {
    char *loc;
    size_t len;
    char buf[GNL];
};


void *thread(void *arg) {

    struct args a = *(struct args *) arg;

    for (; a.len > GNL; a.len -= GNL) {
        memcpy(a.loc, a.buf, GNL);
        pmem_persist(a.loc, GNL);
        a.loc += GNL;
    }

    return NULL;
}

int main(int argc, char **argv) {

    if (argc != 2)return 1;

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
    declare_timer
    start_timer

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(threads + i, NULL, thread, threads_args + i);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }


    stop_timer("elapsed %.2fms", elapsed / 1000.);


    return 0;
}