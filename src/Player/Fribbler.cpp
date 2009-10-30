#include "Player/Fribbler.h"

// factory creation function
Driver* Fribbler_Init(ConfigFile *cf, int section)
{
	return ((Driver *) (new Fribbler(cf, section)));
}

// registration
void Fribbler_Register(DriverTable *table)
{
	table->AddDriver("fribbler", Fribbler_Init);
}

Fribbler::Fribbler(ConfigFile *cf, int section)
: ThreadedDriver(cf, section, true, PLAYER_MSGQUEUE_DEFAULT_MAXLEN)
{
}

Fribbler::~Fribbler()
{
}

void Fribbler::Main()
{
	while (1) {
		this->ProcessMessages();
	}
}

int Fribbler::MainSetup()
{
	this->StartThread();
	return 0;
}

void Fribbler::MainQuit()
{
	this->StopThread();
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
