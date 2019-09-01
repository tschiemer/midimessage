# midimessage

https://github.com/tschiemer/midimessage

Very basic constants, un-/packers, parser, cli and unified human-readable formatters/parser for MIDI Messages.

Intended for usage with frameworks (such as https://github.com/thestk/rtmidi) that do not provide these basics.

No particular structures are enforced such that you can take from it what you want.

## Features

- (many) MIDI Message constants + enumerations (see `include/midimessage.h`)
- *structs* and *types* help to create a unified interface and make handling easier
- Manufacturer Id list (see `include/midimessage/manufacturerids.h`)
 - tool to generate up-to-date header, run `make manufacturerids` (requires wget + php)
- Standard/Common CC list (see `include/midimessage/commonccs.h`)
- DLS Proprietary Chunk ids (see `include/midimessage/dlsproprietarychunkids.h`)
- Basic note helpers (see `include/midimessage/notes.h`)
- *packers* construct the right byte sequence for a particular message ASSERTing valid parameters (see `include/midimessage/packers.h`)
- *unpackers* for a specific message type try to parse the given byte sequence thereby validating the byte sequence (could be used in any combination) (see `include/midimessage/packers.h`)
- packers and unpackers are always complementary and are available as literal-based and struct-based variants
- generic MIDI stream *parser* respecting Running Status and interleaved system real time messages (see `include/midimessage/parser.h`)
- *stringifier* struct to turn (binary) MIDI messages into a uniform human-readable format and vice versa (see `include/midimessage/stringifier.h`; see `src/cli.cpp` for application)
- *Command line utility* to turn human-readable commands into corresponding byte sequence and vice versa (see `src/cli.cpp` and below)


tt = to test

| Feature Set |  Feature / Message | Message Family | Status* |
|-----|-----|----|:-----:|
| Channel Voice Message | Note On / Off | |  tt |
| | Polyphonic Key Pressure (Aftertouch) | | tt |
| | Control Change | | tt |
| | Program Change | | tt |
| | Channel Pressure (Aftertouch) | | tt |
| | Pitch Bend Change | | tt |
| Channel Mode Messages | Specific Control Change (All Sound Off, Reset All Controllers, etc) | Control Change | tt |
| System Common Messages | Song position pointer | | tt |
| | Song select | | tt |
| | Tune request | | tt |
| | Quarter Frames -> MIDI Time Code | | |
| System Real Time Messages | Timing clock | | tt |
| | Start | | tt |
| | Continue | | tt |
| | Stop | | tt |
| | Active Sensing | | tt |
| | Reset | | tt |
| MIDI Time Code | Quarter Frames | System Common | tt |
| | Full Message + User Bits| SysEx Real Time | tt |
| | Real Time MTC Cueing | SysEx Real Time | tt |
| | Non Real Time MTC Cueing | SysEx Non-Real Time | tt |
| MIDI Machine Control | Basics | | tt |
| MIDI Show Control | | SysEx Real Time | tt |
| MIDI Tuning Standard | | | TODO |
| General Handshaking | wait, cancel, ack, nak, end of file | SysEx Non-Real Time | tt |
| Experimental messages |  | SysEx Experimental | tt |
| Manufacturer messages (+ manufacturer ids)|  | SysEx Manufacturer | tt |
| Device Control (Master volume, balance, coarse/fine tuning, global parameters)| | SysEx Real Time | tt |
| General Information | SysEx Non-Real Time | tt |
| General MIDI System Messages | SysEx Non-Real Time | tt |
| Sample Dump | | SysEx Non-Real Time | TODO |
| File Dump | Non-Real Time | TODO |
| Downloadable Sounds | Non-Real Time | TODO |
| Notation Information | Real Time | TODO |

## Docs

https://tschiemer.github.com/tschiemer/midimessage


## Requirements

- `midimessage-cli` requires getopt and [c-utils](https://github.com/tschiemer/c-utils)
- target `manufacturerids` requires wget and php(-cli)

## Building

```bash
git clone --recursive https://github.com/tschiemer/midimessage.git
cd midimessage

cmake .
make

make examples
make docs
make manufacturerids
```



## Code Example

```cpp

void receivedMidiMessageFromSomewhere( uint8_t * buf, int len ){

    MidiMessage::Message_t msg, msg2;
    uint8_t msgBuf[128], msgBuf2[128];

    msg.Data.SysEx.ByteData = msgBuf;
    msg2.Data.SysEx.ByteData = msgBuf2;

    uint8_t buf2[256];
    int len2;

    static uint8_t pitch = 0;


    if (!MidiMessage::unpack( buf, len, &msg )) {
        std::cout << "Discarding invalid message" << std::endl;
        return;
    }

    if (!MidiMessage::isChannelVoiceMessage(&msg)){

        std::cout << "Discarding non-ChannelVoice message" << std::endl;

        return;
    }

    // Struct based approach to create messages
    // (the final message packaging is unified)
    msg2.StatusClass = MidiMessage::StatusClassNoteOn;
    msg2.Channel = msg.Channel;
    msg2.Data.Note.Key = 40 + pitch;
    msg2.Data.Note.Velocity = 100;

    pitch = (pitch + 1) % 13;

    len2 = MidiMessage::pack( buf2, &msg2 );
    if (len2 > 0){
        sendMidiMessageToSomewhere( buf2, len2 );
    }

    // direct approach
    len2 = MidiMessage::packNoteOff( buf2, msg2.Channel, msg2.Data.Note.Key, 50 );
    if (len2 > 0){
        sendMidiMessageToSomewhere( buf2, len2 );
    }
}

```

## Command Line Utility

```
Usage: midimessage-cli [-h?] [--running-status|-r] [--timed|-t[milli|micro]] (--parse|-p [<binary-data>]] | --generate|-g [--prefix=<prefix>] [--suffix=<suffix] [<cmd> ...])

Options:
	 -h|-? 				 show this help
	 --running-status|-r 		 Accept (when parsing) or generate messages that rely on the running status (see MIDI specs)
	 --timed|-t[milli|micro] 	 Enables the capture or playback of delta-time information (ie the time between messages). Optionally the time resolution (milliseconds or microseconds) can be specified (default = micro)
	 --parse|-p [<binary-data>] 	 Enter parse mode and optionally pass as first argument (binary) message to be parsed. If no argument is provided starts reading binary stream from STDIN. Each successfully parsed message will be printed to STDOUT and terminated with a newline.
	 --generate|-g [<cmd> ...] 	 Enter generation mode and optionally pass command to be generated. If no command is given, expects one command from STDIN per line. Generated (binary) messages are written to STDOUT.
	 --prefix=<prefix> 		 Prefixes given string (max 32 bytes) before each binary sequence (only when in generation mode). A single %d can be given which will be replaced with the length of the following binary message (incompatible with running-status mode).
	 --suffix=<suffix> 		 Suffixes given string (max 32 bytes) before each binary sequence (only when in generation mode).

Note: Data bytes have a value range of 0-127 - anything above is considered a control byte. There is no input validation!!
Fancy pants note: the parsing output format is identical to the generation command format ;)

Data types:
	 uN := N bit unsigned integer)
		 u4 (data nibble) < 63 (0x0FF)
		 u7 <= 127 (0x7F)
		 u14 <= 16383 (0x3FFF)
		 u21 <= 2097151 (0x1FFFFF)
		 u28 <= 268435455 (0x0FFFFFFF)
		 u35 <= 34359738367 (0x7FFFFFFFF)
	 sN := N bit signed integer
	 strN ((max) N byte ascii string)
	 xN (N byte hex string <> 2Ns) (note: data bytes must be <= 0x7F)

Voice Commands:
	 note (on|off) <channel> <key> <velocity>
	 cc <channel> <controller> <value>
	 pc <channel> <program>
	 pressure <channel> <pressure>
	 pitch <channel> <pitch>
	 poly <channel> <key> <pressure>

System Commands:
	 start
	 stop
	 continue
	 reset
	 active-sensing
	 tune-request
	 timing-clock
	 quarter-frame <messageType> <nibble>
	 song-position <position>
	 song-select <songNumber>

(General) System Exclusives*:
	 sysex experimental <data (xN)>
	 sysex manufacturer <manufacturer-id (x1..3)> <data (xN)>
	 sysex nonrt <device-id (u7)> (eof|wait|cancel|nak|ack) <packet-number>
	 sysex nonrt <device-id (u7)> info request
	 sysex nonrt <device-id (u7)> info reply <manufacturer-id (x1, x3)> <device-family (u14)> <device-family-member (u14)> <software-revision (x4)>
	 sysex nonrt <device-id (u7)> gm (system-on1|system-off|system-on2)
* <device-id> := 127 is all devices

 MIDI Time Code + Cueing
	 sysex rt <device-id (u7)> mtc full-message <fps = 24,25,29.97,30> <hour <= 23> <minute <= 59> <second <= 59> <frame < fps>
	 sysex rt <device-id (u7)> mtc user-bits <5bytes (x5)>
	 sysex nonrt <device-id (u7)> cueing special (time-code-offset|enable-event-list|disable-event-list|clear-event-list|system-stop|event-list-request|<(u14)>)
	 sysex nonrt <device-id (u7)> cueing (punch-in|punch-out) (add|rm) <fps = 24,25,29.97,30> <hour <= 23> <minute <= 59> <second <= 59> <frame < fps> <fractional-frame <= 99> <event-number (u14)>
	 sysex nonrt <device-id (u7)> cueing (event-start|event-stop|cue-point) (add|rm) <fps = 24,25,29.97,30> <hour <= 23> <minute <= 59> <second <= 59> <frame < fps> <fractional-frame <= 99> <event-number (u14)> [<event-name (str) ..>]
	 sysex nonrt <device-id (u7)> cueing event-name - <fps = 24,25,29.97,30> <hour <= 23> <minute <= 59> <second <= 59> <frame < fps> <fractional-frame <= 99> <event-number (u14)> <event-name (str) ..>
	 sysex rt <device-id (u7)> cueing special (system-stop|<(u14)>)
	 sysex rt <device-id (u7)> cueing (punch-in|punch-out) <event-number (u14)>
	 sysex rt <device-id (u7)> cueing (event-start|event-stop|cue-point) <event-number (u14)> [<event-name (str) ..>]
	 sysex rt <device-id (u7)> cueing event-name <event-number (u14)> <event-name (str) ..>
** <cue-number>, <cue-list>, <cue-path> := ascii numbers (0-9) and/or dots (.)

TODO:

MIDI Show Control
	 sysex rt <device-id (u7)> msc <cmdFmt (u7)> (all-off|restore|reset)
	 sysex rt <device-id (u7)> msc <cmdFmt (u7)> (go|stop|resume|load|go-off|go-jam-lock) <cue-number**> [<cue-list**> [<cue-path**>]]
	 sysex rt <device-id (u7)> msc <cmdFmt (u7)> timed-go <fps = 24,25,29.97,30> <hour <= 23> <minute <= 59> <second <= 59> <frame < fps> < (x1)> <cue-number**> [<cue-list**> [<cue-path**>]]
	 sysex rt <device-id (u7)> msc <cmdFmt (u7)> set <controller (u14)> <value (u14)> <fps = 24,25,29.97,30> <hour <= 23> <minute <= 59> <second <= 59> <frame < fps> < (x1)>
	 sysex rt <device-id (u7)> msc <cmdFmt (u7)> fire <macro-number (u7)>
	 sysex rt <device-id (u7)> msc <cmdFmt (u7)> (standby+|standby-|sequence+|sequence-|start-clock|stop-clock|zero-clock|mtc-chase-on|mtc-chase-off|open-cue-list|close-cue-list) <cue-list**>
	 sysex rt <device-id (u7)> msc <cmdFmt (u7)> (open-cue-path|close-cue-path) <cue-path**>
	 sysex rt <device-id (u7)> msc <cmdFmt (u7)> set-clock <fps = 24,25,29.97,30> <hour <= 23> <minute <= 59> <second <= 59> <frame < fps> < (x1)> <cue-list**>
	 sysex rt <device-id (u7)> msc <cmdFmt (u7)> (standby|go-2-pc) <checksum (u14)> <sequence-number (u14)> <data (x4)> <cue-number**> [<cue-list**> [<cue-path**>]]
	 sysex rt <device-id (u7)> msc <cmdFmt (u7)> standing-by <checksum (u14)> <sequence-number (u14)> <fps = 24,25,29.97,30> <hour <= 23> <minute <= 59> <second <= 59> <frame < fps> < (x1)> <cue-number**> [<cue-list**> [<cue-path**>]]
	 sysex rt <device-id (u7)> msc <cmdFmt (u7)> (cancelled|abort) <checksum (u14)> <status (u16)> <sequence-number (u14)> <data (x4)> <cue-number**> [<cue-list**> [<cue-path**>]]

MIDI Machine Commands (MMC)
For MMC the MIDI format acts as container for a command stream of its own, where several MMC commands can be packed into one MIDI message.

	 sysex rt <device-id (u7) mmc cmd <command1 ..> [<command2 ..> [ .. <commandN ..>] .. ]]
		 <commandN ..> :
		 (stop|play|deferred-play|fast-forward|rewind|record-strobe|record-exit|record-pause|pause|eject|chase|cmd-error-reset|mmc-reset|wait|resume)
		 (variable-play|search|shuttle|deferred-variable-play|record-strobe-variable) <speed (float)>
		 step <step (s7)>
		 write ..
		 locate <fps = 24,25,29.97,30> <hour <= 23> <minute <= 59> <second <= 59> <frame < fps>)
		 move ..
		 <data (xN)>

Examples:
	 bin/midimessage-cli -g note on 60 40 1
	 bin/midimessage-cli -g | bin/midimessage-cli -p
	 bin/midimessage-cli -g --prefix='%d ' --suffix=$'\x0a'
	 bin/midimessage-cli -g | bin/midimessage-cli -ptmilli > test.recording
	 cat test.recording | bin/midimessage-cli -gtmilli | bin/midimessage-cli -p
```

## References

- https://www.midi.org/specifications-old/item/the-midi-1-0-specification
- https://www.midi.org/specifications-old/category/reference-tables
- http://www.personal.kent.edu/~sbirch/Music_Production/MP-II/MIDI/an_introduction_to_midi_contents.htm
- http://www.somascape.org/midi/tech/spec.html


## License MIT

Also see file `LICENSE`
