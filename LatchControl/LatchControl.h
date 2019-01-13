// **************************************************************************
//
//  This is a simple Class to drive a latched shift register (like 78HC595)
//  with an ATMEGA controller
//
//  - one digital output pin of the controller:
//       clockPin
//
//  the other pins are driven via capacity circuit with
//  distinct time delay
//                                                      Johannes Benoit 2017
// **************************************************************************

#ifndef LatchControl_h
#define LatchControl_h

#include "Arduino.h"

typedef volatile uint8_t *port_register;


class LatchControl
{
public:
    LatchControl(byte pinClock);
    // pins of ATMEGA for shift register


    void on(byte pin);
    void off(byte pin);
    //void digitalWrite(byte pin, byte value); // just for compatibility
    void setComplete(byte value); // set all pins at once
    void reset();  // set all to zero
    byte getState();

    // if known that several bits are changed in a code area, you can define a cache section
    // at the end of the section the state is written to chip
    void startCache();
    void flushCache();

//private:
protected:
    void _shiftToLatch(); //write the _latchState register out to the IC
    byte _latchState;
    byte _latchStateLast;

    byte _pinClock;
    byte _mode;
    bool _use_cache;


};

#endif

