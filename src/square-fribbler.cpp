#include <iostream>
#include <libplayerc++/playerc++.h>
#include <unistd.h>
#include <ctime>
#include <sys/time.h>
using namespace std;
using namespace PlayerCc;

int main(int argc, char **argv)
{
	PlayerClient    robot;
	Position2dProxy pp(&robot, 0);

	// ghetto state machine
	enum { STRAIGHT, TURNING } state;
	int stateChanges = 0;

	// odometry
	double distance  = 1.2; // in meters
	double theta     = 3.14159256/2.0; // pi/2 radians

	// initial state: STAIGHT
	state = STRAIGHT;
	pp.ResetOdometry();
	pp.SetSpeed(1, 0);
	// initial state: TURNING
/*
	state = TURNING;
	pp.ResetOdometry();
	pp.SetSpeed(0, 1);
*/

	while (stateChanges < 8) {
		robot.Read(); // read from proxies

		cout << (state == STRAIGHT ? "straight" : "turning") << "::displacement = " << (state == STRAIGHT ? pp.GetXPos() : pp.GetYaw()) << endl;

		switch (state) {
			case STRAIGHT: {
				if (pp.GetXPos() >= distance) {
					// change state to turning.
					pp.SetSpeed(0,0); // stop
					state = TURNING;
					stateChanges++;
					pp.ResetOdometry();
				} else {
					pp.SetSpeed(1, 0); // straight
				}
			} break;
			case TURNING: {
				if (pp.GetYaw() >= theta) {
					// Change state to straight.
					pp.SetSpeed(0,0); // stop
					state = STRAIGHT;
					stateChanges++;
					pp.ResetOdometry();
				} else {
					pp.SetSpeed(0, 1); // counter-clockwise
				}
			} break;
			default: break;
		}
	}

	pp.SetSpeed(0,0);
	sleep(1);

	return 0;
}
