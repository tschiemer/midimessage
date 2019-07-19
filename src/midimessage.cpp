//
// Created by Philip Tschiemer on 19.07.19.
//

#include <midimessage.h>



namespace MidiMessage {

    int pack( uint8_t * bytes, Message_t * msg, uint8_t * sysex ){


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


                switch(getChannel(msg->Status)){
                    case SystemExclusive:

                        ASSERT( sysex != NULL );

                        //TODO
                        ASSERT( false )

                        return 0;

                    case SystemExclusiveMIDITimeCodeQuarterFrame:
                        return packMIDITimeCodeQuarterFrame( bytes, msg->Data.SysEx.MIDITimeCodeQuarterFrame.MessageType, msg->Data.SysEx.MIDITimeCodeQuarterFrame.Values );

                    case SystemExclusiveSongPositionPointer:
                        return packSongPositionPointer( bytes, msg->Data.SysEx.SongPositionPointer.Position );

                    case SystemExclusiveSongSelect:
                        return packSongSelect( bytes, msg->Data.SysEx.SongSelect.Song );

                    case SystemExclusiveTuneRequest:
                    case SystemExclusiveEndOfExclusive:
                    case SystemExclusiveTimingClock:
                    case SystemExclusiveStart:
                    case SystemExclusiveContinue:
                    case SystemExclusiveStop:
                    case SystemExclusiveActiveSensing:
                    case SystemExclusiveReset:

                        bytes[0] = StatusSystemExclusive | getChannel(msg->Status);

                        return 1;

                }
//                unpack( bytes, &msg->Data.NoteOn.Channel, &msg->Data.NoteOn.Note, &msg->Data.NoteOn.Velocity );
                break;

            default:
                break;
        }


        return 0;
    }

    bool unpack( uint8_t * bytes, int len, Message_t * msg, uint8_t * sysex ){
        if ( ! isValidStatusByte(bytes[0]) ){
            return false;
        }

        msg->Status = getStatus(bytes[0]);

        switch (msg->Status){

            case StatusNoteOff:
                return unpackNoteOff( bytes, len, &msg->Channel, &msg->Data.NoteOff.Key, &msg->Data.NoteOff.Velocity );

            case StatusNoteOn:
                return unpackNoteOn( bytes, len, &msg->Channel, &msg->Data.NoteOn.Key, &msg->Data.NoteOn.Velocity );

            case StatusPolyphonicKeyPressure:
                return unpackPolyphonicKeyPressure( bytes, len, &msg->Channel, &msg->Data.PolyphonicKeyPressure.Key, &msg->Data.PolyphonicKeyPressure.Pressure );

            case StatusControlChange:
                return unpackControlChange( bytes, len, &msg->Channel, &msg->Data.ControlChange.Controller, &msg->Data.ControlChange.Value );

            case StatusProgramChange:
                return unpackProgramChange( bytes, len, &msg->Channel, &msg->Data.ProgramChange.Program );

            case StatusChannelPressure:
                return unpackChannelPressure( bytes, len, &msg->Channel, &msg->Data.ChannelPressure.Pressure );

            case StatusPitchBendChange:
                return unpackPitchBendChange( bytes, len, &msg->Channel, &msg->Data.PitchBendChange.Pitch );

            case StatusSystemExclusive:

                msg->Status = (Status_t)bytes[0];

                switch((getChannel(bytes[0]))){
                    case SystemExclusive:
                        //TODO
                        ASSERT( false )
                        return false;

                    case SystemExclusiveMIDITimeCodeQuarterFrame:
                        return unpackMIDITimeCodeQuarterFrame( bytes, len, &msg->Data.SysEx.MIDITimeCodeQuarterFrame.MessageType, &msg->Data.SysEx.MIDITimeCodeQuarterFrame.Values );

                    case SystemExclusiveSongPositionPointer:
                        return unpackSongPositionPointer( bytes, len, &msg->Data.SysEx.SongPositionPointer.Position );

                    case SystemExclusiveSongSelect:
                        return unpackSongSelect( bytes, len, &msg->Data.SysEx.SongSelect.Song );

                    case SystemExclusiveTuneRequest:
                    case SystemExclusiveEndOfExclusive:
                    case SystemExclusiveTimingClock:
                    case SystemExclusiveStart:
                    case SystemExclusiveContinue:
                    case SystemExclusiveStop:
                    case SystemExclusiveActiveSensing:
                    case SystemExclusiveReset:
                        return true;

                }

            default:
                msg->Status = (Status_t)0;
                return false;

        }
    }

}