# midimessage

Very basic constants and un-/packer for MIDI Messages.

Intended for usage with frameworks (such as https://github.com/thestk/rtmidi) that do not provide these basics.


```cpp

void receivedMidiMessageFromSomewhere( uint8_t * buf, int len ){

    MidiMessage::Message_t msg;

    uint8_t buf2[256];
    int len2;

    static uint8_t pitch = 0;
    MidiMessage::Message_t msg2;

    if (MidiMessage::unpack( buf, len, &msg )) {

         if (MidiMessage::isChannelVoiceMessage(msg)){

            msg2.Status = MidiMessage::StatusNoteOn;
            msg2.Channel = msg.Channel;
            msg2.Data.NoteOn.Key = 40 + pitch;
            msg2.Data.NoteOn.Velocity = 100;

            pitch = (pitch + 1) % 13;

            len2 = MidiMessage::pack( buf2, &msg2 );
            if (len2 > 0){

                sendMidiMessageToSomewhere( buf2, len2 );

            }
         }

    }
}

```