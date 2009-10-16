/********************************************************************************/

/********************************************************************************/
#include "SVR.h"
#include <string>
#include <iostream>
using namespace std;

#include "color.h"




Surveyor	robot(ADDRESS);	

YUVRange	ballColor(T_ORANGE_BALL);


void	examineBlobs()
{
	int	hits;
	int	totalHits;	

	robot.setBin(1, ballColor);

	cout << "loadBlobs(1) =" << endl;
	robot.loadBlobs(1);
	BlobNode	*list = robot.getBlobList();
	BlobNode	*node;
	

	totalHits = 0;
	for (node = list;node;node = node->next)
	{
		hits = node->hits;
		cout << node->wind.say() <<  " hits = " << hits << endl;
		totalHits += hits;
	}
	cout << "Total Hits" << totalHits << endl;

	BlobNode	*next;
	
	for (node = list;node;node = next)
	{
		next = node->next;
		delete node;
	}


	robot.takePhoto();
	robot.savePhoto("list.jpg");
}




/************************************************************************/
/*									*/
/*  main								*/
/*									*/
/************************************************************************/


int main( int argc, char *argv[])
{
	char	buf[256];
	int	arg = 0;
	int	ret;

	if (argc > 1)
		arg = atoi(argv[1]);
	robot.setDebuging(arg);
	ret = robot.getVersion(buf);
	printf("getVersion() = %d\n", ret);
	printf("SVR-1 version %s\n", buf);

	examineBlobs();


    	return 0;   
}
	
