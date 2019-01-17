# LatchControl #
This is a simple Class to drive a latched shift register (78HC595) with an ATMEGA controller
with only one pin

---
**CONTENT**

[TOC]

---

## Requirements ##
The setup uses one connection between output pins of the controller and chip:

 * clockPin  (SCK at 74HC595)

The dataPin (SER = pin14 at 74HC595) is driven via capacity and resistor circuit with distinct time delay.  A diode cares of a fast return to HIGH.




## Circuit ##



![circuit](https://github.com/benoitjoh/latchControl/blob/master/latch_circuit_1w.png)



 C1    | R1   | C2    | R2   |   lowDelay
 ------|------| ------|------|--------------
 220pF | 2.2k | 220pF | 10k  |    1µs   
 2.2nF | 330  | 2.2nF |  1k  |    1µs

## Working Scheme ##

### Functional Scheme


To see how it works, see the timing diagram (ascii-oszillogram :-)

 * The dataPin is valid on rising edge of clockPin.
 * So a short negative pulse leaves the data signal on high level, while a long negative pulse results in a low level of data signal.
 * when clockPin returns to high, the data signal is evaluated as 1 or zero.
 * after one 8 bit are transfered, latch returns to high
   * by controller (in MODE_2W)
   * or by the loading capacitor (in MODE_1W)

in the example, a hypothetical binary "B10" is transfered:
```bash

clockPin (SCK) signal (=signal from pinClock of controller):

              |<-- reads a one here
              |
              |           |<-- reads a zero here
              V           V
high --------..-----.     .-----------------------------------
             ||     |     |
   low       |/     |_____|
             200ns

dataPin (SER) resulting signal:

   high -----..-----.     .-------------------------------
                     \_   |
   low                 \__|
               |<-->|
                    |<-  ->|
                     1 µs = lowDelay
                1µs

latchPin (RCK) resulting signal 
                                    |<-- move data to outputs now
                                    V
   high ------.                      .--------------------------
              |                 ..--'
   low        '--------------'''

  
```
 and here how it looks like on a screen:


![oszi](https://github.com/benoitjoh/latchControl/blob/master/latch_oscilloscope.jpg)


### fast portswitch in software

to realize the very short signals ( < 500ns) the data have to be directly written to the PORTB or PORTD depending which pin is used.

Use of digitalWrite() is much too slow (about 4µs). Even use of a pointer to the port register (which would make the code more elegant) needs 1µs.

so the fastest way for the short LOW/HIGH swap is following:

```
// using pin 9: pin1 on PORTB
#define _bitMask B11111101

noInterrupts(); // the timecritical section must be free of interrupts!
PORTB &= ~_bitMask ; // set LOW
PORTB |= _bitMask; // set HIGH;

interrupts();

After end of transmission the latchclock needs about 4µs to recover.

```

## Usage ##

### Initialization


```c++
// --- use driver for latch register
#include <LatchControl.h>
#define PIN_LATCH_DATACLOCK  9           // shiftregister: clock (and data) signal
// PIN_LATCH_DATACLOCK  is PIN_CLOCK + 1 // shiftregister: latch shift clock signal
// MODE is MODE_2W (two wire mode) or MODE_1W (one wire mode)

LatchControl latch(PIN_LATCH_CLOCK, MODE);

```

### Methods

#### on(pin) / off(pin) : change single pins ###

```c++
latch.off(3); //turns off pin #3
latch.on(2);  //turns on pin #2
```

#### startCache(), flushCache() : reduce traffic

```c++
latch.startCache();

// change some single pins
latch.off(3);
latch.off(2);
latch.on(6);

// write the state to the chip once
latch.flushCache();
```

#### reset() : turn of all

```c++
latch.reset(); //turns off all pins
```

#### setComplete() : set all pins at once

```c++
latch.setComplete(0x0f); //turns on 0 .. 3 and off 4 .. 7
```

#### byte getState() : get the actual state of the pins

```c++
a = latch.getState(); //returns the binary state of all pins
```

## Example sketch

``` c++
#include <LatchControl.h>
#define PIN_LATCH_DATACLOCK 9
LatchControl latch(PIN_LATCH_DATACLOCK);

void setup()
{
    latch.reset();
}

void loop()
{
  for(int i=0;i<256;i++)
    {
    latch.setComplete(i);
    delay(50);
    }

}

```

[Johannes Benoit 2017](mailto:jbenoit@t-online.de)


