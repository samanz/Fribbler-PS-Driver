#include <iostream>
#include <libplayerc++/playerc++.h>
using namespace std;
using namespace PlayerCc;

int main(int argc, char **argv)
{
	PlayerClient    robot("localhost");
	Position2dProxy pp(&robot, 0);

	pp.SetMotorEnable(true);
	pp.GetXPos();

	while (true) {
	}

	return 0;
}
