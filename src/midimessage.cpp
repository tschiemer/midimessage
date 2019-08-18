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

                if (msg->Data.SysEx.Id == ReservedSystemExclusiveIdExperimental){

                    bytes[1] = ReservedSystemExclusiveIdExperimental;

                    if (msg->Data.SysEx.Length > 0) {

#if SYSEX_MEMORY == SYSEX_MEMORY_STATIC
                        memcpy(&bytes[2], msg->Data.SysEx.Data.Bytes, msg->Data.SysEx.Length);
#elif SYSEX_MEMORY == SYSEX_MEMORY_DYNAMIC
                        ASSERT( msg->Data.SysEx.Data )
                        memcpy(&bytes[2], msg->Data.SysEx.Data, msg->Data.SysEx.Length);
#endif
                    }

                    return 2 + msg->Data.SysEx.Length;
                }

                if (msg->Data.SysEx.Id == ReservedSystemExclusiveIdRealTime){

                    //TODO

                    return 0;
                }

                if (msg->Data.SysEx.Id == ReservedSystemExclusiveIdRealTime){

                    //TODO

                    return 0;
                }

                // assume custom manufacturer data from here on


                if ( (msg->Data.SysEx.Id & 0xFF00) != 0 ){
                    bytes[1] = ReservedSystemExclusiveIdManufacturerExtension;
                    bytes[2] = (msg->Data.SysEx.Id >> 8) & 0xFF;
                    bytes[3] = msg->Data.SysEx.Id & 0xFF;
                    len = 4;
                } else {
                    bytes[1] = msg->Data.SysEx.Id & 0xFF;
                    len = 2;
                }

                if (msg->Data.SysEx.Length > 0) {
#if SYSEX_MEMORY == SYSEX_MEMORY_STATIC
                    memcpy(&bytes[2], msg->Data.SysEx.Data.Bytes, msg->Data.SysEx.Length);
#elif SYSEX_MEMORY == SYSEX_MEMORY_DYNAMIC
                    ASSERT( msg->Data.SysEx.Data )
                    memcpy(&bytes[2], msg->Data.SysEx.Data, msg->Data.SysEx.Length);
#endif
                }

                return len + msg->Data.SysEx.Length;

            case StatusSystemExclusiveMidiTimeCodeQuarterFrame:
                return packMidiTimeCodeQuarterFrame( bytes, msg->Data.MidiTimeCodeQuarterFrame.MessageType, msg->Data.MidiTimeCodeQuarterFrame.Nibble );

            case StatusSystemExclusiveSongPositionPointer:
                return packSongPositionPointer( bytes, msg->Data.SongPositionPointer.Position );

            case StatusSystemExclusiveSongSelect:
                return packSongSelect( bytes, msg->Data.SongSelect.Song );

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
                            msg->Data.SysEx.Id = ReservedSystemExclusiveIdExperimental;
                            msg->Data.SysEx.Length = length - 2;
                            if (length > 2) {
#if SYSEX_MEMORY == SYSEX_MEMORY_STATIC
                                ASSERT( length - 2 > SYSEX_MEMORY_STATIC_SIZE );
                                if (length - 2 > SYSEX_MEMORY_STATIC_SIZE){
                                    return false;
                                }
                                memcpy( msg->Data.SysEx.Data.Bytes, &bytes[2], length - 2 );
#elif SYSEX_MEMORY == SYSEX_MEMORY_DYNAMIC
                                msg->Data.SysEx.Data = calloc( (length - 2), 1 );
                                memcpy( msg->Data.SysEx.Data, &bytes[2], length - 2 );
#endif
                            }
                            return true;
                        }

                        if (bytes[1] == ReservedSystemExclusiveIdRealTime) {
                            if (length < 4) {
                                return false;
                            }
                            msg->Data.SysEx.Id = ReservedSystemExclusiveIdRealTime;
                            msg->Channel = bytes[2];
                            msg->Data.SysEx.SubId1 = bytes[3];

                            switch (bytes[3]) {
                                case UniversalSysExRtMidiTimeCode:

                                    if (length < 5) {
                                        return false;
                                    }

                                    msg->Data.SysEx.SubId2 = bytes[4];

                                    switch(bytes[4]){

                                        case UniversalSysExRtMidiTimeCodeFullMessage:
                                            if (length < 9) {
                                                return false;
                                            }
#if SYSEX_MEMORY == SYSEX_MEMORY_STATIC
                                            msg->Data.SysEx.Data.MidiTimeCode.FpsHour = bytes[4];
                                            msg->Data.SysEx.Data.MidiTimeCode.Minute = bytes[5];
                                            msg->Data.SysEx.Data.MidiTimeCode.Second = bytes[6];
                                            msg->Data.SysEx.Data.MidiTimeCode.Frame = bytes[7];
#elif SYSEX_MEMORY == SYSEX_MEMORY_DYNAMIC
                                            msg->Data.SysEx.Data = (MidiTimeCode_t*)calloc(1, sizeof(MidiTimeCode_t));
                                            ((MidiTimeCode_t*)msg->Data.SysEx.Data)->FpsHour = bytes[4];
                                            ((MidiTimeCode_t*)msg->Data.SysEx.Data)->Minute = bytes[5];
                                            ((MidiTimeCode_t*)msg->Data.SysEx.Data)->Second = bytes[6];
                                            ((MidiTimeCode_t*)msg->Data.SysEx.Data)->Frame = bytes[7];
#endif
                                            return true;

                                        case UniversalSysExRtMidiTimeCodeUserBits:
                                            if (length < 13) {
                                                return false;
                                            }
                                            msg->Data.SysEx.Length = 5;
#if SYSEX_MEMORY == SYSEX_MEMORY_STATIC
                                            ASSERT( msg->Data.SysEx.Length > SYSEX_MEMORY_STATIC_SIZE );

                                            memcpy( msg->Data.SysEx.Data.Bytes, &bytes[1], msg->Data.SysEx.Length );
#elif SYSEX_MEMORY == SYSEX_MEMORY_DYNAMIC
                                            msg->Data.SysEx.Data = calloc( msg->Data.SysEx.Length, 1 );
                                            memcpy( msg->Data.SysEx.Data, &bytes[1], msg->Data.SysEx.Length );
#endif

                                            return true;
                                    }
                                    return false;

                                case UniversalSysExRtMidiShowControl:
                                    //TODO
                                    return false;

                                case UniversalSysExRtDeviceControl:
                                    //TODO
                                    return false;

                                case UniversalSysExRtRealTimeMtcCueing:
                                    //TODO
                                    return false;

                                case UniversalSysExRtMidiMachineControlCommands:
                                    //TODO
                                    return false;

                                case UniversalSysExRtMidiMachineControlResponses:
                                    //TODO
                                    return false;

                                case UniversalSysExRtMidiTuningStandard:
                                    //TODO
                                    return false;

                                case UniversalSysExRtControllerDestinationSetting:
                                    //TODO
                                    return false;

                                case UniversalSysExRtKeybasedInstrumentControl:
                                    //TODO
                                    return false;

                                case UniversalSysExRtScalablePolyphonyMidiMip:
                                    //TODO
                                    return false;

                                case UniversalSysExRtMobilePhoneControlMessage:
                                    //TODO
                                    return false;
                            }

                            return false;
                        }

                        if (bytes[1] == ReservedSystemExclusiveIdNonRealTime) {
                            if (length < 4) {
                                return false;
                            }
                            msg->Data.SysEx.Id = ReservedSystemExclusiveIdNonRealTime;
                            msg->Channel = bytes[2];
                            msg->Data.SysEx.SubId1 = bytes[3];

                            switch (bytes[3]){
                                case UniversalSysExNonRTSampleDumpHeader:
                                    //TODO
                                    return false;

                                case UniversalSysExNonRTSampleDataPacket:
                                    //TODO
                                    return false;

                                case UniversalSysExNonRTSampleDumpRequest:
                                    //TODO
                                    return false;

                                case UniversalSysExNonRTMidiTimeCode:
                                    //TODO
                                    return false;

                                case UniversalSysExNonRTSampleDumpExtension:
                                    //TODO
                                    return false;

                                case UniversalSysExNonRTGeneralInformation:
                                    //TODO
                                    return false;

                                case UniversalSysExNonRTFileDump:
                                    //TODO
                                    return false;

                                case UniversalSysExNonRTMidiTuningStandard:
                                    //TODO
                                    return false;

                                case UniversalSysExNonRTGeneralMidi:
                                    //TODO
                                    return false;

                                case UniversalSysExNonRTDownloadableSounds:
                                    //TODO
                                    return false;

                                case UniversalSysExNonRTFileReferenceMessage:
                                    //TODO
                                    return false;

                                case UniversalSysExNonRTMidiVisualControl:
                                    //TODO
                                    return false;

                                case UniversalSysExNonRTMidiCapabilityInquiry:
                                    //TODO
                                    return false;

                                case UniversalSysExNonRTEndOfFile:
                                case UniversalSysExNonRTWait:
                                case UniversalSysExNonRTCancel:
                                case UniversalSysExNonRTNAK:
                                case UniversalSysExNonRTACK:
                                    // no further data
                                    return true;

                            }

                            return false;
                        }


                        // assume manufacturer extension from hereon out.

                        // is it a three byte manufacturer id?
                        if (bytes[1] == ReservedSystemExclusiveIdManufacturerExtension) {
                            if (length < 4) {
                                return false;
                            }
                            msg->Data.SysEx.Id = ((uint32_t)bytes[2]) << 8;
                            msg->Data.SysEx.Id += (uint32_t)bytes[3];
                            msg->Data.SysEx.Length = length - 3;
                        } else {
                            msg->Data.SysEx.Id = (uint32_t)bytes[2];
                            msg->Data.SysEx.Length = length - 2;
                        }

                        if (msg->Data.SysEx.Length > 0){
#if SYSEX_MEMORY == SYSEX_MEMORY_STATIC
                            ASSERT( msg->Data.SysEx.Length > SYSEX_MEMORY_STATIC_SIZE );
                            if (length - 2 > SYSEX_MEMORY_STATIC_SIZE){
                                return false;
                            }
                            memcpy( msg->Data.SysEx.Data.Bytes, &bytes[1], msg->Data.SysEx.Length );
#elif SYSEX_MEMORY == SYSEX_MEMORY_DYNAMIC
                            msg->Data.SysEx.Data = calloc( msg->Data.SysEx.Length, 1 );
                            memcpy( msg->Data.SysEx.Data, &bytes[1], msg->Data.SysEx.Length );
#endif
                        }

                        return true;

                    case SystemExclusiveMidiTimeCodeQuarterFrame:
                        return unpackMidiTimeCodeQuarterFrame( bytes, length, &msg->Data.MidiTimeCodeQuarterFrame.MessageType, &msg->Data.MidiTimeCodeQuarterFrame.Nibble );

                    case SystemExclusiveSongPositionPointer:
                        return unpackSongPositionPointer( bytes, length, &msg->Data.SongPositionPointer.Position );

                    case SystemExclusiveSongSelect:
                        return unpackSongSelect( bytes, length, &msg->Data.SongSelect.Song );

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
                msg->Data.SysEx.Length > 0 &&
                msg->Data.SysEx.Data != NULL){


            free(msg->Data.SysEx.Data);

            msg->Data.SysEx.Length = 0;
            msg->Data.SysEx.Data = NULL;
        }
    }
#endif



}