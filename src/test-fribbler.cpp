#include <iostream>
#include <libplayerc++/playerc++.h>
#include <unistd.h>
using namespace std;
using namespace PlayerCc;

int main(int argc, char **argv)
{
	PlayerClient    robot;
	Position2dProxy pp(&robot, 0);

	while (1) {
		robot.Read(); // Read from proxies

/*
		// geometry command
		pp.RequestGeom();
		player_bbox3d_t rect = pp.GetSize();
		cout << "length: " << rect.sl << "   width: " << rect.sw << endl;
*/

/*
		// velocity command
		pp.SetSpeed(100, 0);
		pp.SetCarlike(0,0);
		cout << "speed: " << pp.GetXSpeed() << "   pos: " << pp.GetXPos() << endl;
*/
	}

	return 0;
}
