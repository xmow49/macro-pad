#include "HID-Project.h"

const int MediaFastForward = 0xB3;
const int MediaRewind = 0xB4;
const int MediaNext = 0xB5;
const int MediaPrevious = 0xB6;
const int MediaStop = 0xB7;
const int MediaPlayPause = 0xCD;

const int MediaVolumeMute = 0xE2;
const int MediaVolumeUP = 0xE9;
const int MediaVolumeDOWN = 0xEA;

const int ConsumerEmailReader = 0x18A;
const int ConsumerCalculator = 0x192;
const int ConsumerExplorer = 0x194;

const int ConsumerBrowserHome = 0x223;
const int ConsumerBrowserBack = 0x224;
const int ConsumerBrowserForward = 0x225;
const int ConsumerBrowserRefresh = 0x227;
const int ConsumerBrowserBookmarks = 0x22A;


String strAction[] = {"MediaFastForward", "MediaRewind", "MediaNext", "MediaPrevious", "MediaStop", "MediaPlayPause",
                      "MediaVolumeMute", "MediaVolumeUP", "MediaVolumeDOWN",
                      "ConsumerEmailReader", "ConsumerCalculator", "ConsumerExplorer",
                      "ConsumerBrowserHome", "ConsumerBrowserBack", "ConsumerBrowserForward", "ConsumerBrowserRefresh", "ConsumerBrowserBookmarks"
                     };
int hexAction[] = {0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xCD,
                   0xE2, 0xE9, 0xEA,
                   0x18A, 0x192, 0x194,
                   0x223, 0x224, 0x225, 0x227, 0x22A
                  };

int keyAction[3] = {0,0,0};

const int PinA = 7;
//const int PinB = 6;

const int PinB1 = 6;
const int PinB2 = 5;
const int PinB3 = 4;

int lastCount = 50;
volatile int virtualPosition = 50;

String serialMsg;

String getArgs(String data, char separator, int index)
{
  //Function for separate String with special characters
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++)
  {
    if (data.charAt(i) == separator || i == maxIndex)
    {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void isr ()  {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();

  // If interrupts come faster than 5ms, assume it's a bounce and ignore
  if (interruptTime - lastInterruptTime > 5) {

    if (analogRead(A0) == 0)
    {
      virtualPosition = virtualPosition - 10 ; // Could be -5 or -10
    }
    else {
      virtualPosition = virtualPosition + 10 ; // Could be +5 or +10
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
  //pinMode(PinB, INPUT);

  pinMode(PinB1, INPUT);
  pinMode(PinB2, INPUT);
  pinMode(PinB3, INPUT);

  attachInterrupt(digitalPinToInterrupt(PinA), isr, LOW);
  Serial.println("Start");
  Consumer.begin();

}

void loop() {
  if (digitalRead(PinB1)) {
    while (digitalRead(PinB1)) {}
    Consumer.write(keyAction[0]);
  }
  if (digitalRead(PinB2)) {
    while (digitalRead(PinB2)) {}
    Consumer.write(keyAction[1]);
  }
  if (digitalRead(PinB3)) {
    while (digitalRead(PinB3)) {}
    Consumer.write(keyAction[2]);
  }
  if (virtualPosition != lastCount) {
    // Write out to serial monitor the value and direction
    //Serial.print(virtualPosition > lastCount ? "Up  :" : "Down:");
    //Serial.println(virtualPosition);

    if (virtualPosition > lastCount) {
      Serial.println("Encoder1:UP");
      Consumer.write(MEDIA_VOL_UP);
    } else {
      Consumer.write(MEDIA_VOL_DOWN);
      Serial.println("Encoder1:DOWN");
    }
    lastCount = virtualPosition ;
  }

  if (Serial.available() > 0) {
    serialMsg = Serial.readString();
    serialMsg.trim();
    if (serialMsg.indexOf("set") > -1) {
      if (serialMsg.indexOf("key") > -1) {
        int key = getArgs(serialMsg, ' ', 2).toInt();
        String action = getArgs(serialMsg, ' ', 3);
        keyAction[key-1] = getAction(action);
      }
    }
  }

}


int getAction(String action) {
  for (byte i = 0; i < sizeof(strAction); i++) {
    if (action == strAction[i]) {
      Serial.print("Found: ");
      Serial.println(i);
      return hexAction[i];
      break;
    }
  }
return 0;
}
