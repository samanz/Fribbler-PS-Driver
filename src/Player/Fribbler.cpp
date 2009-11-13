/**
 * Fribbler (Fluke + Scribbler) driver plugin for the Player robot server.
 * Developed by the MetroBotics project at CUNY.
 */
#include "Player/Fribbler.h"
#include "Scribbler/PosixSerial.h"
#include "Scribbler/scribbler.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

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
	memset(&_position_data, 0, sizeof(player_position2d_data_t));
	memset(&_position_geom, 0, sizeof(player_position2d_geom_t));

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

		// Update our robot: read sensors and collect relevant data.
		if (_scribbler->updateScribblerSensors() == 0) {
			#ifdef FRIBBLER_DEBUG
				fprintf(stderr, "Scribbler failed to update its sensors.\n");
			#endif
		}

		// Update the interfaces that we're providing.
		// Position2D
		// FIXME: do we update the geometry, velocity, etc. here or in the message processing method?

		// Publish our updated interfaces.
		// Position2D
		Publish(_position_addr, PLAYER_MSGTYPE_DATA, PLAYER_POSITION2D_DATA_STATE, (void *)&_position_data, sizeof(_position_data), 0);

		ProcessMessages();
		usleep(FRIBBLER_CYCLE);
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

int Fribbler::ProcessMessage(QueuePointer &queue, player_msghdr *msghdr, void *data)
{
	if (Message::MatchMessage(msghdr, PLAYER_MSGTYPE_CMD, PLAYER_POSITION2D_CMD_VEL, _position_addr)) {
		#ifdef FRIBBLER_DEBUG
			fprintf(stderr, "Received Position2D velocity command.\n");
		#endif
		// Extract the data that accompanied this command.
		player_position2d_cmd_vel_t *cmd = (player_position2d_cmd_vel_t *)data;
		// Since the Scribbler is nonholonomic, ignore the y value.
		/* FIXME: the yaw needs to be translated into steering.
		 *        for now, just move the robot in a single direction, which will be determined entirely by the x value.
		 */
		/* FIXME: the x value is in meters/second; we need to translate this meaningfully for the scribbler.
		 *        somebody will have to do some measuring and calibration.
		 */
		#ifdef FRIBBLER_DEBUG
			fprintf(stderr, "Setting Scribbler's velocity to %f m/s.\n", cmd->vel.px);
		#endif
		if (_scribbler->drive(cmd->vel.px, cmd->vel.px) == 0) {
			#ifdef FRIBBLER_DEBUG
				fprintf(stderr, "Scribbler failed to drive.\n");
			#endif
		} else {
			// Update our position data.
			_position_data.vel.px = cmd->vel.px;
			// FIXME: there's more...
		}
		return 0;
	} else if (Message::MatchMessage(msghdr, PLAYER_MSGTYPE_REQ, PLAYER_POSITION2D_REQ_GET_GEOM, _position_addr)) {
		#ifdef FRIBBLER_DEBUG
			fprintf(stderr, "Received Position2D geometry request.\n");
		#endif
		// Fill in the Scribbler's physical dimensions.
		memset(&_position_geom, 0, sizeof(player_position2d_geom_t));
		_position_geom.size.sl = SCRIBBLER_LENGTH;
		_position_geom.size.sw = SCRIBBLER_WIDTH;
		/*_position_geom.size.sh = SCRIBBLER_HEIGHT; // Ignored by Position2D*/
		Publish(_position_addr, queue, PLAYER_MSGTYPE_RESP_ACK, PLAYER_POSITION2D_REQ_GET_GEOM, (void *)&_position_geom, sizeof(_position_geom), 0);
		return 0;
	} else {
		// Unknown message.
		#ifdef FRIBBLER_DEBUG
			fprintf(stderr, "Received an unknown message.\n");
		#endif
		return -1;
	}
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
