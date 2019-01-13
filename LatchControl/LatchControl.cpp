// ----------------------------------------------------------------------
//  module to deal with a latched shift register (78HC595)
//
//                                            (c) Johannes Benoit 2017
// ----------------------------------------------------------------------

#include <LatchControl.h>

#define LOW_DELAY_MYS  2


void shiftOutData(byte pin, byte data)
{
        fio_register _srDataRegister = portOutputRegister(digitalPinToPort(pin));
        uint8_t port = digitalPinToPort(pin); // 2: PORTB 4:PORTD
	    uint8_t _bitMask = digitalPinToBitMask(pin);

        noInterrupts(); //timecritical section, so turn off interrupts
	    for(int i=0; i<8; i++)
	    {
            // take the bit in postion 7 and clear all others
            if (data & B10000000)
            {
                // logical one: a fast pulldown of the clockPin --> no delay
                // only possibility to get a pulse shorter than 500ns ist to
                // address port directly
                if (port == 2)
                {
                PORTB &= ~_bitMask ; //clrMask; // set LOW
                PORTB |= _bitMask; // set HIGH  B00000010;
                }
                else
                {
                PORTD &= ~_bitMask ; //clrMask; // set LOW
                PORTD |= _bitMask; // set HIGH  B00000010;
                }

            }
            else
            {
                // logical zero longer pulldown --> delay
                *_srDataRegister &= ~_bitMask ; //clrMask; // set LOW
                delayMicroseconds(LOW_DELAY_MYS);
                *_srDataRegister |= _bitMask; // set HIGH  B00000010;
             }

            // shift the next bit to position 7
            data = data << 1;

	    }

        interrupts();

}


LatchControl::LatchControl(byte pinClock)
// constructor
{
    _pinClock = pinClock;

    //prepare pins
    pinMode(_pinClock, OUTPUT);
	digitalWrite(_pinClock, HIGH);

    // portadress for fast pinClock. (digitalWrite is to slow... )
    //  _bitMask to set the pin LOW with an AND operation
    // _setMask is to set it HIGH with an OR operation
    // hint: using pointer for distinguish port (_myport = &PORTB ..)
    //       works, but slows down pinswap to 1mySecond.
    //       with direct adressing: 200ns


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
        shiftOutData(_pinClock, _latchState);
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
