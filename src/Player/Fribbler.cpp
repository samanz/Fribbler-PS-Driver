/**
 * Fribbler (Fluke + Scribbler) driver plugin for the Player robot server.
 * Developed by the MetroBotics project at CUNY.
 */
#include "Player/Fribbler.h"
#include "Scribbler/PosixSerial.h"
#include "Scribbler/scribbler.h"
#include "metrobotics.h"
using metrobotics::RealVector3;

#include <stdexcept>
using namespace std;

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#include <libplayercore/playercore.h>
#include <libplayerjpeg/playerjpeg.h>

// John's global debug flag
#define FRIBBLER_DEBUG 1
#ifdef SCRIBBLER_DEBUG // Enable this at your own risk!
	int gDebugging = 1;
#else
	int gDebugging = 0;
#endif

// Uses libplayerjpeg's decompress function to get rgb values from jpeg and stretches out jpeg
unsigned char* jpegStretch(unsigned char * jpegBuffer, int &size) {
	if (jpegBuffer == 0 || size <= 0) return 0;
	const size_t oldSize = 128 * 192 * 3;
	const size_t newSize = 256 * 192 * 3;
	unsigned char decompressedBuffer[oldSize];
	unsigned char* resizedDecompressedBuffer = new unsigned char[newSize];
	if (resizedDecompressedBuffer == 0) return 0;
	jpeg_decompress(decompressedBuffer, oldSize, jpegBuffer, size);
  	for(int h = 0; h < 192; h++) {
  		for(int w = 0; w < 128; w++) {
  			for(int rgb = 0; rgb < 3; rgb++) {
				// This is some crazy ass black magic!
  				resizedDecompressedBuffer
  					[(h * 256 * 3) + (2 * w * 3) + rgb]
  					= decompressedBuffer
  					[(h * 128 * 3) + (w * 3) + rgb];
  				resizedDecompressedBuffer
  					[(h * 256 * 3) + ((2 * w + 1) * 3) + rgb]
  					= decompressedBuffer
  					[(h * 128 * 3) + (w * 3) + rgb];
  			}
		}
  	}
	return resizedDecompressedBuffer;
}


// Gets Jpeg from scribbler, stretches it, and returns it in the correct format
unsigned char * expandedPhotoJPEG(Scribbler * _scribbler) {
	if (_scribbler == 0) return 0;
	Data* jpeg = _scribbler->takePhotoJPEG();
	if (jpeg == 0) return 0;
	unsigned char* adata = (unsigned char *)jpeg->getData();
	int dataSize = jpeg->getDataSize();
	unsigned char* expanded = jpegStretch(adata, dataSize);
	delete jpeg;
	return expanded;
}



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
    memset(&_camera_addr, 0, sizeof(player_devaddr_t));
    memset(&_blob_addr, 0, sizeof(player_devaddr_t));

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
		// Extract velocity data if any
		int points = 0;
		// Linear Velocity
		linear_velocity.clear();
		points = cf->GetTupleCount(section, "linear_velocity");
		if (points > 0) {
			for (int i = 0, j = 0; i < points; i += j) {
				RealVector3 v;
				// Each point has three components.
				for (j = 0; i+j < points && j < 3; ++j) {
					v[j] = cf->ReadTupleLength(section, "linear_velocity", i+j, 0);
				}
				// Complete point?
				if (j == 3) {
					linear_velocity.insert(v);
				}
			}
			#ifdef FRIBBLER_DEBUG
				fprintf(stderr, "Linear velocity data is set: %lu points recorded.\n", linear_velocity.size());
			#endif
		} else {
			#ifdef FRIBBLER_DEBUG
				fprintf(stderr, "No linear velocity data: disabling driving capability.\n");
			#endif
		}
		// Angular Velocity
		angular_velocity.clear();
		points = cf->GetTupleCount(section, "angular_velocity");
		if (points > 0) {
			for (int i = 0, j = 0; i < points; i += j) {
				RealVector3 v;
				// Each point has three components.
				for (j = 0; i+j < points && j < 3; ++j) {
					v[j] = cf->ReadTupleLength(section, "angular_velocity", i+j, 0);
				}
				// Complete point?
				if (j == 3) {
					angular_velocity.insert(v);
				}
			}
			#ifdef FRIBBLER_DEBUG
				fprintf(stderr, "Angular velocity data is set: %lu points recorded.\n", angular_velocity.size());
			#endif
		} else {
			#ifdef FRIBBLER_DEBUG
				fprintf(stderr, "No angular velocity data: disabling turning capability.\n");
			#endif
		}
	}

	// Read framerate from the config file.
	_camrate = cf->ReadInt(section, "framerate", 0);
	_firstPic = true; // Is this this first picture to be taken?
	seconds = time(NULL);
	_hasCamera = false;
	
	// Add camera interface
	if (cf->ReadDeviceAddr(&_camera_addr, section, "provides", PLAYER_CAMERA_CODE, -1, "jpeg") == 0) {
		if (this->AddInterface(_camera_addr) != 0) {
		   PLAYER_ERROR("Could not add Camera interface for Fribbler");
		   this->SetError(-1);
		   return;
		} else {
			_hasCamera = true;
			#ifdef FRIBBLER_DEBUG
				fprintf(stderr, "Fribbler is providing a Camera interface.\n");
			#endif
		}
	}

	if (cf->ReadDeviceAddr(&_blob_addr, section, "provides", PLAYER_CAMERA_CODE, -1, "blobimage") == 0) {
		if (this->AddInterface(_blob_addr) != 0) {
             PLAYER_ERROR("Could not add BlobImage Camera interface for Fribbler");
             this->SetError(-1);
             return;
		} else {
			_hasBlob = true;
			#ifdef FRIBBLER_DEBUG
				fprintf(stderr, "Fribbler is providing a BlobImage Camera interface.\n");
			#endif
		}
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
		if (gettimeofday(&_t0, 0) != 0) {
			// FIXME: error checking?
		}
		// Update our robot: read sensors and collect relevant data.
/*      No reason to be checking sensors all the time!
		if (_scribbler->updateScribblerSensors() == 0) {
			#ifdef FRIBBLER_DEBUG
				fprintf(stderr, "Scribbler failed to update its sensors.\n");
			#endif
		}
*/
        
		// Update the interfaces that we're providing.
		if (_hasPosition) {
			Lock();
				_position_data.pos.px += _framerate * _position_data.vel.px;
				_position_data.pos.py += _framerate * _position_data.vel.py;
				_position_data.pos.pa += _framerate * _position_data.vel.pa;
			Unlock();
			Publish(_position_addr, PLAYER_MSGTYPE_DATA, PLAYER_POSITION2D_DATA_STATE, (void *)&_position_data, sizeof(_position_data), 0);
		}
		if(_hasCamera) {
			int since = difftime(time(NULL), seconds);
			player_camera_data_t camdata;
			memset(&camdata, 0, sizeof(player_camera_data_t));
			camdata.fdiv = 1; // Not sure what this is?
			camdata.bpp = 24; // 24 bits per pixel
			camdata.width = 256;
			camdata.height = 192;
			camdata.format = PLAYER_CAMERA_FORMAT_RGB888;
			camdata.compression = PLAYER_CAMERA_COMPRESS_RAW;
			camdata.image_count = 256*192*3;
			// expandedPhotoJPEG converts to a RAW RBG so we're fine here!
			if((since >= _camrate) || _firstPic) { // _firstPic makes sure that the camera is updated on first access
				if ((picture = expandedPhotoJPEG(_scribbler)) != 0) {
					_firstPic = false;
					seconds = time(NULL);
					camdata.image = picture;
					Publish(_camera_addr, PLAYER_MSGTYPE_DATA, PLAYER_CAMERA_DATA_STATE, (void*) &camdata);
					delete picture;
				}
			}
		}

		ProcessMessages();
		usleep(FRIBBLER_CYCLE); // Breathe!
		if (gettimeofday(&_t1, 0) != 0) { // end of the frame
			// FIXME: error checking?
		}
		_framerate = (_t1.tv_sec + (_t1.tv_usec / 1000000.0)) -
		             (_t0.tv_sec + (_t0.tv_usec / 1000000.0));
		#if 0
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
		#if 0
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
		#if 0
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
		#if 0
			fprintf(stderr, "Received Position2D motor power request.\n");
		#endif
		// Extract the data that accompanied this command.
		bool motorState = ((player_position2d_power_config_t *)data)->state;
		if (motorState) {
			#if 0
				fprintf(stderr, "Turning on the Scribbler's motors.\n");
			#endif
			// FIXME: what do we do here?
		} else {
			#if 0
				fprintf(stderr, "Turning off the Scribbler's motors.\n");
			#endif
			_scribbler->stop();
		}
		// Acknowledge the request.
		Publish(_position_addr, queue, PLAYER_MSGTYPE_RESP_ACK, PLAYER_POSITION2D_REQ_MOTOR_POWER);
		return 0;
	} else if (Message::MatchMessage(msghdr, PLAYER_MSGTYPE_REQ, PLAYER_POSITION2D_REQ_SET_ODOM, _position_addr)) {
		#if 0
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
		#if 0
			fprintf(stderr, "Received Position2D velocity command.\n");
		#endif
		// Extract the data that accompanied this command.
		player_position2d_cmd_vel_t *cmd = (player_position2d_cmd_vel_t *)data;
		// Since the Scribbler is nonholonomic, ignore the y value.
		int leftMotor = 0, rightMotor = 0; // set the motors appropriately before calling Scribbler::drive()
		// FIXME:  keeping turning and driving mutually exclusive for now to keep things simple
		// Case 1: Turning has precedence!
		if (cmd->vel.pa) {
			#ifdef FRIBBLER_DEBUG
				if (cmd->vel.pa > 0) {
					fprintf(stderr, "Turning counter-clockwise.\n");
				} else {
					fprintf(stderr, "Turning clockwise.\n");
				}
			#endif
			try {
				RealVector3 v = angular_velocity.truncate(cmd->vel.pa);
				cmd->vel.px = 0;
				cmd->vel.pa = v[0];
				leftMotor  = static_cast<int>(v[1]);
				rightMotor = static_cast<int>(v[2]);
			} catch (domain_error e) {
				if (angular_velocity.empty()) {
					fprintf(stderr, "Turning is disabled: no angular velocity data!\n");
				} else {
					fprintf(stderr, "Cannot turn beyond the physical limits of the Scribbler!\n");
				}
				cmd->vel.px = 0;
				cmd->vel.pa = 0;
				leftMotor = 0;
				rightMotor = 0;
			}
		// Case 2: Straight driving!
		} else if (cmd->vel.px) {
			#ifdef FRIBBLER_DEBUG
				if (cmd->vel.px > 0) {
					fprintf(stderr, "Driving forward.\n");
				} else {
					fprintf(stderr, "Driving backward.\n");
				}
			#endif
			try {
				RealVector3 v = linear_velocity.truncate(cmd->vel.px);
				cmd->vel.px = v[0];
				cmd->vel.pa = 0;
				leftMotor  = static_cast<int>(v[1]);
				rightMotor = static_cast<int>(v[2]);
			} catch (domain_error e) {
				if (linear_velocity.empty()) {
					fprintf(stderr, "Driving is disabled: no linear velocity data!\n");
				} else {
					fprintf(stderr, "Cannot drive beyond the physical limits of the Scribbler!\n");
				}
				cmd->vel.px = 0;
				cmd->vel.pa = 0;
				leftMotor = 0;
				rightMotor = 0;
			}
		// Case 3: Stop!
		} else {
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
		#if 0
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
		#if 0
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
	// No messages for camera interface, Sam agrees with Carlos... WHY?!
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
