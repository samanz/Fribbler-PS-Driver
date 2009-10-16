/************************************************************************/
/* This was developed by John Cummins at Brooklyn College, with 	*/
/* assistance from M. Q. Azhar and Howard of Surveyor.com, and		*/
/* supervision from Professor Sklar.					*/
/*									*/
/* It is released under the copyleft understanding. That is, any one is	*/
/* free to use, and modify, any part of it so long as it continues to 	*/
/* carry this notice.							*/
/************************************************************************/
#include "surveyor.h"



class	PosixSock : public VirtSurveyor
{
  public:
	char	mDevName[32];
	int	mPortNum;

	int	mSocket;

	char	inMsg[10240];
	int	inCount;
	int	inPosn;

	char	outMsg[512];
	int	outCount;
  public:
	PosixSock(char *addr);
	virtual ~PosixSock();
	

	void	openRobotLink();

	/* virtual functions		*/
	int	readOneChar(char *ch);
	int	writeOneChar(char *ch);
	int	readManyChars(char *buf, int size);
	int	saveNamedData(char *name, 
			      char *data, int size);
	void	flushOutputBuffer();
	void	flushInputBuffer();


};

class	Surveyor : public PosixSock
{
  public:
	Surveyor(char *addr) : PosixSock(addr) {}
};




