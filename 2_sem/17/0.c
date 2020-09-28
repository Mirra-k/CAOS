#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#define ta_assert(stmt) if (stmt) {} else {printf("failed \n"); exit(EXIT_FAILURE); }

typedef struct {
    pthread_mutex_t *mutex;
    pthread_mutex_t *mutex_left;
    pthread_mutex_t *mutex_right;
    double* array;
    int index;
    int N;
    int k;
} context_t;

static void* func(void *arg) {
    const context_t* ctx = arg;
    //printf("start function %d \n", ctx->index);

    const double add_left = 0.99;
    const double add_me = 1;
    const double add_right = 1.01;

    size_t left = ctx->index == 0 ? ctx->k - 1 : ctx->index - 1;
    size_t right = ctx->index == ctx->k - 1 ? 0 : ctx->index + 1;

    for (size_t i = 0; i < ctx->N; ++i) {
        //printf("thread %d, iteration %d \n", ctx->index, i);

        pthread_mutex_lock(ctx->mutex_left);
        ctx->array[left] += add_left;
        pthread_mutex_unlock(ctx->mutex_left);
        //printf("add_left: thread %d, iteration %d \n", ctx->index, i);

        pthread_mutex_lock(ctx->mutex);
        ctx->array[ctx->index] += add_me;
        pthread_mutex_unlock(ctx->mutex);
        //printf("add: thread %d, iteration %d \n", ctx->index, i);

        pthread_mutex_lock(ctx->mutex_right);
        ctx->array[right] += add_right;
        pthread_mutex_unlock(ctx->mutex_right);
        //printf("add_right: thread %d, iteration %d \n", ctx->index, i);
    }
    return 0;
}

int main(int argc, char* argv[]) {
    int N = strtol(argv[1], NULL, 10);
    int k = strtol(argv[2], NULL, 10);
    
    pthread_t thread[k];
    context_t threads[k];
    double array[k];
    memset(array, 0 , sizeof(array));
    
    pthread_mutex_t mutex[k];
    for (size_t i = 0; i < k; ++i) {
        pthread_mutex_init(&mutex[i], NULL);
    }
    
    pthread_attr_t attr;
    pthread_attr_init (&attr);
    pthread_attr_setguardsize(&attr, 0);
    pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);
    
    
    //printf("start cycle \n");
    for (size_t i = 0; i < k; ++i) {
        size_t il = i == 0 ? k - 1 : i- 1;
        size_t ir = i == k - 1 ? 0 : i + 1;

        threads[i].mutex = &mutex[i];
        threads[i].mutex_left = &mutex[il];
        threads[i].mutex_right = &mutex[ir];
        threads[i].array = array;
        threads[i].index = i;
        threads[i].N = N;
        threads[i].k = k;
        ta_assert(pthread_create(&thread[i], &attr, func, (void*)&threads[i]) == 0);
    }
    
    pthread_attr_destroy(&attr);
    for (int i = 0; i < k; ++i) {
        ta_assert(pthread_join(thread[i], NULL) == 0);
    }
    for (int i = 0; i < k; ++i) {
        printf("%.10g ", array[i]);
    }
}
