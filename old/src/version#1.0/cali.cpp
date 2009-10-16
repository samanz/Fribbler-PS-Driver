#include <iostream>

using namespace std;

#include "scribbler.h" 
#include "system.h"
#include <string>
#include <assert.h>


int	gDebugging = 3;




CLIENT_SERIAL	port(ADDRESS);


Scribbler       robot(port); 

//YUVRange	range(0x00FE, 0x7E8C, 0x5A78);
//YUVRange	range(0x00FE, 0x00FF, 0x00FF);
YUVRange	range(0x00FE, 0x3388, 0xBEFE);


void showData(char data[8])
{
	char	buf[256];

	cout << "Fast Forward\tSlow Forward\tSlow Reverse\tFast Reverse\n";
	sprintf(buf, " %d \t %d \t %d \t %d ", 
			data[0], data[1], data[2], data[3]);
	cout << buf << endl;
}

/************************************************************************/
/*									*/
/*  main								*/
/*									*/
/************************************************************************/



int main( int argc, char *argv[])
{
	char	buf[256];	char	*ret;
	char 	data[8];
	int	ff, sf, sr, fr;


	int 	i;

       
	if (argc > 1)
                gDebugging = atoi(argv[1]);


	for (int exit = 0;!exit;)
	{
		robot.getScribblerData(data);
		showData(data);
		cout << "Enter change, test or exit\n";
		cin.getline(buf, sizeof(buf));
		switch (buf[0])
		{
		  case 'c':
		  case 'C':
			cout << "Current Values\n";
			showData(data);
			cout << "Enter new Values\n";
			cin.getline(buf, sizeof(buf));
			sscanf(buf, "%d %d %d %d", ff, sf, sr, fr);
			memset(data, '*', 8);
			data[0] = ff & 0xFF;
			data[1] = sf & 0xFF;
			data[2] = sr & 0xFF;
			data[3] = fr & 0xFF;
			robot.setScribblerData(data);;
			break;
		  case 'T':
		  case 't':
			for (i = 100;i > -100;i -= 10)
			{
				robot.drive(i, i);
				usleep(100);
			}
			robot.drive(0, 0);
			break;
		  case 'e':
		  case 'E':
			exit = 1;
			break;
		}
	}
		

    	return 0;   
}
	
