#include "vic.h"
#include "lpc210x.h"
#include "inttypes.h"

// Make the address and control registers accessible as arrays
#define VICVectAddr_n   (&VICVectAddr0)
#define VICVectCntl_n   (&VICVectCntl0)

int vic_next_slot = 0;

// This is the default VIC handler, executed when an interrupt with no handler
// occurs.  This probably doesn't help, since the interrupt usually needs to be cleared
// in the peripheral that generated it.
void vic_default() __attribute__ ((interrupt("IRQ")));
void vic_default()
{
    VICVectAddr = 0;
}

void vic_init()
{
    // Set the default interrupt to do nothing
    VICDefVectAddr = (uint32_t)vic_default;
    
    // Enable interrupts
    sei();
}

void vic_add_handler(vic_source_t source, irq_handler_t handler)
{
    int slot;
    
    if (VICIntEnable & (1 << source))
    {
        for (slot = 0; slot < 16; ++slot)
        {
            if ((VICVectCntl_n[slot] & 0x1f) == source)
            {
                break;
            }
        }
        
        if (slot == 16)
        {
            // Enabled, but couldn't find the slot.
            slot = vic_next_slot++;
        }
    } else {
        // Use the next available slot
        slot = vic_next_slot++;
    }
    
    if (slot > 15)
    {
        // Out of slots
        return;
    }
    
    VICVectAddr_n[slot] = (uint32_t)handler;
    VICVectCntl_n[slot] = 0x20 | source;
    
    // Writing zeros has no effect, so no need to OR.
    VICIntEnable = 1 << source;
}
