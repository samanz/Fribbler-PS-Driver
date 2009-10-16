
/************************************************************************/
/* This software was developed at the RoboLab of Brooklyn College 	*/
/* by John Cummins under the supervision and inspiration of 		*/
/* Professors Sklar and Parsons						*/
/************************************************************************/

using namespace std;

#include <unistd.h>
#include "SDL_gfxPrimitives.h"
#include <cmath>
#include "system.h" 

int	gDebugging = 0;
const	int	thisModule  = 0x0008;

void log(char const str[])
{
	static	int firstTime = 1;

	if (firstTime)
	{
		unlink("log");
		firstTime = 0;
	}

	FILE	*fd = fopen("log", "a");

	fprintf(fd, str);

	fclose(fd);
}
char	logchunk[512];




#include "VirtScribbler.h"


SERVER_SERIAL	port(ADDRESS);

 
int main(int argc, char **argv)   
{
	if (argc > 1)
                gDebugging = atoi(argv[1]);


       	VirtScribbler Engine(port, "world");

	Engine.Start();
 
	Engine.Run();
 
	return 0;
}
/************************************************************************/
/* mOrientation is the angle in radians that the robot lates to up	*/
/* or due north. Positive is in counter clockwise direction		*/
/* The front of the robot is directly between the antenna.		*/
/*									*/
/*  (0, 0)			col					*/
/*     +-------------------------------------------------------> x	*/
/*     | 								*/
/*     | 								*/
/*     | 								*/
/*     | 								*/
/*     | 								*/
/* row | 								*/
/*     | 								*/
/*     | 								*/
/*     | 								*/
/*     | 								*/
/*     | 								*/
/*     | 								*/
/*     | 								*/
/*     V								*/
/*     y								*/
/*									*/
/* Note (x, y) is (col, row)						*/
/*									*/
/*									*/
/*									*/
/*       -----                        ---		^		*/
/*      /		           ^	 \		|		*/
/*     |   +			   |   -  |		|		*/
/*     \			   \	  /		|		*/
/*      --->                        ------      			*/
/*	positive		   negative		0		*/ 
/************************************************************************/
VirtScribbler::VirtScribbler(SERVER_SERIAL &port, char const fileName[]) : GameEngine(), mPort(&port)
{
	int	row, col;

	mFileName = NULL;
	if (fileName)
	{
		mFileName = new char[strlen(fileName) + 1];
		strcpy(mFileName, fileName);
	}
	
	for (row = 0;row < max_rows/DELTA_ROW; row++)
		for (col = 0;col < max_cols/DELTA_COL;col++)
			board[row][col] = ' ';
	mRobotRow = max_rows/2;
	mRobotCol = max_cols/2;
	mEchoMode = 1;
	mLoaded = true;

	mLeftIR      = 0;
	mRightIR     = 0;
	mLeftLight   = 100;
	mCenterLight = 100;
	mRightLight  = 100;
	mLineLeft    = 0;
	mLineRight   = 0;
	mStall       = 0;

	mSilent = 0;

	mLeftLED = mCenterLED = mRightLED = 0;

	mFlukeLED = 0;
	mFlukeBackLED = 0;

	mLeftMotor = 100;
	mRightMotor = 100;

	mOrientation =  0;

	mForwardess = 0;

	mTorque = 0.0; //M_PI/2;
	mSpeed = 0.0;

	mRobot[0] = new Poly("(-24, -12), (-16, -12), (-16, 12), (-24, 12), (-24, 0)  (-20, 0)"); // Left Wheel
	mRobot[1] = new Poly("( 24, -12), ( 16, -12), ( 16, 12), ( 24, 12), ( 24, 0)  ( 20, 0)"); // Right Wheel
	mRobot[2] = new Poly("(-16,  -2), ( 16,  -2), ( 16,  2), (-16,  2)"); // Axle 
	mRobot[3] = new Poly("(-8,   -8), (  8,  -8), (  8,  8), ( -8,  8)"); // Box

	mLeftAntenna   = new Poly("(0, 0), (-15, -19), (-30, -38), (-6, -38), (-3, -19)");
	mRightAntenna  = new Poly("(0, 0), ( 15, -19), ( 30, -38), ( 6, -38), ( 3, -19)");

	mLinesMax = 10;
	mLines = new Poly *[mLinesMax];
	mLinesCount = 0;
} 
VirtScribbler::~VirtScribbler()
{
	log("VirtScribbler distructor\n");
}
/****************************************************************/
/* Load the initial state of the Robot World			*/
/* Currently we just have obstacles 'X' and light sources 'L'	*/
/* We may later have lines for the line sensors			*/
/****************************************************************/
void	VirtScribbler::AdditionalInit()
{
	FILE	*fp;
	char	buf[128];
	int	row, col;

	SetTitle("Robot World from Agents Lab at Brooklyn College");

	mLogoImage = IMG_Load("logo.gif");
	if (!mLogoImage)
		mLogoImage = IMG_Load("logo.jpg");
	if (!mLogoImage)
		log("Can't load logo image\n");
	mScribblerImage = IMG_Load("scribbler.gif");
	if (!mScribblerImage)
		mScribblerImage = IMG_Load("scribbler.jpg");
	if (!mScribblerImage)
		log("Can't load Scribbler image\n");
	mFlukeImage = IMG_Load("fluke.gif");
	if (!mFlukeImage)
		mFlukeImage = IMG_Load("fluke.jpg");
	if (!mFlukeImage)
		log("Can't load Fluke image\n");


	mBeep = Mix_LoadMUS("sampleBeep.wav");
	if (!mBeep)
		log("Can't load sampleBeep.wav\n");

	mJpeg = loadNamedData("samplephoto.jpeg", 0);
	if (!mJpeg)
		log("Can't load samplePhoto.jpeg\n");


	mJpegColorHeader = loadNamedData("samplechead.dat", LITTLENDIAN);
	if (!mJpegColorHeader)
		log("Can't load samplechead.dat\n");
	mJpegColorScan   = loadNamedData("samplecscan.dat", LITTLENDIAN);
	if (!mJpegColorScan)
		log("Can't load samplecscan.dat\n");
	mJpegGrayHeader = loadNamedData("sampleghead.dat", LITTLENDIAN);
	if (!mJpegGrayHeader)
		log("Can't load samplechead.dat\n");
	mJpegGrayScan   = loadNamedData("samplegscan.dat", LITTLENDIAN);
	if (!mJpegGrayScan)
		log("Can't load samplegscan.dat\n");
	
	mImage   = loadNamedData("sampleimage.dat", LITTLENDIAN);
	if (!mImage)
		log("Can't load sampleimage.dat\n");
	mBitmap   = loadNamedData("samplebitmap.dat", LITTLENDIAN);
	if (!mBitmap)
		log("Can't load samplebitmap.dat\n");

	if (!mFileName)
		return;

	fp = fopen(mFileName, "r");
	if (!fp)
	{
		cerr << "can't open " << mFileName << endl;
		return;
	}
	for (;fgets(buf, 100, fp);)
	{
		if (strncmp(buf, "LINE", 4) == 0)
		{
			Poly *pol = new Poly(&buf[4]);
			if (mLinesCount == mLinesMax)
			{
				Poly 	**p = new Poly *[mLinesMax + 10];
				for (int i = 0;i < mLinesMax;i++)
					p[i] = mLines[i];
				mLinesMax += 10;
				delete [] mLines;
				mLines = p;
			}
			pol->position(0, 0, 0.0);
			pol->commit();
			mLines[mLinesCount++] = pol;
			
		}
		else if (strncmp(buf, "WORLD", 5) == 0)
			break;
	}
			

	for (row = 0;row < max_rows/DELTA_ROW;row++)
	{
		if (!fgets(buf, 100, fp))
			break;
		for (col = 0;col < max_cols/DELTA_COL;col++)
		{
			if (buf[col] == 0 || buf[col] == '\n')
				break;
		}
		for (;col < max_cols/DELTA_COL;col++)
			buf[col] = ' ';
		for (col = 0;col < max_cols/DELTA_COL;col++)
		{
			switch (buf[col])
			{
			  case 'X':
				board[row] [col] = 'X';
				break;
			  case 'L':
				board[row] [col] = 'L';
				mLightSources.append(col * DELTA_COL + DELTA_COL/2,
						     row * DELTA_ROW + DELTA_ROW/2);	
				break;
			  default:
				board[row] [col] = ' ';
				break;
			}
		}
	}
	for (;row < max_rows/DELTA_ROW;row++)
	{
		for (col = 0;col < max_cols/DELTA_COL;col++)
			board[row] [col] = ' ';
	}	
	fclose(fp);
} 
int	VirtScribbler::getDistance(double theta)
{
	int	dist;
	int	row, col;
	double	c = cos(theta);
	double	s = sin(theta);

	for (dist = 5;dist < 2000;dist += 5)
	{
		row = mRobotRow - dist * c;
		col = mRobotCol - dist * s;
		while (row < 0)
			row += max_rows;
		while (row >= max_rows)
			row -= max_rows;
		while (col < 0)
			col += max_cols;
		while (col >= max_cols)
			col -= max_cols;
		if (board[row/DELTA_ROW] [col/DELTA_COL] == 'X')
			return dist;
	}
	return 9999;
}

char	VirtScribbler::getCharBlocking()
{
	unsigned char	ch;
	int	ret;

	ret = mPort->getCharBlocking(ch);
 
	return ch;
}

void VirtScribbler::UpdateInternals(int iElapsedTime )
{
	if (mLoaded)
		Update();
}
/****************************************************************/
/* Thread							*/
/* This is the "background" thread that communicates with	*/
/* the socket and updates the internal representation		*/
/* of the robot							*/
/****************************************************************/
void	VirtScribbler::Thread()
{
	unsigned char	ch;

	Lock();

	if (gDebugging & thisModule)
		log("Thread 1\n");

	while (true)
	{
		Unlock();
		for (;;)
		{
			if (mPort->getChar(ch))
				break;
			if (!mPort->getStatus())
			{
				mPort->waitForConnection();
				mRobotRow = max_rows/2;
				mRobotCol = max_cols/2;
			}
			else
				SDL_Delay(100);
		} 
		Lock();

		if (!flukeCommand(ch))
		{
			if (!scribblerCommand(ch))
			{
				if (gDebugging & thisModule)
				{
					sprintf(logchunk, "Unknown command %d\n", ch);
					log(logchunk);
				}
			}
		}
	}
}

/****************************************************************/
/* Update							*/
/* Update the internal state of the robot			*/
/*								*/
/* We also construct the image of the robot in the		*/
/* correct orientation.						*/
/****************************************************************/ 
void VirtScribbler::Update(  )
{
	int	i;
	int	row, col;
	double	orientation;
	short	xs[10];
	short	ys[10];
	int	count;
	int	lightCol, lightRow;


	/* get the tenative robot parameters			*/
	/* mOrientation is pointing straight up			*/
	/* as row decreases as we go up we need to subtract	*/
	/* the delta row					*/
	row = mRobotRow - mSpeed * cos(mOrientation);
	col = mRobotCol - mSpeed * sin(mOrientation);
	if (row < 0)
		row += max_rows;
	if (row >= max_rows)
		row -= max_rows;
	if (col < 0)
		col += max_cols;
	if (col >= max_cols)
		col -= max_cols;
	orientation = mOrientation + mTorque;
	while (orientation > M_PI)
	{
		orientation -= 2 * M_PI;
	}
	while (orientation < -1 * M_PI)
	{
		orientation += 2 * M_PI;
	}

//sprintf(logchunk, "orent %f\n", orientation);
//log(logchunk);
	/* can the robot go there?			*/
	mStall = 0;
	for (i = 0;i < 4;i++)
	{
		mRobot[i]->position(col, row, orientation);
	}
	for (i = 0;i < 4;i++)
	{
		count = mRobot[i]->getTentative(xs, ys);
		for (int j = 0;j < count;j++)
		{
			if (board[ys[j]/DELTA_ROW] [xs[j]/DELTA_COL] == 'X')
				mStall = 1;
		}
	}
	if (board[row/DELTA_ROW] [col/DELTA_COL] == 'X')
		mStall = 1;
	if (mStall)
	{
		/* robot can't go to tentative parameters	*/
		/* so we stay where we are and stop		*/
		mSpeed = 0;
//		mTorque = 0;
		return;
	}
	/* robot can go there so commit				*/
	for (i = 0;i < 4;i++)
		mRobot[i]->commit();
	mLeftAntenna->position(col, row, orientation);
	mLeftAntenna->commit();
	mRightAntenna->position(col, row, orientation);
	mRightAntenna->commit();


	mOrientation = orientation;
	mRobotRow = row;
	mRobotCol = col;
//sprintf(logchunk, " (%d, %d) %f\n", col, row, orientation);
//log(logchunk);

	/* update the IR sensors				*/
	/* set IR 1 means clear, 0 means blocked		*/
	mRightIR = mLeftIR = 1;
	count = mLeftAntenna->getActual(xs, ys);
	for (i = 1;i < count;i++)
	{
		if (board[ys[i]/DELTA_ROW] [xs[i]/DELTA_COL] == 'X')
			mLeftIR = 0;
	}
	count = mRightAntenna->getActual(xs, ys);
	for (i = 1;i < count;i++)
	{
		if (board[ys[i]/DELTA_ROW] [xs[i]/DELTA_COL] == 'X')
			mRightIR = 0;
	}
	/* update the Light Sensors				*/
	/* atan2 returns theta in range -M_PI to M_PI		*/
	mLeftLight   = mCenterLight = mRightLight  = 100;
	for (i = 0;mLightSources.get(i, lightCol, lightRow);i++)
	{
		double deltaRow = mRobotRow - lightRow  + 1.0;
		double deltaCol = mRobotCol - lightCol + 1.0;	
		double lenSq = deltaRow * deltaRow + deltaCol * deltaCol;
		double len = sqrt(lenSq);
		double t = deltaCol/deltaRow;
		double s = deltaCol / len;
		double c = deltaRow / len;
		double rho = (atan2(deltaCol, deltaRow) - mOrientation);
//		double rho = (atan2(deltaRow, deltaCol) - mOrientation);

		/* rho in range 0 to 2 * M_PI			*/
		while (rho > M_PI)
			rho -= 2 * M_PI;
		while (rho <  -M_PI)
			rho += 2 * M_PI;

		double light = 4000000.0/lenSq;

//		sprintf(logchunk, "rho = %f, orient %f\n", rho, mOrientation);
//		log(logchunk);

		if (rho < 1.0 && rho > 0.3)
		{
//			log("Light Left\n");
			mLeftLight += light;
		}
		else if (rho < 0.3 && rho > - 0.3)
		{
//			log("Light Center\n");
			mCenterLight += light;
		}
		else if (rho < -0.3 && rho > -1.0)
		{
//			log("Light Right\n");
			mRightLight += light;
		}
	}
			
}

/****************************************************************/
/* UpdateScreen							*/
/* Transfer the internal state of the robot to the screen	*/
/****************************************************************/ 
void VirtScribbler::UpdateScreen( SDL_Surface* screen )
{
	int		count;
  	int 		row, col, yofs, ofs;
	unsigned int	val;
	short		xs[10], ys[10];
	short		x, y;

  	// Draw to screen
  	yofs = 0;
  	for (row = 0; row < max_rows; row++)
  	{
    		for (col = 0, ofs = yofs; col < max_cols + 1; col++, ofs++)
    		{
			if (row%DELTA_ROW == 0 || col%DELTA_COL == 0)
				val = SDL_MapRGB(screen->format, 250,  0,  0);
			else 
			{
				switch (board[row/DELTA_ROW] [col/DELTA_COL])
				{
				  case 'X':
					val = SDL_MapRGB(screen->format,   0, 250,   0);
					break;
				  case 'L':
					val = SDL_MapRGB(screen->format, 250, 250,   0);
					break;
				  default:
					val = SDL_MapRGB(screen->format, 250, 250, 250);
					break;
				}
			}
      			((unsigned int*)screen->pixels)[ofs] = val;
    		}
    		yofs += screen->pitch / 4;
  	}
	/* put the lines on the screen				*/
	for (int i = 0;i < mLinesCount;i++)
	{
		count = mLines[i]->getActual(xs, ys);
		for (int j = 1;j < count;j++)
		{
			lineRGBA(screen, xs[j-1], ys[j-1], xs[j], ys[j],
					0, 0, 0, 255);
		}
	}
	/* put the obstacles on the screen			*/
	for (row = 0; row < max_rows; row += DELTA_ROW)
  	{
    		for (col = 0; col < max_cols; col += DELTA_COL)
    		{
			switch (board[row/DELTA_ROW] [col/DELTA_COL])
			{
			  case 'X':
				boxRGBA(screen, col, row ,
						col + DELTA_COL, row + DELTA_ROW,
						    0, 250, 0, 255);
				break;
			}
    		}
    	}
	mLineLeft = mLineRight   = 0;

	/* put the robot on the screen, First the Antenna	*/
	count = mLeftAntenna->getActual(xs, ys);
	polygonRGBA(screen, xs, ys, count, 0, 0, 0, 250);
	count = mRightAntenna->getActual(xs, ys);
	polygonRGBA(screen, xs, ys, count, 0, 0, 0, 250);

	/* now the wheels, left first					*/
	count = mRobot[0]->getActual(xs, ys);
	unsigned int black = SDL_MapRGB(screen->format, 0,  0,  0);
	/* check for black line						*/
	for (y = -10; y < 10;y++)
	{
		if ((y + ys[count - 1]) < 0)
			continue;
		if ((y + ys[count - 1]) >= max_rows)	
			continue;
		yofs = (y + ys[count - 1]) * screen->pitch / 4;
		for (x = -10;x < 10;x++)
		{
			if ((x + xs[count - 1]) < 0)
				continue;
			if ((x + xs[count - 1]) >= max_cols)
				continue;
			ofs = yofs + (x + xs[count - 1]);
			if (ofs < 0)
				continue;
			if (((unsigned int*)screen->pixels)[ofs] == black)
				 mLineLeft = 1;
 		}
	}
	filledPolygonRGBA(screen, xs, ys, count - 1, 0, 0, 0, 250);


	/* now the right wheel						*/
	count = mRobot[1]->getActual(xs, ys);
	/* check for black lines					*/
	for (y = -10; y < 10;y++)
	{
		if ((y + ys[count - 1]) < 0)
			continue;
		if ((y + ys[count - 1]) >= max_rows)	
			continue;
		yofs = (y + ys[count - 1]) * screen->pitch / 4;
		for (x = -10;x < 10;x++)
		{
			if ((x + xs[count - 1]) < 0)
				continue;
			if ((x + xs[count - 1]) >= max_cols)
				continue;
			ofs = yofs + (x + xs[count - 1]);
			if (ofs < 0)
				continue;
			if (((unsigned int*)screen->pixels)[ofs] == black)
				 mLineRight = 1;
 		}
	}
	filledPolygonRGBA(screen, xs, ys, count - 1, 0, 0, 0, 250);

	/* noe the rest of the robot					*/
	for (int i = 2;i < 4;i++)
	{
		count = mRobot[i]->getActual(xs, ys);
		filledPolygonRGBA(screen, xs, ys, count, 0, 0, 0, 250);
	}
	filledEllipseRGBA(screen,  mRobotCol, mRobotRow, 12,  12,  0,   0,   250, 250);

	/* now Blit the canned images				*/
	SDL_Rect	src, dest;

	src.x = 0; src.y = 0;
	src.w = 100; src.h = 100;
	
	dest.x = 800; dest.y = 500;
	/* the Scribbler LEDs					*/
	filledEllipseRGBA(mScribblerImage,  46,  65, 3, 3,  mLeftLED   ? 255 : 0,   0,   0, 250);
	filledEllipseRGBA(mScribblerImage,  54,  65, 3, 3,  mCenterLED ? 255 : 0,   0,   0, 250);
	filledEllipseRGBA(mScribblerImage,  62,  65, 3, 3,  mRightLED  ? 255 : 0,   0,   0, 250);
	
	SDL_BlitSurface(mScribblerImage, &src, screen, &dest);

	dest.x = 800; dest.y = 350;
	/* the Fluke LEDs					*/
	filledEllipseRGBA(mFlukeImage,  22,  35, 3, 3,  mFlukeLED  ? 255 : 0,   0,   0, 250);
	filledEllipseRGBA(mFlukeImage,  3,  3, 3, 3,  mFlukeBackLED,   0,   0, 250);
	SDL_BlitSurface(mFlukeImage, &src, screen, &dest);

	dest.x = 810; dest.y = 10;
	SDL_BlitSurface(mLogoImage, &src, screen, &dest);
}
/****************************************************************/
/* User interface functions					*/
/* currently we ignore the user					*/
/****************************************************************/ 
void 	VirtScribbler::MouseButtonDown(int iButton, 
				    int iX, int iY, 
				    int iRelX, int iRelY)
{
	if (iX > 800)
		return;
	mRobotRow = iY;
	mRobotCol = iX;
	for (int i = 0;i < 4;i++)
	{
		mRobot[i]->position(mRobotCol, mRobotRow, mOrientation);
		mRobot[i]->commit();
	}
	mLeftAntenna->position(mRobotCol, mRobotRow, mOrientation);
	mLeftAntenna->commit();
	mRightAntenna->position(mRobotCol, mRobotRow, mOrientation);
	mRightAntenna->commit();

}
void 	VirtScribbler::OnKeyPress(int iKeyEnum) 
{
	switch (iKeyEnum)
	{
	  case SDLK_UP:
		mOrientation = 0;
		break;
	  case SDLK_DOWN:
		mOrientation = M_PI;
		break;
	  case SDLK_RIGHT:
		mOrientation = 3 * M_PI / 2;
		break;
	  case SDLK_LEFT:
		mOrientation = M_PI/2;
		break;
	}
}
	
CordList::CordList()
{
	mCount = 0;
	mMax = 10;

	mX = new int[mMax];
	mY = new int[mMax];
}
CordList::~CordList()
{
	delete [] mX;
	delete [] mY;
}
void	CordList::append(int x, int y)
{
	if (mCount == mMax)
	{
		int *newX = new int[mMax + 10];
		int *newY = new int[mMax + 10];

		for (int i = 0;i < mMax;i++)
		{
			newX[i] = mX[i];
			newY[i] = mY[i];
		}
		delete [] mX;
		delete [] mY;

		mX = newX;
		mY = newY;

		mMax += 10;
	}
 	mX[mCount] = x;
	mY[mCount] = y;
	mCount++;
}
int	CordList::get(int id, int &x, int &y)
{
	if (id < mCount)
	{
		x = mX[id];
		y = mY[id];
		return 1;
	}
	else
		return 0;
}
