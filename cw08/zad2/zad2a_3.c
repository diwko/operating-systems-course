#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/syscall.h>

int threads_number;

long gettid() {
    return syscall(SYS_gettid);
}

void parse_args(int argc, char *argv[], int *threads_number) {
    if(argc != 2) {
        fprintf(stderr, "Niepoprawna liczba argumentów\n");
        exit(EXIT_FAILURE);
    }
    *threads_number = atoi(argv[1]);
}

void sig_handler(int sig) {
    fprintf(stderr, "PID: %d, TID: %ld - SIGNAL RECEIVED\n", getpid(), gettid());
}

void* thread_action(void *arguments) {
    long my_tid = gettid();
    while(1) {
        fprintf(stderr, "TID: %ld - DZIAŁAM\n", my_tid);
        sleep(1);
    }
    return NULL;
}



int main(int argc, char *argv[]) {
    parse_args(argc, argv, &threads_number);

    if(signal(SIGUSR1, sig_handler) == SIG_ERR) {
        perror("signal error");
    }
    if(signal(SIGTERM, sig_handler) == SIG_ERR) {
        perror("signal error");
    }


    pthread_t *threads = malloc(sizeof(pthread_t)*threads_number);

    int i;
    for(i = 0; i < threads_number; i++) {
        if(pthread_create(&threads[i], NULL, &thread_action, NULL) != 0) {
            perror("pthread_create error");
        }
    }

    while(1) {
        raise(SIGUSR1);
        sleep(1);

    }


    while(1) {
        fprintf(stderr, "TID: %ld - DZIAŁAM (main)\n", gettid());
        sleep(1);
    }

    free(threads);

    return 0;
}