//
// Created by Philip Tschiemer on 19.07.19.
//


#include <cstddef>

#include <midimessage.h>
#include <midimessage/packers.h>



namespace MidiMessage {

    inline void * memcpy( void * dst, void * src, uint8_t count ){
        for(uint8_t i = 0; i < count; i++){
            ((uint8_t*)dst)[i] = ((uint8_t*)src)[i];
        }
        return dst;
    }

    uint8_t pack( uint8_t * bytes, Message_t * msg ){
        ASSERT( bytes != NULL);
        ASSERT( msg != NULL );
        ASSERT( msg->Data.SysEx.ByteData != NULL );

        int len;

        if (isStatusClass(msg->StatusClass) && msg->StatusClass != StatusClassSystemMessage) {
            switch (msg->StatusClass) {

                case StatusClassNoteOff:
                    return packNoteOff(bytes, msg);

                case StatusClassNoteOn:
                    return packNoteOn(bytes, msg);

                case StatusClassPolyphonicKeyPressure:
                    return packPolyphonicKeyPressure(bytes, msg);

                case StatusClassControlChange:
                    return packControlChange(bytes, msg);

                case StatusClassProgramChange:
                    return packProgramChange(bytes, msg);

                case StatusClassChannelPressure:
                    return packChannelPressure(bytes, msg);

                case StatusClassPitchBendChange:
                    return packPitchBendChange(bytes, msg);

                case StatusClassSystemMessage:
                    // processed below
                    break;

                default:
                    ASSERT(false); // should not occur, if msg setup correctly
                    return 0;
            }
        }


        if (msg->StatusClass == StatusClassSystemMessage && isSystemMessage(msg->SystemMessage) && msg->SystemMessage != SystemMessageSystemExclusive) {

            switch (msg->SystemMessage)
            {
                case SystemMessageMtcQuarterFrame:
                    return packMtcQuarterFrame( bytes, msg );

                case SystemMessageSongPositionPointer:
                    return packSongPositionPointer( bytes, msg );

                case SystemMessageSongSelect:
                    return packSongSelect( bytes, msg );

                case SystemMessageTuneRequest:
                case SystemMessageEndOfExclusive:
                case SystemMessageTimingClock:
                case SystemMessageStart:
                case SystemMessageContinue:
                case SystemMessageStop:
                case SystemMessageActiveSensing:
                case SystemMessageReset:
                    return packSystemMessage( bytes, msg );

                case SystemMessageSystemExclusive:
                    // processed below
                    break;

                default:
                    ASSERT(false); // should not occur, if msg setup correctly
                    return 0;
            }
        }

        if (msg->StatusClass == StatusClassSystemMessage && msg->SystemMessage == SystemMessageSystemExclusive){
            
            bytes[0] = msg->SystemMessage;

            if (msg->Data.SysEx.Id == SysExIdExperimental){
                return packSysExExperimentalMessage( bytes, msg );
            } // (msg->Data.SysEx.Id == SysExIdExperimental)

            if (isSysExManufacturerId( msg->Data.SysEx.Id )){
                return packSysExManufacturerMessage( bytes, msg );
            } // (isSysExManufacturerId( msg->Data.SysEx.Id ))

            if (msg->Data.SysEx.Id == SysExIdRealTime){

                switch (msg->Data.SysEx.SubId1) {

                    case SysExRtMidiTimeCode:
                        return packSysExRtMidiTimeCode(bytes, msg);

                    case SysExRtMidiShowControl:
                        return packSysExRtMidiShowControl( bytes, msg );

                    case SysExRtDeviceControl:
                        return packSysExRtDeviceControl( bytes, msg );

                    case SysExRtMidiTimeCodeCueing:
                        return packSysExRtMtcCueingSetupMessage( bytes, msg );

                    case SysExRtMidiMachineControlCommand:
                        return packSysExRtMmcCommandMessage( bytes, msg );

                    case SysExRtMidiMachineControlResponse:
                        return packSysExRtMmcResponseMessage( bytes, msg );

                    case SysExRtMidiTuningStandard:
                        //TODO
                        return false;

                    case SysExRtControllerDestinationSetting:
                        return packSysExRtControllerDestinationSetting( bytes, msg );

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
                        return packSysExNonRtSdsHeader( bytes, msg );

                    case SysExNonRtSampleDataPacket:
                        return packSysExNonRtSdsDataPacket( bytes, msg );

                    case SysExNonRtSampleDumpRequest:
                        return packSysExNonRtSdsRequest( bytes, msg );

                    case SysExNonRtMidiTimeCode:
                        return packSysExNonRtMtcCueingSetupMessage( bytes, msg );

                    case SysExNonRtSampleDumpExtension:
                        return packSysExNonRtSdsExt( bytes, msg );

                    case SysExNonRtGeneralInformation:
                        return packSysExNonRtGeneralInformation( bytes, msg );

                    case SysExNonRtFileDump:
                        //TODO
                        return false;

                    case SysExNonRtMidiTuningStandard:
                        //TODO
                        return false;

                    case SysExNonRtGeneralMidi:


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
                        return packSysExNonRtHandshake( bytes, msg );

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
        ASSERT( msg->Data.SysEx.ByteData != NULL );

        if (length == 0){
            return false;
        }

        if ( ! isControlByte(bytes[0]) ){
            return false;
        }

        uint8_t statusClass = getStatusClass(bytes[0]);
        uint8_t systemMessage = bytes[0];

        if (isStatusClass(statusClass) && statusClass != StatusClassSystemMessage) {

            switch (statusClass) {

                case StatusClassNoteOff:
                    return unpackNoteOff(bytes, length, msg);

                case StatusClassNoteOn:
                    return unpackNoteOn(bytes, length, msg);

                case StatusClassPolyphonicKeyPressure:
                    return unpackPolyphonicKeyPressure(bytes, length, msg);

                case StatusClassControlChange:
                    return unpackControlChange(bytes, length, msg);

                case StatusClassProgramChange:
                    return unpackProgramChange(bytes, length, msg);

                case StatusClassChannelPressure:
                    return unpackChannelPressure(bytes, length, msg);

                case StatusClassPitchBendChange:
                    return unpackPitchBendChange(bytes, length, msg);

                case StatusClassSystemMessage:
                    // processed below
                    break;

                default:
                    return false;

            } // switch(msg->StatusClass)
        } // (isStatusClass(msg->StatusClass) && msg->StatusClass != StatusClassSystemMessage)


        if (statusClass == StatusClassSystemMessage && isSystemMessage(systemMessage) && systemMessage != SystemMessageSystemExclusive) {

            switch(systemMessage){

                case SystemMessageMtcQuarterFrame:
                    return unpackMtcQuarterFrame( bytes, length, msg );

                case SystemMessageSongPositionPointer:
                    return unpackSongPositionPointer( bytes, length, msg );

                case SystemMessageSongSelect:
                    return unpackSongSelect( bytes, length, msg );

                case SystemMessageTuneRequest:
                case SystemMessageEndOfExclusive:
                case SystemMessageTimingClock:
                case SystemMessageStart:
                case SystemMessageContinue:
                case SystemMessageStop:
                case SystemMessageActiveSensing:
                case SystemMessageReset:
                    return unpackSystemMessage( bytes, length, msg );

                case SystemMessageSystemExclusive:
                    // processed below
                    break;

                default:
                    return false;

            } // switch(msg->Status)

        } // (isSystemMessage(msg->Status) && msg->Status != SystemMessageSystemExclusive)


        if (statusClass == StatusClassSystemMessage && systemMessage == SystemMessageSystemExclusive) {

            // minimum of status byte and manufacturerid (assuming and optional ending)
            if (length < 2) {
                return false;
            }

            if (bytes[1] == SysExIdExperimentalByte) {
                return unpackSysExExperimentalMessage( bytes, length, msg );
            } // (bytes[1] == SysExIdExperimental)

            if (isSysExManufacturerIdByte(bytes[1])){
                return unpackSysExManufacturerMessage( bytes, length, msg );
            } // isSysExManufacturerId(bytes[1])

            if (bytes[1] == SysExIdRealTimeByte) {
                if (length < 4) {
                    return false;
                }

                switch (bytes[3]) {
                    case SysExRtMidiTimeCode:
                        return unpackSysExRtMidiTimeCode(bytes, length, msg);

                    case SysExRtMidiShowControl:
                        return unpackSysExRtMidiShowControl(bytes, length, msg);

                    case SysExRtDeviceControl:
                        return unpackSysExRtDeviceControl( bytes, length, msg );

                    case SysExRtMidiTimeCodeCueing:
                        return unpackSysExRtMtcCueingSetupMessage( bytes, length, msg );

                    case SysExRtMidiMachineControlCommand:
                        return unpackSysExRtMmcCommandMessage( bytes, length, msg);

                    case SysExRtMidiMachineControlResponse:
                        return unpackSysExRtMmcResponseMessage( bytes, length, msg);

                    case SysExRtMidiTuningStandard:
                        //TODO
                        return false;

                    case SysExRtControllerDestinationSetting:
                        return unpackSysExRtControllerDestinationSetting(bytes, length, msg);

                    case SysExRtKeybasedInstrumentControl:
                        //TODO
                        return false;

                    case SysExRtScalablePolyphonyMidiMip:
                        //TODO
                        return false;

                    case SysExRtMobilePhoneControlMessage:
                        //TODO
                        return false;

                    default:
                        break;
                }

                return false;
            } // (bytes[1] == SysExIdRealTime)

            if (bytes[1] == SysExIdNonRealTimeByte) {
                if (length < 4) {
                    return false;
                }

//                msg->Data.SysEx.Id = SysExIdNonRealTime;
//                msg->Channel = bytes[2]; // deviceId
//                msg->Data.SysEx.SubId1 = bytes[3];

                switch (bytes[3]) {
                    case SysExNonRtSampleDumpHeader:
                        return unpackSysExNonRtSdsHeader( bytes, length, msg );

                    case SysExNonRtSampleDataPacket:
                        return unpackSysExNonRtSdsDataPacket( bytes, length, msg );

                    case SysExNonRtSampleDumpRequest:
                        return unpackSysExNonRtSdsRequest( bytes, length, msg );

                    case SysExNonRtMidiTimeCode:
                        return unpackSysExNonRtMtcCueingSetupMessage( bytes, length, msg );

                    case SysExNonRtSampleDumpExtension:
                        return unpackSysExNonRtSdsExt( bytes, length, msg);

                    case SysExNonRtGeneralInformation:
                        return unpackSysExNonRtGeneralInformation( bytes, length, msg );


                    case SysExNonRtFileDump:
                        //TODO
                        return false;

                    case SysExNonRtMidiTuningStandard:
                        //TODO
                        return false;

                    case SysExNonRtGeneralMidi:
                        return unpackSysExNonRtGeneralMidi( bytes, length, msg );

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
                        return unpackSysExNonRtHandshake(bytes, length, msg);

                }

                return false;

            } // (bytes[1] == SysExIdNonRealTimeByte)

        } // (msg->Status == SystemMessageSystemExclusive)

        return false;
    }

}