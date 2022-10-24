// examples with latchregister

#include <LatchControl.h>
#define CLOCKPIN 9
LatchControl latch(CLOCKPIN, MODE_1W);

int i = 0;
void setup() {
  latch.reset();
  pinMode(5, OUTPUT);
}

void loop() {
    
    latch.setComplete(i);//1 << i);
    i++;
    if (i>255)
    {
        i=0;
    }
    delay(100);
  
}

//  Wiring example: 
// ----------------------------------------------------------------------
//                                                C1 2.2nF
//                         74HC595     (VCC)    +----||----(GND)
//                       +----u----+     |      |       diode
//  out1   ------------1-|QB    VCC|-16--+      +-----|<|-------+
//  out2   ------------2-|QC     QA|-15-- out0  |               |
//  out3   ------------3-|QD    SER|-14---------+-------[R1]----+
//  out4   ------------4-|QE    /OE|-13--(GND)           330    |
//  out5   ------------5-|QF    RCK|-12--------------+          |
//                       |         |                  \         |
//  out6   ------------6-|QG    SCK|-11-------+--------)--------+--(Serial PIN)
//  out7   ------------7-|QH   /CLR|-10-(VCC) |       /         |
//                  +--8-|GND   Q'H|--9       +--|<|-+--[R2]----+
//                  |    +---------+           diode |   1k
//                  |                                |
//                  |      0.1uF                     |    C2
//                (GND)-----||----(VCC)              +----||----(GND)
//                                                       2.2nF
//
//  Timing examples (times in mircoseconds)
//  C1     R1    C2    R2    LOW_DELAY_MYS 
//  2.2nF  220 2.2nF  1k        2     
//   
//  --> transfer speed results to 25 kByte/sec 
// ----------------------------------------------------------------------
