/**
   roomba.cpp
   31-jan-2007/sklar

   this program demonstrates a simple robot (called "roomba")
   wandering around in a simulated room. the robot can move in any of
   four directions (forward, backward, left or right) within a "grid"
   so that any move changes the robot's location by one grid square.
   for example, if the robot starts at location (0,0) and moves forward,
   it will end up at location (0,1).

*/

/************************************************************************/
/* Modified from Professor Sklar's Roomba.cpp to control the Surveyor	*/
/* January 22 ed 2008							*/
/************************************************************************/


// section 1: include C++ library definitions
#include <iostream>
using namespace std;
// and Surveyor definitions
#include "SVR.h"

#define RIGHT_ANGLE	 2000000



// section 2: declare variables
int x;
int y;

int	pointedLeft = 0;
// and robot

Surveyor	robot(ADDRESS);	



// section 3: declare methods
int display() {
  cout << "the roomba is at location (";
  cout << x;
  cout << ",";
  cout << y;
  cout << ")\n";
  return 0;
} // end of display()


int moveForward() {
  cout << "moving forward...\n";
  y = y + 1;

  if (pointedLeft)
  {
	  robot.drive(40, -40);		// turn
	  usleep(RIGHT_ANGLE);
  }
  pointedLeft = 0;
  
  robot.drive(40, 40);
  sleep(2); 	// wait a second
  robot.drive(0, 0);
		
  return 0;
} // end of moveForward()


int moveBackward() {
  cout << "moving backward...\n";
  y = y - 1;

  if (pointedLeft)
  {
	  robot.drive(40, -40);		// turn
	  usleep(RIGHT_ANGLE);
  }
  pointedLeft = 0;

  robot.drive(-40, -40);
  sleep(2);	// wait a second
  robot.drive(0, 0);

  return 0;
} // end of moveBackward()


int moveLeft() {
  cout << "moving left...\n";
  x = x - 1;

  if (!pointedLeft)
  {
	  robot.drive(-40, 40);		// turn
	  usleep(RIGHT_ANGLE);
  }
  pointedLeft = 1;
  robot.drive(40, 40);		// move
  sleep(2);
  robot.drive(0, 0);

  return 0;
} // end of moveLeft()


int moveRight() {
  cout << "moving right...\n";
  x = x + 1;

  if (!pointedLeft)
  {
	  robot.drive(-40, 40);		// turn
	  usleep(RIGHT_ANGLE);
  }
  pointedLeft = 1;
  robot.drive(-40, -40);		// move
  sleep(2);
  robot.drive(0, 0);	
  return 0;
} // end of moveRight()



// section 4: define main method
int main() {
  x = 0;
  y = 1;
  display();

  moveForward();
  moveForward();
  moveRight();
  moveRight();
  moveBackward();
  moveBackward();
  moveLeft();
  moveLeft();
	
  display();
/*
  moveBackward();
  display();
  moveLeft();
  display();
  moveRight();
  display();
 */
} // end of main()

