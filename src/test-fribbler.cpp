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

	// timing
	double framerate = 0.0;
	double tTotal    = 0.31;
	double tElapsed  = 0.0;

	pp.SetSpeed(100, 1);

	while (tElapsed <= tTotal) {
		time_t t0 = time(0); // time the frame
		robot.Read(); // Read from proxies

		// Test to see which methods generate which messages:

/*
		// geometry command
		pp.RequestGeom();
		player_bbox3d_t rect = pp.GetSize();
		cout << "length: " << rect.sl << "   width: " << rect.sw << endl;

		// velocity command
		pp.SetSpeed(100, 0);
		cout << "speed: " << pp.GetXSpeed() << "   pos: " << pp.GetXPos() << endl;
*/

		framerate = difftime(time(0), t0); // time the frame
		tElapsed += framerate;
	}

	pp.SetSpeed(0,0);
	sleep(1);

	return 0;
}
