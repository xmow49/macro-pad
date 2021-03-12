#include "HID-Project.h"

const int PinA = 7;
const int PinB = 6;

const int PinSW = 8;


int lastCount = 50;
volatile int virtualPosition = 50;


void isr ()  {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();

  // If interrupts come faster than 5ms, assume it's a bounce and ignore
  if (interruptTime - lastInterruptTime > 5) {
    if (digitalRead(PinB) == LOW)
    {
      virtualPosition-- ; // Could be -5 or -10
    }
    else {
      virtualPosition++ ; // Could be +5 or +10
    }

    // Restrict value from 0 to +100
    //virtualPosition = min(100, max(0, virtualPosition));


  }
  // Keep track of when we were here last (no more than every 5ms)
  lastInterruptTime = interruptTime;
}

void setup() {
  // Just whilst we debug, view output on serial monitor
  Serial.begin(9600);

  // Rotary pulses are INPUTs
  pinMode(PinA, INPUT);
  pinMode(PinB, INPUT);

  // Switch is floating so use the in-built PULLUP so we don't need a resistor
  pinMode(PinSW, INPUT_PULLUP);

  // Attach the routine to service the interrupts
  attachInterrupt(digitalPinToInterrupt(PinA), isr, LOW);

  // Ready to go!
  Serial.println("Start");
  //Keyboard.begin();
  Consumer.begin();

}

void loop() {


  // If the current rotary switch position has changed then update everythinguuuuuuuu
  if (virtualPosition != lastCount) {

    // Write out to serial monitor the value and direction
    Serial.print(virtualPosition > lastCount ? "Up  :" : "Down:");
    Serial.println(virtualPosition);

    if (virtualPosition > lastCount) {
      Consumer.write(MEDIA_VOL_UP);
    } else {
      Consumer.write(MEDIA_VOL_DOWN);
    }

    // Keep track of this new valueuuu
    lastCount = virtualPosition ;
  }

}
