# [midimessage](https://github.com/tschiemer/midimessage)

Very basic constants and un-/packer for MIDI Messages.

Intended for usage with frameworks (such as https://github.com/thestk/rtmidi) that do not provide these basics.

No particular structures are enforced such that you can take from it what you want.

## Features

- (many) MIDI Message constants + enumerations
- Manufacturer Id list (see `include/manufacturerids.h`)
 - tool to generate up-to-date header, run `make manufacturerids` (requires wget + php)
- Standard/Common CC list (see `include/commonccs.h`)
- *packers* construct the right byte sequence for a particular message ASSERTing valid parameters
- *unpackers* for a specific message type try to parse the given byte sequence thereby validating the byte sequence (could be used in any combination)
- packers and unpackers are always complementary and are available as literal-based and struct-based variants
- *structs* and *types* help to create a unified interface and make handling easier
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
| System Exclusive | Experimental (custom) messages | | tt |
| | Manufacturer messages (+ manufacturer ids) | | tt |
| | Device Control (Master volume, balance, coarse/fine tuning, global parameters) | Real Time | tt |
| | General Information | Non-Real Time | tt |
| | Sample Dump | Non-Real Time | TODO |
| | File Dump | Non-Real Time | TODO |
| | General MIDI System Messages | Non-Real Time | TODO |
| | Downloadable Sounds | Non-Real Time | TODO |
| | Notation Information | Real Time | TODO |



## Requirements

- `midimessage-cli` requires getopt
- target `manufacturerids` requires wget and php(-cli)

## Building

```bash
git clone https://github.com/tschiemer/midimessage.git
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
Usage: midimessage-cli [-h?] [--parse|-p [--timed|-t[milli|micro]] [<binary-data>]] [--generate|-g [--timed|-t[milli|micro]] [<cmd> ...]]
If no <data> or <cmd>.. is given reads from STDIN assuming either a continuous data stream (when parsing) or one generation command per line
Output to STDOUT (when generating this will be binary).
Note: parsed message output format is identical to the required generation format ;)

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

System Exclusives:
	 sysex experimental <n> <data-of-length-n>
	 sysex manufacturer <hex-manufacturer-id> <n> <data-of-length-n>
	 sysex nonrt info (<request> <device-id>|<reply> <device-id> <hex-manufacturer-id> <hex-device-family> <hex-device-family-member> <hex-software-revision>

Note: Data bytes have a value range of 0-127 - anything above is considered a control byte.

Recordings and Replay
Using the --timed|-t option the utility will enter a record mode (when parsing) or replay mode (when generating) message. The generation commands will then keep the delay between message in the given time scale (micro or milli seconds, default = micro).

Examples:
	 ./midimessage-cli -g note on 60 40 1
	 ./midimessage-cli -g | ./midimessage-cli -p
	 ./midimessage-cli -g | ./midimessage-cli -ptmilli > test.recording
	 cat test.recording | ./midimessage-cli -gtmilli | ./midimessage-cli -p
```

## References

- https://www.midi.org/specifications-old/item/the-midi-1-0-specification
- https://www.midi.org/specifications-old/category/reference-tables
- http://www.personal.kent.edu/~sbirch/Music_Production/MP-II/MIDI/an_introduction_to_midi_contents.htm
- http://www.somascape.org/midi/tech/spec.html
- http://www.indiana.edu/~emusic/etext/MIDI/chapter3_MIDI6.shtml (standard CC)
- http://www.nortonmusic.com/midi_cc.html (standard CC)


## License MIT

Also see file `LICENSE`
