// ----------------------------------------------------------------------
//  module to deal with a latched shift register (78HC595)
//
//                                            (c) Johannes Benoit 2017
// ----------------------------------------------------------------------

#include <LatchControl.h>

#define LOW_DELAY_MYS 10


#define DELAY LOW_DELAY_MYS + 1

void LatchControl::_shiftOutData(byte data)
{
        port_register _srDataRegister = portOutputRegister(digitalPinToPort(_pinClock));
        uint8_t port = digitalPinToPort(_pinClock); // 2: PORTB 4:PORTD
	    uint8_t _bitMask = digitalPinToBitMask(_pinClock);

        noInterrupts(); //timecritical section, so turn off interrupts
	    for(int i=0; i<8; i++)
	    {
            // take the bit in postion 7 and clear all others
            if (data & B10000000)
            {
                // logical one: a fast pulldown of the clockPin
                // only possibility to get a pulse shorter than 500ns ist to
                // address port directly
                if (port == 2)
                {
                PORTB &= ~_bitMask ; //clrMask; // set LOW
                PORTB |= _bitMask; // set HIGH  B00000010;
                }
                else
                {
                PORTD &= ~_bitMask ; // set LOW
                PORTD |= _bitMask; // set HIGH
                }

            }
            else
            {
                // logical zero longer pulldown --> delay
                *_srDataRegister &= ~_bitMask ; // set LOW
                delayMicroseconds(DELAY);

                *_srDataRegister |= _bitMask; // set HIGH
             }

            // shift the next bit to position 7
            data = data << 1;

	    }

        interrupts();
        if (_mode == MODE_2W)
        {
            // two wire: set latchClock
            digitalWrite(_pinLatchClock, HIGH);
            digitalWrite(_pinLatchClock, LOW);
        }
        else
        {
            // for one wire: just allow latchClock to recover to HIGH
            delayMicroseconds(4);
        }


}


LatchControl::LatchControl(byte pinClock, byte mode)
// constructor
{
    _pinClock = pinClock;
    _mode = mode;
    _pinLatchClock = pinClock + 1;
    //prepare pins
    pinMode(_pinClock, OUTPUT);
	digitalWrite(_pinClock, HIGH);
    if (_mode = MODE_2W)
    {
        // in two wire: init the latchClock..
        pinMode(_pinLatchClock, OUTPUT);
        digitalWrite(_pinLatchClock, LOW);
    }


    _latchState = 0;
    _latchStateLast = 0;
    _use_cache = false;
    reset();
}

void LatchControl::_shiftToLatch()
// write the byte to pins
{
    if (_latchState != _latchStateLast and _use_cache == false )
    {
        // only write to ShiftRegister if anything has changed and cachemode is off.
        _shiftOutData( _latchState);
        _latchStateLast = _latchState;

    }
}



void LatchControl::on(byte pin)
{
    _latchState = _latchState | (B00000001 << pin);
    _shiftToLatch();
}

void LatchControl::off(byte pin)
{
    _latchState = _latchState & ~(B00000001 << pin);
    _shiftToLatch();
}


void LatchControl::reset()
{
    _latchState = 0;
    _use_cache = false;
    _shiftToLatch();
}

void LatchControl::setComplete(byte value)
{
    _latchState = value;
    _shiftToLatch();
}

void LatchControl::startCache()
{
    _use_cache = true;
}

void LatchControl::flushCache()
{
    _use_cache = false;
    _shiftToLatch();
}

byte LatchControl::getState(void)
{
    return _latchState;
}
