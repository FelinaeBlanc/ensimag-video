#include <SDL2/SDL.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "oggstream.h"
#include "stream_common.h"

pthread_t theora2sdlthread;

int main(int argc, char *argv[]) {
  int res;

  if (argc != 2) {
    fprintf(stderr, "Usage: %s FILE", argv[0]);
    exit(EXIT_FAILURE);
  }
  assert(argc == 2);

  // Initialisation de la SDL
  res = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS);
  atexit(SDL_Quit);
  assert(res == 0);

  // Your code HERE
  // start the two stream readers (theoraStreamReader and vorbisStreamReader)
  // each in a thread
  pthread_t thread_theora;
  pthread_t thread_vorbis;

  pthread_create(&thread_theora, NULL, theoraStreamReader, argv[1]);
  pthread_create(&thread_vorbis, NULL, vorbisStreamReader, argv[1]);
  
  // wait for vorbis thread 
  pthread_join(thread_vorbis, NULL);

  // 1 seconde of sound in advance, thus wait 1 seconde
  // before leaving
  sleep(1);

  // Wait for theora and theora2sdl threads
  pthread_cancel(thread_theora);
  pthread_cancel(theora2sdlthread);

  pthread_join(thread_theora, NULL);
  pthread_join(theora2sdlthread, NULL);
  

  // TODO
  /* liberer des choses ? */

  exit(EXIT_SUCCESS);
}
