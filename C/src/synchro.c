#include "ensitheora.h"
#include "synchro.h"
#include <pthread.h>


/* les variables pour la synchro, ici */
static pthread_mutex_t mutex_rdy = PTHREAD_MUTEX_INITIALIZER;

static pthread_cond_t cond_taille = PTHREAD_COND_INITIALIZER;
static pthread_cond_t cond_texture = PTHREAD_COND_INITIALIZER;
bool isFenetreTextureReady = false;

/* l'implantation des fonctions de synchro ici */
void envoiTailleFenetre(th_ycbcr_buffer buffer) {
    pthread_mutex_lock(&mutex_rdy);

    windowsx = buffer[0].width;
    windowsy = buffer[0].height;

    pthread_cond_signal(&cond_taille);
    pthread_mutex_unlock(&mutex_rdy);

}

void attendreTailleFenetre() {
    pthread_mutex_lock(&mutex_rdy);
    while (windowsx == 0 || windowsy == 0) {
        pthread_cond_wait(&cond_taille, &mutex_rdy);
    }
    pthread_mutex_unlock(&mutex_rdy);
}

void signalerFenetreEtTexturePrete() { // ICI
    pthread_mutex_lock(&mutex_rdy);
        isFenetreTextureReady = true;
        pthread_cond_signal(&cond_texture);
    pthread_mutex_unlock(&mutex_rdy);
}

void attendreFenetreTexture() { // ICI
    pthread_mutex_lock(&mutex_rdy);
    while (!isFenetreTextureReady) {
        pthread_cond_wait(&cond_texture, &mutex_rdy);
    }
    pthread_mutex_unlock(&mutex_rdy);
}

static pthread_mutex_t mutex_lecteur_consom = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond_consom = PTHREAD_COND_INITIALIZER;
static pthread_cond_t cond_produit = PTHREAD_COND_INITIALIZER;
int cptToRead = 0;

// tex_iwri, tex_iaff est déjà codée par le squelette.
//Le nombre maximal de textures stockables est NBTEX .
void debutConsommerTexture() {
    pthread_mutex_lock(&mutex_lecteur_consom);

    while (cptToRead <= 0) {
        pthread_cond_wait(&cond_produit, &mutex_lecteur_consom);
    }
    cptToRead--;

    pthread_mutex_unlock(&mutex_lecteur_consom);
}

void finConsommerTexture() {
    pthread_mutex_lock(&mutex_lecteur_consom);
    pthread_cond_signal(&cond_consom);
    pthread_mutex_unlock(&mutex_lecteur_consom);
}

void debutDeposerTexture() {
    pthread_mutex_lock(&mutex_lecteur_consom);

    while (cptToRead >= NBTEX) {
        pthread_cond_wait(&cond_consom, &mutex_lecteur_consom);
    }
    cptToRead++;

    pthread_mutex_unlock(&mutex_lecteur_consom);
}

void finDeposerTexture() {
    pthread_mutex_lock(&mutex_lecteur_consom);
    pthread_cond_signal(&cond_produit);

    pthread_mutex_unlock(&mutex_lecteur_consom);
}
