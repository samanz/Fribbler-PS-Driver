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
	double tStraight = 3.50;
	double tTurning  = 0.10;
	double tElapsed  = 0.00;

	// initial state
	state = STRAIGHT;
	pp.SetSpeed(100, 0);

	while (stateChanges < 4) {
		t0 = time(0); // start of frame
		robot.Read(); // Read from proxies

		switch (state) {
			case STRAIGHT: {
				if (tElapsed >= tStraight) {
					// Change state to turning.
					state = TURNING;
					stateChanges++;
					pp.SetSpeed(100, 1); // counter-clockwise
					tElapsed = 0.0;
				}
			} break;
			case TURNING: {
				if (tElapsed >= tTurning) {
					// Change state to straight.
					state = STRAIGHT;
					stateChanges++;
					pp.SetSpeed(100, 0); // straight
					tElapsed = 0.0;
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
