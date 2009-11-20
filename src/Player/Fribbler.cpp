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
#include <time.h>
#include <sys/time.h>

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
  _portname(0), _port(0), _scribbler(0), _framerate(0)
{
	#ifdef FRIBBLER_DEBUG
		fprintf(stderr, "Creating a new Fribbler object.\n");
	#endif

	// Zero out the interface structures.

	// Position2D
	_hasPosition = false; // assume no
	memset(&_position_addr, 0, sizeof(player_devaddr_t));
	memset(&_position_data, 0, sizeof(player_position2d_data_t));

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
		if (gettimeofday(&_t0, 0) != 0) { // start of the frame
			// FIXME: error checking?
		}

		// Update our robot: read sensors and collect relevant data.
		if (_scribbler->updateScribblerSensors() == 0) {
			#ifdef FRIBBLER_DEBUG
				fprintf(stderr, "Scribbler failed to update its sensors.\n");
			#endif
		}

		// Update the interfaces that we're providing.
		// Position2D
		Lock();
			_position_data.pos.px += _framerate * _position_data.vel.px;
			_position_data.pos.py += _framerate * _position_data.vel.py;
			_position_data.pos.pa += _framerate * _position_data.vel.pa;
		Unlock();

		// Publish our updated interfaces.
		// Position2D
		Publish(_position_addr, PLAYER_MSGTYPE_DATA, PLAYER_POSITION2D_DATA_STATE, (void *)&_position_data, sizeof(_position_data), 0);

		ProcessMessages();
		//usleep(FRIBBLER_CYCLE); // Breathe!
		if (gettimeofday(&_t1, 0) != 0) { // end of the frame
			// FIXME: error checking?
		}
		_framerate = (_t1.tv_sec + (_t1.tv_usec / 1000000.0)) -
		             (_t0.tv_sec + (_t0.tv_usec / 1000000.0));
		if (_framerate < 0) _framerate = 0; // why is this even necessary?
		#ifdef FRIBBLER_DEBUG
			fprintf(stderr, "frame: %f s\n", _framerate);
		#endif
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
	// FIXME: might be a good idea to wrap this stuff into functions to avoid clutter; maybe individual interface classes
	if (Message::MatchMessage(msghdr, PLAYER_MSGTYPE_REQ, PLAYER_POSITION2D_REQ_GET_GEOM, _position_addr)) {
		#ifdef FRIBBLER_DEBUG
			fprintf(stderr, "Received Position2D geometry request.\n");
		#endif
		// Fill in the Scribbler's physical dimensions.
		memset(&_position_geom, 0, sizeof(player_position2d_geom_t));
		_position_geom.size.sl = SCRIBBLER_LENGTH;
		_position_geom.size.sw = SCRIBBLER_WIDTH;
		/*_position_geom.size.sh = SCRIBBLER_HEIGHT; // Ignored by Position2D*/
		// Acknowledge the request; publish the geometry.
		Publish(_position_addr, queue, PLAYER_MSGTYPE_RESP_ACK, PLAYER_POSITION2D_REQ_GET_GEOM, (void *)&_position_geom, sizeof(player_position2d_geom_t), 0);
		return 0;
	} else if (Message::MatchMessage(msghdr, PLAYER_MSGTYPE_REQ, PLAYER_POSITION2D_REQ_RESET_ODOM, _position_addr)) {
		#ifdef FRIBBLER_DEBUG
			fprintf(stderr, "Received Position2D request to reset odometry.\n");
		#endif
		// Reset the odometry.
		Lock();
			_position_data.pos.px = 0;
			_position_data.pos.py = 0;
			_position_data.pos.pa = 0;
		Unlock();
		// Acknowledge the request.
		Publish(_position_addr, queue, PLAYER_MSGTYPE_RESP_ACK, PLAYER_POSITION2D_REQ_RESET_ODOM);
		return 0;
	} else if (Message::MatchMessage(msghdr, PLAYER_MSGTYPE_REQ, PLAYER_POSITION2D_REQ_MOTOR_POWER, _position_addr)) {
		#ifdef FRIBBLER_DEBUG
			fprintf(stderr, "Received Position2D motor power request.\n");
		#endif
		// Extract the data that accompanied this command.
		bool motorState = ((player_position2d_power_config_t *)data)->state;
		if (motorState) {
			#ifdef FRIBBLER_DEBUG
				fprintf(stderr, "Turning on the Scribbler's motors.\n");
			#endif
			// FIXME: what do we do here?
		} else {
			#ifdef FRIBBLER_DEBUG
				fprintf(stderr, "Turning off the Scribbler's motors.\n");
			#endif
			_scribbler->stop();
		}
		// Acknowledge the request.
		Publish(_position_addr, queue, PLAYER_MSGTYPE_RESP_ACK, PLAYER_POSITION2D_REQ_MOTOR_POWER);
		return 0;
	} else if (Message::MatchMessage(msghdr, PLAYER_MSGTYPE_REQ, PLAYER_POSITION2D_REQ_SET_ODOM, _position_addr)) {
		#ifdef FRIBBLER_DEBUG
			fprintf(stderr, "Received Position2D odometry request.\n");
		#endif
		// Reset the odometry.
		Lock();
			_position_data.pos.px = 0;
			_position_data.pos.py = 0;
			_position_data.pos.pa = 0;
		Unlock();
		// Fill in the current odometry.
		memset(&_position_odom, 0, sizeof(player_position2d_set_odom_req_t));
		_position_odom.pose = _position_data.pos;
		// Acknowledge the request; publish the odometry.
		Publish(_position_addr, queue, PLAYER_MSGTYPE_RESP_ACK, PLAYER_POSITION2D_REQ_SET_ODOM, (void *)&_position_odom, sizeof(player_position2d_set_odom_req_t), 0);
		return 0;
	} else if (Message::MatchMessage(msghdr, PLAYER_MSGTYPE_CMD, PLAYER_POSITION2D_CMD_VEL, _position_addr)) {
		#ifdef FRIBBLER_DEBUG
			fprintf(stderr, "Received Position2D velocity command.\n");
		#endif
		// Extract the data that accompanied this command.
		player_position2d_cmd_vel_t *cmd = (player_position2d_cmd_vel_t *)data;
		// Since the Scribbler is nonholonomic, ignore the y value.
		int leftMotor = 0, rightMotor = 0; // set the motors appropriately before calling Scribbler::drive()
		// FIXME:  keeping turning and driving mutually exclusive for now to keep things simple
		// FIXME: get rid of the magic numbers!
		// turning has precedence
		if (cmd->vel.pa > 0) {
			#ifdef FRIBBLER_DEBUG
				fprintf(stderr, "Turning counter-clockwise.\n");
			#endif
			cmd->vel.px = 0.0; // we're turning; no x velocity
			// FIXME: this is specific to Scribbler #15
			if (cmd->vel.pa > 3.0) {
				cmd->vel.pa = 3.85 ; // radians/second
				rightMotor =  100;
				leftMotor  = -100;
			} else {
				cmd->vel.pa = 2.02 ; // radians/second
				rightMotor =  60;
				leftMotor  = -60;
			}
		} else if (cmd->vel.pa < 0) {
			#ifdef FRIBBLER_DEBUG
				fprintf(stderr, "Turning clockwise.\n");
			#endif
			// FIXME: yea...
		} else if (cmd->vel.px > 0) { // forward calibration
			#ifdef FRIBBLER_DEBUG
				fprintf(stderr, "Driving forward.\n");
			#endif
			// FIXME: this is specific to Scribbler #15
			// FIXME: drive at max speed for now
			cmd->vel.px = 0.3; // meters/second
			cmd->vel.pa = 0;
			leftMotor = 100;
			int error_correction = -5;
			if (leftMotor + error_correction < 10) {
				// too slow to make any reasonable difference;
				// keep the motors at the same rate
				rightMotor = leftMotor;
			} else {
				// otherwise compensate for error
				rightMotor = leftMotor + error_correction;
			}
		} else if (cmd->vel.px < 0) { // reverse calibration
			#ifdef FRIBBLER_DEBUG
				fprintf(stderr, "Driving backward.\n");
			#endif
			// FIXME: yea...
		} else { // stop!
			#ifdef FRIBBLER_DEBUG
				fprintf(stderr, "Stopping.\n");
			#endif
			cmd->vel.px = 0;
			cmd->vel.pa = 0;
			rightMotor = leftMotor = 0;
		}
		// leftMotor and rightMotor should be set appropriately;
		// go Scribbler, go!
		if (_scribbler->drive(leftMotor, rightMotor) == 0) {
			#ifdef FRIBBLER_DEBUG
				fprintf(stderr, "Scribbler failed to drive.\n");
			#endif
		} else {
			// Update our position data.
			Lock();
				_position_data.vel.px = cmd->vel.px;
				_position_data.vel.pa = cmd->vel.pa;
			Unlock();
		}
		return 0;
	} else if (Message::MatchMessage(msghdr, PLAYER_MSGTYPE_CMD, PLAYER_POSITION2D_CMD_CAR, _position_addr)) {
		#ifdef FRIBBLER_DEBUG
			fprintf(stderr, "Received Position2D car-like command.\n");
		#endif
		// Extract the data that accompanied this command.
		player_position2d_cmd_car_t *cmd = (player_position2d_cmd_car_t *)data;
		#ifdef FRIBBLER_DEBUG
			fprintf(stderr, "Setting Scribbler's velocity to %f m/s.\n", cmd->velocity);
		#endif
		// FIXME: we need to translate angle of steering into motor differential
		if (_scribbler->drive(cmd->velocity, cmd->velocity) == 0) {
			#ifdef FRIBBLER_DEBUG
				fprintf(stderr, "Scribbler failed to drive.\n");
			#endif
		} else {
			// Update our position data.
			_position_data.vel.px = cmd->velocity;
			// FIXME: there's more...
		}
		return 0;
	} else if (Message::MatchMessage(msghdr, PLAYER_MSGTYPE_CMD, PLAYER_POSITION2D_CMD_POS, _position_addr)) {
		#ifdef FRIBBLER_DEBUG
			fprintf(stderr, "Received Position2D position command.\n");
		#endif
		// TODO: Go to a specific position.
		return 0;
	} else {
		// Unknown message.
		#ifdef FRIBBLER_DEBUG
			/* Redundant since Player will complain for us...
			fprintf(stderr, "Received an unknown message; TYPE: %d; SUBTYPE: %d\n", msghdr->type, msghdr->subtype);
			*/
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
