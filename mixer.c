// $$DATE$$ : sam. 03 novembre 2018 (20:07:54)
// https://www.libsdl.org/projects/SDL_mixer/docs/SDL_mixer_frame.html

#include<stdio.h>
#include<stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

void mix_quit()
{
  fprintf(stderr,"[mix_quit] sortie du mixer\n");
  Mix_CloseAudio();
  Mix_Quit();
  SDL_QuitSubSystem(SDL_INIT_AUDIO);
}


int mixer_init()
{
  int flags = MIX_INIT_OGG;
  int flags_set = 0;

  // initialise uniquement le sous-system AUDIO, au cas où
  // le SDL_Init() serait déjà fait ailleurs (ce qui est le cas en fait)
  int is_err = SDL_InitSubSystem(SDL_INIT_AUDIO);
  if (is_err) {
    fprintf(stderr,"SDL_InitSubSystem: %s\n", SDL_GetError());
  } else {

    is_err = Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);
    if (is_err) {
      fprintf(stderr,"Mix_OpenAudio: %s\n", Mix_GetError());
    }
  }


  if (!is_err) {
    flags_set = Mix_Init(flags);
  }

  if ( (flags_set & flags) != flags) {
    fprintf(stderr, "mixer_init : initialisation failed (%s)\n", Mix_GetError());
  } else {
    atexit(mix_quit);
  }

  return flags ^ flags_set;
}


