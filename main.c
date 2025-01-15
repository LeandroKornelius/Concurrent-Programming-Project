#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Initial amount of materials to make the shirts
#define COTTON 2000 // amount in grams
#define TAGS 4
#define LOGOS 4
#define PERSONALIZATION 2

// Stock of shirts
int stockFast = 10;
int stockUnique = 2;

// Available materials
int cotton = COTTON;
int tags = TAGS;
int logos = LOGOS;
int personalization = PERSONALIZATION;

// Mutex and condition variables 
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condMaterials = PTHREAD_COND_INITIALIZER;
pthread_cond_t condStock = PTHREAD_COND_INITIALIZER;

void *produce(void *args) {
    while (1) {
        pthread_mutex_lock(&mutex); // locks to access the materials

        // Checks to see if there are enought materials to produce a Unique shirt 
        if (cotton >= 500 && tags >= 1 && logos >= 1 && personalization >= 1) {
            cotton -= 500;
            tags--;
            logos--;
            personalization--;
            stockUnique++;
            printf("Produced one Unique Shirt. Unique Shirt stock: %d\n", stockUnique);
        }

        else if (cotton >= 500 && tags >= 1 && logos >= 1) {
            cotton -= 500;
            tags--;
            logos--;
            stockFast++;
            printf("Produced one Fast Shirt. Fast Shirt stock: %d\n", stockFast);
        } 

        else {
            // There is not enough material to produce any of the two shirts, must wait for more material
            printf("Waiting for more materials to produce shirts...\n");
            pthread_cond_wait(&condMaterials, &mutex);
        }

        pthread_mutex_unlock(&mutex); // unlocks the materials so other threads can manipulate it
        sleep(2);
    }

    return NULL;
}

void *sell(void *args) {
    while (1) {
        pthread_mutex_lock(&mutex); // locks to access the shirt stocks

        // Checks to see if there are shirts in stock to sell
        if (stockUnique > 0) {
            stockUnique--;
            printf("Sold one Unique Shirt. Unique Shirt stock: %d\n", stockUnique);
            pthread_cond_signal(&condStock); // signals the seller that a just produced unique shirt is available
        }

        else if (stockFast > 0) {
            stockFast--;
            printf("Sold one Fast Shirt. Fast Shirt stock: %d\n", stockFast);
            pthread_cond_signal(&condStock); // signals the seller that a just produced fast shirt is available
        }

        else {
            // There are none shirts available, must wait so other shirts are produced
            printf("Waiting for more shirts to be produced...\n");
            pthread_cond_wait(&condStock, &mutex);
        }

        pthread_mutex_unlock(&mutex); // unlocks the stock so other threads can manipulate it
        sleep(1); 
    }

    return NULL;
}

void *restock(void *args) {
    while (1) {
        pthread_mutex_unlock(&mutex); // locks the materials to manipulate it 

        cotton += 1000;
        tags += 2;
        logos += 2;
        personalization += 1;
        printf("Restocked the materials in the below amount:\nCotton: %d;\nTags: %d;\nLogos: %d;\nPersonalization: %d\n",
               cotton, tags, logos, personalization);

        // Must signal that there is new material so new shirts can be produced and later bought
        pthread_cond_signal(&condMaterials); 
        pthread_mutex_unlock(&mutex);
        sleep(8);
    }

    return NULL;
}

int main() {
    pthread_t thread_producer, thread_client, thread_supplier;

    // Creates threads
    pthread_create(&thread_producer, NULL, produce, NULL);
    pthread_create(&thread_client, NULL, sell, NULL);
    pthread_create(&thread_supplier, NULL, restock, NULL);

    // Waits threads, which won't stop because of the while loops
    pthread_join(thread_producer, NULL);
    pthread_join(thread_client, NULL);
    pthread_join(thread_supplier, NULL);

    return 0;
}
