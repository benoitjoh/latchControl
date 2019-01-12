
#include <LatchControl.h>
#define CLOCKPIN 9

LatchControl latch(CLOCKPIN);

void setup() {
}

void loop() {
  int delayTime = 60 ;
  for(int i=0;i<256;i++)
    {
    latch.setComplete(i);
    delay(200);
    }
  
}
