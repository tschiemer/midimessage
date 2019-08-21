//
// Created by Philip Tschiemer on 19.07.19.
//

#include <midimessage.h>
#include <mmstdlib.h>
#include <cstddef>



namespace MidiMessage {

    uint32_t pack( uint8_t * bytes, Message_t * msg ){
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

            if (msg->Data.SysEx.Id == ReservedSysExIdExperimental){

                bytes[1] = ReservedSysExIdExperimental;

                if (msg->Data.SysEx.Length > 0) {

#if SYSEX_MEMORY == SYSEX_MEMORY_DYNAMIC
                    ASSERT( msg->Data.SysEx.ByteData != NULL );
#endif
                    memcpy(&bytes[2], msg->Data.SysEx.ByteData, msg->Data.SysEx.Length);
                }

                return 2 + msg->Data.SysEx.Length;
            } // (msg->Data.SysEx.Id == ReservedSysExIdExperimental)

            if (isManufacturerSysExId( msg->Data.SysEx.Id )){

                len = 1 + packSysExId( bytes, msg->Data.SysEx.Id );

                if (msg->Data.SysEx.Length > 0) {
#if SYSEX_MEMORY == SYSEX_MEMORY_DYNAMIC
                    ASSERT( msg->Data.SysEx.ByteData != NULL );
#endif
                    memcpy(&bytes[2], msg->Data.SysEx.ByteData, msg->Data.SysEx.Length);
                }

                return len + msg->Data.SysEx.Length;
            } // (isManufacturerSysExId( msg->Data.SysEx.Id ))

            if (msg->Data.SysEx.Id == ReservedSysExIdRealTime){

                switch (msg->Data.SysEx.SubId1) {

                    case UniversalSysExRtMidiTimeCode:

                        switch (msg->Data.SysEx.SubId2) {

                            case UniversalSysExRtMidiTimeCodeFullMessage:
                                return packSysExMidiTimeCodeFullMessage( bytes, msg->Channel, &msg->Data.SysEx.Data.MidiTimeCode );

                            case UniversalSysExRtMidiTimeCodeUserBits:
                                return packSysExMidiTimeCodeUserBits(bytes, msg->Channel, msg->Data.SysEx.ByteData);

                        }

                        return 0;

                    case UniversalSysExRtMidiShowControl:
                        //TODO
                        return false;

                    case UniversalSysExRtDeviceControl:

                        switch(msg->Data.SysEx.SubId2) {
                            case UniversalSysExRtDeviceControlMasterVolume:
                            case UniversalSysExRtDeviceControlMasterBalance:
                            case UniversalSysExRtDeviceControlMasterCoarseTuning:
                            case UniversalSysExRtDeviceControlMasterFineTuning:
                                return packDeviceControl( bytes, msg->Channel, (UniversalSysExRtDeviceControl_t)msg->Data.SysEx.SubId2, msg->Data.SysEx.Data.DeviceControlValue );

                            case UniversalSysExRtDeviceControlGlobalParameterControl:
                                return packGlobalParameterControl( bytes, msg->Channel, &msg->Data.SysEx.Data.GlobalParameterControl);

                        }

                        return false;

                    case UniversalSysExRtMidiTimeCodeCueing:
                        return packSysExRtMidiCueingSetupMessage( bytes, (UniversalSysExRtMidiTimeCodeCueing_t)msg->Data.SysEx.SubId2, msg->Channel, msg->Data.SysEx.Data.Cueing.EventNumber, msg->Data.SysEx.ByteData, msg->Data.SysEx.Length );

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

            } // (msg->Data.SysEx.Id == ReservedSysExIdRealTime)

            if (msg->Data.SysEx.Id == ReservedSysExIdNonRealTime){


                switch (msg->Data.SysEx.SubId1) {
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

                        return packSysExNonRtMidiCueingSetupMessage( bytes, (UniversalSysExNonRtMidiTimeCode_t)msg->Data.SysEx.SubId2, msg->Channel, &msg->Data.SysEx.Data.Cueing.MidiTimeCode, msg->Data.SysEx.Data.Cueing.EventNumber, msg->Data.SysEx.ByteData, msg->Data.SysEx.Length );


                    case UniversalSysExNonRtSampleDumpExtension:
                        //TODO
                        return false;

                    case UniversalSysExNonRtGeneralInformation:

                        switch(msg->Data.SysEx.SubId2) {
                            case UniversalSysExNonRtGeneralInformationIdentityRequest:
                                return packGeneralInformationIdentityRequest( bytes, msg->Channel );

                            case UniversalSysExNonRtGeneralInformationIdentityReply:
                                return packGeneralInformationIdentityReply( bytes, msg->Channel, msg->Data.SysEx.Data.GeneralInfo.ManufacturerId, msg->Data.SysEx.Data.GeneralInfo.DeviceFamily, msg->Data.SysEx.Data.GeneralInfo.DeviceFamilyMember, msg->Data.SysEx.Data.GeneralInfo.SoftwareRevision );
                        }

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

                return 0;
            } // (msg->Data.SysEx.Id == ReservedSysExIdNonRealTime)


        } // msg->Status == SystemMessageSystemExclusive



        ASSERT( false ); // should never reach here. You only do, when the given parameters are invalid.

        return 0;
    }

    bool unpack( uint8_t * bytes, uint32_t length, Message_t * msg ){

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

            msg->StatusClass = StatusClassSystemMessage;
            msg->Status = bytes[0];

            // minimum of status byte and manufacturerid (assuming and optional ending)
            if (length < 2) {
                return false;
            }

            if (bytes[1] == ReservedSysExIdExperimental) {

                msg->Data.SysEx.Id = ReservedSysExIdExperimental;
                msg->Data.SysEx.Length = length - 2;

                if (length > 2) {
#if SYSEX_MEMORY == SYSEX_MEMORY_STATIC
                    ASSERT( length - 2 <= SYSEX_MEMORY_STATIC_SIZE );
#elif SYSEX_MEMORY == SYSEX_MEMORY_DYNAMIC
                    msg->Data.SysEx.ByteData = (uint8_t*)calloc((length - 2), 1);
#endif
                    memcpy( msg->Data.SysEx.ByteData, &bytes[2], length - 2 );
                }
                return true;
            } // (bytes[1] == ReservedSysExIdExperimental)

            if (isManufacturerSysExId(bytes[1])){

                // is it a three byte manufacturer id?
                if (bytes[1] == ReservedSysExIdManufacturerExtension) {
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
                    ASSERT( msg->Data.SysEx.Length <= SYSEX_MEMORY_STATIC_SIZE );
#elif SYSEX_MEMORY == SYSEX_MEMORY_DYNAMIC
                    msg->Data.SysEx.ByteData = (uint8_t*)calloc(msg->Data.SysEx.Length, 1);
#endif
                    memcpy( msg->Data.SysEx.ByteData, &bytes[1], msg->Data.SysEx.Length);
                }
                return true;
            } // isManufacturerSysExId(bytes[1])

            if (bytes[1] == ReservedSysExIdRealTime) {
                if (length < 4) {
                    return false;
                }
                msg->Data.SysEx.Id = ReservedSysExIdRealTime;
                msg->Channel = bytes[2]; // DeviceId
                msg->Data.SysEx.SubId1 = bytes[3];

                switch (bytes[3]) {
                    case UniversalSysExRtMidiTimeCode:

                        if (length < 5) {
                            return false;
                        }

                        msg->Data.SysEx.SubId2 = bytes[4];

                        switch (bytes[4]) {

                            case UniversalSysExRtMidiTimeCodeFullMessage:
                                return unpackSysExMidiTimeCodeFullMessage( bytes, length, &msg->Channel, &msg->Data.SysEx.Data.MidiTimeCode );

                            case UniversalSysExRtMidiTimeCodeUserBits:
                                if (length < 13) {
                                    return false;
                                }
                                msg->Data.SysEx.Length = 5;

#if SYSEX_MEMORY == SYSEX_MEMORY_STATIC
                                ASSERT( msg->Data.SysEx.Length <= SYSEX_MEMORY_STATIC_SIZE );
#elif SYSEX_MEMORY == SYSEX_MEMORY_DYNAMIC
                                msg->Data.SysEx.ByteData = (uint8_t*)calloc(msg->Data.SysEx.Length, 1);
#endif
                                if ( !unpackSysExMidiTimeCodeUserBits(bytes, length, &msg->Channel, msg->Data.SysEx.ByteData) ){
#if SYSEX_MEMORY == SYSEX_MEMORY_DYNAMIC
                                    free(msg->Data.SysEx.ByteData);
#endif
                                    return false;
                                }

                                return true;
                        }
                        return false;

                    case UniversalSysExRtMidiShowControl:
                        //TODO
                        return false;

                    case UniversalSysExRtDeviceControl:

                        if (length < 7 || !isUniversalSysExRtDeviceControl(bytes[4])){
                            return false;
                        }
                        msg->Data.SysEx.SubId2 = bytes[4];

                        switch(bytes[4]) {
                            case UniversalSysExRtDeviceControlMasterVolume:
                            case UniversalSysExRtDeviceControlMasterBalance:
                            case UniversalSysExRtDeviceControlMasterCoarseTuning:
                            case UniversalSysExRtDeviceControlMasterFineTuning:

                                return unpackDeviceControl( bytes, length, &msg->Channel, (UniversalSysExRtDeviceControl_t*)&msg->Data.SysEx.SubId2, &msg->Data.SysEx.Data.DeviceControlValue );

                            case UniversalSysExRtDeviceControlGlobalParameterControl:

#if SYSEX_MEMORY == SYSEX_MEMORY_STATIC
                                ASSERT( length - 8 <= SYSEX_MEMORY_STATIC_SIZE );
#elif SYSEX_MEMORY == SYSEX_MEMORY_DYNAMIC
                                msg->Data.SysEx.ByteData = (uint8_t*)calloc(length - 8, 1);
#endif
                                // use sysex raw byte data container for gpc
                                msg->Data.SysEx.Data.GlobalParameterControl.Data = msg->Data.SysEx.ByteData;

                                if ( ! unpackGlobalParameterControl( bytes, length, &msg->Channel, &msg->Data.SysEx.Data.GlobalParameterControl )){
#if SYSEX_MEMORY == SYSEX_MEMORY_DYNAMIC
                                    free( msg->Data.SysEx.ByteData );
#endif
                                }

                                msg->Data.SysEx.Length = msg->Data.SysEx.Data.GlobalParameterControl.DataLength;

                                return true;
                        }

                        return false;

                    case UniversalSysExRtMidiTimeCodeCueing:

                        if (length < MsgLenSysExRtMidiCueingSetupMessageMin - 1) {
                            return false;
                        }

                        if ( isUniversalSysExRtMidiTimeCodeCueingWithAddInfo(bytes[4]) ){
#if SYSEX_MEMORY == SYSEX_MEMORY_STATIC
                            ASSERT( length - MsgLenSysExRtMidiCueingSetupMessageMin <= SYSEX_MEMORY_STATIC_SIZE );
#elif SYSEX_MEMORY == SYSEX_MEMORY_DYNAMIC
                            msg->Data.SysEx.ByteData = (uint8_t*)calloc(length - MsgLenSysExRtMidiCueingSetupMessageMin, 1);
#endif
                        }

                        if ( ! unpackSysExRtMidiCueingSetupMessage( bytes, length, (UniversalSysExRtMidiTimeCodeCueing_t*)&msg->Data.SysEx.SubId2, &msg->Channel, &msg->Data.SysEx.Data.Cueing.EventNumber, msg->Data.SysEx.ByteData, &msg->Data.SysEx.Length )){
#if SYSEX_MEMORY == SYSEX_MEMORY_DYNAMIC
                            if ( isUniversalSysExRtMidiTimeCodeCueingWithAddInfo(bytes[4]) ) {
                                free(msg->Data.SysEx.ByteData);
                            }
#endif
                            return false;
                        }

                        return true;

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
            } // (bytes[1] == ReservedSysExIdRealTime)

            if (bytes[1] == ReservedSysExIdNonRealTime) {
                if (length < 4) {
                    return false;
                }
                msg->Data.SysEx.Id = ReservedSysExIdNonRealTime;
                msg->Channel = bytes[2]; // deviceId
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

                        if (length < MsgLenSysExNonRtMidiCueingSetupMessageMin - 1) {
                            return false;
                        }

                        if ( isUniversalSysExNonRtMidiTimeCodeWithAddInfo(bytes[4]) ){
#if SYSEX_MEMORY == SYSEX_MEMORY_STATIC
                            ASSERT( length - MsgLenSysExRtMidiCueingSetupMessageMin <= SYSEX_MEMORY_STATIC_SIZE );
#elif SYSEX_MEMORY == SYSEX_MEMORY_DYNAMIC
                            msg->Data.SysEx.ByteData = (uint8_t*)calloc(length - MsgLenSysExNonRtMidiCueingSetupMessageMin, 1);
#endif
                        }

                        if ( ! unpackSysExNonRtMidiCueingSetupMessage( bytes, length, (UniversalSysExNonRtMidiTimeCode_t*)&msg->Data.SysEx.SubId2, &msg->Channel, &msg->Data.SysEx.Data.Cueing.MidiTimeCode, &msg->Data.SysEx.Data.Cueing.EventNumber, msg->Data.SysEx.ByteData, &msg->Data.SysEx.Length )){
#if SYSEX_MEMORY == SYSEX_MEMORY_DYNAMIC
                            if ( isUniversalSysExNonRtMidiTimeCodeWithAddInfo(bytes[4]) ) {
                                free(msg->Data.SysEx.ByteData);
                            }
#endif
                            return false;
                        }

                        return true;
                        return false;

                    case UniversalSysExNonRtSampleDumpExtension:
                        //TODO
                        return false;

                    case UniversalSysExNonRtGeneralInformation:

                        switch(msg->Data.SysEx.SubId2) {
                            case UniversalSysExNonRtGeneralInformationIdentityRequest:
                                // no further data
                                return true;

                            case UniversalSysExNonRtGeneralInformationIdentityReply:
                                return unpackGeneralInformationIdentityReply( bytes, length, &msg->Channel, &msg->Data.SysEx.Data.GeneralInfo.ManufacturerId, &msg->Data.SysEx.Data.GeneralInfo.DeviceFamily, &msg->Data.SysEx.Data.GeneralInfo.DeviceFamilyMember, msg->Data.SysEx.Data.GeneralInfo.SoftwareRevision );
                        }

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

            } // (bytes[1] == ReservedSysExIdNonRealTime)

        } // (msg->Status == SystemMessageSystemExclusive)

        return false;
    }

#if SYSEX_MEMORY == SYSEX_MEMORY_DYNAMIC
    void freeMessage( Message_t * msg ){

        if (msg->Status == SystemMessageSystemExclusive &&
                msg->Data.SysEx.Length > 0 &&
                msg->Data.SysEx.ByteData != NULL){

            free(msg->Data.SysEx.ByteData);

            msg->Data.SysEx.Length = 0;
            msg->Data.SysEx.ByteData = NULL;
        }
    }
#endif



}