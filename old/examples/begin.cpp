/********************************************************************************/
/* start  									*/ 
/* a simple demonstration program for the Surveyor class			*/
/*										*/
/* usage: start									*/
/* 										*/
/* Description: robot will move forward about a foot or two			*/
/* 										*/
/********************************************************************************/
#include "SVR.h"

#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <assert.h>
using namespace std;


/* make a global Surveyor object called robot */
Surveyor	robot(ADDRESS);


int	main()
{
	char	buffer[256];


	robot.getVersion(buffer);
	cout << "SRV-1 version " << buffer << endl;

	robot.drive(50, 50, 100);

	return 0;
}
