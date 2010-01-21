/**
 * Fribbler (Fluke + Scribbler) driver plugin for the Player robot server.
 * Developed by the MetroBotics project at CUNY.
 */
#ifndef FRIBBLER_H
#define FRIBBLER_H

#define FRIBBLER_DEBUG 1
#define FRIBBLER_CYCLE 10000 // 1000000/10000 == 0.01 seconds

#include "libplayercore/playercore.h"
#include "Scribbler/scribbler.h"

Driver* Fribbler_Init(ConfigFile *cf, int section);
void Fribbler_Register(DriverTable *table);

// Fluke + Scribbler = Fribbler!
// -- A Player driver.
class Fribbler : public ThreadedDriver
{
	private:
		const char *_portname;
		Serial     *_port;
		Scribbler  *_scribbler;

		// Timing
		timeval _t0,
				_t1;
		time_t seconds;	  
		double  _framerate; // duration of each frame (in seconds)
		int		_camrate; // Camera framrate
		bool 	_firstPic;
		
		// Camera
		unsigned char * picture;

		// Player interfaces.
		// Position2D
		bool                             _hasPosition;
		player_devaddr_t                 _position_addr;
		player_position2d_data_t         _position_data;
		player_position2d_geom_t         _position_geom;
		player_position2d_set_odom_req_t _position_odom;
		player_devaddr_t				 _camera_addr;

	public:
		Fribbler(ConfigFile *cf, int section);
		virtual ~Fribbler();

		// Thread life-cycle
		virtual void Main();
		virtual int  MainSetup();
		virtual void MainQuit();

		// Message handling
		virtual int Subscribe(player_devaddr_t id);
		virtual int Unsubscribe(player_devaddr_t id);
		virtual int ProcessMessage(QueuePointer &queue, player_msghdr *msghdr, void *data);
};

#endif
