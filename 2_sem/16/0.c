#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define ta_assert(stmt) if (stmt) {} else {printf("failed"); exit(EXIT_FAILURE); }

static void* thread_func(void* arg) {
    int value;
    if (scanf("%d", &value) == EOF) {
        return NULL;
    }

    pthread_t thread;
    ta_assert(pthread_create(&thread, NULL, thread_func, 0) == 0);
    ta_assert(pthread_join(thread, NULL) == 0);
    printf("%d ", value);
    return NULL;
}

int main() {
    pthread_t thread;
    ta_assert(pthread_create(&thread, NULL, thread_func, 0) == 0);
    ta_assert(pthread_join(thread, NULL) == 0);
    return 0;
}
