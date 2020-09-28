#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/syscall.h>
#include <pthread.h>

#define ta_assert(stmt) if (stmt) {} else {printf("failed"); exit(EXIT_FAILURE); }

typedef struct {
    int value;
} thread_answer;

static thread_answer *thread_func(void *arg) {     
    thread_answer *sum;
    if (!(sum = (thread_answer*)malloc(sizeof(thread_answer)))) {
        printf("Allocation error.");
        exit(0);
    }
    sum->value = 0;
    int value;
    while (scanf("%d", &value) != EOF) {
        sum->value += value;
    }
    return sum;
}

int main(int argc, char* argv[]) {
    int N = strtol(argv[1], NULL, 10);

    pthread_t thread[N];
    pthread_attr_t thread_attr; 
    ta_assert(pthread_attr_init(&thread_attr) == 0);
    ta_assert(pthread_attr_setstacksize(&thread_attr, 16384) == 0);

    for (int i = 0; i < N; ++i) {
        //printf("Thread creating\n");
        ta_assert(pthread_create(&thread[i], &thread_attr, (void* (*)(void*))thread_func, 0) == 0);
    }
    int final_sum = 0;
    for (int i = 0; i < N; ++i) {
        thread_answer* partial_sum;
        ta_assert(pthread_join(thread[i], (void**)&partial_sum) == 0);
        //printf("Thread joined\n");
        final_sum += partial_sum->value;
        free(partial_sum);
    }
    ta_assert(pthread_attr_destroy(&thread_attr) == 0);

    printf("%d ", final_sum);
    return 0;
}
