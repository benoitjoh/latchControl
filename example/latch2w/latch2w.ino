#include <LatchControl.h>
#define CLOCKPIN 9

// latchPin = CLOCKPIN + 1
LatchControl latch(CLOCKPIN, MODE_2W);

void setup() {
  
}

void loop() {
  int delayTime = 60 ;
  for(int i=0;i<256;i++)
    {
    latch.setComplete(i);
    delayMicroseconds(100);
    }
  
}
