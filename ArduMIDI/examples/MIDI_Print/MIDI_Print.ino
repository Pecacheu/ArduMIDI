//ArduMIDI "Print" Example, by Pecacheu.

//Print MIDI Events to the USB serial.

//Note: This sketch requires an Arduino with multiple serial ports, such as an Arduino MEGA.

#include <ArduMIDI.h>

//Create new instance of ArduMIDI library:
//ArduMIDI(SerialObject (Serial, Serial1...), ListenChannel (CH1-CH16 or CH_ALL))
ArduMIDI midi = ArduMIDI(Serial1, CH1);

void setup() {
  //Turn of MEGA 2560's anoying always-on status LED:
  pinMode(13, OUTPUT); digitalWrite(13, LOW);
  
  //Begin MIDI library:
  midi.begin();
  
  //Begin regular Serial:
  Serial.begin(57600);
  
  //Control how data is forwarded:
  //ThruModes: FORWARD_OFF, FORWARD_ALL, FORWARD_OTHER (default), FORWARD_SELF
  midi.setThruMode(FORWARD_SELF);
  
  //Set processData function as handler for any recieved MIDI data:
  //Use "clearHandlerAny" to remove the handler.
  midi.setHandlerAny(processData);
}

void loop() {
  //Call this function as frequently as possible for MIDI reading to work:
  midi.scanForData();
}

void processData(CommandType cmd, Channel ch, byte data[], unsigned int datalen) {
  //Print MIDI Event Data:
  Serial.print(midi.commandTypeToString(cmd));
  Serial.print(", ");
  if(ch == CH_ALL) Serial.print("[N/A]");
  else Serial.print(ch+1);
  Serial.print(", {");
  for(int i=0; i<datalen; i++) {
    Serial.print(data[i]);
    if(i+1<datalen) Serial.print(",");
  }
  Serial.println("}");
}
