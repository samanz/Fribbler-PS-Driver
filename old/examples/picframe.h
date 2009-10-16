#ifndef _PicFrame_
#include "SDL/SDL.h"
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#include "SDL_mixer.h"
#include <SDL/SDL_thread.h>

class	PicFrame
{
	SDL_Surface *screen;
	SDL_Surface *image;
  public:
	PicFrame();
	~PicFrame();

	void	loadPic(void *, int);
};
#endif
