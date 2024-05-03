#include <pthread.h>
#include <stdio.h>

typedef struct {
    int counter;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} Moniteur;

void* helloWorld() {
    printf("Hello World!\n");
    return NULL;
}
void* done() {
    printf("Done!\n");
    return NULL;
}


void* increment(void *arg){
    Moniteur *moniteur = (Moniteur *)arg;

    pthread_mutex_lock(&moniteur->mutex);
    moniteur->counter++;
    helloWorld();
    pthread_cond_broadcast(&moniteur->cond);
    pthread_mutex_unlock(&moniteur->mutex);
}
void* waitCompteur(void *arg){
    Moniteur *moniteur = (Moniteur *)arg;

    pthread_mutex_lock( &moniteur->mutex);

    while(moniteur->counter < 10){
        pthread_cond_wait(&moniteur->cond, &moniteur->mutex);
    }
    done();

    pthread_mutex_unlock(&moniteur->mutex);
}

int main() {
    Moniteur moniteur;
    moniteur.counter = 0;
    pthread_mutex_init(&moniteur.mutex, NULL);
    pthread_cond_init(&moniteur.cond, NULL);

    pthread_t lesThreadsID[11];

    pthread_create(&lesThreadsID[10], NULL, waitCompteur, &moniteur);

    for (int i=0;i<10;i++){
        pthread_create(&lesThreadsID[i], NULL, increment, &moniteur);
    }
    
    pthread_join(lesThreadsID[10], NULL);


  return 0;
} 