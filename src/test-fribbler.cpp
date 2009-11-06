#include <iostream>
#include <libplayerc++/playerc++.h>
using namespace std;
using namespace PlayerCc;

int main(int argc, char **argv)
{
	PlayerClient    robot("localhost");
	Position2dProxy pp(&robot, 0);

	while (true) {
		robot.Read();
	}

	return 0;
}
