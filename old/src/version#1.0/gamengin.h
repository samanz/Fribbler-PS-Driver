#ifndef GameEngine_H
#define GameEngine_H
 
#include "SDL/SDL.h"
#include <SDL/SDL_image.h>
//#include <SDL/SDL_ttf.h>
#include "SDL_mixer.h"
#include <SDL/SDL_thread.h>

extern	int	Thread(void *);
 
/** The base GameEngine class. **/
class GameEngine  
{
private:
	/** Last iteration's tick value **/
	long mPriorTick;

	int mWidth;
	int mHeight;
 
	char const *mTitle;
 
	/** Screen surface **/
	SDL_Surface* mScreen;

	SDL_mutex	*mMutex;
	SDL_Thread	*mThread;
 
	/** Is the window minimized? **/
	bool mMinimized;
 
	void Init();
	virtual void UpdateInternalRep();
	void UpdateScreenRep();
 
protected:
	/** Do we want to terminate window**/
	bool 	mQuit;
 
	static	int EntryPoint(void *p);

	void SetSize(int width, int height);
 
	void EventLoop();
 
public:
	GameEngine();
	virtual ~GameEngine();
 
	void Run();

	/* overloaded any additional initilization */
	virtual	void AdditionalInit() {};

	/** OVERLOADED - All the games calculation and updating. 
		param iElapsedTime milliseconds since called last.
	**/
	virtual void UpdateInternals(int elapsedTime )  = 0;

	/** OVERLOADED - All the games UpdateScreening. 
		param pDestSurface The main screen surface.
	**/
	virtual void UpdateScreen(SDL_Surface* pDestSurface ) {}
 
	/* OVERLOADED - Allocated data that should be cleaned up. */
	virtual void End() {}
 
	/* OVERLOADED - Window is active  */
	virtual void WindowActive() {}
 
	/* OVERLOADED - Window is inactive. */
	virtual void WindowInactive() {}
 
 
	/** OVERLOADED - Keyboard key has been released. */
	virtual void OnKeyRelease(int key) {}
 
	/* OVERLOADED - Keyboard key has been pressed. */
	virtual void OnKeyPress(int key) {}
 
 
	/* OVERLOADED - The mouse has been moved.  */
	virtual void MouseMoved		(int button, int x, int y, int delx, int dely) {}
 
	/* OVERLOADED - A mouse button has been released. */
	virtual void MouseButtonUp	(int button, int x, int y, int delx, int dely) {}
 
	/* OVERLOADED - A mouse button has been released. */
	virtual void MouseButtonDown	(int button, int x, int y, int delx, int dely) {}


 
	void		SetTitle	(char const title[]);
	const char* 	GetTitle	();
 
	SDL_Surface* 	GetSurface	();

	int	Start();	
	virtual	void	Thread()	{}
	void	Lock();
	void	Unlock();
	
};
 
#endif
