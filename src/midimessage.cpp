//
// Created by Philip Tschiemer on 19.07.19.
//

#include <midimessage.h>
#include <mmstdlib.h>
#include <cstddef>



namespace MidiMessage {

    uint8_t pack( uint8_t * bytes, Message_t * msg ){
        ASSERT( bytes != NULL);
        ASSERT( msg != NULL );

        int len;

        if (isStatusClass(msg->StatusClass) && msg->StatusClass != StatusClassSystemMessage) {
            switch (msg->StatusClass) {

                case StatusClassNoteOff:
                    return packNoteOff(bytes, msg->Channel, msg->Data.Note.Key, msg->Data.Note.Velocity);

                case StatusClassNoteOn:
                    return packNoteOn(bytes, msg->Channel, msg->Data.Note.Key, msg->Data.Note.Velocity);

                case StatusClassPolyphonicKeyPressure:
                    return packPolyphonicKeyPressure(bytes, msg->Channel, msg->Data.PolyphonicKeyPressure.Key, msg->Data.PolyphonicKeyPressure.Pressure);

                case StatusClassControlChange:
                    return packControlChange(bytes, msg->Channel, msg->Data.ControlChange.Controller, msg->Data.ControlChange.Value);

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
                case SystemMessageMtcQuarterFrame:
                    return packMtcQuarterFrame( bytes, msg->Data.MtcQuarterFrame.MessageType, msg->Data.MtcQuarterFrame.Nibble );

                case SystemMessageSongPositionPointer:
                    return packSongPositionPointer( bytes, msg->Data.SongPositionPointer.Position );

                case SystemMessageSongSelect:
                    return packSongSelect( bytes, msg->Data.SongSelect.Song );

                case SystemMessageTuneRequest:
                case SystemMessageEndOfExclusive:
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

            if (msg->Data.SysEx.Id == SysExIdExperimental){

                bytes[1] = SysExIdExperimental;

                if (msg->Data.SysEx.Length > 0) {

#if SYSEX_MEMORY == SYSEX_MEMORY_DYNAMIC
                    ASSERT( msg->Data.SysEx.ByteData != NULL );
#endif
                    memcpy(&bytes[2], msg->Data.SysEx.ByteData, msg->Data.SysEx.Length);
                }

                return 2 + msg->Data.SysEx.Length;
            } // (msg->Data.SysEx.Id == SysExIdExperimental)

            if (isSysExManufacturerId( msg->Data.SysEx.Id )){

                len = 1 + packSysExId( bytes, msg->Data.SysEx.Id );

                if (msg->Data.SysEx.Length > 0) {
#if SYSEX_MEMORY == SYSEX_MEMORY_DYNAMIC
                    ASSERT( msg->Data.SysEx.ByteData != NULL );
#endif
                    memcpy(&bytes[2], msg->Data.SysEx.ByteData, msg->Data.SysEx.Length);
                }

                return len + msg->Data.SysEx.Length;
            } // (isSysExManufacturerId( msg->Data.SysEx.Id ))

            if (msg->Data.SysEx.Id == SysExIdRealTime){

                switch (msg->Data.SysEx.SubId1) {

                    case SysExRtMidiTimeCode:

                        switch (msg->Data.SysEx.SubId2) {

                            case SysExRtMidiTimeCodeFullMessage:
                                return packSysExMidiTimeCodeFullMessage( bytes, msg->Channel, &msg->Data.SysEx.Data.MidiTimeCode );

                            case SysExRtMidiTimeCodeUserBits:
                                return packSysExMidiTimeCodeUserBits(bytes, msg->Channel, msg->Data.SysEx.ByteData);

                        }

                        return 0;

                    case SysExRtMidiShowControl:
                        //TODO
                        return false;

                    case SysExRtDeviceControl:

                        switch(msg->Data.SysEx.SubId2) {
                            case SysExRtDcMasterVolume:
                            case SysExRtDcMasterBalance:
                            case SysExRtDcMasterCoarseTuning:
                            case SysExRtDcMasterFineTuning:
                                return packDeviceControl( bytes, msg->Channel, (SysExRtDc_t)msg->Data.SysEx.SubId2, msg->Data.SysEx.Data.DeviceControlValue );

                            case SysExRtDcGlobalParameterControl:
                                return packGlobalParameterControl( bytes, msg->Channel, &msg->Data.SysEx.Data.GlobalParameterControl);

                        }

                        return false;

                    case SysExRtMidiTimeCodeCueing:
                        return packSysExRtMtcCueingSetupMessage( bytes, (SysExRtMtcCueing_t)msg->Data.SysEx.SubId2, msg->Channel, msg->Data.SysEx.Data.Cueing.EventNumber, msg->Data.SysEx.ByteData, msg->Data.SysEx.Length );

                    case SysExRtMidiMachineControlCommand:
                        return packMmcCommand( bytes, msg->Channel, (SysExRtMmcCommand_t)msg->Data.SysEx.SubId2, msg->Data.SysEx.ByteData, msg->Data.SysEx.Length );

                    case SysExRtMidiMachineControlResponse:
                        //TODO
                        return false;

                    case SysExRtMidiTuningStandard:
                        //TODO
                        return false;

                    case SysExRtControllerDestinationSetting:
                        //TODO
                        return false;

                    case SysExRtKeybasedInstrumentControl:
                        //TODO
                        return false;

                    case SysExRtScalablePolyphonyMidiMip:
                        //TODO
                        return false;

                    case SysExRtMobilePhoneControlMessage:
                        //TODO
                        return false;
                }

            } // (msg->Data.SysEx.Id == SysExIdRealTime)

            if (msg->Data.SysEx.Id == SysExIdNonRealTime){


                switch (msg->Data.SysEx.SubId1) {
                    case SysExNonRtSampleDumpHeader:
                        //TODO
                        return false;

                    case SysExNonRtSampleDataPacket:
                        //TODO
                        return false;

                    case SysExNonRtSampleDumpRequest:
                        //TODO
                        return false;

                    case SysExNonRtMidiTimeCode:

                        return packSysExNonRtMtcCueingSetupMessage( bytes, (SysExNonRtMtc_t)msg->Data.SysEx.SubId2, msg->Channel, &msg->Data.SysEx.Data.Cueing.MidiTimeCode, msg->Data.SysEx.Data.Cueing.EventNumber, msg->Data.SysEx.ByteData, msg->Data.SysEx.Length );


                    case SysExNonRtSampleDumpExtension:
                        //TODO
                        return false;

                    case SysExNonRtGeneralInformation:

                        switch(msg->Data.SysEx.SubId2) {
                            case SysExNonRtGenInfoIdentityRequest:
                                return packGeneralInformationIdentityRequest( bytes, msg->Channel );

                            case SysExNonRtGenInfoIdentityReply:
                                return packGeneralInformationIdentityReply( bytes, msg->Channel, msg->Data.SysEx.Data.GeneralInfo.ManufacturerId, msg->Data.SysEx.Data.GeneralInfo.DeviceFamily, msg->Data.SysEx.Data.GeneralInfo.DeviceFamilyMember, msg->Data.SysEx.Data.GeneralInfo.SoftwareRevision );
                        }

                        return false;

                    case SysExNonRtFileDump:
                        //TODO
                        return false;

                    case SysExNonRtMidiTuningStandard:
                        //TODO
                        return false;

                    case SysExNonRtGeneralMidi:
                        //TODO
                        return false;

                    case SysExNonRtDownloadableSounds:
                        //TODO
                        return false;

                    case SysExNonRtFileReferenceMessage:
                        //TODO
                        return false;

                    case SysExNonRtMidiVisualControl:
                        //TODO
                        return false;

                    case SysExNonRtMidiCapabilityInquiry:
                        //TODO
                        return false;

                    case SysExNonRtEndOfFile:
                    case SysExNonRtWait:
                    case SysExNonRtCancel:
                    case SysExNonRtNAK:
                    case SysExNonRtACK:
                        // no further data
                        return true;

                }

                return 0;
            } // (msg->Data.SysEx.Id == SysExIdNonRealTime)


        } // msg->Status == SystemMessageSystemExclusive



        ASSERT( false ); // should never reach here. You only do, when the given parameters are invalid.

        return 0;
    }

    bool unpack( uint8_t * bytes, uint8_t length, Message_t * msg ){

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
                    return unpackNoteOff(bytes, length, &msg->Channel, &msg->Data.Note.Key,
                                         &msg->Data.Note.Velocity);

                case StatusClassNoteOn:
                    return unpackNoteOn(bytes, length, &msg->Channel, &msg->Data.Note.Key,
                                        &msg->Data.Note.Velocity);

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

                case SystemMessageMtcQuarterFrame:
                    return unpackMtcQuarterFrame( bytes, length, &msg->Data.MtcQuarterFrame.MessageType, &msg->Data.MtcQuarterFrame.Nibble );

                case SystemMessageSongPositionPointer:
                    return unpackSongPositionPointer( bytes, length, &msg->Data.SongPositionPointer.Position );

                case SystemMessageSongSelect:
                    return unpackSongSelect( bytes, length, &msg->Data.SongSelect.Song );

                case SystemMessageTuneRequest:
                case SystemMessageEndOfExclusive:
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

            if (bytes[1] == SysExIdExperimental) {

                msg->Data.SysEx.Id = SysExIdExperimental;
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
            } // (bytes[1] == SysExIdExperimental)

            if (isSysExManufacturerId(bytes[1])){

                // is it a three byte manufacturer id?
                if (bytes[1] == SysExIdManufacturerExtension) {
                    if (length < 4) {
                        return false;
                    }
                }

                msg->Data.SysEx.Length = length - unpackSysExId( &bytes[1], &msg->Data.SysEx.Id );

                if (msg->Data.SysEx.Length > 0) {
#if SYSEX_MEMORY == SYSEX_MEMORY_STATIC
                    ASSERT( msg->Data.SysEx.Length <= SYSEX_MEMORY_STATIC_SIZE );
#elif SYSEX_MEMORY == SYSEX_MEMORY_DYNAMIC
                    msg->Data.SysEx.ByteData = (uint8_t*)calloc(msg->Data.SysEx.Length, 1);
#endif
                    memcpy( msg->Data.SysEx.ByteData, &bytes[1], msg->Data.SysEx.Length);
                }
                return true;
            } // isSysExManufacturerId(bytes[1])

            if (bytes[1] == SysExIdRealTime) {
                if (length < 4) {
                    return false;
                }
                msg->Data.SysEx.Id = SysExIdRealTime;
                msg->Channel = bytes[2]; // DeviceId
                msg->Data.SysEx.SubId1 = bytes[3];

                switch (bytes[3]) {
                    case SysExRtMidiTimeCode:

                        if (length < 5) {
                            return false;
                        }

                        msg->Data.SysEx.SubId2 = bytes[4];

                        switch (bytes[4]) {

                            case SysExRtMidiTimeCodeFullMessage:
                                return unpackSysExMidiTimeCodeFullMessage( bytes, length, &msg->Channel, &msg->Data.SysEx.Data.MidiTimeCode );

                            case SysExRtMidiTimeCodeUserBits:
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

                    case SysExRtMidiShowControl:
                        //TODO
                        return false;

                    case SysExRtDeviceControl:

                        if (length < 7 || !isSysExRtDeviceControl(bytes[4])){
                            return false;
                        }
                        msg->Data.SysEx.SubId2 = bytes[4];

                        switch(bytes[4]) {
                            case SysExRtDcMasterVolume:
                            case SysExRtDcMasterBalance:
                            case SysExRtDcMasterCoarseTuning:
                            case SysExRtDcMasterFineTuning:

                                return unpackDeviceControl( bytes, length, &msg->Channel, (SysExRtDc_t*)&msg->Data.SysEx.SubId2, &msg->Data.SysEx.Data.DeviceControlValue );

                            case SysExRtDcGlobalParameterControl:

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

                    case SysExRtMidiTimeCodeCueing:

                        if (length < MsgLenSysExRtMtcCueingSetupMessageMin - 1) {
                            return false;
                        }
                        msg->Data.SysEx.SubId2 = bytes[4];

                        if ( isSysExRtMtcCueingWithAddInfo(bytes[4]) ){
#if SYSEX_MEMORY == SYSEX_MEMORY_STATIC
                            ASSERT( length - MsgLenSysExRtMtcCueingSetupMessageMin <= SYSEX_MEMORY_STATIC_SIZE );
#elif SYSEX_MEMORY == SYSEX_MEMORY_DYNAMIC
                            msg->Data.SysEx.ByteData = (uint8_t*)calloc(length - MsgLenSysExRtMtcCueingSetupMessageMin, 1);
#endif
                        }

                        if ( ! unpackSysExRtMtcCueingSetupMessage( bytes, length, (SysExRtMtcCueing_t*)&msg->Data.SysEx.SubId2, &msg->Channel, &msg->Data.SysEx.Data.Cueing.EventNumber, msg->Data.SysEx.ByteData, &msg->Data.SysEx.Length )){
#if SYSEX_MEMORY == SYSEX_MEMORY_DYNAMIC
                            if ( isSysExRtMtcCueingWithAddInfo(bytes[4]) ) {
                                free(msg->Data.SysEx.ByteData);
                            }
#endif
                            return false;
                        }

                        return true;

                    case SysExRtMidiMachineControlCommand:

                        if (length < MsgLenSysExRtMmcCommandWithoutInfo){
                            return false;
                        }

                        if (length > MsgLenSysExRtMmcCommandWithoutInfo){
#if SYSEX_MEMORY == SYSEX_MEMORY_STATIC
                            ASSERT( length - MsgLenSysExRtMmcCommandWithoutInfo <= SYSEX_MEMORY_STATIC_SIZE );
#elif SYSEX_MEMORY == SYSEX_MEMORY_DYNAMIC
                            msg->Data.SysEx.ByteData = (uint8_t*)calloc(length - MsgLenSysExRtMmcCommandWithoutInfo, 1);
#endif
                        }

                        msg->Data.SysEx.SubId2 = bytes[4];

                        if ( ! unpackMmcCommand( bytes, length, &msg->Channel, (SysExRtMmcCommand_t*)&msg->Data.SysEx.SubId2, msg->Data.SysEx.ByteData, &msg->Data.SysEx.Length) ){

#if SYSEX_MEMORY == SYSEX_MEMORY_DYNAMIC
                            if (length > MsgLenSysExRtMmcCommandWithoutInfo){
                                free( msg->Data.SysEx.ByteData );
                            }
#endif

                            return true;
                        }

                        return true;

                    case SysExRtMidiMachineControlResponse:
                        //TODO
                        return false;

                    case SysExRtMidiTuningStandard:
                        //TODO
                        return false;

                    case SysExRtControllerDestinationSetting:
                        //TODO
                        return false;

                    case SysExRtKeybasedInstrumentControl:
                        //TODO
                        return false;

                    case SysExRtScalablePolyphonyMidiMip:
                        //TODO
                        return false;

                    case SysExRtMobilePhoneControlMessage:
                        //TODO
                        return false;
                }

                return false;
            } // (bytes[1] == SysExIdRealTime)

            if (bytes[1] == SysExIdNonRealTime) {
                if (length < 4) {
                    return false;
                }
                msg->Data.SysEx.Id = SysExIdNonRealTime;
                msg->Channel = bytes[2]; // deviceId
                msg->Data.SysEx.SubId1 = bytes[3];

                switch (bytes[3]) {
                    case SysExNonRtSampleDumpHeader:
                        //TODO
                        return false;

                    case SysExNonRtSampleDataPacket:
                        //TODO
                        return false;

                    case SysExNonRtSampleDumpRequest:
                        //TODO
                        return false;

                    case SysExNonRtMidiTimeCode:

                        if (length < MsgLenSysExNonRtMtcCueingSetupMessageMin - 1) {
                            return false;
                        }

                        if ( isSysExNonRtMtcWithAddInfo(bytes[4]) ){
#if SYSEX_MEMORY == SYSEX_MEMORY_STATIC
                            ASSERT( length - MsgLenSysExRtMtcCueingSetupMessageMin <= SYSEX_MEMORY_STATIC_SIZE );
#elif SYSEX_MEMORY == SYSEX_MEMORY_DYNAMIC
                            msg->Data.SysEx.ByteData = (uint8_t*)calloc(length - MsgLenSysExNonRtMtcCueingSetupMessageMin, 1);
#endif
                        }

                        if ( ! unpackSysExNonRtMtcCueingSetupMessage( bytes, length, (SysExNonRtMtc_t*)&msg->Data.SysEx.SubId2, &msg->Channel, &msg->Data.SysEx.Data.Cueing.MidiTimeCode, &msg->Data.SysEx.Data.Cueing.EventNumber, msg->Data.SysEx.ByteData, &msg->Data.SysEx.Length )){
#if SYSEX_MEMORY == SYSEX_MEMORY_DYNAMIC
                            if ( isSysExNonRtMtcWithAddInfo(bytes[4]) ) {
                                free(msg->Data.SysEx.ByteData);
                            }
#endif
                            return false;
                        }

                        return true;
                        return false;

                    case SysExNonRtSampleDumpExtension:
                        //TODO
                        return false;

                    case SysExNonRtGeneralInformation:

                        switch(msg->Data.SysEx.SubId2) {
                            case SysExNonRtGenInfoIdentityRequest:
                                // no further data
                                return true;

                            case SysExNonRtGenInfoIdentityReply:
                                return unpackGeneralInformationIdentityReply( bytes, length, &msg->Channel, &msg->Data.SysEx.Data.GeneralInfo.ManufacturerId, &msg->Data.SysEx.Data.GeneralInfo.DeviceFamily, &msg->Data.SysEx.Data.GeneralInfo.DeviceFamilyMember, msg->Data.SysEx.Data.GeneralInfo.SoftwareRevision );
                        }

                        return false;

                    case SysExNonRtFileDump:
                        //TODO
                        return false;

                    case SysExNonRtMidiTuningStandard:
                        //TODO
                        return false;

                    case SysExNonRtGeneralMidi:
                        //TODO
                        return false;

                    case SysExNonRtDownloadableSounds:
                        //TODO
                        return false;

                    case SysExNonRtFileReferenceMessage:
                        //TODO
                        return false;

                    case SysExNonRtMidiVisualControl:
                        //TODO
                        return false;

                    case SysExNonRtMidiCapabilityInquiry:
                        //TODO
                        return false;

                    case SysExNonRtEndOfFile:
                    case SysExNonRtWait:
                    case SysExNonRtCancel:
                    case SysExNonRtNAK:
                    case SysExNonRtACK:
                        // no further data
                        return true;

                }

                return false;

            } // (bytes[1] == SysExIdNonRealTime)

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