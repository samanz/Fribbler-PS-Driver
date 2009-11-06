#include "Player/Fribbler.h"
#include "Scribbler/PosixSerial.h"
#include "Scribbler/scribbler.h"

#include <stdio.h>
#include <string.h>

// John's global debug flag
#ifdef FRIBBLER_DEBUG
	int gDebugging = 1;
#else
	int gDebugging = 0;
#endif

// Driver Class Factory
Driver* Fribbler_Init(ConfigFile *cf, int section)
{
	#ifdef FRIBBLER_DEBUG
		fprintf(stderr, "Initializing the Fribbler driver.\n");
	#endif
	return ((Driver *) (new Fribbler(cf, section)));
}

// Driver Register Hook
void Fribbler_Register(DriverTable *table)
{
	#ifdef FRIBBLER_DEBUG
		fprintf(stderr, "Registering the Fribbler driver.\n");
	#endif
	table->AddDriver("Fribbler", Fribbler_Init);
}

// Driver-specific initialization: performed once upon startup of the server.
Fribbler::Fribbler(ConfigFile *cf, int section)
: ThreadedDriver(cf, section, true, PLAYER_MSGQUEUE_DEFAULT_MAXLEN),
  _portname(0), _port(0), _scribbler(0)
{
	#ifdef FRIBBLER_DEBUG
		fprintf(stderr, "Creating a new Fribbler object.\n");
	#endif

	// Zero out the interface structures.

	// Position2D
	_hasPosition = false; // assume no
	memset(&_position_addr, 0, sizeof(player_devaddr_t));

	// Extract the port name from the configuration file.
	_portname = cf->ReadString(section, "port", 0);
	#ifdef FRIBBLER_DEBUG
		if (_portname) {
			fprintf(stderr, "Scribbler is connected to port: %s\n", _portname);
		} else {
			fprintf(stderr, "Missing the port through which the Scribbler is connected.");
		}
	#endif

	// Extract the interface information from the configuration file, and
	// register the interfaces that we're providing with the Fribbler driver.

	// Position2D
	if ((cf->ReadDeviceAddr(&_position_addr, section, "provides", PLAYER_POSITION2D_CODE, -1, 0) == 0) &&
	    (AddInterface(_position_addr) == 0)) {
		_hasPosition = true;
		#ifdef FRIBBLER_DEBUG
			fprintf(stderr, "Fribbler is providing a Position2D interface.\n");
		#endif
	}

	#ifdef FRIBBLER_DEBUG
		fprintf(stderr, "Fribbler object was created successfully.\n");
	#endif
}

// Device-specific initialization: called everytime the driver goes from having no subscribers
// to receiving the first subscription.
int Fribbler::MainSetup()
{
	#ifdef FRIBBLER_DEBUG
		fprintf(stderr, "Fribbler is entering MainSetup().\n");
	#endif

	// Check the current state of the driver.
	// User must specify the port!
	if (_portname == 0) {
		PLAYER_ERROR("you must specify the device's port name in the configuration file");
		return -1;
	}
	// Driver must be providing a position2d interface!
	if (!_hasPosition) {
		PLAYER_ERROR("failed to add Position2D interface for Scribbler");
		return -1;
	}

	// Initialize our hardware.
	#ifdef FRIBBLER_DEBUG
		fprintf(stderr, "Establishing a connection to the Scribbler.\n");
	#endif
	// Create the serial object.
	if ((_port = new PosixSerial(_portname)) == 0) {
		PLAYER_ERROR("failed to initialize serial device");
		return -1;
	}
	// Create the Scribbler object.
	if ((_scribbler = new Scribbler(*_port)) == 0) {
		PLAYER_ERROR("failed to initialize the Scribbler");
		return -1;
	}
	#ifdef FRIBBLER_DEBUG
		fprintf(stderr, "A connection to the Scribbler has been successfully established.\n");
		char scribblerName[16];
		if (_scribbler->getScribblerName(scribblerName)) {
			fprintf(stderr, "%s says, Hello Player!\n", scribblerName);
		}
	#endif

	#ifdef FRIBBLER_DEBUG
		fprintf(stderr, "Fribbler is exiting MainSetup().\n");
	#endif

	// Looking good; cross your fingers!
	StartThread();
	return 0;
}

// Driver-specific shutdown: performed once upon shutdown of the server.
Fribbler::~Fribbler()
{
	#ifdef FRIBBLER_DEBUG
		fprintf(stderr, "Destroying the Fribbler object.\n");
	#endif

	// This space for rent...

	#ifdef FRIBBLER_DEBUG
		fprintf(stderr, "The Fribbler object has been destroyed.\n");
	#endif
}

// Device-specific shutdown: called whenever the driver goes from a state in which clients
// are subscribed to a device to a state in which there are no longer any clients subscribed.
void Fribbler::MainQuit()
{
	StopThread();

	// Disconnect from the Scribbler and return any resources used for it.
	#ifdef FRIBBLER_DEBUG
		fprintf(stderr, "Disconnecting from the Scribbler.\n");
		char scribblerName[16];
		if (_scribbler->getScribblerName(scribblerName)) {
			fprintf(stderr, "%s says, Nooooo!\n", scribblerName);
		}
	#endif
	if (_scribbler) {
		delete _scribbler;
		_scribbler = 0;
		delete _port;
		_port = 0;
	}
	#ifdef FRIBBLER_DEBUG
		fprintf(stderr, "Connection to the Scribbler has been severed.\n");
	#endif

	// Just in case...
	if (_port) {
		delete _port;
		_port = 0;
	}
}

void Fribbler::Main()
{
	#ifdef FRIBBLER_DEBUG
		fprintf(stderr, "Fribbler is entering Main().\n");
	#endif

	while (1) {
		pthread_testcancel(); // required

		// FIXME: Collect data here...

		// FIXME: Publish data here...

		ProcessMessages();
	}

	#ifdef FRIBBLER_DEBUG
		fprintf(stderr, "Fribbler is exiting Main().\n");
	#endif
}

int Fribbler::Subscribe(player_devaddr_t id)
{
	return Driver::Subscribe(id);
}

int Fribbler::Unsubscribe(player_devaddr_t id)
{
	return Driver::Unsubscribe(id);
}

int Fribbler::ProcessMessage(MessageQueue *queue, player_msghdr *msghdr, void *data)
{
	return 0;
}

/* NOTE: need the extern to avoid C++ name-mangling */
extern "C"
{
	// Driver Load Hook
	int player_driver_init(DriverTable* table)
	{
		Fribbler_Register(table);
		return 0;
	}
}
