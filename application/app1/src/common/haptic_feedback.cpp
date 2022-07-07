#include "haptic_feedback.h"

HapticFeedback::HapticFeedback(PinName pin) : _device(pin),
                                              _queue(0),
                                              _event(&_queue, callback(this, &HapticFeedback::stop_vibration))
{
    _event.delay(HAPTIC_VIBRATION_MS);
    _dispatch_thread.start(callback(&_queue, &EventQueue::dispatch_forever));
}

HapticFeedback::~HapticFeedback()
{
    _dispatch_thread.terminate();
    _dispatch_thread.join();
}

void HapticFeedback::vibrate()
{
    _device = 1;
    _event.call();
}

void HapticFeedback::stop_vibration()
{
    _device = 0;
}