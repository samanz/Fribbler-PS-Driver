#include "scribbler.h" 
//#include "WinSerial.h"
#include "PosixSerial.h"
#include "PosixFile.h"
#include <string>
#include <assert.h>
#include <unistd.h>

int	gDebugging = 3;




PosixSerial	port(ADDRESS);

Scribbler       robot(port); 

//YUVRange	range(0x00FE, 0x7E8C, 0x5A78);
//YUVRange	range(0x00FE, 0x00FF, 0x00FF);
YUVRange	range(0x00FE, 0x3388, 0xBEFE);



/************************************************************************/
/*									*/
/*  main								*/
/*									*/
/************************************************************************/


int main( int argc, char *argv[])
{
	char	buf[256];
	char	*ret;
	RLE	*rle;
	JPEG	*jpeg;
	FileOut	*file;

	int 	i;
printf("point 0\n");
       
	if (argc > 1)
                gDebugging = atoi(argv[1]);


try
{
	robot.setEchoMode(0);

printf("Beep\n"); 
        robot.beep(512, 1000);

printf("setForwardness(1)\n");
	robot.setForwardness(1);
printf("setWhiteBalance(1)\n");
	robot.setWhiteBalance(1);
printf("setWhiteBalance(0)\n");
	robot.setWhiteBalance(0);
printf("getCamParam(1, i)");
	robot.getCamParam(1, i);
printf(" = %d\n", i);
printf("setCamParam(1,i)");
	robot.setCamParam(1,i);

	
printf("getVersion(buf)\n");
	robot.getVersion(buf);
printf("Version = <%s>\n", buf);
//	robot.getMessageIR(buf);
	jpeg = robot.grabJPEG();
	if (jpeg)
	{
		jpeg->talk(stdout);
		printf("got a jpeg - save it\n");
		file = new FileOut("test.jpeg");
		jpeg->write(*file);
		delete file;
	}

printf("Beep\n"); 
	robot.beep(512, 1000);
}
catch (DataError err)
{
	printf("DataError on Main\n");
}
	
 



    	return 0;   
}
	
