//ArduMIDI "Test" Example, by Pecacheu.

//Simple test to make sure your MIDI OUT port is wired correctly.

#include <HardwareSerial.h> //<-- Not Necessary, But Recomended for Stability Across Arduino IDE Versions.
#include <ArduMIDI.h> //<-- Include ArduMIDI Library.

//Create new instance of ArduMIDI library:
ArduMIDI midi = ArduMIDI(Serial, CH1);

void setup() {
  //Initialize MIDI Libary:
  midi.begin();
}

void loop() {
  //Generate random note:
  byte rand = random(0, 127);
  //Turn note on, then off again:
  midi.noteOn(CH1, rand, 127);
  delay(300);
  midi.noteOff(CH1, rand);
  delay(1000);
}
