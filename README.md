# midimessage

https://github.com/tschiemer/midimessage

Extensive library for MIDI constants, un-/packers, parser, cli and unified human-readable formatters/parser for MIDI Messages.

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
| MIDI Time Code (MTC) | Quarter Frames | System Common | tt |
| | Full Message + User Bits| SysEx Real Time | tt |
| | Real Time MTC Cueing | SysEx Real Time | tt |
| | Non Real Time MTC Cueing | SysEx Non-Real Time | tt |
| General SysEx NonRT Handshaking  | wait, cancel, ack, nak, end of file | SysEx Non-Real Time | tt |
| MIDI Machine Control (MMC) |  | | work in progress |
| MIDI Show Control (MSC) | All | SysEx Real Time | tt |
| MIDI Visual Control (MVC)| Set Parameter Message (all) | SysEx Non-Real Time | tt |
| MIDI Tuning Standard | | | TODO |
| Experimental messages |  | SysEx Experimental | tt |
| Manufacturer messages (+ manufacturer ids)|  | SysEx Manufacturer | tt |
| General MIDI (GM) | System on/off | SysEx Non-Real Time | tt |
| General Information | Identify request/reply | SysEx Non-Real Time | tt |
| Device Control | Master volume, balance, coarse/fine tuning, global parameters | SysEx Real Time | tt |
| Controller Destination Settings | Channel/Key pressure, control change | SysEx Real Time | tt |
| Key-based Instrument Control | | SysEx Real Time | tt |
| Sample Dump | (Extended) Header, Request, DataPacket, (Extended) Loop Point Request & Transmission, Name Request & Transmission  | SysEx Non-Real Time | tt |
| File Dump | Request, Header, DataPacket | Non-Real Time | tt |
| Downloadable Sounds | | Non-Real Time | TODO |
| Notation Information | Bar Number, Time Signature (delayed, immediate) | Real Time | tt |
| Capability Inquiry (MIDI-CI) | | SysEx Non-Real Time | TODO |
| Scalable Polyphony MIDI MIP (SP-MIDI MIP)| | SysEx Real Time | TODO |
| Mobile Phone Control | on, off, reset, set level, set color, follow MIDI channels, manufacturer specific | SysEx Real Time | tt |

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
Usage:
	 midimessage-cli [-h?]
	 midimessage-cli [--running-status|-r] [--timed|-t[milli|micro]] (--parse|-p [-d]]
	 midimessage-cli [--running-status|-r] [--timed|-t[milli|micro]] --generate|-g [-x0|-x1] [-v[N]] [--prefix=<prefix>] [--suffix=<suffix] [<cmd> ...])
	 midimessage-cli --convert=(nibblize|denibblize|sevenbitize|desevenbitize)

Options:
	 -h|-? 				 show this help
	 --running-status|-r 		 Accept (when parsing) or generate messages that rely on the running status (see MIDI specs)
	 --timed|-t[milli|micro] 	 Enables the capture or playback of delta-time information (ie the time between messages). Optionally the time resolution (milliseconds or microseconds) can be specified (default = micro)
	 --parse|-p [<binary-data>] 	 Enter parse mode and optionally pass as first argument (binary) message to be parsed. If no argument is provided starts reading binary stream from STDIN. Each successfully parsed message will be printed to STDOUT and terminated with a newline.
	 -d 				 In parsing mode, instead of silent discarding output any discarded data to STDERR.
	 --generate|-g [<cmd> ...] 	 Enter generation mode and optionally pass command to be generated. If no command is given, expects one command from STDIN per line. Generated (binary) messages are written to STDOUT.
	 --prefix=<prefix> 		 Prefixes given string (max 32 bytes) before each binary sequence (only when in generation mode). A single %d can be given which will be replaced with the length of the following binary message (incompatible with running-status mode).
	 --suffix=<suffix> 		 Suffixes given string (max 32 bytes) before each binary sequence (only when in generation mode).
	 -x0, -x1 			 In generation mode, exit on input error (-x1) or continue processing (-x0). Default := continue (-x0).
	 -v0, -v1 			 In generation mode, print command parsing result (on error only) to STDERR. Default := do NOT print (-v0).
	 --convert=.. 			 Enter convertion mode, ie transform incoming STDIN using convertion method and write to STDOUT (raw bytes).

Fancy pants note: the parsing output format is identical to the generation command format ;)

Data types:
	 uN := N bit unsigned integer)
		 u4 (data nibble) < 15 (0x0F)
		 u7 <= 127 (0x7F)
		 u14 <= 16383 (0x3FFF)
		 u21 <= 2097151 (0x1FFFFF)
		 u28 <= 268435455 (0x0FFFFFFF)
		 u35 <= 34359738367 (0x7FFFFFFFF)
	 sN := N bit signed integer
	 strN ((max) N byte ascii string)
	 xN (N byte hex string <> 2Ns) (note: data bytes must be <= 0x7F)

Voice Commands:
	 note (on|off) <channel (u4)> <key (u7)> <velocity (u7)>
	 cc <channel (u4)> <controller (u7)> <value (u7)>
	 pc <channel (u4)> <program (u7)>
	 pressure <channel (u4)> <pressure (u7)>
	 pitch <channel (u4)> <pitch (u14)>
	 poly <channel (u4)> <key (u7)> <pressure (u7)>

System Commands:
	 start
	 stop
	 continue
	 reset
	 active-sensing
	 tune-request
	 timing-clock
	 quarter-frame <messageType (u3)> <nibble (u4)>
	 song-position <position (u14)>
	 song-select <songNumber (u7)>

(Basic) System Exclusives:
	 sysex experimental <data (xN)>
	 sysex manufacturer <manufacturer-id (x1..3)> <data (xN)>
	 sysex nonrt <device-id* (u7)> (eof|wait|cancel|nak|ack) <packet-number (u7)>
* <device-id> := 127 is all devices (broadcast)

General Information
	 sysex nonrt <device-id (u7)> info request
	 sysex nonrt <device-id (u7)> info reply <manufacturer-id (x1, x3)> <device-family (u14)> <device-family-member (u14)> <software-revision (x4)>

General MIDI (extension)
	 sysex nonrt <device-id (u7)> gm (system-on1|system-off|system-on2)

Controller Destination Setting
	 sysex nonrt <device-id (u7)> cds <channel (u4)> (channel-pressure|key-pressure)  <parameter1 (u7)> <range1 (u7)> [<parameter2 (u7)> <range2 (u7)> .. <parameterN (u7)> <rangeN (u7)>]
	 sysex nonrt <device-id (u7)> cds <channel (u4)> cc <controller (u7)> <parameter1 (u7)> <range1 (u7)> [<parameter2 (u7)> <range2 (u7)> .. <parameterN (u7)> <rangeN (u7)>]

Keybased Instrument Control
	 sysex nonrt <device-id (u7)> keys <channel (u7)> <key (u7)> <controller1 (u7)> <value1 (u7)> [<controller2 (u7)> <value2 (u7)> .. <controllerN (u7)> <valueN (u7)>]

Device Control
	 sysex rt <device-id (u7)> dc** (master-volume|master-balance|coarse-tuning|fine-tuning) <value (u14)>
	 sysex rt <device-id (u7)> dc global-parameter <slot-count (u7)> <parameter-id-width (u7)> <parameter-value-width (u7)> [<slot-path1 (u14)> [.. <slot-pathN (u14)>]] [<parameter-id1 (xN)> <parameter-value1 (xN)> [..  <parameter-idN (xN)> <parameter-valueN (xN)>]]

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

MIDI Show Control (MSC)
	 sysex rt <device-id (u7)> msc <cmdFmt*> (all-off|restore|reset)
	 sysex rt <device-id (u7)> msc <cmdFmt*> (go|stop|resume|load|go-off|go-jam-lock) <cue-number**> [<cue-list**> [<cue-path**>]]
	 sysex rt <device-id (u7)> msc <cmdFmt*> timed-go <fps = 24,25,29.97,30> <hour <= 23> <minute <= 59> <second <= 59> <frame < fps> < (x1)> <cue-number**> [<cue-list**> [<cue-path**>]]
	 sysex rt <device-id (u7)> msc <cmdFmt*> set <controller (u14)> <value (u14)> <fps = 24,25,29.97,30> <hour <= 23> <minute <= 59> <second <= 59> <frame < fps> < (x1)>
	 sysex rt <device-id (u7)> msc <cmdFmt*> fire <macro-number (u7)>
	 sysex rt <device-id (u7)> msc <cmdFmt*> (standby+|standby-|sequence+|sequence-|start-clock|stop-clock|zero-clock|mtc-chase-on|mtc-chase-off|open-cue-list|close-cue-list) <cue-list**>
	 sysex rt <device-id (u7)> msc <cmdFmt*> (open-cue-path|close-cue-path) <cue-path**>
	 sysex rt <device-id (u7)> msc <cmdFmt*> set-clock <fps = 24,25,29.97,30> <hour <= 23> <minute <= 59> <second <= 59> <frame < fps> < (x1)> <cue-list**>
	 sysex rt <device-id (u7)> msc <cmdFmt*> (standby|go-2-pc) <checksum (u14)> <sequence-number (u14)> <data (x4)> <cue-number**> [<cue-list**> [<cue-path**>]]
	 sysex rt <device-id (u7)> msc <cmdFmt*> standing-by <checksum (u14)> <sequence-number (u14)> <fps = 24,25,29.97,30> <hour <= 23> <minute <= 59> <second <= 59> <frame < fps> < (x1)> <cue-number**> [<cue-list**> [<cue-path**>]]
	 sysex rt <device-id (u7)> msc <cmdFmt*> (cancelled|abort) <checksum (u14)> <status (u16)> <sequence-number (u14)> <data (x4)> <cue-number**> [<cue-list**> [<cue-path**>]]
* <cmdFmt> := lighting|moving-lights|color-changers|strobes|lasers|chasers|sound|music|cd-players|eprom-playback|audio-tape-machines|intercoms|amplifiers|audio-fx|equalizers|machinery|rigging|flys|lifts|turntables|trusses|robots|animation|floats|breakaways|barges|video|video-tape-machines|video-cassette-machines|video-disc-players|video-switchers|video-fx|video-char-generators|video-still-stores|video-monitors|projection|film-projects|slide-projectors|video-projectors|dissolvers|shutter-controls|process-control|hydraulic-oil|h2o|co2|compressed-air|natural-gas|fog|smoke|cracked-haze|pyro|fireworks|explosions|flame|smoke-pots|all
** <cue-number>, <cue-list>, <cue-path> := ascii numbers (0-9) and/or dots (.)

MIDI Machine Commands (MMC)
For MMC the MIDI format acts as container for a command stream of its own, where several MMC commands can be packed into one MIDI message.

	 sysex rt <device-id (u7) mcc <command1 ..> [<command2 ..> [ .. <commandN ..>] .. ]]
		 <commandN ..> :
		 (stop|play|deferred-play|fast-forward|rewind|record-strobe|record-exit|record-pause|pause|eject|chase|command-error-reset|mmc-reset|wait|resume)
		 (variable-play|search|shuttle|deferred-variable-play|record-strobe-variable) <speed (float)>
		 step <step (s7)>
		 locate (field <field>|mtc <fps = 24,25,29.97,30> <hour <= 23> <minute <= 59> <second <= 59> <frame < fps>)
		 assign-system-master <master (u7)>
	 TODO:
		 generator-command (stop|run|copy-jam)
		 mtc-command (off|follow)
		 write ..
		 move ..
		 etc

Mobile Phone Control
	 sysex rt <phone-id (u7)> mpc (vibrator|led|display|keypad|all|<manufacturer-id (x1,x3)> <cmd-id (u7)>> <device-index (u7)> <reset|on|off>
	 sysex rt <phone-id (u7)> mpc (vibrator|led|display|keypad|all|<manufacturer-id (x1,x3)> <cmd-id (u7)>> <device-index (u7)> <manufacturer-cmd (x1, x3)> <data (xN)>
	 sysex rt <phone-id (u7)> mpc (vibrator|led|display|keypad|all|<manufacturer-id (x1,x3)> <cmd-id (u7)>> <device-index (u7)> follow-midi-channels [<channel1 (u7)> <low-note1 (u7)> <high-note1  (u7)> [ .. [<channelN (u7)> <low-noteN (u7)> <high-noteN (u7)>] .. ]
	 sysex rt <phone-id (u7)> mpc (vibrator|led|display|keypad|all|<manufacturer-id (x1,x3)> <cmd-id (u7)>> <device-index (u7)> set-color <red (u7)> <green (u7)> <blue (u7)>
	 sysex rt <phone-id (u7)> mpc (vibrator|led|display|keypad|all|<manufacturer-id (x1,x3)> <cmd-id (u7)>> <device-index (u7)> set-level

Sample Dump Standard (SDS)
	 sysex nonrt <device-id (u7)> sds-header <sample-number (u14)> <sample-fmt (u7)> <sample-period (u21)> <sample-length (u21)> <loop-start (u21)> <loop-end (u21)> (uni-forward|bi-forward|forward-once)
	 sysex nonrt <device-id (u7)> sds-request <sample-number (u14)>
	 sysex nonrt <device-id (u7)> sds-data <packet-index (u7)> <data (xN)> [<checksum-data (x1)> [<checksum-computed (x1)>]]*
	 sysex nonrt <device-id (u7)> sds-ext loop-point-tx <sample-number (u14)> <loop-number (u14)> (uni-forward|bi-forward|forward-once) <loop-start (u21)> <loop-end (u21)>
	 sysex nonrt <device-id (u7)> sds-ext loop-point-request <sample-number (u14)> <loop-number (u14)>
	 sysex nonrt <device-id (u7)> sds-ext ext-header <sample-number (u14)> <sample-fmt (u7)> <sample-rate-integer (u28)> <sample-rate-fraction (u28)> <sample-length (u35)> <loop-start (u35)> <loop-end (u35)> <loop-type**>
	 sysex nonrt <device-id (u7)> sds-ext ext-loop-point-tx <sample-number (u14)> <loop-number (u14)> <loop-type**> <loop-start (u35)> <loop-end (u35)>
	 sysex nonrt <device-id (u7)> sds-ext ext-loop-point-request <sample-number (u14)> <loop-number (u14)>
	 sysex nonrt <device-id (u7)> sds-ext name-tx <sample-number (u14)> -*** <sample-name (strN) ...>
	 sysex nonrt <device-id (u7)> sds-ext name-request <sample-number (u14)>
* Both checksums are given when parsing a MIDI stream (to allow verification). During generation, if <checksum-data> (see specification) is not given it is computed (recommended) and the <checksum-computed> is always ignored if given.
** <loop-type> := uni-forward|bi-forward|uni-forward-release|bi-forward-release|uni-backward|bi-backward|uni-backward-release|bi-backward-release|backward-once|forward-once
*** In principle there is a language tag to support localization, but apart from the default (English) none are documented and thus likely not used. Thus momentarily only the default is supported which is chosen by specifying '-' as argument.

MIDI Visual Control (MVC)
	 sysex nonrt <device-id (u7)> mvc (on-off|clip-control-channel|fx-control-channel|note-msg-enabled) <data (xN)>
	 sysex nonrt <device-id (u7)> mvc (playback-assign-msn|playback-assign-lsn|dissolve-assign-msn|dissolve-assign-lsn) <data (xN)>
	 sysex nonrt <device-id (u7)> mvc (fx1-assign-msn|fx1-assign-lsn|fx2-assign-msn|fx2-assign-lsn|fx3-assign-msn|fx3-assign-lsn) <data (xN)>
	 sysex nonrt <device-id (u7)> mvc (playback-speed-range|keyboard-range-lower|keyboard-range-upper) <data (xN)>
	 sysex nonrt <device-id (u7)> mvc <parameter-address (x3)> <data (xN)>

File Dump
	 sysex nonrt <device-id (u7)> file-dump request <sender-id (u7)> <type (str4)*> <name (strN)..>
	 sysex nonrt <device-id (u7)> file-dump header <sender-id (u7)> <type (str4)*> <length (u28)> <name (strN)..>
	 sysex nonrt <device-id (u7)> file-dump data <packet-number (u7)> <data (xN)> [<checksum (x1)> [<checksum-verification (x1)>]]**
*<type> := four 7-bit ASCII bytes. Defined in specification: MIDI, MIEX, ESEQ, TEXT, BIN<space>, MAC<space>
** <checksum> := as sent/to be sent in message, <checksum-verification> := as computed. Both checksums are given when parsing a MIDI stream but during generation, if no <checksum> is given it is computed (recommended) otherwise its value will be used; <checksum-verification> will always be ignored.

Notation Information
	 sysex rt <device-id (u7)> notation bar-number (not-running|running-unknown|<s14>)
	 sysex rt <device-id (u7)> notation time-signature (immediate|delayed) <midi-clock-in-metronome-click (u7)> <32nd-notes-in-midi-quarter-note (u7)> <time-signature-nominator1 (u7)> <time-signature-denominator1 (u7)>  [<nominator2 (u7)> <denominator2 (u7)> ..]

Examples:
	 bin/midimessage-cli -g note on 60 40 1
	 bin/midimessage-cli -g sysex experimental 1337
	 bin/midimessage-cli -g sysex rt 1 msc sound reset
	 bin/midimessage-cli -g | bin/midimessage-cli -p
	 bin/midimessage-cli -g --prefix='%d ' --suffix=$'\x0a'
	 bin/midimessage-cli -g | bin/midimessage-cli -ptmilli > test.recording
	 cat test.recording | bin/midimessage-cli -gtmilli | bin/midimessage-cli -p
	 echo -n $'\x13\x37'| bin/midimessage-cli --convert=nibblize | xxd
	 echo -n $'\x03\x01\x07\x03'| bin/midimessage-cli --convert=denibblize | xxd
```
## Docs

TODO github page

## References

- https://www.midi.org/specifications-old/item/the-midi-1-0-specification
- https://www.midi.org/specifications-old/category/reference-tables
- http://www.personal.kent.edu/~sbirch/Music_Production/MP-II/MIDI/an_introduction_to_midi_contents.htm
- http://www.somascape.org/midi/tech/spec.html


## License MIT

Also see file `LICENSE`


## Applications

Here you will find applications that make use of this library. If you're using the library you can also drop me a line (which I appreciate either way).

- [PureData (PD) External](https://github.com/tschiemer/midimessage-pd-external)
- [Max/MSP External](https://github.com/tschiemer/midimessage-max-external)
