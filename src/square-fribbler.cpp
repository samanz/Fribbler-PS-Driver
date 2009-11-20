#include <iostream>
#include <libplayerc++/playerc++.h>
#include <unistd.h>
#include <ctime>
using namespace std;
using namespace PlayerCc;

int main(int argc, char **argv)
{
	PlayerClient    robot;
	Position2dProxy pp(&robot, 0);

	// ghetto state machine
	enum { STRAIGHT, TURNING } state;
	int stateChanges = 0;

	// timing
	time_t t0;
	double framerate = 0.00;
	double tElapsed  = 0.00;

	// odometry
	double distance  = 1.0; // meter
	double theta     = 3.14159256/2.0; // pi/2 radians
	double epsilon   = 0.0;

	// initial state
	state = STRAIGHT;
	pp.SetSpeed(1, 0);

	while (stateChanges < 1) {
		t0 = time(0); // start of frame
		robot.Read(); // Read from proxies

		cout << (state == STRAIGHT ? "straight" : "turning") << "::epsilon = " << epsilon << endl;

		switch (state) {
			case STRAIGHT: {
				if (epsilon >= distance) {
					// Change state to turning.
					pp.SetSpeed(0,0); // stop
					state = TURNING;
					stateChanges++;
					pp.ResetOdometry();
					epsilon = 0.0;
				} else {
					pp.SetSpeed(1, 0); // straight
					epsilon += pp.GetXPos();
				}
			} break;
			case TURNING: {
				if (epsilon >= theta) {
					// Change state to straight.
					pp.SetSpeed(0,0); // stop
					state = STRAIGHT;
					stateChanges++;
					pp.ResetOdometry();
					epsilon = 0.0;
				} else {
					pp.SetSpeed(0, 1); // counter-clockwise
					epsilon += pp.GetYaw();
				}
			} break;
			default: break;
		}
		framerate = difftime(time(0), t0); // time the frame
		tElapsed += framerate;
	}

	pp.SetSpeed(0,0);
	sleep(1);

	return 0;
}
