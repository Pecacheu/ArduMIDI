// (Code available on GitHub at 'http://github.com/Pecacheu/ArduMIDI/')
// ArduMIDI Libary HEADER FILE v2.1 by Pecacheu

#ifndef ArduMIDI_h
#define ArduMIDI_h

#include <HardwareSerial.h>
#include <Arduino.h>

//Receivable Message Types:
enum CommandType {
  NO_NEW_MSG       = 0x00, //No New Events Recieved
  INVALID          = 0x01, //Invalid or Badly Formatted Event Recieved!
  //Voice & Mode Messages:
  NOTE_OFF         = 0x80, //NoteOff Event
  NOTE_ON          = 0x90, //NoteOn Event
  KEY_PRESSURE     = 0xA0, //Key AfterTouch
  CTRL_CHANGE      = 0xB0, //Control Change
  CHANNEL_MODE     = 0x02, //Channel Mode (Uses Same Command ID as CTRL_CHANGE)
  PROG_CHANGE      = 0xC0, //Program Change
  CHANNEL_PRESSURE = 0xD0, //Channel AfterTouch
  PITCH_BEND       = 0xE0, //Pitch Bend Wheel Change
  //System Messages:
  SYSEX_START      = 0xF0, //Begin SysEx Dump
  Q_TIME_FRAME     = 0xF1, //Qarter Time Frame
  SONG_POS         = 0xF2, //Maybe Changes Song Position in Your DAW?
  SONG_SELECT      = 0xF3, //Does something or the other...
  TUNE_REQ         = 0xF6, //Tune Your Insturments!
  SYSEX_END        = 0xF7, //End SysEx Dump
  //Real-Time Messages:
  TIMING_CLOCK     = 0xF8, //Sent 24 Times per Quarter Note When SYNC is Enabled
  START            = 0xFA, //Start
  CONTINUE         = 0xFB, //Continue
  STOP             = 0xFC, //Stop
  ACTIVE_SENSE     = 0xFE, //Just don't use it. It's pointless.
  SOFT_RESET       = 0xFF, //Perform Soft Reset
};

//Possible Channels:
enum Channel {CH1, CH2, CH3, CH4, CH5, CH6, CH7, CH8, CH9, CH10, CH11, CH12, CH13, CH14, CH15, CH16, CH_ALL};

//Traffic Forwarding Modes:
enum ThruMode {
  FORWARD_OFF,   //Thru Disabled. Nothing received on the MIDI IN port is forwarded to the MIDI OUT port.
  FORWARD_OTHER, //Forwards received commands to MIDI OUT unless they're on the listening channel. This makes the most sense, and is the default setting.
  FORWARD_SELF,  //Forwards received commands to MIDI OUT only if they are on the listening channel.
  FORWARD_ALL,   //Any traffic received on MIDI IN is immediately forwarded to MIDI OUT. Warning, may cause a feedback loop in some setups!
};

//ArduMIDI Class Object:
class ArduMIDI {
  public:
    //---- Setup ----
    ArduMIDI(HardwareSerial& serialObject, Channel listenCh);
    void begin();
    void end();
    void setChannel(Channel);
    void setThruMode(ThruMode);
    //---- Extra ----
    char getEventLength(CommandType);
    String commandTypeToString(CommandType);
    //---- Send MIDI Messages ----
    boolean sendMidiEvent(CommandType, Channel, byte, byte);
    boolean sendMidiEventRaw(byte, byte, byte);
    //Voice Messages:
    void noteOn(Channel, byte, byte);
    void keyPressure(Channel, byte, byte);
    void noteOff(Channel, byte);
    void controlChange(Channel, byte, byte);
    void programChange(Channel, byte);
    void channelPressure(Channel, byte);
    void pitchBendChange(Channel, int16_t);
    //Mode Messages:
    void modeChange(Channel, byte, byte);
    //System Messages:
    void sendSysEx(byte[], unsigned int, byte[], unsigned int);
    void quarterTimeFrame(byte, byte);
    void songPosition(uint16_t);
    void songSelect(byte);
    void tuneRequest();
    //Real-Time Messages:
    void timingClock();
    void startPlay();
    void continuePlay();
    void stopPlay();
    void activeSense();
    void resetAll();
    //---- Recieve MIDI Messages ----
    //--- Trigger Callbacks:
    void scanForData();
    //--- Set Callbacks:
    //Voice Messages:
    void setHandlerAny(void (*handler)(CommandType, Channel, byte[], unsigned int)); //command, channel, rawEventData (0-127 [1+]), dataLength (1+)
    void setHandlerNoteOn(void (*handler)(Channel, byte, byte)); //channel, key (0-127), velocity (0-127)
    void setHandlerNoteOff(void (*handler)(Channel, byte, byte)); //channel, key (0-127), velocity (0-127)
    void setHandlerKeyPressure(void (*handler)(Channel, byte, byte)); //channel, key (0-127), afterTouchPressure (0-127)
    void setHandlerControlChange(void (*handler)(Channel, byte, byte)); //channel, controller (0-119), value (0-127)
    void setHandlerProgramChange(void (*handler)(Channel, byte)); //channel, program (0-127)
    void setHandlerChannelPressure(void (*handler)(Channel, byte)); //channel, afterTouchPressure (0-127)
    void setHandlerPitchBend(void (*handler)(Channel, int16_t)); //channel, wheelValue (-8192-8191)
    //Mode Messages:
    void setHandlerChannelMode(void (*handler)(Channel, byte, byte)); //channel, settingCode (120-127), value (0-127)
    //System Messages:
    void setHandlerSysEx(void (*handler)(byte[], unsigned int, byte[], unsigned int)); //idCode[] (0-127 [1-3]), idCodeLength (1-3), data[] (0-127 [1+]), dataLength (1+)
    void setHandlerQuarterFrame(void (*handler)(byte, byte)); //msgType (0-7), values (0-15)
    void setHandlerSongPosition(void (*handler)(uint16_t)); //position (0-16383)
    void setHandlerSongSelect(void (*handler)(byte)); //song (0-127)
    void setHandlerTuneRequest(void (*handler)()); //[No Parameters]
    //Real-Time Messages:
    void setHandlerTimingClock(void (*handler)()); //[No Parameters]
    void setHandlerStart(void (*handler)()); //[No Parameters]
    void setHandlerContinue(void (*handler)()); //[No Parameters]
    void setHandlerStop(void (*handler)()); //[No Parameters]
    void setHandlerActiveSense(void (*handler)()); //[No Parameters]
    void setHandlerReset(void (*handler)()); //[No Parameters]
    //--- Clear Callbacks:
    //Voice Messages:
    void clearHandlerAny();
    void clearHandlerNoteOn();
    void clearHandlerNoteOff();
    void clearHandlerKeyPressure();
    void clearHandlerControlChange();
    void clearHandlerProgramChange();
    void clearHandlerChannelPressure();
    void clearHandlerPitchBend();
    //Mode Messages:
    void clearHandlerChannelMode();
    //System Messages:
    void clearHandlerSysEx();
    void clearHandlerQuarterFrame();
    void clearHandlerSongPosition();
    void clearHandlerSongSelect();
    void clearHandlerTuneRequest();
    //Real-Time Messages:
    void clearHandlerTimingClock();
    void clearHandlerStart();
    void clearHandlerContinue();
    void clearHandlerStop();
    void clearHandlerActiveSense();
    void clearHandlerReset();
  private:
    //---- Internal Functions ----
    CommandType checkForMIDI();
    CommandType getEventCmdType(byte);
    char getEvtLength(byte);
    void forwardTraffic(byte);
    boolean allowHandlers();
    //---- Private Callback Variables ----
    //Voice Messages:
    void (*_handleAny)(CommandType, Channel, byte[], unsigned int);
    void (*_handleNoteOn)(Channel, byte, byte);
    void (*_handleNoteOff)(Channel, byte, byte);
    void (*_handleKeyPressure)(Channel, byte, byte);
    void (*_handleControlChange)(Channel, byte, byte);
    void (*_handleProgramChange)(Channel, byte);
    void (*_handleChannelPressure)(Channel, byte);
    void (*_handlePitchBend)(Channel, int16_t);
    //Mode Messages:
    void (*_handleChannelMode)(Channel, byte, byte);
    //System Messages:
    void (*_handleSysEx)(byte[], unsigned int, byte[], unsigned int);
    void (*_handleQuarterFrame)(byte, byte);
    void (*_handleSongPosition)(uint16_t);
    void (*_handleSongSelect)(byte);
    void (*_handleTuneRequest)();
    //Real-Time Messages:
    void (*_handleTimingClock)();
    void (*_handleStart)();
    void (*_handleContinue)();
    void (*_handleStop)();
    void (*_handleActiveSense)();
    void (*_handleReset)();
    //---- Main Variables ----
    HardwareSerial& _SerialObj;
    byte _listenCh;
    byte _thruMode;
    boolean _sysExMode = false;
    CommandType _previousCmdType;
    Channel _previousCmdChannel;
    unsigned int _dataLength = 0;
    byte _rawData[] = {};
};

#endif