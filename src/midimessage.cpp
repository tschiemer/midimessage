//
// Created by Philip Tschiemer on 19.07.19.
//


#include <stddef.h>

#include <midimessage.h>
#include <midimessage/packers.h>


#ifdef __cplusplus
namespace MidiMessage {
    extern "C" {
#endif

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
                    return packNoteOffObj(bytes, msg);

                case StatusClassNoteOn:
                    return packNoteOnObj(bytes, msg);

                case StatusClassPolyphonicKeyPressure:
                    return packPolyphonicKeyPressureObj(bytes, msg);

                case StatusClassControlChange:
                    return packControlChangeObj(bytes, msg);

                case StatusClassProgramChange:
                    return packProgramChangeObj(bytes, msg);

                case StatusClassChannelPressure:
                    return packChannelPressureObj(bytes, msg);

                case StatusClassPitchBendChange:
                    return packPitchBendChangeObj(bytes, msg);

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
                    return packMtcQuarterFrameObj( bytes, msg );

                case SystemMessageSongPositionPointer:
                    return packSongPositionPointerObj( bytes, msg );

                case SystemMessageSongSelect:
                    return packSongSelectObj( bytes, msg );

                case SystemMessageTuneRequest:
                case SystemMessageEndOfExclusive:
                case SystemMessageTimingClock:
                case SystemMessageStart:
                case SystemMessageContinue:
                case SystemMessageStop:
                case SystemMessageActiveSensing:
                case SystemMessageReset:
                    return packSystemMessageObj( bytes, msg );

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
                return packSysExExperimentalMessageObj( bytes, msg );
            } // (msg->Data.SysEx.Id == SysExIdExperimental)

            if (isSysExManufacturerId( msg->Data.SysEx.Id )){
                return packSysExManufacturerMessageObj( bytes, msg );
            } // (isSysExManufacturerId( msg->Data.SysEx.Id ))

            if (msg->Data.SysEx.Id == SysExIdRealTime){

                switch (msg->Data.SysEx.SubId1) {

                    case SysExRtMidiTimeCode:
                        return packSysExRtMidiTimeCodeObj(bytes, msg);

                    case SysExRtMidiShowControl:
                        return packSysExRtMidiShowControlObj( bytes, msg );

                    case SysExRtDeviceControl:
                        return packSysExRtDeviceControlObj( bytes, msg );

                    case SysExRtMidiTimeCodeCueing:
                        return packSysExRtMtcCueingSetupMessageObj( bytes, msg );

                    case SysExRtMidiMachineControlCommand:
                        return packSysExRtMmcCommandMessageObj( bytes, msg );

                    case SysExRtMidiMachineControlResponse:
                        return packSysExRtMmcResponseMessageObj( bytes, msg );

                    case SysExRtMidiTuningStandard:
                        //TODO
                        return false;

                    case SysExRtControllerDestinationSetting:
                        return packSysExRtControllerDestinationSettingObj( bytes, msg );

                    case SysExRtKeybasedInstrumentControl:
                        return packSysExRtKeybasedInstrumentControlObj( bytes, msg );

                    case SysExRtScalablePolyphonyMidiMip:
                        //TODO
                        return false;

                    case SysExRtMobilePhoneControlMessage:
                        return packSysExRtMobilePhoneControlObj( bytes, msg );
                }

            } // (msg->Data.SysEx.Id == SysExIdRealTime)

            if (msg->Data.SysEx.Id == SysExIdNonRealTime){


                switch (msg->Data.SysEx.SubId1) {
                    case SysExNonRtSampleDumpHeader:
                        return packSysExNonRtSdsHeaderObj( bytes, msg );

                    case SysExNonRtSampleDataPacket:
                        return packSysExNonRtSdsDataPacketObj( bytes, msg );

                    case SysExNonRtSampleDumpRequest:
                        return packSysExNonRtSdsRequestObj( bytes, msg );

                    case SysExNonRtMidiTimeCode:
                        return packSysExNonRtMtcCueingSetupMessageObj( bytes, msg );

                    case SysExNonRtSampleDumpExtension:
                        return packSysExNonRtSdsExtObj( bytes, msg );

                    case SysExNonRtGeneralInformation:
                        return packSysExNonRtGeneralInformationObj( bytes, msg );

                    case SysExNonRtFileDump:
                        //TODO
                        return false;

                    case SysExNonRtMidiTuningStandard:
                        return packSysExNonRtTuningObj( bytes, msg );

                    case SysExNonRtGeneralMidi:
                        return packSysExNonRtGeneralMidiObj( bytes, msg );

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
                        return packSysExNonRtHandshakeObj( bytes, msg );

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
                    return unpackNoteOffObj(bytes, length, msg);

                case StatusClassNoteOn:
                    return unpackNoteOnObj(bytes, length, msg);

                case StatusClassPolyphonicKeyPressure:
                    return unpackPolyphonicKeyPressureObj(bytes, length, msg);

                case StatusClassControlChange:
                    return unpackControlChangeObj(bytes, length, msg);

                case StatusClassProgramChange:
                    return unpackProgramChangeObj(bytes, length, msg);

                case StatusClassChannelPressure:
                    return unpackChannelPressureObj(bytes, length, msg);

                case StatusClassPitchBendChange:
                    return unpackPitchBendChangeObj(bytes, length, msg);

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
                    return unpackMtcQuarterFrameObj( bytes, length, msg );

                case SystemMessageSongPositionPointer:
                    return unpackSongPositionPointerObj( bytes, length, msg );

                case SystemMessageSongSelect:
                    return unpackSongSelectObj( bytes, length, msg );

                case SystemMessageTuneRequest:
                case SystemMessageEndOfExclusive:
                case SystemMessageTimingClock:
                case SystemMessageStart:
                case SystemMessageContinue:
                case SystemMessageStop:
                case SystemMessageActiveSensing:
                case SystemMessageReset:
                    return unpackSystemMessageObj( bytes, length, msg );

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

            if (bytes[1] == SysExIdExperimental_Byte) {
                return unpackSysExExperimentalMessageObj( bytes, length, msg );
            } // (bytes[1] == SysExIdExperimental)

            if (isSysExManufacturerIdByte(bytes[1])){
                return unpackSysExManufacturerMessageObj( bytes, length, msg );
            } // isSysExManufacturerId(bytes[1])

            if (bytes[1] == SysExIdRealTime_Byte) {
                if (length < 4) {
                    return false;
                }

                switch (bytes[3]) {
                    case SysExRtMidiTimeCode:
                        return unpackSysExRtMidiTimeCodeObj(bytes, length, msg);

                    case SysExRtMidiShowControl:
                        return unpackSysExRtMidiShowControlObj(bytes, length, msg);

                    case SysExRtDeviceControl:
                        return unpackSysExRtDeviceControlObj( bytes, length, msg );

                    case SysExRtMidiTimeCodeCueing:
                        return unpackSysExRtMtcCueingSetupMessageObj( bytes, length, msg );

                    case SysExRtMidiMachineControlCommand:
                        return unpackSysExRtMmcCommandMessageObj( bytes, length, msg);

                    case SysExRtMidiMachineControlResponse:
                        return unpackSysExRtMmcResponseMessageObj( bytes, length, msg);

                    case SysExRtMidiTuningStandard:
                        //TODO
                        return false;

                    case SysExRtControllerDestinationSetting:
                        return unpackSysExRtControllerDestinationSettingObj(bytes, length, msg);

                    case SysExRtKeybasedInstrumentControl:
                        return unpackSysExRtKeybasedInstrumentControlObj( bytes, length, msg );

                    case SysExRtScalablePolyphonyMidiMip:
                        //TODO
                        return false;

                    case SysExRtMobilePhoneControlMessage:
                        return unpackSysExRtMobilePhoneControlObj( bytes, length, msg);

                    default:
                        break;
                }

                return false;
            } // (bytes[1] == SysExIdRealTime)

            if (bytes[1] == SysExIdNonRealTime_Byte) {
                if (length < 4) {
                    return false;
                }

//                msg->Data.SysEx.Id = SysExIdNonRealTime;
//                msg->Channel = bytes[2]; // deviceId
//                msg->Data.SysEx.SubId1 = bytes[3];

                switch (bytes[3]) {
                    case SysExNonRtSampleDumpHeader:
                        return unpackSysExNonRtSdsHeaderObj( bytes, length, msg );

                    case SysExNonRtSampleDataPacket:
                        return unpackSysExNonRtSdsDataPacketObj( bytes, length, msg );

                    case SysExNonRtSampleDumpRequest:
                        return unpackSysExNonRtSdsRequestObj( bytes, length, msg );

                    case SysExNonRtMidiTimeCode:
                        return unpackSysExNonRtMtcCueingSetupMessageObj( bytes, length, msg );

                    case SysExNonRtSampleDumpExtension:
                        return unpackSysExNonRtSdsExtObj( bytes, length, msg);

                    case SysExNonRtGeneralInformation:
                        return unpackSysExNonRtGeneralInformationObj( bytes, length, msg );


                    case SysExNonRtFileDump:
                        //TODO
                        return false;

                    case SysExNonRtMidiTuningStandard:
                        return unpackSysExNonRtTuningObj( bytes, length, msg );

                    case SysExNonRtGeneralMidi:
                        return unpackSysExNonRtGeneralMidiObj( bytes, length, msg );

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
                        return unpackSysExNonRtHandshakeObj(bytes, length, msg);

                }

                return false;

            } // (bytes[1] == SysExIdNonRealTime_Byte)

        } // (msg->Status == SystemMessageSystemExclusive)

        return false;
    }

#ifdef __cplusplus
} // extern "C"
} // namespace MidiMessage
#endif