# midimessage

Very basic constants and un-/packer for MIDI Messages.

Intended for usage with frameworks (such as https://github.com/thestk/rtmidi) that do not provide these basics.

## Contained contants & un-/packers

- Manufacturer Id list
- Standard/Common CC list

* tt = to test

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
| MIDI Show Control | | | TODO |
| MIDI Tuning Standard | | | TODO |
| System Exclusive | Experimental (custom) messages | | tt |
| | Manufacturer messages (+ manufacturer ids) | | tt |
| | Sample Dump | Non-Real Time | TODO |
| | General Information | Non-Real Time | tt |
| | File Dump | Non-Real Time | TODO |
| | General MIDI System Messages | Non-Real Time | TODO |
| | Downloadable Sounds | Non-Real Time | TODO |
| | Device Control (Master volume, balance, coarse/fine tuning, global parameters) | Real Time | tt |
| | Notation Information | Real Time | TODO |




## Using libmidimessage

`libmidimessage` relies on wrapper functions (as defined in `mmstdlib.h`) that have to be implemented by you.

 To use the stdlib implementations you can use the example implementation `mmstdlib.cpp`.

## Code Example

```cpp

void receivedMidiMessageFromSomewhere( uint8_t * buf, int len ){

    MidiMessage::Message_t msg, msg2;

    uint8_t buf2[256];
    int len2;

    static uint8_t pitch = 0;


    if (!MidiMessage::unpack( buf, len, &msg )) {
        std::cout << "Discarding invalid message" << std::endl;
        return;
    }

    if (!MidiMessage::isChannelVoiceMessage(&msg)){

        std::cout << "Discarding non-ChannelVoice message" << std::endl;

        // if above a SysEx message with data content was unpacked, we have to make sure to free the memory again.
        MidiMessage::freeMessage(&msg);

        return;
    }

    // Struct based approach to create messages
    // (the final message packaging is unified)
    msg2.StatusClass = MidiMessage::StatusClassNoteOn;
    msg2.Channel = msg.Channel;
    msg2.Data.NoteOn.Key = 40 + pitch;
    msg2.Data.NoteOn.Velocity = 100;

    pitch = (pitch + 1) % 13;

    len2 = MidiMessage::pack( buf2, &msg2 );
    if (len2 > 0){
        sendMidiMessageToSomewhere( buf2, len2 );
    }

    // direct approach
    len2 = MidiMessage::packNoteOff( buf2, msg2.Channel, msg2.Data.NoteOn.Key, 50 );
    if (len2 > 0){
        sendMidiMessageToSomewhere( buf2, len2 );
    }
}

```

## References

- https://www.midi.org/specifications-old/item/the-midi-1-0-specification
- https://www.midi.org/specifications-old/category/reference-tables
- http://www.personal.kent.edu/~sbirch/Music_Production/MP-II/MIDI/an_introduction_to_midi_contents.htm
- http://www.somascape.org/midi/tech/spec.html
- http://www.indiana.edu/~emusic/etext/MIDI/chapter3_MIDI6.shtml (standard CC)
- http://www.nortonmusic.com/midi_cc.html (standard CC)


## License MIT
