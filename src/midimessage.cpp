//
// Created by Philip Tschiemer on 19.07.19.
//

#include <midimessage.h>
#include <mmstdlib.h>
#include <cstddef>



namespace MidiMessage {

    int pack( uint8_t * bytes, Message_t * msg ){
        int len;

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

                if (msg->Data.SysEx.Custom.Id == ReservedSystemExclusiveIdExperimental){

                    bytes[1] = ReservedSystemExclusiveIdExperimental;

                    if (msg->Data.SysEx.Custom.Length > 0) {
                        ASSERT( msg->Data.SysEx.Custom.Buffer)

                        memcpy(&bytes[2], msg->Data.SysEx.Custom.Data, msg->Data.SysEx.Custom.Length);
                    }

                    return 2 + msg->Data.SysEx.Custom.Length;
                }

                if (msg->Data.SysEx.Custom.Id == ReservedSystemExclusiveIdRealTime){

                    //TODO

                    return 0;
                }

                if (msg->Data.SysEx.Custom.Id == ReservedSystemExclusiveIdRealTime){

                    //TODO

                    return 0;
                }

                // assume custom manufacturer data from here on


                if ( (msg->Data.SysEx.Custom.Id & 0xFF00) != 0 ){
                    bytes[1] = ReservedSystemExclusiveIdManufacturerExtension;
                    bytes[2] = (msg->Data.SysEx.Custom.Id >> 8) & 0xFF;
                    bytes[3] = msg->Data.SysEx.Custom.Id & 0xFF;
                    len = 4;
                } else {
                    bytes[1] = msg->Data.SysEx.Custom.Id & 0xFF;
                    len = 2;
                }

                if (msg->Data.SysEx.Custom.Length > 0) {
                    ASSERT( msg->Data.SysEx.Custom.Buffer)

                    memcpy(&bytes[2], msg->Data.SysEx.Custom.Data, msg->Data.SysEx.Custom.Length);
                }

                return len + msg->Data.SysEx.Custom.Length;

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

                        // minimum of status byte and manufacturerid (assuming and optional ending)
                        if (length < 2) {
                            return false;
                        }

                        if (bytes[1] == ReservedSystemExclusiveIdExperimental) {
                            msg->Data.SysEx.Custom.Id = ReservedSystemExclusiveIdExperimental;
                            msg->Data.SysEx.Custom.Length = length - 2;
                            if (length > 2) {
#if SYSEX_MEMORY == SYSEX_MEMORY_STATIC
                                ASSERT( length - 2 > SYSEX_MEMORY_STATIC_SIZE );
                                if (length - 2 > SYSEX_MEMORY_STATIC_SIZE){
                                    return false;
                                }
#elif SYSEX_MEMORY == SYSEX_MEMORY_DYNAMIC
                                msg->Data.SysEx.Custom.Data = calloc( length - 2, 1 );
#endif
                                memcpy( msg->Data.SysEx.Custom.Data, &bytes[2], length - 2 );
                            }
                            return true;
                        }

                        if (bytes[1] == ReservedSystemExclusiveIdRealTime || bytes[1] == ReservedSystemExclusiveIdNonRealTime) {
                            msg->Data.SysEx.Custom.Id = bytes[1];
                            msg->Channel = bytes[2];
                            //TODO
                            return false;
                        }

                        if (bytes[1] == ReservedSystemExclusiveIdNonRealTime) {
                            msg->Data.SysEx.Custom.Id = ReservedSystemExclusiveIdNonRealTime;
                            //TODO
                            return false;
                        }


                        // assume manufacturer extension from hereon out.

                        // is it a three byte manufacturer id?
                        if (bytes[1] == ReservedSystemExclusiveIdManufacturerExtension) {
                            if (length < 4) {
                                return false;
                            }
                            msg->Data.SysEx.Custom.Id = ((uint32_t)bytes[2]) << 8;
                            msg->Data.SysEx.Custom.Id += (uint32_t)bytes[3];
                            msg->Data.SysEx.Custom.Length = length - 3;
                        } else {
                            msg->Data.SysEx.Custom.Id = (uint32_t)bytes[2];
                            msg->Data.SysEx.Custom.Length = length - 2;
                        }

                        if (msg->Data.SysEx.Custom.Length > 0){
#if SYSEX_MEMORY == SYSEX_MEMORY_STATIC
                            ASSERT( msg->Data.SysEx.Custom.Length > SYSEX_MEMORY_STATIC_SIZE );
                            if (length - 2 > SYSEX_MEMORY_STATIC_SIZE){
                                return false;
                            }
#elif SYSEX_MEMORY == SYSEX_MEMORY_DYNAMIC
                            msg->Data.SysEx.Custom.Data = calloc( msg->Data.SysEx.Custom.Length, 1 );
#endif
                            memcpy( msg->Data.SysEx.Custom.Data, &bytes[1], msg->Data.SysEx.Custom.Length );
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

#if SYSEX_MEMORY == SYSEX_MEMORY_DYNAMIC
    void freeMessage( Message_t * msg ){

        if (msg->Status == StatusSystemExclusive &&
                msg->Data.SysEx.Custom.Length > 0 &&
                msg->Data.SysEx.Custom.Data != NULL){


            free(msg->Data.SysEx.Custom.Data);

            msg->Data.SysEx.Custom.Length = 0;
            msg->Data.SysEx.Custom.Data = NULL;
        }
    }
#endif

}