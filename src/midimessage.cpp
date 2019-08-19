//
// Created by Philip Tschiemer on 19.07.19.
//

#include <midimessage.h>
#include <mmstdlib.h>
#include <cstddef>



namespace MidiMessage {

    int pack( uint8_t * bytes, Message_t * msg ){
        ASSERT( bytes != NULL);
        ASSERT( msg != NULL );

        int len;

        if (isStatusClass(msg->StatusClass) && msg->StatusClass != StatusClassSystemMessage) {
            switch (msg->StatusClass) {

                case StatusClassNoteOff:
                    return packNoteOff(bytes, msg->Channel, msg->Data.NoteOff.Key, msg->Data.NoteOff.Velocity);

                case StatusClassNoteOn:
                    return packNoteOn(bytes, msg->Channel, msg->Data.NoteOn.Key, msg->Data.NoteOn.Velocity);

                case StatusClassPolyphonicKeyPressure:
                    return packPolyphonicKeyPressure(bytes, msg->Channel, msg->Data.PolyphonicKeyPressure.Key,
                                                     msg->Data.PolyphonicKeyPressure.Pressure);

                case StatusClassControlChange:
                    return packControlChange(bytes, msg->Channel, msg->Data.ControlChange.Controller,
                                             msg->Data.ControlChange.Value);

                case StatusClassProgramChange:
                    return packProgramChange(bytes, msg->Channel, msg->Data.ProgramChange.Program);

                case StatusClassChannelPressure:
                    return packChannelPressure(bytes, msg->Channel, msg->Data.ChannelPressure.Pressure);

                case StatusClassPitchBendChange:
                    return packPitchBendChange(bytes, msg->Channel, msg->Data.PitchBendChange.Pitch);

                case StatusClassSystemMessage:
                    // left here to avoid compiler warning
                    break;
            }
        }


        if (isSystemMessage(msg->Status) && msg->Status != SystemMessageSystemExclusive) {

            switch (msg->Status)
            {
                case SystemMessageMidiTimeCodeQuarterFrame:
                    return packMidiTimeCodeQuarterFrame( bytes, msg->Data.MidiTimeCodeQuarterFrame.MessageType, msg->Data.MidiTimeCodeQuarterFrame.Nibble );

                case SystemMessageSongPositionPointer:
                    return packSongPositionPointer( bytes, msg->Data.SongPositionPointer.Position );

                case SystemMessageSongSelect:
                    return packSongSelect( bytes, msg->Data.SongSelect.Song );

                case SystemMessageTuneRequest:
                case SystemMessageEndOfSystemExclusive:
                case SystemMessageTimingClock:
                case SystemMessageStart:
                case SystemMessageContinue:
                case SystemMessageStop:
                case SystemMessageActiveSensing:
                case SystemMessageReset:

                    bytes[0] = msg->Status;

                    return 1;

                case SystemMessageSystemExclusive:
                    // left here to avoid compiler warning
                    break;
            }
        }

        if (msg->Status == SystemMessageSystemExclusive){

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
        } // msg->Status == SystemMessageSystemExclusive



        ASSERT( false ); // should never reach here. You only do, when the given parameters are invalid.

        return 0;
    }

    bool unpack( uint8_t * bytes, int length, Message_t * msg ){

        ASSERT( bytes != NULL);
        ASSERT( msg != NULL );
        ASSERT( length > 0 ); // Catch in case you passe a zero-length sequence...

        if ( ! isControlByte(bytes[0]) ){
            return false;
        }

        msg->StatusClass = getStatusClass(bytes[0]);
        msg->Status = bytes[0];

        if (isStatusClass(msg->StatusClass) && msg->StatusClass != StatusClassSystemMessage) {
            switch (msg->StatusClass) {

                case StatusClassNoteOff:
                    return unpackNoteOff(bytes, length, &msg->Channel, &msg->Data.NoteOff.Key,
                                         &msg->Data.NoteOff.Velocity);

                case StatusClassNoteOn:
                    return unpackNoteOn(bytes, length, &msg->Channel, &msg->Data.NoteOn.Key,
                                        &msg->Data.NoteOn.Velocity);

                case StatusClassPolyphonicKeyPressure:
                    return unpackPolyphonicKeyPressure(bytes, length, &msg->Channel,
                                                       &msg->Data.PolyphonicKeyPressure.Key,
                                                       &msg->Data.PolyphonicKeyPressure.Pressure);

                case StatusClassControlChange:
                    return unpackControlChange(bytes, length, &msg->Channel, &msg->Data.ControlChange.Controller,
                                               &msg->Data.ControlChange.Value);

                case StatusClassProgramChange:
                    return unpackProgramChange(bytes, length, &msg->Channel, &msg->Data.ProgramChange.Program);

                case StatusClassChannelPressure:
                    return unpackChannelPressure(bytes, length, &msg->Channel, &msg->Data.ChannelPressure.Pressure);

                case StatusClassPitchBendChange:
                    return unpackPitchBendChange(bytes, length, &msg->Channel, &msg->Data.PitchBendChange.Pitch);

                case StatusClassSystemMessage:
                    // left here to avoid compiler warning
                    break;

            } // switch(msg->StatusClass)
        } // (isStatusClass(msg->StatusClass) && msg->StatusClass != StatusClassSystemMessage)


        if (isSystemMessage(msg->Status) && msg->Status != SystemMessageSystemExclusive) {

            switch( msg->Status){

                case SystemMessageMidiTimeCodeQuarterFrame:
                    return unpackMidiTimeCodeQuarterFrame( bytes, length, &msg->Data.MidiTimeCodeQuarterFrame.MessageType, &msg->Data.MidiTimeCodeQuarterFrame.Nibble );

                case SystemMessageSongPositionPointer:
                    return unpackSongPositionPointer( bytes, length, &msg->Data.SongPositionPointer.Position );

                case SystemMessageSongSelect:
                    return unpackSongSelect( bytes, length, &msg->Data.SongSelect.Song );

                case SystemMessageTuneRequest:
                case SystemMessageEndOfSystemExclusive:
                case SystemMessageTimingClock:
                case SystemMessageStart:
                case SystemMessageContinue:
                case SystemMessageStop:
                case SystemMessageActiveSensing:
                case SystemMessageReset:
                    if (length != 1){
                        return false;
                    }
                    return true;

                case SystemMessageSystemExclusive:
                    // left here to avoid compiler warning
                    break;

            } // switch(msg->Status)

        } // (isSystemMessage(msg->Status) && msg->Status != SystemMessageSystemExclusive)


        if (msg->Status == SystemMessageSystemExclusive) {


            msg->Status = bytes[0];


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
                    msg->Data.SysEx.Data = calloc((length - 2), 1);
                    memcpy(msg->Data.SysEx.Data, &bytes[2], length - 2);
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

                        switch (bytes[4]) {

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
                                msg->Data.SysEx.Data = (MidiTimeCode_t *) calloc(1, sizeof(MidiTimeCode_t));
                                ((MidiTimeCode_t *) msg->Data.SysEx.Data)->FpsHour = bytes[4];
                                ((MidiTimeCode_t *) msg->Data.SysEx.Data)->Minute = bytes[5];
                                ((MidiTimeCode_t *) msg->Data.SysEx.Data)->Second = bytes[6];
                                ((MidiTimeCode_t *) msg->Data.SysEx.Data)->Frame = bytes[7];
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
                                msg->Data.SysEx.Data = calloc(msg->Data.SysEx.Length, 1);
                                memcpy(msg->Data.SysEx.Data, &bytes[1], msg->Data.SysEx.Length);
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

                switch (bytes[3]) {
                    case UniversalSysExNonRtSampleDumpHeader:
                        //TODO
                        return false;

                    case UniversalSysExNonRtSampleDataPacket:
                        //TODO
                        return false;

                    case UniversalSysExNonRtSampleDumpRequest:
                        //TODO
                        return false;

                    case UniversalSysExNonRtMidiTimeCode:
                        //TODO
                        return false;

                    case UniversalSysExNonRtSampleDumpExtension:
                        //TODO
                        return false;

                    case UniversalSysExNonRtGeneralInformation:
                        //TODO
                        return false;

                    case UniversalSysExNonRtFileDump:
                        //TODO
                        return false;

                    case UniversalSysExNonRtMidiTuningStandard:
                        //TODO
                        return false;

                    case UniversalSysExNonRtGeneralMidi:
                        //TODO
                        return false;

                    case UniversalSysExNonRtDownloadableSounds:
                        //TODO
                        return false;

                    case UniversalSysExNonRtFileReferenceMessage:
                        //TODO
                        return false;

                    case UniversalSysExNonRtMidiVisualControl:
                        //TODO
                        return false;

                    case UniversalSysExNonRtMidiCapabilityInquiry:
                        //TODO
                        return false;

                    case UniversalSysExNonRtEndOfFile:
                    case UniversalSysExNonRtWait:
                    case UniversalSysExNonRtCancel:
                    case UniversalSysExNonRtNAK:
                    case UniversalSysExNonRtACK:
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
                msg->Data.SysEx.Id = ((uint32_t) bytes[2]) << 8;
                msg->Data.SysEx.Id += (uint32_t) bytes[3];
                msg->Data.SysEx.Length = length - 3;
            } else {
                msg->Data.SysEx.Id = (uint32_t) bytes[2];
                msg->Data.SysEx.Length = length - 2;
            }

            if (msg->Data.SysEx.Length > 0) {
#if SYSEX_MEMORY == SYSEX_MEMORY_STATIC
                ASSERT( msg->Data.SysEx.Length > SYSEX_MEMORY_STATIC_SIZE );
                    if (length - 2 > SYSEX_MEMORY_STATIC_SIZE){
                        return false;
                    }
                    memcpy( msg->Data.SysEx.Data.Bytes, &bytes[1], msg->Data.SysEx.Length );
#elif SYSEX_MEMORY == SYSEX_MEMORY_DYNAMIC
                msg->Data.SysEx.Data = calloc(msg->Data.SysEx.Length, 1);
                memcpy(msg->Data.SysEx.Data, &bytes[1], msg->Data.SysEx.Length);
#endif
            }

            return true;
        } // (msg->Status == SystemMessageSystemExclusive)

    }

#if SYSEX_MEMORY == SYSEX_MEMORY_DYNAMIC
    void freeMessage( Message_t * msg ){

        if (msg->Status == SystemMessageSystemExclusive &&
                msg->Data.SysEx.Length > 0 &&
                msg->Data.SysEx.Data != NULL){


            free(msg->Data.SysEx.Data);

            msg->Data.SysEx.Length = 0;
            msg->Data.SysEx.Data = NULL;
        }
    }
#endif



}