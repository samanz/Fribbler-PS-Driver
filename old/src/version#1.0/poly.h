#ifndef POLY_STUFF
#define POLY_STUFF

class	Poly
{
	int	mPointCount;
	int	mMaxPoints;
	short	*mXdef;
	short	*mYdef;
	short	*mXtentative;
	short	*mYtentative;
	short	*mXactual;
	short	*mYactual;
  public:
	Poly(char const parms[]);

	void	say(FILE *fp);

	void	position(int x, int y, double theta);

	void	commit();

	int	getTentative(short *x, short *y);
	int	getActual(short *x, short *y);
};
#endif

