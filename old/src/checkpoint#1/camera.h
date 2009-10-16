/****************************************************************/
/* YUVRange							*/
/* a range of colors						*/
/****************************************************************/
class	YUVRange
{
  private:
	unsigned char	mYFrom;
	unsigned char	mYTo;
	unsigned char	mUFrom;
	unsigned char	mUTo;
	unsigned char	mVFrom;
	unsigned char	mVTo;
  public:
	YUVRange();
 	YUVRange(int Y, int U, int V);


	void	setY(unsigned char from, unsigned char to);
	void	setU(unsigned char from, unsigned char to);
	void	setV(unsigned char from, unsigned char to);


	void	getY(unsigned char &from, unsigned char &to);
	void	getU(unsigned char &from, unsigned char &to);
	void	getV(unsigned char &from, unsigned char &to);
 
	
	void	expandY(int del);
	void	expandV(int del);
	void	expandU(int del);
	
	char	*say();

};




/****************************************************************/
/* VidWindow							*/
/* a rectangular section of the video picture screen		*/
/****************************************************************/
class	VidWindow
{
  private:
	unsigned char	xFrom;
	unsigned char	xTo;
	unsigned char	yFrom;
	unsigned char	yTo;
  public:
	VidWindow();
	VidWindow(char *buf);
	VidWindow(int xF, int xT, int yF, int yT);

	void	setX(unsigned char from, unsigned char to);
	void	setY(unsigned char from, unsigned char to);

	void	getX(unsigned char &from, unsigned char &to);
	void	getY(unsigned char &from, unsigned char &to);

	char	*say();

};
