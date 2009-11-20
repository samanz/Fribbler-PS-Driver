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
	double distance  = 1.5; // in meters
	double theta     = 1.5708; // pi/2 radians

	// initial state
	state = STRAIGHT;
	pp.ResetOdometry();
	pp.SetSpeed(1, 0);

	while (stateChanges < 4) {
		t0 = time(0); // start of frame
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
		framerate = difftime(time(0), t0); // time the frame
		tElapsed += framerate;
	}

	pp.SetSpeed(0,0);
	sleep(1);

	return 0;
}
