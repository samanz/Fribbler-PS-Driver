#include "libplayercore/playercore.h"
#include "Scribbler/scribbler.h"

// Fluke + Scribbler = Fribbler!
// -- A Player driver.
class Fribbler : public ThreadedDriver
{
	private:
		Serial *_port;
		Scribbler *_scribbler;

	public:
		Fribbler(ConfigFile *cf, int section);
		virtual ~Fribbler();

		// Thread life-cycle
		virtual void Main();
		virtual int Setup();
		virtual int Shutdown();

		// Message handling
		virtual int Subscribe(player_devaddr_t id);
		virtual int Unsubscribe(player_devaddr_t id);
		virtual int ProcessMessage(MessageQueue *queue, player_msghdr *msghdr, void *data);
};
