using namespace std;
#include <stdlib.h>
#include "gamengin.h"
#include <iostream>
#include <assert.h>


extern  int	gDebugging;
const	int	thisModule  = 0x0004;

extern void	log(char const []);
extern char	logchunk[512];



 

GameEngine::GameEngine()
{
	mPriorTick	= 0;
	mWidth		= 900;
	mHeight		= 600;
	mTitle		= 0;
 
	mScreen		= 0;

	mMinimized	= false;

	mThread         = 0;

	mMutex = SDL_CreateMutex();

	mQuit 		= false;

}
 
GameEngine::~GameEngine()
{
	log("GameEngin distructor\n");
	SDL_DestroyMutex(mMutex);
	log("about to Kill Thread\n");
// SDL_KillThread does not work on OS X causes "The Spinning Pizza of Death" SPOD
//	SDL_KillThread(mThread);
	log("Thread Killed\n");
	SDL_Quit();
	log("SDL_Quit\n");
	
}
 
void GameEngine::SetSize(int width, int height)
{
	mWidth  = width;
	mHeight = height;
	mScreen = SDL_SetVideoMode(width, height, 0, SDL_SWSURFACE);
}
 
/** Initialize SDL, the window and the additional data. **/
void GameEngine::Init()
{
	// set up some constants for the mixer
  	int audio_rate = 22050;
  	Uint16 audio_format = AUDIO_S16; 
  	int audio_channels = 2;
  	int audio_buffers = 4096;
	// Register SDL_Quit to be called at exit, to clean up SDL.
	atexit(SDL_Quit);
 
	// Initialize SDL's subsystems - in this case, only video and audio.
	if (SDL_Init( SDL_INIT_VIDEO  | SDL_INIT_AUDIO) < 0) 
	{
		cerr << "Unable to init SDL: %s\n" << SDL_GetError();
		exit( 1 );
	}

  	if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers)) 
	{
    		sprintf(logchunk, "Unable to open audio! %s\n", SDL_GetError());
		log(logchunk);
//    		exit(1);
  	}
  


	// Attempt to create a window with the specified height and width.
	SetSize( mWidth, mHeight );
 
	// If we fail, abort
	if ( mScreen == NULL ) 
	{
   		sprintf(logchunk, "Unable to set up video:  %s\n", SDL_GetError());
		log(logchunk);
		exit( 1 );
	}
	AdditionalInit();
}
 
void GameEngine::Run()
{
	Init();
	mPriorTick = SDL_GetTicks();

	// Main loop: 
	while ( !mQuit )
	{
		// Handle mouse and keyboard input
		EventLoop();
 
		if ( mMinimized ) 
		{
			// don't do anything if small
		} 
		else 
		{
			UpdateInternalRep();
			UpdateScreenRep();
		}
		SDL_Delay(100);
	}
	log("Quitting\n");
 
	End();
}

void GameEngine::EventLoop()
{
	// proccess events we are interested in
	SDL_Event event;

	while (SDL_PollEvent(&event)) 
	{
		switch (event.type) 
		{
		case SDL_KEYDOWN:
			// If escape is pressed set the Quit-flag
			if (event.key.keysym.sym == SDLK_ESCAPE)
			{
				log("SDLK_ESCAPE event\n");
				mQuit = true;
				break;
			}
 			OnKeyPress(event.key.keysym.sym);
			break;
 
		case SDL_KEYUP:
			OnKeyRelease(event.key.keysym.sym);
			break;
 
		case SDL_QUIT:
			mQuit = true;
			break;
 
		case SDL_MOUSEMOTION:
			MouseMoved(event.button.button, event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel);
			break;
 
		case SDL_MOUSEBUTTONUP:
			MouseButtonUp(event.button.button, event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel);
			break;
 
		case SDL_MOUSEBUTTONDOWN:
			MouseButtonDown(event.button.button, event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel);
			break;
 
		case SDL_ACTIVEEVENT:
			if (event.active.state & SDL_APPACTIVE) 
			{
				if (event.active.gain) 
				{
					mMinimized = false;
					WindowActive();
				} 
				else 
				{
					mMinimized = true;
					WindowInactive();
				}
			}
			break;
		} 
	} 
}
 
void GameEngine::UpdateInternalRep() 
{
	long elapsedTicks = SDL_GetTicks() - mPriorTick;
	mPriorTick = SDL_GetTicks();


	UpdateInternals(elapsedTicks);
}
 
void GameEngine::UpdateScreenRep()
{
	SDL_FillRect(mScreen, 0, SDL_MapRGB(mScreen->format, 0, 0, 0 ));
 
// on OS X we think we are using HW surface and hence MUSTLOCK
// but then we can't Blit
// so the Locking is disabled
#ifdef UNDEFINED
	// Lock surface if needed
	if (SDL_MUSTLOCK(mScreen))
		if (SDL_LockSurface(mScreen) < 0 )
			return;
#endif
 
	Lock(); /* lock out other threads */
	/* we now own the Mutex */
	/* so the representation will not change while we report it */
	UpdateScreen(GetSurface()); 

	Unlock();
 
#ifdef UNDEFINED
	// Unlock if needed
	if (SDL_MUSTLOCK(mScreen)) 
		SDL_UnlockSurface(mScreen);
#endif
 
	// Tell SDL to update the whole screen
	SDL_Flip(mScreen);
}
 
/** Sets the title of the window 
**/
void GameEngine::SetTitle(char const title[])
{
	mTitle = title;
	SDL_WM_SetCaption(title, 0);
}
 
/** Retrieve the title of the application window.
	return The current title
**/
const char* GameEngine::GetTitle()
{
	return mTitle;
}
 
/** Retrieve the main screen surface.
	return A pointer to the SDL_Surface surface
**/
SDL_Surface* GameEngine::GetSurface()
{ 
	return mScreen;
}

/**
This code is heavely dependent on SDL (SimpleDirectLayer) which allows device
independent 2D graphics to be even simpler that device dependent graphics.
Much use was made of the code in "Programming Lixux Games" by John R. Hall
Which is a tutorial in SDL and C. My part is in porting this to a simple
C++ object and impleminting a simple version of John Conway's Game of Life
Some code may have been copied verbatum with only the names changed for
ease of understanding
Credit it hereby given 
JC
**/

/* functions necessary to manage dual threading  */
/* a static function to satisfy SDL_CreateThread */
int GameEngine::EntryPoint(void *p)
{
	GameEngine	*pt = (GameEngine *)p;
	pt->Thread();

	return 1;
}
/* to Start the Thread                                            */
/* note how the this pointer is passed to the static function     */
/* so we can start the Thread nicely                              */
/* with out this hack I was reduced to only having global objects */
int	GameEngine::Start()
{
	mThread = SDL_CreateThread(GameEngine::EntryPoint, this);
	SDL_Delay(100);

	return 1;
}
/* Lock, Unlock simple low level semaphore to avoid updating our */
/* internals as we changing them, also keeps all SDL stuff in the*/
/* "main" Thread as SDL is not Thread safe                       */
void	GameEngine::Lock()
{
	int	ret;

	ret = SDL_LockMutex(mMutex);

	assert(ret == 0);
}
void	GameEngine::Unlock()
{
	int	ret;

	ret = SDL_UnlockMutex(mMutex);

	assert(ret == 0);
}

		
	
 


