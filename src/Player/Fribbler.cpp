#include "Player/Fribbler.h"
#include "Scribbler/PosixSerial.h"
#include "Scribbler/scribbler.h"

#include <stdio.h>
#include <string.h>

// factory creation function
Driver* Fribbler_Init(ConfigFile *cf, int section)
{
	return ((Driver *) (new Fribbler(cf, section)));
}

// registration
void Fribbler_Register(DriverTable *table)
{
	table->AddDriver("Fribbler", Fribbler_Init);
}

Fribbler::Fribbler(ConfigFile *cf, int section)
: ThreadedDriver(cf, section, true, PLAYER_MSGQUEUE_DEFAULT_MAXLEN),
  _portname(0), _port(0), _scribbler(0)
{
	// Zero out the interface structures.
	// Position2D
	_hasPosition = false;
	memset(&_position_addr, 0, sizeof(player_devaddr_t));

	// Extract the port name from the configuration file.
	_portname = cf->ReadString(section, "port", 0);

	// Extract the interface information from the configuration file.
	_hasPosition = cf->ReadDeviceAddr(&_position_addr, section, "provides", PLAYER_POSITION2D_CODE, -1, 0) == 0;
}

Fribbler::~Fribbler()
{
	// Free any resources that we may have used.
	if (_port) {
		delete _port;
		_port = 0;
	}
	if (_scribbler) {
		delete _port;
		_port = 0;
	}
}

void Fribbler::Main()
{
	while (1) {
		pthread_testcancel(); // For Player 3
		ProcessMessages();
	}
}

int Fribbler::MainSetup()
{
	// Step 1: connect to the Scribbler.

	// User must specify the port!
	if (_portname == 0) {
		PLAYER_ERROR("you must specify the device's port name in the configuration file");
		return -1;
	}
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

	// Step 2: add the interfaces.

	// Position2D
	if (_hasPosition && AddInterface(_position_addr) == 0) {
		PLAYER_ERROR("failed to add Position2D interface for Scribbler");
		return -1;
	}

	// Looking good; cross your fingers!
	StartThread();
	return 0;
}

void Fribbler::MainQuit()
{
	StopThread();
}

int Fribbler::Subscribe(player_devaddr_t id)
{
	return 0;
}

int Fribbler::Unsubscribe(player_devaddr_t id)
{
	return 0;
}

int Fribbler::ProcessMessage(MessageQueue *queue, player_msghdr *msghdr, void *data)
{
	return 0;
}

/* NOTE: need the extern to avoid C++ name-mangling */
extern "C"
{
   int player_driver_init(DriverTable* table)
   {
      Fribbler_Register(table);
      return 0;
   }
}
