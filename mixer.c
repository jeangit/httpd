// $$DATE$$ : ven. 02 novembre 2018 (19:47:55)
#include<stdio.h>
#include <SDL2/SDL_mixer.h>


int mixer_init()
{
  int is_err = 0;

  int flags = MIX_INIT_OGG;
  int flags_set = Mix_Init(flags);

  if ( (flags_set & flags) != flags) {
    fprintf(stderr, "mixer_init : initialisation failed (%s)\n", Mix_GetError());
    is_err = 1;
  }

  return is_err;
}


void mix_quit()
{
  Mix_Quit();
}
