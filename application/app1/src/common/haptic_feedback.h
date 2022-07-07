#ifndef HAPTIC_FEEDBACK_H_
#define HAPTIC_FEEDBACK_H_

#include "mbed.h"
#include "mbed_events.h"

#define HAPTIC_VIBRATION_MS 50

// Class representing an haptic feedback device
// This class provide the haptic feedback functionality
class HapticFeedback
{
public:
    HapticFeedback(PinName pin);
    ~HapticFeedback();

    // Start the haptic feedback
    void vibrate();

private:
    DigitalOut _device;
    EventQueue _queue;
    UserAllocatedEvent<mbed::Callback<void()>, void()> _event;
    Thread _dispatch_thread;
    
    void stop_vibration();
};

#endif // HAPTIC_FEEDBACK_H_