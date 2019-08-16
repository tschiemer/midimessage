//
// Created by Philip Tschiemer on 19.07.19.
//

#include <midimessage.h>
#include <string.h>
#include <cstdlib>


namespace MidiMessage {

    int pack( uint8_t * bytes, Message_t * msg ){


        switch (msg->Status){

            case StatusNoteOff:
                return packNoteOff( bytes, msg->Channel, msg->Data.NoteOff.Key, msg->Data.NoteOff.Velocity );

            case StatusNoteOn:
                return packNoteOn( bytes, msg->Channel, msg->Data.NoteOn.Key, msg->Data.NoteOn.Velocity );

            case StatusPolyphonicKeyPressure:
                return packPolyphonicKeyPressure( bytes, msg->Channel, msg->Data.PolyphonicKeyPressure.Key, msg->Data.PolyphonicKeyPressure.Pressure );

            case StatusControlChange:
                return packControlChange( bytes, msg->Channel, msg->Data.ControlChange.Controller, msg->Data.ControlChange.Value );

            case StatusProgramChange:
                return packProgramChange( bytes, msg->Channel, msg->Data.ProgramChange.Program );

            case StatusChannelPressure:
                return packChannelPressure( bytes, msg->Channel, msg->Data.ChannelPressure.Pressure );

            case StatusPitchBendChange:
                return packPitchBendChange( bytes, msg->Channel, msg->Data.PitchBendChange.Pitch );

            case StatusSystemExclusive:

                ASSERT( bytes != NULL );

                bytes[0] = msg->Status;

                if (msg->Data.SysEx.Custom.Length > 0) {
                    ASSERT( msg->Data.SysEx.Custom.Buffer)

                    memcpy(&bytes[1], msg->Data.SysEx.Custom.Buffer, msg->Data.SysEx.Custom.Length);
                }

                return 1 + msg->Data.SysEx.Custom.Length;

            case StatusSystemExclusiveMIDITimeCodeQuarterFrame:
                return packMIDITimeCodeQuarterFrame( bytes, msg->Data.SysEx.MIDITimeCodeQuarterFrame.MessageType, msg->Data.SysEx.MIDITimeCodeQuarterFrame.Values );

            case StatusSystemExclusiveSongPositionPointer:
                return packSongPositionPointer( bytes, msg->Data.SysEx.SongPositionPointer.Position );

            case StatusSystemExclusiveSongSelect:
                return packSongSelect( bytes, msg->Data.SysEx.SongSelect.Song );

            case StatusSystemExclusiveTuneRequest:
            case StatusSystemExclusiveEndOfExclusive:
            case StatusSystemExclusiveTimingClock:
            case StatusSystemExclusiveStart:
            case StatusSystemExclusiveContinue:
            case StatusSystemExclusiveStop:
            case StatusSystemExclusiveActiveSensing:
            case StatusSystemExclusiveReset:

                bytes[0] = msg->Status;

                return 1;

            default:
                break;
        }


        return 0;
    }

    bool unpack( uint8_t * bytes, int length, Message_t * msg ){

        if ( ! isValidStatusByte(bytes[0]) ){
            return false;
        }

        msg->Status = getStatus(bytes[0]);

        switch (msg->Status){

            case StatusNoteOff:
                return unpackNoteOff( bytes, length, &msg->Channel, &msg->Data.NoteOff.Key, &msg->Data.NoteOff.Velocity );

            case StatusNoteOn:
                return unpackNoteOn( bytes, length, &msg->Channel, &msg->Data.NoteOn.Key, &msg->Data.NoteOn.Velocity );

            case StatusPolyphonicKeyPressure:
                return unpackPolyphonicKeyPressure( bytes, length, &msg->Channel, &msg->Data.PolyphonicKeyPressure.Key, &msg->Data.PolyphonicKeyPressure.Pressure );

            case StatusControlChange:
                return unpackControlChange( bytes, length, &msg->Channel, &msg->Data.ControlChange.Controller, &msg->Data.ControlChange.Value );

            case StatusProgramChange:
                return unpackProgramChange( bytes, length, &msg->Channel, &msg->Data.ProgramChange.Program );

            case StatusChannelPressure:
                return unpackChannelPressure( bytes, length, &msg->Channel, &msg->Data.ChannelPressure.Pressure );

            case StatusPitchBendChange:
                return unpackPitchBendChange( bytes, length, &msg->Channel, &msg->Data.PitchBendChange.Pitch );

            case StatusSystemExclusive:

                msg->Status = (Status_t)bytes[0];

                switch((getChannel(bytes[0]))){
                    case SystemExclusive:

                        if (length <= 1){
                            msg->Data.SysEx.Custom.Length = 0;
                            msg->Data.SysEx.Custom.Buffer = NULL;
                        } else {
                            msg->Data.SysEx.Custom.Length = length - 1;
                            if (length - 1 > 0){
                                msg->Data.SysEx.Custom.Buffer = (uint8_t*)malloc( length - 1 );
                                memcpy( msg->Data.SysEx.Custom.Buffer, &bytes[1], length - 1 );
                            }
                        }

                        return true;

                    case SystemExclusiveMIDITimeCodeQuarterFrame:
                        return unpackMIDITimeCodeQuarterFrame( bytes, length, &msg->Data.SysEx.MIDITimeCodeQuarterFrame.MessageType, &msg->Data.SysEx.MIDITimeCodeQuarterFrame.Values );

                    case SystemExclusiveSongPositionPointer:
                        return unpackSongPositionPointer( bytes, length, &msg->Data.SysEx.SongPositionPointer.Position );

                    case SystemExclusiveSongSelect:
                        return unpackSongSelect( bytes, length, &msg->Data.SysEx.SongSelect.Song );

                    case SystemExclusiveTuneRequest:
                    case SystemExclusiveEndOfExclusive:
                    case SystemExclusiveTimingClock:
                    case SystemExclusiveStart:
                    case SystemExclusiveContinue:
                    case SystemExclusiveStop:
                    case SystemExclusiveActiveSensing:
                    case SystemExclusiveReset:
                        if (length != 1){
                            return false;
                        }
                        return true;

                }

            default:
                return false;

        }
    }

    void freeMessage( Message_t * msg ){

        if (msg->Status == StatusSystemExclusive &&
                msg->Data.SysEx.Custom.Length > 0 &&
                msg->Data.SysEx.Custom.Buffer != NULL){
            free(msg->Data.SysEx.Custom.Buffer);
        }
    }

}