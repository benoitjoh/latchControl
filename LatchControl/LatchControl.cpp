// ----------------------------------------------------------------------
//  module to deal with a latched shift register (78HC595)
//
//  this variant uses two pins (MODE_2W) or one pin (MODE_1W):
//
//  
//                                            (c) Johannes Benoit 2017
// ----------------------------------------------------------------------

#include <LatchControl.h>

#define LOW_DELAY_MYS  2 

LatchControl::LatchControl(byte pinClock, byte mode)
// constructor
{
    _pinClock = pinClock;
    _pinLatchClock = pinClock + 1;
    _mode = mode;

    //prepare pins
    pinMode(_pinClock, OUTPUT);
	digitalWrite(_pinClock, HIGH);
    
    if (_mode = MODE_2W)
    {
		// in two wire: init the latchClock.. 
        pinMode(_pinLatchClock, OUTPUT);
        digitalWrite(_pinLatchClock, LOW);
    }

    // portadress for fast pinClock. (digitalWrite is to slow... )
    //  _bitMask to set the pin LOW with an AND operation
    // _setMask is to set it HIGH with an OR operation
    // hint: using pointer for distinguish port (_myport = &PORTB ..)
    //       works, but slows down pinswap to 1mySecond.
    //       with direct adressing: 200ns

	_use_port_b = (_pinClock > 7); // PORTB 8..13, PORTD 0..7
	_bitMask = digitalPinToBitMask(_pinClock);

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


        byte value = _latchState;

		
        noInterrupts(); //timecritical section, so turn off interrupts
        
	if (_use_port_b == true)
	{  
	
	    for(int i=0; i<8; i++)
	    {   
		// take the bit in postion 7 and clear all others 
		byte mybit = value & B10000000;
		
		if (mybit == B10000000)
		{
		    // logical one: a fast pulldown of the clockPin --> no delay
		    PORTB &=  ~_bitMask; 
		    PORTB |= _bitMask;
		}
		else
		{
		    // logical zero longer pulldown --> delay
		    PORTB &= ~_bitMask; 
		    delayMicroseconds(LOW_DELAY_MYS);
		    PORTB |= _bitMask;
		}		
		
		// shift the next bit to position 7
		value = value << 1;
		
	    }
	}
	else //PORT D
	{  
	    
	    for(int i=0; i<8; i++)
	    {   
		// take the bit in postion 7 and clear all others 
		byte mybit = value & B10000000;
		
		if (mybit == B10000000)
		{
		    // logical one: a fast pulldown of the clockPin --> no delay
		    PORTD &= ~_bitMask;  
		    PORTD |= _bitMask;
		}
		else
		{
		    // logical zero longer pulldown --> delay
		    PORTD &= ~_bitMask;  
		    delayMicroseconds(LOW_DELAY_MYS);
		    PORTD |= _bitMask;
		}		
    
		// shift the next bit to position 7
		value = value << 1;
    
	    }
	}
        
        interrupts();
        
        if (_mode == MODE_2W)
        {
            // two wire: set latchClock
            digitalWrite(_pinLatchClock, HIGH);
            digitalWrite(_pinLatchClock, LOW);
        }
        // for one wire: just allow latchClock to recover to HIGH
        // which is done by the subroutine overhead it self
 
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
