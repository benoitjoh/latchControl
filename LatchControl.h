// **************************************************************************
//
//  This is a simple Class to drive a latched shift register (like 78HC595)
//  with an ATMEGA controller
//
//  This variant uses
//  - two digital output pins of the controller in mode = MODE_2W :
//       a clockPin and a latchPin
//       (latchPin = clockPin +1)
//  - one digital output pin of the controller in mode = MODE_1W :
//       clockPin
//
//  the other pins are driven via capacity circuit with
//  distinct time delay
//                                                      Johannes Benoit 2017
// **************************************************************************

#ifndef LatchControl_h
#define LatchControl_h

#include "Arduino.h"

#define MODE_2W  2 // use 2 pins (pinClock and pinLatch)
#define MODE_1W  1 // use only one pin (pinClock)



typedef volatile uint8_t *port_register;


class LatchControl
{
public:
    LatchControl(byte pinClock, byte mode);
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
    void _shiftOutData(byte data);
    void _shiftToLatch(); //write the _latchState register out to the IC
    byte _latchState;

    byte _pinClock;
    byte _pinLatchClock;
    byte _mode;
    bool _use_cache;


};

#endif

