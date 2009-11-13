#include <iostream>
#include <libplayerc++/playerc++.h>
#include <unistd.h>
using namespace std;
using namespace PlayerCc;

int main(int argc, char **argv)
{
	PlayerClient    robot;
	Position2dProxy pp(&robot, 0);
	pp.ResetOdometry();
	pp.RequestGeom();

	while (1) {
		robot.Read(); // Read from proxies

		// Test to see which methods generate which messages:

		// geometry command
		pp.RequestGeom();
		player_bbox3d_t rect = pp.GetSize();
		cout << "length: " << rect.sl << "   width: " << rect.sw << endl;

		// velocity command
		pp.SetSpeed(100, 0);
		cout << "speed: " << pp.GetXSpeed() << "   pos: " << pp.GetXPos() << endl;
	}

	return 0;
}
