#include "gamengin.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include <iostream>

#include "const.h"

#include "system.h"
#include "scribbler.h"

#include "poly.h"

#define DELTA_ROW		20
#define DELTA_COL		20

struct Wind
{
	VidWindow	mW;
	int		mStepX;
	int		mStepY;
};

extern 	void log(char const str[]);
extern char	logchunk[512];

class	CordList
{
  public:
	CordList();
	~CordList();

	void	append(int x, int y);

	int	get(int id, int &x, int &y);

  private:
	int	mCount;
	int	mMax;

	int	*mX;
	int	*mY;
};


class VirtScribbler: public GameEngine
{
	ServerSerial	*mPort;

	static int const max_rows = 600;
	static int const max_cols = 800;

	Poly		**mLines;
	int		mLinesCount;
	int		mLinesMax;	

	Poly		*mRobot[4];
	Poly		*mLeftAntenna;
	Poly		*mRightAntenna;

	SDL_Surface	*mLogoImage;
	SDL_Surface 	*mScribblerImage;
	SDL_Surface 	*mFlukeImage;
	Mix_Music	*mBeep;
	Data		*mJpeg;

	Data		*mJpegColorHeader;
	Data		*mJpegColorScan;
	Data		*mJpegGrayHeader;
	Data		*mJpegGrayScan;
	Data		*mBitmap;
	Data		*mImage;

	
// Sensors
	char	mLeftIR;
	char	mRightIR;
	int	mLeftLight;
	int	mCenterLight;
	int	mRightLight;
	char	mLineLeft;
	char	mLineRight;
	char	mStall;

// LEDS
	int	mLeftLED;
	int	mCenterLED;
	int 	mRightLED;

// Scribbler State
	char	IPRE_data[8];
	char	mName[16];
	char	mPass[16];
	int	mSilent; 

	int	mEchoMode;
	int	mLeftMotor;
	int	mRightMotor;

// Fluke State
	int	mForwardess;
	int	mFlukeRightIR;
	int	mFlukeCenterIR;
	int	mFlukeLeftIR;
	int	mFlukeBackLED;
	int	mFlukePowerIR;
	int	mFlukeLED;
	int	mFlukeUseEmitters;
	Wind	mWin[4];

	int	mRobotRow;
	int	mRobotCol;
	double	mOrientation;
	double	mTorque;
	double	mSpeed;

	CordList	mLightSources;

	bool	mLoaded;

	char	*mFileName;
	char	board[max_rows/DELTA_ROW] [max_cols/DELTA_COL];	

	int	getDistance(double theta);

	void	getData(unsigned char data[8]);
	void	echoCommand(char cmd, unsigned char data[8]);
	void	Update();
	int	scribblerCommand(char cmd);
	int	flukeCommand(char cmd);

public:
	VirtScribbler(ServerSerial &, char const []);
	~VirtScribbler();
	void	AdditionalInit();
	void 	UpdateInternals(int iElapsedTime);
	void 	UpdateScreen(SDL_Surface* pDestSurface);
	virtual void 	MouseButtonDown(int iButton, 
				int iX, int iY, int iRelX, int iRelY);

	void 	OnKeyPress(int iKeyEnum);

	char	getCharBlocking();

	void 	scribblerCmd(char cmd);

	void	sendAllSensors();

	void	Thread();

};

