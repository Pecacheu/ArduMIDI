//ArduMIDI "Save" Example, by Pecacheu.

//Saves recieved MIDI data and plays it back when you send any character over USB serial.

//Note: This sketch requires an Arduino with multiple serial ports, such as an Arduino MEGA.
//Note 2: Results not guarenteed. Played-back music will sound terrible. I can't seem to figure out why.

#include <ArduMIDI.h>

//Maximum number of MIDI commands:
//(More commands = More space. Even an Arduino MEGA can only handle ~1024 commands.)
#define MAXCOMMANDS 512

//Create new instance of ArduMIDI library:
ArduMIDI midi = ArduMIDI(Serial1, CH_ALL);

//Setup some tables to store the data:
unsigned long prevMillis = 0;
uint16_t totalCmds = 0;
byte cmdTable[MAXCOMMANDS] = {};
byte dataTable[MAXCOMMANDS][2] = {};
uint32_t delayTable[MAXCOMMANDS] = {};

void setup() {
  pinMode(13, OUTPUT); digitalWrite(13, LOW);
  //Begin MIDI library:
  midi.begin();
  //No need for fast USB Serial here:
  Serial.begin(9600);
  //Do not forward any MIDI data, so we can use more CPU power for recording:
  midi.setThruMode(FORWARD_OFF);
  //Setup the recording function below:
  midi.setHandlerAny(storeEvent);
}

void loop() {
  //Check for new MIDI data:
  midi.scanForData();
  
  //Check for new USB Serial data:
  if(Serial.available()) {
    while(Serial.available()) Serial.read();
    Serial.println();
    Serial.print("Playing "); Serial.print(totalCmds); Serial.println(" Lines of Recorded data...");
    //Resend saved MIDI data:
    for(int i=0; i<totalCmds; i++) {
      //Using sendMidiEventRaw:
      //This is the simplest way to send MIDI data stored in a table...
      midi.sendMidiEventRaw(cmdTable[i], dataTable[i][0], dataTable[i][1]);
      
      //Using sendMidiEvent:
      //if(cmdTable[i] >= 0xF0) midi.sendMidiEvent(CommandType(cmdTable[i] & 0xF0), CH1, dataTable[i][0], dataTable[i][1]);
      //else midi.sendMidiEvent(CommandType(cmdTable[i] & 0xF0), Channel(cmdTable[i] & 0x0F), dataTable[i][0], dataTable[i][1]);
      
      //Using Serial.write:
      //Serial1.write(cmdTable[i]);
      //if(midi.getEventLength((CommandType)cmdTable[i]) >= 1) Serial1.write(dataTable[i][0]);
      //if(midi.getEventLength((CommandType)cmdTable[i]) >= 2) Serial1.write(dataTable[i][1]);
      if(delayTable[i]) delay(delayTable[i] / 100);
      //else Serial1.flush();
    }
    if(totalCmds > 0) Serial.println("Done!");
    else Serial.println("You have no recorded data.");
    //Wait for output buffer to clear before returning to regular code:
    Serial.flush();
  }
}

void storeEvent(CommandType cmd, Channel ch, byte data[], unsigned int datalen) {
  //Store MIDI event data:
  if(cmd != SYSEX_START && totalCmds < MAXCOMMANDS) {
    if(totalCmds) delayTable[totalCmds] = (micros() - prevMillis) / 10;
    else delayTable[0] = 0;
    if(ch == CH_ALL) cmdTable[totalCmds] = cmd;
    else cmdTable[totalCmds] = cmd + ch;
    if(datalen >= 1) dataTable[totalCmds][0] = data[0];
    if(datalen >= 2) dataTable[totalCmds][1] = data[1];
    totalCmds++; prevMillis = micros();
  }
}
