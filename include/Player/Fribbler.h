/**
 * Fribbler (Fluke + Scribbler) driver plugin for the Player robot server.
 * Developed by the MetroBotics project at CUNY.
 */
#ifndef FRIBBLER_H
#define FRIBBLER_H

#define FRIBBLER_DEBUG 1
#define FRIBBLER_CYCLE 10000 // 1000000/100 == 0.01 seconds

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
		Serial *_port;
		Scribbler *_scribbler;

		// Player interfaces.
		bool                     _hasPosition;
		player_devaddr_t         _position_addr;
		player_position2d_data_t _position_data;

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
