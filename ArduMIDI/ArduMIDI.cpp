// (Code available on GitHub at 'http://github.com/Pecacheu/ArduMIDI/')
// ArduMIDI Libary C++ FILE v2.1 by Pecacheu

#include <HardwareSerial.h>
#include <Arduino.h>

#include "ArduMIDI.h"

//Main Library Functions:

ArduMIDI::ArduMIDI(HardwareSerial& serialObject, Channel listenCh) : _SerialObj(serialObject) { //Need to initialise references before body
  if(listenCh > CH_ALL) listenCh = CH_ALL;
  _listenCh = listenCh;
  if(_listenCh == CH_ALL) _thruMode = FORWARD_OFF;
  else _thruMode = FORWARD_OTHER;
  //Make Sure Handler Pointers Are Empty:
  _handleAny             = 0; _handleNoteOn        = 0;
  _handleNoteOff         = 0; _handleKeyPressure   = 0;
  _handleControlChange   = 0; _handleProgramChange = 0;
  _handleChannelPressure = 0; _handlePitchBend     = 0;
  _handleChannelMode     = 0; _handleSysEx         = 0;
  _handleQuarterFrame    = 0; _handleSongPosition  = 0;
  _handleSongSelect      = 0; _handleTuneRequest   = 0;
  _handleTimingClock     = 0; _handleStart         = 0;
  _handleContinue        = 0; _handleStop          = 0;
  _handleActiveSense     = 0; _handleReset         = 0;
}

void ArduMIDI::begin() {
  _SerialObj.begin(31250); //Baud-Rate of MIDI Protocol.
  //Give microcontroller some time to think:
  delay(100);
}

void ArduMIDI::end() {
  _SerialObj.end(); //End serial communication.
  //Give microcontroller some time to think:
  delay(100);
}

void ArduMIDI::setChannel(Channel listenCh) {
  if(listenCh > CH_ALL) listenCh = CH_ALL;
  _listenCh = listenCh;
}

void ArduMIDI::setThruMode(ThruMode mode) {
  if(mode > FORWARD_ALL) mode = FORWARD_ALL;
  _thruMode = mode;
}

//---------------------------------------- Send MIDI Messages ----------------------------------------

//Send any type of message, except SysEx:
//Parameters: cmd (CommandType), channel (Channel), arg1 (0-127), arg2 (0-127)
boolean ArduMIDI::sendMidiEvent(CommandType cmd, Channel channel, byte arg1, byte arg2) {
  //Error Checking:
  if(cmd < NOTE_OFF || cmd > SOFT_RESET) return false;
  if(cmd == CHANNEL_MODE) cmd = CTRL_CHANGE;
  else if(cmd == SYSEX_START || cmd == SYSEX_END) return false;
  if(arg1 > 127) arg1 = 127;
  if(arg1 > 127) arg1 = 127;
  //Write Data to MIDI Port:
  char len = getEvtLength(cmd);
  if(cmd > 0xF0) _SerialObj.write(cmd);
  else _SerialObj.write(cmd + channel);
  if(len > 0) _SerialObj.write(arg1);
  if(len > 1) _SerialObj.write(arg2);
  return true;
}

//Send any type of message, except SysEx:
//Parameters: cmd (CommandType), channel (Channel), arg1 (0-127), arg2 (0-127)
boolean ArduMIDI::sendMidiEventRaw(byte cmd, byte arg1, byte arg2) {
  //Error Checking:
  if(cmd < NOTE_OFF || cmd > SOFT_RESET + 0x0F) return false;
  if(cmd == SYSEX_START || cmd == SYSEX_END) return false;
  if(arg1 > 127) arg1 = 127;
  if(arg1 > 127) arg1 = 127;
  //Write Data to MIDI Port:
  char len = getEvtLength(cmd);
  _SerialObj.write(cmd);
  if(len > 0) _SerialObj.write(arg1);
  if(len > 1) _SerialObj.write(arg2);
  return true;
}

//Channel Voice Messages:

//Send when a note is depressed:
//Parameters: channel (Channel), key (0-127), velocity (0-127)
void ArduMIDI::noteOn(Channel channel, byte key, byte velocity) {
  //Error Checking:
  if(channel > CH16) channel = CH16;
  if(key > 127) key = 127;
  if(velocity > 127) velocity = 127;
  //Perform Command Calculation:
  uint8_t cmd = NOTE_ON + channel;
  //Write Data to MIDI Port:
  _SerialObj.write(cmd);
  _SerialObj.write(key);
  _SerialObj.write(velocity);
}

//Send if the velocity changes while holding down the note:
//Parameters: channel (Channel), key (0-127), pressure (0-127)
void ArduMIDI::keyPressure(Channel channel, byte key, byte pressure) {
  //Error Checking:
  if(channel > CH16) channel = CH16;
  if(key > 127) key = 127;
  if(pressure > 127) pressure = 127;
  //Perform Command Calculation:
  uint8_t cmd = KEY_PRESSURE + channel;
  //Write Data to MIDI Port:
  _SerialObj.write(cmd);
  _SerialObj.write(key);
  _SerialObj.write(pressure);
}

//Send when a note is released:
//Parameters: channel (Channel), key (0-127)
void ArduMIDI::noteOff(Channel channel, byte key) {
  //Error Checking:
  if(channel > CH16) channel = CH16;
  if(key > 127) key = 127;
  //Perform Command Calculation:
  uint8_t cmd = NOTE_OFF + channel;
  //Write Data to MIDI Port:
  _SerialObj.write(cmd);
  _SerialObj.write(key);
  _SerialObj.write(0x00);
}

//Send when the value of a control changes:
//(Controls include things like pedals and levers.)
//Parameters: channel (Channel), controlNum (0-119), value (0-127)
void ArduMIDI::controlChange(Channel channel, byte controlNum, byte value) {
  //Error Checking:
  if(channel > CH16) channel = CH16;
  if(controlNum > 119) controlNum = 119;
  if(value > 127) value = 127;
  //Perform Command Calculation:
  uint8_t cmd = CTRL_CHANGE + channel;
  //Write Data to MIDI Port:
  _SerialObj.write(cmd);
  _SerialObj.write(controlNum);
  _SerialObj.write(value);
}

//Send when... um... I don't really know, actually:
//Parameters: channel (Channel), value (0-127)
void ArduMIDI::programChange(Channel channel, byte program) {
  //Error Checking:
  if(channel > CH16) channel = CH16;
  if(program > 127) program = 127;
  //Perform Command Calculation:
  uint8_t cmd = PROG_CHANGE + channel;
  //Write Data to MIDI Port:
  _SerialObj.write(cmd);
  _SerialObj.write(program);
}

//Basically the same as keyPressure, but affects all keys at once:
//Parameters: channel (Channel), pressure (0-127)
void ArduMIDI::channelPressure(Channel channel, byte pressure) {
  //Error Checking:
  if(channel > CH16) channel = CH16;
  if(pressure > 127) pressure = 127;
  //Perform Command Calculation:
  uint8_t cmd = CHANNEL_PRESSURE + channel;
  //Write Data to MIDI Port:
  _SerialObj.write(cmd);
  _SerialObj.write(pressure);
}

//Send when the pitch bend wheel is moved:
//(The wheel value is a 14-bit-percision number. The center is 0.)
//Parameters: channel (Channel), value (-8192-8191)
void ArduMIDI::pitchBendChange(Channel channel, int16_t value) {
  //Error Checking:
  if(channel > CH16) channel = CH16;
  if(value > 8191) value = 8191;
  if(value < -8192) value = -8192;
  //Perform Command Calculation:
  uint8_t cmd = PITCH_BEND + channel;
  //Convert signed 14 bit to un-signed 14 bit:
  uint16_t uvalue = value + 8192;
  //This is a 14 bit number, and 8 is half of 16, so
  //we'll clear out the un-used 8th bit with a binary AND:
  uint8_t lessSigBits = uvalue & 0b01111111;
  //As for the most significant 7 bits, we'll just do some bit-shifting:
  uint8_t moreSigBits = uvalue >> 7;
  //Write Data to MIDI Port:
  _SerialObj.write(cmd);
  _SerialObj.write(lessSigBits);
  _SerialObj.write(moreSigBits);
}

//Channel Mode Messages:

//Some special settings (take a look at the MIDI spec):
//Parameters: channel (Channel), settingCode (120-127), value (0-127)
void ArduMIDI::modeChange(Channel channel, byte settingCode, byte value) {
  //Error Checking:
  if(channel > CH16) channel = CH16;
  if(settingCode > 127) settingCode = 127;
  if(settingCode < 120) settingCode = 120;
  if(value > 127) value = 127;
  //Perform Command Calculation:
  uint8_t cmd = CTRL_CHANGE + channel;
  //Write Data to MIDI Port:
  _SerialObj.write(cmd);
  _SerialObj.write(settingCode);
  _SerialObj.write(value);
}

//System Common Messages:

//Send SysEx strings:
//(SysEx messages can have as many lines as you want, and each line is a 7-bit (0-127) number.)
//Parameters: idCode (1-3 length table [0-127], mfg ID code), idCodeLength (1-3), data (1+ length table [0-127]), dataLength (1+)
void ArduMIDI::sendSysEx(byte idCode[], unsigned int idCodeLength, byte data[], unsigned int dataLength) {
  //Error Checking:
  if(idCodeLength > 3) idCodeLength = 3;
  if(idCodeLength == 0 || dataLength == 0) return;
  //Write Command to MIDI Port:
  _SerialObj.write(SYSEX_START);
  //Write ID Code to MIDI Port:
  for(int i=0; i<idCodeLength; i++) {
    if(idCode[i] > 127) _SerialObj.write(127);
    else _SerialObj.write(idCode[i]);
  }
  //Write SysEx Data to MIDI Port:
  for(int i=0; i<dataLength; i++) {
    if(data[i] > 127) _SerialObj.write(127);
    else _SerialObj.write(data[i]);
  }
  //Write End Command to MIDI Port:
  _SerialObj.write(SYSEX_END);
}

//Quarter time frame message: (No idea)
//Parameters: msgType (0-7), values (0-15)
void ArduMIDI::quarterTimeFrame(byte msgType, byte values) {
  //Error Checking:
  if(msgType > 7) msgType = 7;
  if(values > 15) values = 15;
  //Write Data to MIDI Port:
  _SerialObj.write(Q_TIME_FRAME);
  _SerialObj.write((msgType << 4) + values);
}

//Song Position Pointer: (Maybe Changes Song Position in Your DAW?)
//Parameters: position (0-16383)
void ArduMIDI::songPosition(uint16_t position) {
  //Error Checking:
  if(position > 16383) position = 16383;
  //Perform Data Calculation:
  uint8_t lessSigBits = position & 0b01111111;
  uint8_t moreSigBits = position >> 7;
  //Write Data to MIDI Port:
  _SerialObj.write(SONG_POS);
  _SerialObj.write(lessSigBits);
  _SerialObj.write(moreSigBits);
}

//Does something or the other:
//Parameters: position (0-127)
void ArduMIDI::songSelect(byte song) {
  //Error Checking:
  if(song > 127) song = 127;
  //Write Data to MIDI Port:
  _SerialObj.write(SONG_SELECT);
  _SerialObj.write(song);
}

//Forces Analog Synths to Re-Tune:
void ArduMIDI::tuneRequest() { _SerialObj.write(TUNE_REQ); }

//Real-Time Messages:

//Send 24 times per quarter note when SYNC is enabled:
void ArduMIDI::timingClock() { _SerialObj.write(TIMING_CLOCK); }

//Start, continue, and stop playing:
void ArduMIDI::startPlay() { _SerialObj.write(START); }
void ArduMIDI::continuePlay() { _SerialObj.write(CONTINUE); }
void ArduMIDI::stopPlay() { _SerialObj.write(STOP); }

//Just don't use it. It's pointless.
void ArduMIDI::activeSense() { _SerialObj.write(ACTIVE_SENSE); }

//Trigger software-reset of all connected devices:
void ArduMIDI::resetAll() { _SerialObj.write(SOFT_RESET); }




//TODO: Support high-resolution velocitiy prefix ( http://www.midi.org/techspecs/ca31.pdf ), and other such expansions to the MIDI spec...




//--------------------------------------- Recieve MIDI Messages --------------------------------------

//----- Add Event Handlers -----

//Voice Messages:
void ArduMIDI::setHandlerAny(void (*handler)(CommandType, Channel, byte[], unsigned int)) { _handleAny = handler; }
void ArduMIDI::setHandlerNoteOn(void (*handler)(Channel, byte, byte)) { _handleNoteOn = handler; }
void ArduMIDI::setHandlerNoteOff(void (*handler)(Channel, byte, byte)) { _handleNoteOff = handler; }
void ArduMIDI::setHandlerKeyPressure(void (*handler)(Channel, byte, byte)) { _handleKeyPressure = handler; }
void ArduMIDI::setHandlerControlChange(void (*handler)(Channel, byte, byte)) { _handleControlChange = handler; }
void ArduMIDI::setHandlerProgramChange(void (*handler)(Channel, byte)) { _handleProgramChange = handler; }
void ArduMIDI::setHandlerChannelPressure(void (*handler)(Channel, byte)) { _handleChannelPressure = handler; }
void ArduMIDI::setHandlerPitchBend(void (*handler)(Channel, int16_t)) { _handlePitchBend = handler; }
//Mode Messages:
void ArduMIDI::setHandlerChannelMode(void (*handler)(Channel, byte, byte)) { _handleChannelMode = handler; }
//System Messages:
void ArduMIDI::setHandlerSysEx(void (*handler)(byte[], unsigned int, byte[], unsigned int)) { _handleSysEx = handler; }
void ArduMIDI::setHandlerQuarterFrame(void (*handler)(byte, byte)) { _handleQuarterFrame = handler; }
void ArduMIDI::setHandlerSongPosition(void (*handler)(uint16_t)) { _handleSongPosition = handler; }
void ArduMIDI::setHandlerSongSelect(void (*handler)(byte)) { _handleSongSelect = handler; }
void ArduMIDI::setHandlerTuneRequest(void (*handler)()) { _handleTuneRequest = handler; }
//Real-Time Messages:
void ArduMIDI::setHandlerTimingClock(void (*handler)()) { _handleTimingClock = handler; }
void ArduMIDI::setHandlerStart(void (*handler)()) { _handleStart = handler; }
void ArduMIDI::setHandlerContinue(void (*handler)()) { _handleContinue = handler; }
void ArduMIDI::setHandlerStop(void (*handler)()) { _handleStop = handler; }
void ArduMIDI::setHandlerActiveSense(void (*handler)()) { _handleActiveSense = handler; }
void ArduMIDI::setHandlerReset(void (*handler)()) { _handleReset = handler; }

//----- Remove Event Handlers -----

//Voice Messages:
void ArduMIDI::clearHandlerAny() { if(_handleAny) _handleAny = 0; }
void ArduMIDI::clearHandlerNoteOn() { if(_handleNoteOn) _handleNoteOn = 0; }
void ArduMIDI::clearHandlerNoteOff() { if(_handleNoteOff) _handleNoteOff = 0; }
void ArduMIDI::clearHandlerKeyPressure() { if(_handleKeyPressure) _handleKeyPressure = 0; }
void ArduMIDI::clearHandlerControlChange() { if(_handleControlChange) _handleControlChange = 0; }
void ArduMIDI::clearHandlerProgramChange() { if(_handleProgramChange) _handleProgramChange = 0; }
void ArduMIDI::clearHandlerChannelPressure() { if(_handleChannelPressure) _handleChannelPressure = 0; }
void ArduMIDI::clearHandlerPitchBend() { if(_handlePitchBend) _handlePitchBend = 0; }
//Mode Messages:
void ArduMIDI::clearHandlerChannelMode() { if(_handleChannelMode) _handleChannelMode = 0; }
//System Messages:
void ArduMIDI::clearHandlerSysEx() { if(_handleSysEx) _handleSysEx = 0; }
void ArduMIDI::clearHandlerQuarterFrame() { if(_handleQuarterFrame) _handleQuarterFrame = 0; }
void ArduMIDI::clearHandlerSongPosition() { if(_handleSongPosition) _handleSongPosition = 0; }
void ArduMIDI::clearHandlerSongSelect() { if(_handleSongSelect) _handleSongSelect = 0; }
void ArduMIDI::clearHandlerTuneRequest() { if(_handleTuneRequest) _handleTuneRequest = 0; }
//Real-Time Messages:
void ArduMIDI::clearHandlerTimingClock() { if(_handleTimingClock) _handleTimingClock = 0; }
void ArduMIDI::clearHandlerStart() { if(_handleStart) _handleStart = 0; }
void ArduMIDI::clearHandlerContinue() { if(_handleContinue) _handleContinue = 0; }
void ArduMIDI::clearHandlerStop() { if(_handleStop) _handleStop = 0; }
void ArduMIDI::clearHandlerActiveSense() { if(_handleActiveSense) _handleActiveSense = 0; }
void ArduMIDI::clearHandlerReset() { if(_handleReset) _handleReset = 0; }

//----- Event Handler Firing Code -----

void ArduMIDI::scanForData() {
  CommandType event = checkForMIDI();
  //Trigger Events:
  if(event != NO_NEW_MSG && event != INVALID) {
    if(allowHandlers()) { //If on correct channel:
      if(_handleAny != 0) { _handleAny(event, _previousCmdChannel, _rawData, _dataLength); }
      //Voice Messages:
      if     (event == NOTE_ON && _handleNoteOn) _handleNoteOn(_previousCmdChannel, _rawData[0], _rawData[1]);
      else if(event == NOTE_OFF && _handleNoteOff) _handleNoteOff(_previousCmdChannel, _rawData[0], _rawData[1]);
      else if(event == KEY_PRESSURE && _handleKeyPressure) _handleKeyPressure(_previousCmdChannel, _rawData[0], _rawData[1]);
      else if(event == CTRL_CHANGE && _handleControlChange) _handleControlChange(_previousCmdChannel, _rawData[0], _rawData[1]);
      else if(event == PROG_CHANGE && _handleProgramChange) _handleProgramChange(_previousCmdChannel, _rawData[0]);
      else if(event == CHANNEL_PRESSURE && _handleChannelPressure) _handleChannelPressure(_previousCmdChannel, _rawData[0]);
      else if(event == PITCH_BEND && _handlePitchBend) _handlePitchBend(_previousCmdChannel, ((_rawData[1] << 7)+_rawData[0]-8192));
      //Mode Messages:
      else if(event == CHANNEL_MODE && _handleChannelMode) _handleChannelMode(_previousCmdChannel, _rawData[0], _rawData[1]);
      //System Messages:
      else if(event == SYSEX_START && _handleSysEx && _dataLength > 1) {
        //Determine ID Code Length:
        byte idTrim; byte idCode[3] = {}; if(_rawData[0] == 0 && _dataLength > 3) { idTrim = 3; } else { idTrim = 1; }
        //Strip ID Code off of SysEx Data:
        for(byte r=0; r<idTrim; r++) {
          idCode[r] = _rawData[0]; for(byte i=0; i<_dataLength-1; i++) { _rawData[i] = _rawData[i+1]; } _dataLength--; _rawData[_dataLength] = 0;
        }
        //Trigger Handler:
        _handleSysEx(idCode, idTrim, _rawData, _dataLength);
      }
      else if(event == Q_TIME_FRAME && _handleQuarterFrame) _handleQuarterFrame((_rawData[0] >> 4), (_rawData[0] & 0x0F));
      else if(event == SONG_POS && _handleSongPosition) _handleSongPosition((_rawData[1] << 7)+_rawData[0]);
      else if(event == SONG_SELECT && _handleSongSelect) _handleSongSelect(_rawData[0]);
      else if(event == TUNE_REQ && _handleTuneRequest) _handleTuneRequest();
      //Real-Time Messages:
      else if(event == TIMING_CLOCK && _handleTimingClock) _handleTimingClock();
      else if(event == START && _handleStart) _handleStart();
      else if(event == CONTINUE && _handleContinue) _handleContinue();
      else if(event == STOP && _handleStop) _handleStop();
      else if(event == ACTIVE_SENSE && _handleActiveSense) _handleActiveSense();
      else if(event == SOFT_RESET && _handleReset) _handleReset();
      //Give microcontroller some time to think:
      //delayMicroseconds(1);
    }
    //Reset Vars:
    if(_dataLength) { memset(_rawData, 0, _dataLength*2); _dataLength = 0; } _previousCmdType = NO_NEW_MSG;
  }
}

//------------------------------------------ Extra Functions -----------------------------------------

//Get the expected length, in bytes, of a particular type of event when completed:
//(Returns -1 if cmd is SysEx or INVALID)
char ArduMIDI::getEventLength(CommandType cmd) {
  if(cmd & 0xF0 == CHANNEL_MODE) cmd = CTRL_CHANGE;
  if(cmd >= NOTE_OFF && cmd <= SOFT_RESET) return getEvtLength(cmd);
  else return -1;
}

//Convert CommandType to string:
String ArduMIDI::commandTypeToString(CommandType cmd) {
  String cmdName;
  switch(cmd) {
    //Voice & Mode Messages:
    case NOTE_OFF: cmdName = "NOTE_OFF"; break;
    case NOTE_ON: cmdName = "NOTE_ON"; break;
    case KEY_PRESSURE: cmdName = "KEY_PRESSURE"; break;
    case CTRL_CHANGE: cmdName = "CTRL_CHANGE"; break;
    case CHANNEL_MODE: cmdName = "CHANNEL_MODE"; break;
    case PROG_CHANGE: cmdName = "PROG_CHANGE"; break;
    case CHANNEL_PRESSURE: cmdName = "CHANNEL_PRESSURE"; break;
    case PITCH_BEND: cmdName = "PITCH_BEND"; break;
    //System Messages:
    case SYSEX_START: cmdName = "SYSEX_START"; break;
    case Q_TIME_FRAME: cmdName = "Q_TIME_FRAME"; break;
    case SONG_POS: cmdName = "SONG_POS"; break;
    case SONG_SELECT: cmdName = "SONG_SELECT"; break;
    case TUNE_REQ: cmdName = "TUNE_REQ"; break;
    case SYSEX_END: cmdName = "SYSEX_END"; break; //<-- FOR DEBUG
    //Real-Time Messages:
    case TIMING_CLOCK: cmdName = "TIMING_CLOCK"; break;
    case START: cmdName = "START"; break;
    case CONTINUE: cmdName = "CONTINUE"; break;
    case STOP: cmdName = "STOP"; break;
    case ACTIVE_SENSE: cmdName = "ACTIVE_SENSE"; break;
    case SOFT_RESET: cmdName = "SOFT_RESET"; break;
    //Other:
    case NO_NEW_MSG: cmdName = "NO_NEW_MSG"; break; //<-- FOR DEBUG
    default: cmdName = "INVALID";
  }
  return cmdName;
}

//----------------------------------------- Private Functions ----------------------------------------

//This special function triggers automatically whenever serial data is recieved.
//Unfortunatly, there's no way to tell which one to use for which serial object.
//They also don't support the popular Arduino Leonardo board at the moment.
/*void serialEvent() {
 checkForMIDI();
 }*/

//Central function to check for and process MIDI data:
CommandType ArduMIDI::checkForMIDI() {
  //Read Available MIDI Data:
  while(_SerialObj.available() > 0) {
    //Read Next Byte:
    byte newByte = _SerialObj.read();
    //Forward traffic in Thru-Mode:
    if(_thruMode == FORWARD_ALL) _SerialObj.write(newByte);
    //Process New Byte:
    if(_sysExMode) { //----------------------------------- SysEx-Dump-Mode is Enabled:
      //Forward traffic in Thru-Mode:
      forwardTraffic(newByte);
      //If new byte is a command byte:
      if(newByte > 127) {
        //Determine command type:
        _previousCmdType = getEventCmdType(newByte);
        //Check if dump is over:
        if(_previousCmdType == SYSEX_END) { _sysExMode = false; return SYSEX_START; }
        else if(_previousCmdType >= TIMING_CLOCK) { //Exception for real-time messages:
          return _previousCmdType; //After return is processed, SysEx mode is still enabled, and dump can continue.
        } else { //Error out for any other command:
          _sysExMode = false; if(_dataLength) { memset(_rawData, 0, _dataLength*2); _dataLength = 0; }
          _previousCmdType = NO_NEW_MSG; /*Serial.println("Error: Command not allowed during SysEx dump.");*/ return INVALID; //Error: Command not allowed during SysEx dump.
        }
      }
      //Otherwise, add new byte to data table:
      _rawData[_dataLength++] = newByte;
    } else if(newByte >= 0x80 && newByte < 0xF0) { //----- Command Bytes with Channels:
      //Erase any previous command data:
      if(_previousCmdType || _dataLength > 0) { if(_dataLength) { memset(_rawData, 0, _dataLength*2); _dataLength = 0; } /*Serial.println("Warn: Data had to be forcibly cleard.");*/ } //<-- Maybe Should return INVALID?
      //Store Event Type & Channel:
      _previousCmdType = getEventCmdType(newByte);
      if(_previousCmdType == INVALID) { _previousCmdType = NO_NEW_MSG; /*Serial.println("Error: Function 'getEventCmdType' returned INVALID.");*/ return INVALID; } //Error: Function 'getEventCmdType' returned INVALID.
      _previousCmdChannel = (Channel)(newByte & 0x0F);
      //Forward traffic in Thru-Mode:
      forwardTraffic(newByte);
    } else if(newByte >= 0xF0) { //----------------------- Command Bytes without Channels:
      //Erase any previous command data:
      if(_previousCmdType || _dataLength > 0) { if(_dataLength) { memset(_rawData, 0, _dataLength*2); _dataLength = 0; } /*Serial.println("Warn: Data had to be forcibly cleard.");*/ } //<-- Maybe Should return INVALID?
      //Store Event Type:
      _previousCmdType = getEventCmdType(newByte);
      if(_previousCmdType == INVALID) { _previousCmdType = NO_NEW_MSG; /*Serial.println("Error: Function 'getEventCmdType' returned INVALID.");*/ return INVALID; } //Error: Function 'getEventCmdType' returned INVALID.
      else if(_previousCmdType == SYSEX_END) { _previousCmdType = NO_NEW_MSG; /*Serial.println("Error: SysEx end byte sent with no SysEx dump in progress.");*/ return INVALID; } //Error: SysEx end byte sent with no SysEx dump in progress.
      else if(_previousCmdType == SYSEX_START) _sysExMode = true;
      _previousCmdChannel = CH_ALL;
      //Forward traffic in Thru-Mode:
      forwardTraffic(newByte);
    } else if(newByte <= 127) { //------------------------ Data Bytes:
      //Check for previous command:
      if(!_previousCmdType) { /*Serial.println("Error: No command sent before data.");*/ return INVALID; } //Error: No command sent before data.
      //Forward traffic in Thru-Mode:
      forwardTraffic(newByte);
      //Add new byte to data table:
      _rawData[_dataLength++] = newByte;
      //Determine if event is finished:
      if(_dataLength == getEvtLength(_previousCmdType)) {
        //Decide if CTRL_CHANGE event is actually a CHANNEL_MODE event:
        if(_previousCmdType == CTRL_CHANGE && _rawData[0] >= 120) _previousCmdType = CHANNEL_MODE;
        //Return Event CommandType:
        return _previousCmdType;
      } else if(_dataLength > getEvtLength(_previousCmdType)) {
        if(_dataLength) { memset(_rawData, 0, _dataLength*2); _dataLength = 0; }
        _previousCmdType = NO_NEW_MSG; /*Serial.println("Error: Event was too many bytes long!");*/ return INVALID; //Error: Event was too many bytes long!
      }
    } else { _previousCmdType = NO_NEW_MSG; if(_dataLength) { memset(_rawData, 0, _dataLength*2); _dataLength = 0; } /*Serial.println("Error: Reserved ID Space.");*/ return INVALID; } //Error: Reserved ID Space.
  }
  return NO_NEW_MSG;
}

//Get CommandType from cmd number.
CommandType ArduMIDI::getEventCmdType(byte fullCmd) {
  byte cmd = (fullCmd >> 4);
  if(cmd >= 0b1000 && cmd < 0b1111) { //Voice or Mode Messages:
    return CommandType(fullCmd & 0xF0);
  } else if(cmd == 0b1111 && fullCmd != 0b11110100 && fullCmd != 0b11110101 && fullCmd != 0b11111001 && fullCmd != 0b11111101) { //Channel-less Messages:
    return CommandType(fullCmd);
  }
  return INVALID; //Error: Reserved or Unknown Command.
}

//Get total byte length of event when completed.
//Returns -1 if cmd is SysEx start or end byte.
char ArduMIDI::getEvtLength(byte cmd) {
  if(cmd == 0b11110000 || cmd == 0b11110111) return -1;
  else if(cmd >= 0b11110100) return 0;
  else if((cmd >> 4) == 0b1100 || (cmd >> 4) == 0b1101 || cmd == 0b11110001 || cmd == 0b11110011) return 1;
  else return 2;
}

//Determine how to forward traffic.
void ArduMIDI::forwardTraffic(byte newByte) {
  if(_thruMode == FORWARD_OTHER) { //FORWARD_OTHER:
    if(_listenCh == CH_ALL) { //If listen to all channels:
      if(_previousCmdChannel == CH_ALL) { //If no channel:
        _SerialObj.write(newByte);
      }
    } else { //If listen to one channel:
      if(_previousCmdChannel != _listenCh || _previousCmdChannel == CH_ALL) { //If on different channel or no channel:
        _SerialObj.write(newByte);
      }
    }
  } else if(_thruMode == FORWARD_SELF) { //FORWARD_SELF:
    if(_listenCh == CH_ALL) { //If listen to all channels:
      if(_previousCmdChannel != CH_ALL) { //If has channel:
        _SerialObj.write(newByte);
      }
    } else { //If listen to one channel:
      if(_previousCmdChannel == _listenCh || _previousCmdChannel == CH_ALL) { //If on same channel or no channel:
        _SerialObj.write(newByte);
      }
    }
  }
}

//Check if handlers should be allowed to run.
boolean ArduMIDI::allowHandlers() {
  if(_listenCh == CH_ALL) return true; //<-- If listen to all channels. vvv If listen to one channel.
  else if(_previousCmdChannel == _listenCh || _previousCmdChannel == CH_ALL) return true; //If on same channel or no channel.
  else return false;
}