#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#define N 3
int counter = 0;

void * pthread_func() {
    for (int i = 0; i < 1000; i++) {
        counter++;
    }
}

int main()
{
    pthread_t a[N];
    printf("Starting threads...\n");
    
    int * id;
    for (int i = 0; i < N; i++) {
        id = (int *) malloc(sizeof(int));
        *id = i;
        pthread_create(&a[i], NULL, pthread_func, (void *) (id));
    }
    
    for (int i = 0; i < N; i++) {
        pthread_join(a[i], NULL);
    }
    
    printf("The counter ended at: %d \n", counter);
    printf("Ended threads...\n");

    return 0;
}