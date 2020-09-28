#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <inttypes.h>

typedef struct {
    pthread_mutex_t mutex_one;
    pthread_cond_t condvar_one;

    int64_t A;
    int16_t B;
    int32_t N;
    int64_t answer;

    int8_t read;
} prime_t;

void prime_init(prime_t* promise, int64_t a, int64_t b, int32_t n) {
    pthread_mutex_init(&promise->mutex_one, NULL);
    pthread_cond_init(&promise->condvar_one, NULL);
    promise->A = a;
    promise->B = b;
    promise->N = n;
    promise->answer = a - 1;
    promise->read = 0;
}

int64_t is_prime(int64_t number) {
    if (number <= 1) {
        return 0;
    }
    if (number % 2 == 0 && number > 2) return 0;
    for (size_t i = 3; i < number / 2; i += 2) {
        if (number % i == 0) {
            return 0;
        }
    }
    return 1;
}

int64_t find_prime(int64_t start) {
    for (size_t i = start + 1; ; ++i) {
        if (is_prime(i)) {
            return i;
        }
    }
}

static void* thread_func(void* arg) {
    prime_t* answ = (prime_t*) arg;
    for(int i = 0; i < answ->N; ++i) {
        pthread_mutex_lock(&answ->mutex_one);
        answ->answer = find_prime(answ->answer);

        pthread_cond_signal(&answ->condvar_one);

        while (!answ->read)
        {
            pthread_cond_wait(&answ->condvar_one, &answ->mutex_one);
        }
        answ->read = 0;
        pthread_mutex_unlock(&answ->mutex_one);
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    uint64_t A = strtoll(argv[1], NULL, 10);
    uint64_t B = strtoll(argv[2], NULL, 10);
    uint32_t N = strtol(argv[3], NULL, 10);

    prime_t args;
    prime_init(&args, A, B, N);

    pthread_t thread;
    pthread_mutex_lock(&args.mutex_one);
    pthread_create(&thread, NULL, thread_func, (void*)&args);

    uint64_t our_prime = A - 1;
    for(int i = 0; i < N; ++i) {
        while(our_prime == args.answer) {
            pthread_cond_wait(&args.condvar_one, &args.mutex_one);
        }
        our_prime = args.answer;

        pthread_mutex_unlock(&args.mutex_one);
        printf("%lu \n", our_prime);
        pthread_mutex_lock(&args.mutex_one);

        args.read = 1;
        pthread_cond_signal(&args.condvar_one);
    }
    pthread_mutex_unlock(&args.mutex_one);
    pthread_join(thread, NULL);

    return 0;
}
