#include "picframe.h"


PicFrame::PicFrame()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0) 
	{
		printf("Unable to initialize SDL: %s\n", SDL_GetError());
		return;
	}

	screen = SDL_SetVideoMode(160, 120, 0, SDL_SWSURFACE);
	if (screen == NULL) 
	{
		printf("Unable to set video mode: %s\n", SDL_GetError());
		return;
	}
}
PicFrame::~PicFrame()
{
//	SDL_FreeSurface(image);
}
void	PicFrame::loadPic(void *mem, int size)
{
	SDL_Event	event;

	for (;SDL_PollEvent(&event) != 0;)
	{
		switch (event.type)
		{ 
		  case SDL_QUIT:
			exit(0);
			break;
		  case SDL_KEYDOWN:
			// If escape is pressed set the Quit-flag
			if (event.key.keysym.sym == SDLK_ESCAPE)
			{
				exit(0);
			}
			break;
		}
	}

	SDL_Surface *temp;

	temp = IMG_LoadJPG_RW(SDL_RWFromMem(mem, size));
	image = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);

	SDL_Rect src, dest;
 
	src.x = 0;
	src.y = 0;
	src.w = image->w;
	src.h = image->h;
 
	dest.x = 0;
	dest.y = 0;
	dest.w = image->w;
	dest.h = image->h;

	SDL_BlitSurface(image, &src, screen, &dest);

	SDL_Flip(screen);
}

