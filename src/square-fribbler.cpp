#include <iostream>
#include <fstream>
#include <libplayerc++/playerc++.h>
#include <unistd.h>
#include <ctime>
#include <sys/time.h>
#include <stdio.h>
#include <time.h>
using namespace std;
using namespace PlayerCc;

PlayerClient    robot;
Position2dProxy pp(&robot, 0);
CameraProxy 	cp(&robot);

int main(int argc, char **argv)
{
	// ghetto state machine
	enum { STRAIGHT, TURNING } state;
	int stateChanges = 0;
	bool fInMotion = false;

	// odometry
	double distance  = 1.0; // in meters
	double theta     = 1.7; // pi/2 radians

	// initial state: STAIGHT
	state = STRAIGHT;
	pp.ResetOdometry();
	pp.SetSpeed(1, 0);
	fInMotion = true;
	// initial state: TURNING
/*
	state = TURNING;
	pp.ResetOdometry();
	pp.SetSpeed(0, 1);
*/
  time_t seconds;
	while (stateChanges < 8) {
		robot.Read(); // read from proxies
		
		cout << (state == STRAIGHT ? "straight" : "turning") << "::displacement = " << (state == STRAIGHT ? pp.GetXPos() : pp.GetYaw()) << endl;

		switch (state) {
			case STRAIGHT: {
				if (pp.GetXPos() >= distance) {
					// change state to turning.
					pp.SetSpeed(0,0); // stop
					fInMotion = false;
					state = TURNING;
					stateChanges++;
					pp.ResetOdometry();
				} else if (!fInMotion) {
					pp.SetSpeed(1, 0); // straight
					fInMotion = true;
				}
			} break;
			case TURNING: {
				if (pp.GetYaw() >= theta) {
					// Change state to straight.
					pp.SetSpeed(0,0); // stop
					fInMotion = false;
					state = STRAIGHT;
					stateChanges++;
					pp.ResetOdometry();
				} else if (!fInMotion) {
					pp.SetSpeed(0, 1); // counter-clockwise
					fInMotion = true;
				}
			} break;
			default: break;
		}
	}

	pp.SetSpeed(0,0);
	sleep(1);
	return 0;
}
