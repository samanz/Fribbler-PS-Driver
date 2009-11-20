#include <iostream>
#include <libplayerc++/playerc++.h>
#include <unistd.h>
#include <signal.h>
#include <ctime>
using namespace std;
using namespace PlayerCc;

PlayerClient *robot;
Position2dProxy *pp;
volatile bool fDone;

void stop(int sig)
{
	if (sig == SIGINT) {
		fDone = true;
	}
}

int main(int argc, char **argv)
{
	robot = new PlayerClient();
	pp = new Position2dProxy(robot, 0);

	// make sure we're good to go
	if (robot == 0 || pp == 0) {
		cout << "Failed to create our player objects." << endl;
		exit(1);
	}

	// intercept SIGINT
	struct sigaction sigint;
	sigaction(SIGINT, 0, &sigint); // previous disposition
	sigint.sa_handler = stop;
	sigaction(SIGINT, &sigint, 0); // new disposition

	// timing
	double framerate = 0.0;
	double tTotal    = 20.00;
	double tElapsed  = 0.0;

	pp->SetSpeed(1, 0);

	//while (tElapsed <= tTotal) { // turns out that timing is not a certain way of doing things :(
	fDone = false;
	while (!fDone) {
		time_t t0 = time(0); // time the frame
		robot->Read(); // Read from proxies
		pp->ResetOdometry();
		cout << "Xpos: " << pp->GetXPos() << endl;

		// Test to see which methods generate which messages:

/*
		// geometry command
		pp->RequestGeom();
		player_bbox3d_t rect = pp->GetSize();
		cout << "length: " << rect.sl << "   width: " << rect.sw << endl;

		// velocity command
		pp->SetSpeed(100, 0);
		cout << "speed: " << pp->GetXSpeed() << "   pos: " << pp->GetXPos() << endl;
*/

		framerate = difftime(time(0), t0); // time the frame
		tElapsed += framerate;
	}

	pp->SetSpeed(0,0);
	sleep(1); // catch your breath!
	return 0;
}
