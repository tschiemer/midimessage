//
// Created by Philip Tschiemer on 19.07.19.
//

#ifndef MIDIMESSAGE_H
#define MIDIMESSAGE_H

#include <cstdint>

#ifdef USE_ASSERTS
#ifndef ASSERT
#include <cassert>
#define ASSERT(x) assert(x)
#endif // ASSERT
#else
#define ASSERT(x)
#endif


#define SYSEX_MEMORY_STATIC 1
#define SYSEX_MEMORY_DYNAMIC 2

#ifndef SYSEX_MEMORY
//#define SYSEX_MEMORY SYSEX_MEMORY_STATIC
#define SYSEX_MEMORY SYSEX_MEMORY_DYNAMIC
#endif

#if SYSEX_MEMORY == SYSEX_MEMORY_STATIC
#ifndef SYSEX_MEMORY_STATIC_SIZE
#define SYSEX_MEMORY_STATIC_SIZE 128
#endif
#if SYSEX_MEMORY_STATIC_SIZE < 1
#warning If using static sysex memory please specify memory size
#endif
#endif // SYSEX_MEMORY == SYSEX_MEMORY_STATIC

namespace MidiMessage {

    // Constants & Enumerations
    /////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////

    /**
     * Non-Data Bytes always have a non-zero match
     */
    const uint8_t ControlByteMask = 0x80;

    inline bool isControlByte( uint8_t byte ) {
        return (byte & ControlByteMask) != 0;
    }



    const uint8_t DataMask = 0b01111111; // = 127
    const uint16_t DoubleValueMask  = 0b0111111101111111;

    const uint8_t MaxValue          = 127;
    const uint16_t MaxDoubleValue   = 0x3FFF;

    inline bool isDataByte( uint8_t byte ){
        return (byte & DataMask);
    }

    inline uint8_t getData( uint8_t value ){
        return value & DataMask;
    }

    inline void unpackDoubleValue( uint8_t * bytes, uint16_t * value ){
        ASSERT( bytes[0] & DataMask == bytes[0] );
        ASSERT( bytes[1] & DataMask == bytes[1] );

        *value = (((uint16_t)bytes[1]) << 8) | ((uint16_t)bytes[0]);
    }

    inline void packDoubleValue( uint8_t * bytes, uint16_t value ) {
        bytes[0] = value & DataMask;
        bytes[1] = (value >> 7) & DataMask;
    }



    const uint8_t StatusClassMask = 0xF0;
    const uint8_t ChannelMask     = 0x0F;

    inline uint8_t getChannel( uint8_t value ){
        return value & ChannelMask;
    }



    const int MsgLenNoteOff                         = 3;
    const int MsgLenNoteOn                          = 3;
    const int MsgLenPolyphonicKeyPressure           = 3;
    const int MsgLenControlChange                   = 3;
    const int MsgLenProgramChange                   = 2;
    const int MsgLenChannelPressure                 = 2;
    const int MsgLenPitchBendChange                 = 3;
    const int MsgLenMidiTimeCodeQuarterFrame        = 2;
    const int MsgLenSongPositionPointer             = 3;
    const int MsgLenSongSelect                      = 2;
    const int MsgLenSysExMidiTimeCodeFullMessage    = 10;


    const uint16_t PitchCenter      = 0x2000;


    const uint8_t SysExBroadcastDeviceId = 0x7F; // for Universal SysEx (Non-)Realtime Messages


    const uint8_t MaxHour = 23;
    const uint8_t MaxMinute = 59;
    const uint8_t MaxSecond = 59;
    const uint8_t MaxFps[] = {23,24,29,29}; // According to index given by FrameRate enum @see MidiTimeCodeFrameRate_t


    typedef enum {
        StatusClassNoteOff                  = 0x80,
        StatusClassNoteOn                   = 0x90,
        StatusClassPolyphonicKeyPressure    = 0xA0,
        StatusClassControlChange            = 0xB0,
        StatusClassProgramChange            = 0xC0,
        StatusClassChannelPressure          = 0xD0,
        StatusClassPitchBendChange          = 0xE0,
        StatusClassSystemMessage            = 0xF0
    } StatusClass_t;

    inline bool isStatusClass( uint8_t value ){
        return (value == StatusClassNoteOff ||
                value == StatusClassNoteOn ||
                value == StatusClassPolyphonicKeyPressure ||
                value == StatusClassControlChange ||
                value == StatusClassProgramChange ||
                value == StatusClassChannelPressure ||
                value == StatusClassPitchBendChange ||
                value == StatusClassSystemMessage);
    }


    inline StatusClass_t getStatusClass( uint8_t value ){
        return (StatusClass_t)(value & StatusClassMask);
    }

    typedef enum {
        SystemMessageSystemExclusive = 0xF0,
        SystemMessageMidiTimeCodeQuarterFrame = 0xF1,
        SystemMessageSongPositionPointer = 0xF2,
        SystemMessageSongSelect = 0xF3,
        SystemMessageTuneRequest = 0xF6,
        SystemMessageEndOfSystemExclusive = 0xF7,
        SystemMessageTimingClock = 0xF8,
        SystemMessageStart = 0xFA,
        SystemMessageContinue = 0xFB,
        SystemMessageStop = 0xFC,
        SystemMessageActiveSensing = 0xFE,
        SystemMessageReset = 0xFF
    } SystemMessage_t;

    inline bool isSystemMessage( uint8_t value ){
        return (value == SystemMessageSystemExclusive ||
                value == SystemMessageMidiTimeCodeQuarterFrame ||
                value == SystemMessageSongPositionPointer ||
                value == SystemMessageSongSelect ||
                value == SystemMessageTuneRequest ||
                value == SystemMessageEndOfSystemExclusive ||
                value == SystemMessageTimingClock ||
                value == SystemMessageStart ||
                value == SystemMessageContinue ||
                value == SystemMessageStop ||
                value == SystemMessageActiveSensing ||
                value == SystemMessageReset);
    }

    inline bool isSystemCommonMessage( uint8_t value ){
        return (value == SystemMessageSystemExclusive ||
                value == SystemMessageMidiTimeCodeQuarterFrame ||
                value == SystemMessageSongPositionPointer ||
                value == SystemMessageSongSelect ||
                value == SystemMessageTuneRequest ||
                value == SystemMessageEndOfSystemExclusive);
    }


    inline bool isSystemRealTimeMessage( uint8_t status ){
        return (status == SystemMessageTimingClock ||
                status == SystemMessageStart ||
                status == SystemMessageContinue ||
                status == SystemMessageStop ||
                status == SystemMessageActiveSensing ||
                status == SystemMessageReset);
    }

    typedef enum {
        ChannelModeControllerAllSoundOff            = 120,
        ChannelModeControllerResetAllControllers    = 121,
        ChannelModeControllerLocalControl           = 122,
        ChannelModeControllerAllNotesOff            = 123,
        ChannelModeControllerOmniModeOff            = 124,
        ChannelModeControllerOmniModeOn             = 125,
        ChannelModeControllerMonoModeOn             = 126,
        ChannelModeControllerPolyModeOn             = 127
    } ChannelModeController_t;

    inline bool isChannelModeController( uint8_t value ){
        return (value == ChannelModeControllerAllSoundOff ||
                value == ChannelModeControllerResetAllControllers ||
                value == ChannelModeControllerLocalControl ||
                value == ChannelModeControllerAllNotesOff ||
                value == ChannelModeControllerOmniModeOff ||
                value == ChannelModeControllerOmniModeOn ||
                value == ChannelModeControllerMonoModeOn ||
                value == ChannelModeControllerPolyModeOn);
    }


    typedef enum {
        ReservedSystemExclusiveIdManufacturerExtension  = 0x00,
        ReservedSystemExclusiveIdExperimental           = 0x7D,
        ReservedSystemExclusiveIdNonRealTime            = 0x7E,
        ReservedSystemExclusiveIdRealTime               = 0x7F
    } ReservedSystemExclusiveId_t;

    inline bool isReservedSystemExclusiveId( uint8_t value ){
        return (value == ReservedSystemExclusiveIdManufacturerExtension ||
                value == ReservedSystemExclusiveIdExperimental ||
                value == ReservedSystemExclusiveIdNonRealTime ||
                value == ReservedSystemExclusiveIdRealTime);
    }

    typedef enum {
        UniversalSysExNonRtSampleDumpHeader       = 0x01,
        UniversalSysExNonRtSampleDataPacket       = 0x02,
        UniversalSysExNonRtSampleDumpRequest      = 0x03,
        UniversalSysExNonRtMidiTimeCode           = 0x04,
        UniversalSysExNonRtSampleDumpExtension    = 0x05,
        UniversalSysExNonRtGeneralInformation     = 0x06,
        UniversalSysExNonRtFileDump               = 0x07,
        UniversalSysExNonRtMidiTuningStandard     = 0x08,
        UniversalSysExNonRtGeneralMidi            = 0x09,
        UniversalSysExNonRtDownloadableSounds     = 0x0A,
        UniversalSysExNonRtFileReferenceMessage   = 0x0B,
        UniversalSysExNonRtMidiVisualControl      = 0x0C,
        UniversalSysExNonRtMidiCapabilityInquiry  = 0x0D,

        UniversalSysExNonRtEndOfFile              = 0x7B,
        UniversalSysExNonRtWait                   = 0x7C,
        UniversalSysExNonRtCancel                 = 0x7D,
        UniversalSysExNonRtNAK                    = 0x7E,
        UniversalSysExNonRtACK                    = 0x7F
    } UniversalSysExNonRt_t;
    
    inline bool isUniversalSysExNonRt( uint8_t value ){
        return (value == UniversalSysExNonRtSampleDumpHeader ||
                value == UniversalSysExNonRtSampleDataPacket ||
                value == UniversalSysExNonRtSampleDumpRequest ||
                value == UniversalSysExNonRtMidiTimeCode ||
                value == UniversalSysExNonRtSampleDumpExtension ||
                value == UniversalSysExNonRtGeneralInformation ||
                value == UniversalSysExNonRtFileDump ||
                value == UniversalSysExNonRtMidiTuningStandard ||
                value == UniversalSysExNonRtGeneralMidi ||
                value == UniversalSysExNonRtDownloadableSounds ||
                value == UniversalSysExNonRtFileReferenceMessage ||
                value == UniversalSysExNonRtMidiVisualControl ||
                value == UniversalSysExNonRtMidiCapabilityInquiry ||
                value == UniversalSysExNonRtEndOfFile ||
                value == UniversalSysExNonRtWait ||
                value == UniversalSysExNonRtCancel ||
                value == UniversalSysExNonRtNAK ||
                value == UniversalSysExNonRtACK);
    }

    typedef enum {
        UniversalSysExNonRtMidiTimeCodeSpecial                  = 0x00,
        UniversalSysExNonRtMidiTimeCodePunchInPoint             = 0x01,
        UniversalSysExNonRtMidiTimeCodePunchOutPoint            = 0x02,
        UniversalSysExNonRtMidiTimeCodeDeletePunchInPoint       = 0x03,
        UniversalSysExNonRtMidiTimeCodeDeletePunchOutPoint      = 0x04,
        UniversalSysExNonRtMidiTimeCodeEventStartPoint          = 0x05,
        UniversalSysExNonRtMidiTimeCodeEventStopPoint           = 0x06,
        UniversalSysExNonRtMidiTimeCodeEventStartPointsWithInfo = 0x07,
        UniversalSysExNonRtMidiTimeCodeEventStopPointsWithInfo  = 0x08,
        UniversalSysExNonRtMidiTimeCodeDeleteEventStartPoint    = 0x09,
        UniversalSysExNonRtMidiTimeCodeDeleteEventStopPoint     = 0x0A,
        UniversalSysExNonRtMidiTimeCodeCuePoints                = 0x0B,
        UniversalSysExNonRtMidiTimeCodeCuePointsWithInfo        = 0x0C,
        UniversalSysExNonRtMidiTimeCodeDeleteCuePoint           = 0x0D,
        UniversalSysExNonRtMidiTimeCodeEventNameInInfo          = 0x0E
    } UniversalSysExNonRtMidiTimeCode_t;

    inline bool isUniversalSysExNonRtMidiTimeCode( uint8_t value ){
        return (value == UniversalSysExNonRtMidiTimeCodeSpecial ||
                value == UniversalSysExNonRtMidiTimeCodePunchInPoint ||
                value == UniversalSysExNonRtMidiTimeCodePunchOutPoint ||
                value == UniversalSysExNonRtMidiTimeCodeDeletePunchInPoint ||
                value == UniversalSysExNonRtMidiTimeCodeDeletePunchOutPoint ||
                value == UniversalSysExNonRtMidiTimeCodeEventStartPoint ||
                value == UniversalSysExNonRtMidiTimeCodeEventStopPoint ||
                value == UniversalSysExNonRtMidiTimeCodeEventStartPointsWithInfo ||
                value == UniversalSysExNonRtMidiTimeCodeEventStopPointsWithInfo ||
                value == UniversalSysExNonRtMidiTimeCodeDeleteEventStartPoint ||
                value == UniversalSysExNonRtMidiTimeCodeDeleteEventStopPoint ||
                value == UniversalSysExNonRtMidiTimeCodeCuePoints ||
                value == UniversalSysExNonRtMidiTimeCodeCuePointsWithInfo ||
                value == UniversalSysExNonRtMidiTimeCodeDeleteCuePoint ||
                value == UniversalSysExNonRtMidiTimeCodeEventNameInInfo);
    }

    typedef enum {
        UniversalSysExNonRtSampleDumpExtensionLoopPointsTransmission    = 0x01,
        UniversalSysExNonRtSampleDumpExtensionLoopPointsRequest         = 0x02,
        UniversalSysExNonRtSampleDumpExtensionSampleNameTransmission    = 0x03,
        UniversalSysExNonRtSampleDumpExtensionSampleNameRequest         = 0x04,
        UniversalSysExNonRtSampleDumpExtensionExtendedDumpHeader        = 0x05,
        UniversalSysExNonRtSampleDumpExtensionExtendedLoopPointsTransmission    = 0x06,
        UniversalSysExNonRtSampleDumpExtensionExtendedLoopPointsRequest = 0x07
    } UniversalSysExNonRtSampleDumpExtension_t;

    inline bool isUniversalSysExNonRtSampleDumpExtension( uint8_t value ){
        return (value == UniversalSysExNonRtSampleDumpExtensionLoopPointsTransmission ||
                value == UniversalSysExNonRtSampleDumpExtensionLoopPointsRequest ||
                value == UniversalSysExNonRtSampleDumpExtensionSampleNameTransmission ||
                value == UniversalSysExNonRtSampleDumpExtensionSampleNameRequest ||
                value == UniversalSysExNonRtSampleDumpExtensionExtendedDumpHeader ||
                value == UniversalSysExNonRtSampleDumpExtensionExtendedLoopPointsTransmission ||
                value == UniversalSysExNonRtSampleDumpExtensionExtendedLoopPointsRequest);
    }

    typedef enum {
        UniversalSysExNonRtGeneralInformationIdentityRequest    = 0x01,
        UniversalSysExNonRtGeneralInformationIdentityReply      = 0x02
    } UniversalSysExNonRtGeneralInformation_t;

    inline bool isUniversalSysExNonRtGeneralInformation( uint8_t value ){
        return (value == UniversalSysExNonRtGeneralInformationIdentityRequest ||
                value == UniversalSysExNonRtGeneralInformationIdentityReply);
    }

    typedef enum {
        UniversalSysExNonRtFileDumpHeader       = 0x01,
        UniversalSysExNonRtFileDumpDataPacket   = 0x02,
        UniversalSysExNonRtFileDumpRequest      = 0x03
    } UniversalSysExNonRtFileDump_t;

    inline bool isUniversalSysExNonRtFileDump( uint8_t value ){
        return (value == UniversalSysExNonRtFileDumpHeader ||
                value == UniversalSysExNonRtFileDumpDataPacket ||
                value == UniversalSysExNonRtFileDumpRequest);
    }

    typedef enum {
        UniversalSysExNonRtMidiTuningStandardBulkDumpRequest        = 0x00,
        UniversalSysExNonRtMidiTuningStandardBulkDumpReply          = 0x01,
        UniversalSysExNonRtMidiTuningStandardTuningDumpRequest      = 0x02,
        UniversalSysExNonRtMidiTuningStandardKeybasedTuningDump     = 0x03,
        UniversalSysExNonRtMidiTuningStandardScaleTuningDump1Byte   = 0x04,
        UniversalSysExNonRtMidiTuningStandardScaleTuningDump2Byte   = 0x05,
        UniversalSysExNonRtMidiTuningStandardSingleNoteTuningChange = 0x06,
        UniversalSysExNonRtMidiTuningStandardScaleTuning1Byte       = 0x07,
        UniversalSysExNonRtMidiTuningStandardScaleTuning2Byte       = 0x08
    } UniversalSysExNonRtMidiTuningStandard_t;

    inline bool isUniversalSysExNonRtMidiTuningStandard( uint8_t value ){
        return (value == UniversalSysExNonRtMidiTuningStandardBulkDumpRequest ||
                value == UniversalSysExNonRtMidiTuningStandardBulkDumpReply ||
                value == UniversalSysExNonRtMidiTuningStandardTuningDumpRequest ||
                value == UniversalSysExNonRtMidiTuningStandardKeybasedTuningDump ||
                value == UniversalSysExNonRtMidiTuningStandardScaleTuningDump1Byte ||
                value == UniversalSysExNonRtMidiTuningStandardSingleNoteTuningChange ||
                value == UniversalSysExNonRtMidiTuningStandardScaleTuning1Byte ||
                value == UniversalSysExNonRtMidiTuningStandardScaleTuning2Byte);
    }

    typedef enum {
        UniversalSysExNonRtGeneralMidi1SystemOn     = 0x01,
        UniversalSysExNonRtGeneralMidiSystemOff     = 0x02,
        UniversalSysExNonRtGeneralMidi2SystemOn     = 0x03
    } UniversalSysExNonRtGeneralMidi_t;

    inline bool isUniversalSysExNonRtGeneralMidi( uint8_t value ){
        return (value == UniversalSysExNonRtGeneralMidi1SystemOn ||
                value == UniversalSysExNonRtGeneralMidiSystemOff ||
                value == UniversalSysExNonRtGeneralMidi2SystemOn);
    }

    typedef enum {
        UniversalSysExNonRtDownloadableSoundsTurnDlsOn      = 0x01,
        UniversalSysExNonRtDownloadableSoundsTurnDlsOff     = 0x02,
        UniversalSysExNonRtDownloadableSoundsTurnDlsVoiceAllocOff = 0x03,
        UniversalSysExNonRtDownloadableSoundsTurnDlsVoiceAllocOn  = 0x04
    } UniversalSysExNonRtDownloadableSounds_t;

    inline bool isUniversalSysExNonRtDownloadableSounds( uint8_t value ){
        return (value == UniversalSysExNonRtDownloadableSoundsTurnDlsOn ||
                value == UniversalSysExNonRtDownloadableSoundsTurnDlsOff ||
                value == UniversalSysExNonRtDownloadableSoundsTurnDlsVoiceAllocOff ||
                value == UniversalSysExNonRtDownloadableSoundsTurnDlsVoiceAllocOn);
    }

    typedef enum {
        UniversalSysExNonRtFileReferenceMessageOpenFile         = 0x01,
        UniversalSysExNonRtFileReferenceMessageSelectContents   = 0x02,
        UniversalSysExNonRtFileReferenceMessageOpenAndSelect    = 0x03,
        UniversalSysExNonRtFileReferenceMessageCloseFile        = 0x04
    } UniversalSysExNonRtFileReferenceMessage_t;

    inline bool isUniversalSysExNonRtFileReferenceMessage( uint8_t value ){
        return (value == UniversalSysExNonRtFileReferenceMessageOpenFile ||
                value == UniversalSysExNonRtFileReferenceMessageSelectContents ||
                value == UniversalSysExNonRtFileReferenceMessageOpenAndSelect ||
                value == UniversalSysExNonRtFileReferenceMessageCloseFile);
    }

    typedef enum {
        UniversalSysExNonRtMidiVisualControlTODO //TODO
    } UniversalSysExNonRtMidiVisualControl_t;

    inline bool isUniversalSysExNonRtMidiVisualControl( uint8_t value ){
        return false; //TODO
    }

    typedef enum {
        UniversalSysExNonRtMidiCapabilityInquiryTODO //TODO
    } UniversalSysExNonRtMidiCapabilityInquiry_t;

    inline bool isUniversalSysExNonRtMidiCapabilityInquiry( uint8_t value ){
        return false; //TODO
    }

    typedef enum {
        UniversalSysExRtMidiTimeCode                  = 0x01,
        UniversalSysExRtMidiShowControl               = 0x02,
        UniversalSysExRtDeviceControl                 = 0x04,
        UniversalSysExRtRealTimeMtcCueing             = 0x05,
        UniversalSysExRtMidiMachineControlCommands    = 0x06,
        UniversalSysExRtMidiMachineControlResponses   = 0x07,
        UniversalSysExRtMidiTuningStandard            = 0x08,
        UniversalSysExRtControllerDestinationSetting  = 0x09,
        UniversalSysExRtKeybasedInstrumentControl     = 0x0A,
        UniversalSysExRtScalablePolyphonyMidiMip      = 0x0B,
        UniversalSysExRtMobilePhoneControlMessage     = 0x0C
    } UniversalSysExRt_t;

    inline bool isUniversalSysExRt( uint8_t value ){
        return (value == UniversalSysExRtMidiTimeCode ||
                value == UniversalSysExRtMidiShowControl ||
                value == UniversalSysExRtDeviceControl ||
                value == UniversalSysExRtRealTimeMtcCueing ||
                value == UniversalSysExRtMidiMachineControlCommands ||
                value == UniversalSysExRtMidiMachineControlResponses ||
                value == UniversalSysExRtMidiTuningStandard ||
                value == UniversalSysExRtControllerDestinationSetting ||
                value == UniversalSysExRtKeybasedInstrumentControl ||
                value == UniversalSysExRtScalablePolyphonyMidiMip ||
                value == UniversalSysExRtMobilePhoneControlMessage);
    }

    typedef enum {
        UniversalSysExRtMidiTimeCodeFullMessage           = 0x01,
        UniversalSysExRtMidiTimeCodeUserBits              = 0x02
    } UniversalSysExRtMidiTimeCode_t;

    inline bool isUniversalSysExRtMidiTimeCode( uint8_t value ){
        return (value == UniversalSysExRtMidiTimeCodeFullMessage ||
                value == UniversalSysExRtMidiTimeCodeUserBits);
    }

    typedef enum {
        UniversalSysExRtMidiShowControlTODO //TODO
    } UniversalSysExRtMidiShowControl_t;

    inline bool isUniversalSysExRtMidiShowControl( uint8_t value ){
        return false; //TODO
    }


    typedef enum {
        UniversalSysExRtNotationInformationBarNumber                = 0x01,
        UniversalSysExRtNotationInformationTimeSignatureImmediate   = 0x02,
        UniversalSysExRtNotationInformationTimeSignatureDelayed     = 0x03
    } UniversalSysExRtNotationInformation_t;

    inline bool isUniversalSysExRtNotationInformation( uint8_t value ){
        return (value == UniversalSysExRtNotationInformationBarNumber ||
                value == UniversalSysExRtNotationInformationTimeSignatureImmediate ||
                value == UniversalSysExRtNotationInformationTimeSignatureDelayed);
    }

    typedef enum {
        UniversalSysExRtDeviceControlMasterVolume           = 0x01,
        UniversalSysExRtDeviceControlMasterBalance          = 0x02,
        UniversalSysExRtDeviceControlMasterFineTuning       = 0x03,
        UniversalSysExRtDeviceControlMasterCoarseTuning     = 0x04,
        UniversalSysExRtDeviceControlGlobalParameterControl = 0x05
    } UniversalSysExRtDeviceControl_t;

    inline bool isUniversalSysExRtDeviceControl( uint8_t value ) {
        return (value == UniversalSysExRtDeviceControlMasterVolume ||
                value == UniversalSysExRtDeviceControlMasterBalance ||
                value == UniversalSysExRtDeviceControlMasterFineTuning ||
                value == UniversalSysExRtDeviceControlMasterCoarseTuning ||
                value == UniversalSysExRtDeviceControlGlobalParameterControl);
    }

    typedef enum {
        UniversalSysExRtRealTimeMtcCueingSpecial                    = 0x00, // ??
        UniversalSysExRtRealTimeMtcCueingPunchInPoints              = 0x01,
        UniversalSysExRtRealTimeMtcCueingPunchOutPoints             = 0x02,
        // 3 - 4 reserved
        UniversalSysExRtRealTimeMtcCueingEventStartPoints           = 0x05,
        UniversalSysExRtRealTimeMtcCueingEventStopPoints            = 0x06,
        UniversalSysExRtRealTimeMtcCueingEvenStartPointsWithInfo    = 0x07,
        UniversalSysExRtRealTimeMtcCueingEventStopPointsWithInfo    = 0x08,
        // 9 - A reserved
        UniversalSysExRtRealTimeMtcCueingCuePoints                  = 0x0B,
        UniversalSysExRtRealTimeMtcCueingCuePointsWithInfo          = 0x0C,
        // D reserved
        UniversalSysExRtRealTimeMtcCueingEventNameInInfo            = 0x0E
    } UniversalSysExRtRealTimeMtcCueing_t;

    inline bool isUniversalSysExRtRealTimeMtcCueing( uint8_t value ){
        return (value == UniversalSysExRtRealTimeMtcCueingSpecial ||
                value == UniversalSysExRtRealTimeMtcCueingPunchInPoints ||
                value == UniversalSysExRtRealTimeMtcCueingPunchOutPoints ||
                value == UniversalSysExRtRealTimeMtcCueingEventStartPoints ||
                value == UniversalSysExRtRealTimeMtcCueingEventStopPoints ||
                value == UniversalSysExRtRealTimeMtcCueingEvenStartPointsWithInfo ||
                value == UniversalSysExRtRealTimeMtcCueingEventStopPointsWithInfo ||
                value == UniversalSysExRtRealTimeMtcCueingCuePoints ||
                value == UniversalSysExRtRealTimeMtcCueingCuePointsWithInfo ||
                value == UniversalSysExRtRealTimeMtcCueingEventNameInInfo);
    }

    typedef enum {
        UniversalSysExRtMidiMachineControlCommandsTODO //TODO
    } UniversalSysExRtMidiMachineControlCommands_t;

    inline bool isUniversalSysExRtMidiMachineControlCommands( uint8_t value ) {
        return false; //TODO
    }

    typedef enum {
        UniversalSysExRtMidiMachineControlResponsesTODO //TODO
    } UniversalSysExRtMidiMachineControlResponses_t;

    inline bool isUniversalSysExRtMidiMachineControlResponses( uint8_t value ) {
        return false; //TODO
    }

    typedef enum {
        UniversalSysExRtMidiTuningStandardSingleNoteTuningChange                = 0x01,
        UniversalSysExRtMidiTuningStandardSingleNoteTuningChangeWithBankSelect  = 0x02,
        UniversalSysExRtMidiTuningStandardScaleTuning1Byte                      = 0x03,
        UniversalSysExRtMidiTuningStandardScaleTuning2Byte                      = 0x04
    } UniversalSysExRtMidiTuningStandard_t;

    inline bool isUniversalSysExRtMidiTuningStandard( uint8_t value ) {
        return (value == UniversalSysExRtMidiTuningStandardSingleNoteTuningChange ||
                value == UniversalSysExRtMidiTuningStandardSingleNoteTuningChangeWithBankSelect ||
                value == UniversalSysExRtMidiTuningStandardScaleTuning1Byte ||
                value == UniversalSysExRtMidiTuningStandardScaleTuning2Byte);
    }

    typedef enum {
        UniversalSysExRtControllerDestinationSettingChannelPressure         = 0x01,
        UniversalSysExRtControllerDestinationSettingPolyphonicKeyPressure   = 0x02,
        UniversalSysExRtControllerDestinationSettingController              = 0x03
    } UniversalSysExRtControllerDestinationSetting_t;

    inline bool isUniversalSysExRtControllerDestinationSetting( uint8_t value ) {
        return (value == UniversalSysExRtControllerDestinationSettingChannelPressure ||
                value == UniversalSysExRtControllerDestinationSettingPolyphonicKeyPressure ||
                value == UniversalSysExRtControllerDestinationSettingController);
    }

    typedef enum {
        MidiTimeCodeQuarterFrameMessageTypeFrameLS      = 0b000,
        MidiTimeCodeQuarterFrameMessageTypeFrameMS      = 0b001,
        MidiTimeCodeQuarterFrameMessageTypeSecondLS     = 0b010,
        MidiTimeCodeQuarterFrameMessageTypeSecondMS     = 0b011,
        MidiTimeCodeQuarterFrameMessageTypeMinuteLS     = 0b100,
        MidiTimeCodeQuarterFrameMessageTypeMinuteMS     = 0b101,
        MidiTimeCodeQuarterFrameMessageTypeHourLS       = 0b110,
        MidiTimeCodeQuarterFrameMessageTypeHourMS       = 0b111
    } MidiTimeCodeQuarterFrameMessageType_t;

    inline bool isMidiTimeCodeQuarterMessageType( uint8_t value ){
        return (value == MidiTimeCodeQuarterFrameMessageTypeFrameLS ||
                value == MidiTimeCodeQuarterFrameMessageTypeFrameMS ||
                value == MidiTimeCodeQuarterFrameMessageTypeSecondLS ||
                value == MidiTimeCodeQuarterFrameMessageTypeSecondMS ||
                value == MidiTimeCodeQuarterFrameMessageTypeMinuteLS ||
                value == MidiTimeCodeQuarterFrameMessageTypeMinuteMS ||
                value == MidiTimeCodeQuarterFrameMessageTypeHourLS ||
                value == MidiTimeCodeQuarterFrameMessageTypeHourMS);
    }

    typedef enum {
        MidiTimeCodeFrameRate24fps      = 0b00,
        MidiTimeCodeFrameRate25fps      = 0b01,
        MidiTimeCodeFrameRate29_97fps   = 0b10, // 30fps Drop-Frame
        MidiTimeCodeFrameRate30fpsDropFrame = MidiTimeCodeFrameRate29_97fps,
        MidiTimeCodeFrameRate30fps      = 0b11  // 30fps Non-Drop
    } MidiTimeCodeFrameRate_t;

    inline bool isMidiTimeCodeFrameRate( uint8_t fps ) {
        return (fps == MidiTimeCodeFrameRate24fps ||
                fps == MidiTimeCodeFrameRate25fps ||
                fps == MidiTimeCodeFrameRate29_97fps ||
                fps == MidiTimeCodeFrameRate30fps);
    }

    // Structures
    /////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////


    typedef struct {
        uint8_t MessageType;
        uint8_t Nibble;
    } MidiTimeCodeQuarterFrame_t;

    typedef struct {
        uint8_t FpsHour;
        uint8_t Minute;
        uint8_t Second;
        uint8_t Frame;
    } MidiTimeCode_t;
    
    typedef struct {
        StatusClass_t StatusClass;
        uint8_t Status;
        uint8_t Channel; // Also DeviceId for SysEx messages
        union {
            struct {
                uint8_t Key;
                uint8_t Velocity;
            } NoteOn;
            struct {
                uint8_t Key;
                uint8_t Velocity;
            } NoteOff;
            struct {
                uint8_t Key;
                uint8_t Pressure;
            } PolyphonicKeyPressure;
            struct {
                uint8_t Controller;
                uint8_t Value;
            } ControlChange;
            struct {
                uint8_t Program;
            } ProgramChange;
            struct {
                uint8_t Pressure;
            } ChannelPressure;
            struct {
                uint16_t Pitch;
            } PitchBendChange;
            struct {
                uint32_t Id;
                uint8_t Length;
                uint8_t SubId1;
                uint8_t SubId2;
#if SYSEX_MEMORY == SYSEX_MEMORY_STATIC
                union {
                    uint8_t Bytes[SYSEX_MEMORY_STATIC_SIZE];
                    MidiTimeCode_t MidiTimeCode;
                } Data;
#elif SYSEX_MEMORY == SYSEX_MEMORY_DYNAMIC
                void * Data;
#endif
            } SysEx;

            MidiTimeCodeQuarterFrame_t MidiTimeCodeQuarterFrame;
            struct {
                uint16_t Position;
            } SongPositionPointer;
            struct {
                uint8_t Song;
            } SongSelect;
        } Data;
    } Message_t;



    inline bool isSystemCommonMessage( Message_t * msg ){
        return isSystemCommonMessage( msg->Status );
    }


    inline bool isSystemRealTimeMessage( Message_t * msg ){
        return isSystemRealTimeMessage( msg->Status );
    }


    inline bool isChannelModeMessage( uint8_t status, uint8_t controller ){
        return getStatusClass(status) == StatusClassControlChange && isChannelModeController(controller);
    }

    inline bool isChannelModeMessage( Message_t * msg ){
        return isChannelModeMessage( msg->Status, msg->Data.ControlChange.Controller );
    }


    inline bool isChannelVoiceMessage( uint8_t status, uint8_t byte1 ) {
        return isStatusClass( (status & StatusClassMask) )  &&
               (status & StatusClassMask) != StatusClassSystemMessage &&
               !isChannelModeMessage(status, byte1);
    }

    inline bool isChannelVoiceMessage( Message_t * msg ){
        return isChannelVoiceMessage( msg->Status, msg->Data.ControlChange.Controller );
    }





    inline int packNoteOff( uint8_t * bytes, uint8_t channel, uint8_t key, uint8_t velocity ){
        ASSERT( bytes != NULL );
        ASSERT( (channel & ChannelMask) == channel );
        ASSERT((key & DataMask) == key );
        ASSERT((velocity & DataMask) == velocity );

        bytes[0] = StatusClassNoteOff | (channel & ChannelMask);
        bytes[1] = key & DataMask;
        bytes[2] = velocity & DataMask;

        return MsgLenNoteOff;
    }

    inline int unpackNoteOff( uint8_t * bytes, int len, uint8_t * channel, uint8_t * key, uint8_t  * velocity ){
        ASSERT( bytes != NULL );

        if (len != MsgLenNoteOff) {
            return false;
        }
        if ((bytes[0] & StatusClassMask) != StatusClassNoteOff) {
            return false;
        }
        if (((bytes[1] & DataMask) != bytes[1]) || ((bytes[2] & DataMask) != bytes[2]) ) {
            return false;
        }

        *channel = bytes[0] & ChannelMask;
        *key = bytes[1];
        *velocity = bytes[2];

        return MsgLenNoteOff;
    }


    inline int packNoteOn( uint8_t * bytes, uint8_t channel, uint8_t key, uint8_t velocity ){
        ASSERT( bytes != NULL );
        ASSERT( (channel & ChannelMask) == channel );
        ASSERT((key & DataMask) == key );
        ASSERT((velocity & DataMask) == velocity );

        bytes[0] = StatusClassNoteOn | (channel & ChannelMask);
        bytes[1] = key & DataMask;
        bytes[2] = velocity & DataMask;

        return MsgLenNoteOn;
    }

    inline int unpackNoteOn( uint8_t * bytes, int len, uint8_t * channel, uint8_t * key, uint8_t * velocity ){
        ASSERT( bytes != NULL );

        if ( len != MsgLenNoteOn ) {
            return false;
        }
        if ((bytes[0] & StatusClassMask) == StatusClassNoteOn) {
            return false;
        }
        if (((bytes[1] & DataMask) != bytes[1]) || ((bytes[2] & DataMask) != bytes[2]) ) {
            return false;
        }

        *channel = bytes[0] & ChannelMask;
        *key = bytes[1];
        *velocity = bytes[2];

        return MsgLenNoteOn;
    }


    inline int packPolyphonicKeyPressure( uint8_t * bytes, uint8_t channel, uint8_t key, uint8_t pressure ){
        ASSERT( bytes != NULL );
        ASSERT( (channel & ChannelMask) == channel );
        ASSERT((key & DataMask) == key );
        ASSERT((pressure & DataMask) == pressure)

        bytes[0] = StatusClassPolyphonicKeyPressure | (channel & ChannelMask);
        bytes[1] = key & DataMask;
        bytes[2] = pressure & DataMask;

        return MsgLenPolyphonicKeyPressure;
    }

    inline int unpackPolyphonicKeyPressure( uint8_t * bytes, int len, uint8_t * channel, uint8_t * key, uint8_t * pressure ){
        ASSERT( bytes != NULL );

        if ( len != MsgLenPolyphonicKeyPressure ) {
            return false;
        }
        if ((bytes[0] & StatusClassMask) == StatusClassPolyphonicKeyPressure) {
            return false;
        }
        if (((bytes[1] & DataMask) != bytes[1]) || ((bytes[2] & DataMask) != bytes[2]) ) {
            return false;
        }

        *channel = bytes[0] & ChannelMask;
        *key = bytes[1];
        *pressure = bytes[2];

        return MsgLenPolyphonicKeyPressure;
    }


    inline int packControlChange( uint8_t * bytes, uint8_t channel, uint8_t controller, uint8_t value ){
        ASSERT( bytes != NULL );
        ASSERT( (channel & ChannelMask) == channel );
        ASSERT((controller & DataMask) == controller );
        ASSERT((value & DataMask) == value );

        bytes[0] = StatusClassControlChange | (channel & ChannelMask);
        bytes[1] = controller & DataMask;
        bytes[2] = value & DataMask;

        return MsgLenControlChange;
    }

    inline int unpackControlChange( uint8_t * bytes, int len, uint8_t * channel, uint8_t * controller, uint8_t * value ){
        ASSERT( bytes != NULL );

        if ( len != MsgLenControlChange ) {
            return false;
        }
        if ((bytes[0] & StatusClassMask) == StatusClassControlChange) {
            return false;
        }
        if (((bytes[1] & DataMask) != bytes[1]) || ((bytes[2] & DataMask) != bytes[2]) ) {
            return false;
        }

        *channel = bytes[0] & ChannelMask;
        *controller = bytes[1];
        *value = bytes[2];

        return MsgLenControlChange;
    }


    inline int packProgramChange( uint8_t * bytes, uint8_t channel, uint8_t program ){
        ASSERT( bytes != NULL );
        ASSERT( (channel & ChannelMask) == channel );
        ASSERT((program & DataMask) == program );

        bytes[0] = StatusClassProgramChange | (channel & ChannelMask);
        bytes[1] = program & DataMask;

        return MsgLenProgramChange;
    }

    inline int unpackProgramChange( uint8_t * bytes, int len, uint8_t * channel, uint8_t * program ){
        ASSERT( bytes != NULL );

        if ( len != MsgLenProgramChange ) {
            return false;
        }
        if ((bytes[0] & StatusClassMask) == StatusClassProgramChange) {
            return false;
        }
        if ((bytes[1] & DataMask) != bytes[1] ) {
            return false;
        }

        *channel = bytes[0] & ChannelMask;
        *program = bytes[1];

        return MsgLenProgramChange;
    }


    inline int packChannelPressure( uint8_t * bytes, uint8_t channel, uint8_t pressure ){
        ASSERT( bytes != NULL );
        ASSERT( (channel & ChannelMask) == channel );

        bytes[0] = StatusClassChannelPressure | (channel & ChannelMask);
        bytes[1] = pressure & DataMask;

        return MsgLenChannelPressure;
    }

    inline int unpackChannelPressure( uint8_t * bytes, int len, uint8_t * channel, uint8_t * pressure ){
        ASSERT( bytes != NULL );

        if ( len != MsgLenChannelPressure ) {
            return false;
        }
        if ((bytes[0] & StatusClassMask) == StatusClassChannelPressure) {
            return false;
        }
        if ((bytes[1] & DataMask) != bytes[1] ) {
            return false;
        }

        *channel = bytes[0];
        *pressure = bytes[1];

        return MsgLenChannelPressure;
    }


    inline int packPitchBendChange( uint8_t * bytes, uint8_t channel, uint16_t pitch ){
        ASSERT( bytes != NULL );
        ASSERT( (channel & ChannelMask) == channel );
        ASSERT( pitch <= MaxDoubleValue );

        bytes[0] = StatusClassPitchBendChange | (channel & ChannelMask);
        packDoubleValue( &bytes[1], pitch );

        return MsgLenPitchBendChange;
    }

    inline int unpackPitchBendChange( uint8_t * bytes, int len, uint8_t * channel, uint16_t * pitch ){
        ASSERT( bytes != NULL );

        if ( len != MsgLenPitchBendChange ) {
            return false;
        }
        if ((bytes[0] & StatusClassMask) == StatusClassPitchBendChange) {
            return false;
        }
        if (((bytes[1] & DataMask) != bytes[1]) || ((bytes[2] & DataMask) != bytes[2]) ) {
            return false;
        }

        *channel = bytes[0] & ChannelMask;
        unpackDoubleValue( &bytes[1], pitch );

        return MsgLenPitchBendChange;
    }



    inline int packAllSoundOff( uint8_t * bytes, uint8_t channel ){
        ASSERT( bytes != NULL );
        ASSERT( (channel & ChannelMask) == channel );

        bytes[0] = StatusClassControlChange | (channel & ChannelMask);
        bytes[1] = ChannelModeControllerAllSoundOff;
        bytes[2] = 0;

        return MsgLenControlChange;
    }

    inline int unpackAllSoundOff( uint8_t * bytes, int len, uint8_t * channel ) {
        ASSERT( bytes != NULL );

        if ( len != MsgLenControlChange ) {
            return false;
        }

        if ((bytes[0] & StatusClassMask) != StatusClassControlChange) {
            return false;
        }

        if ( (bytes[1] != ChannelModeControllerAllSoundOff) || (bytes[2] != 0) ) {
            return false;
        }

        *channel = bytes[0] & ChannelMask;

        return MsgLenControlChange;
    }

    inline int packResetAllControllers( uint8_t * bytes, uint8_t channel ){
        ASSERT( bytes != NULL );
        ASSERT( (channel & ChannelMask) == channel );

        bytes[0] = StatusClassControlChange | (channel & ChannelMask);
        bytes[1] = ChannelModeControllerResetAllControllers;
        bytes[2] = 0;

        return MsgLenControlChange;
    }

    inline int unpackResetAllControllers( uint8_t * bytes, int len, uint8_t * channel ) {
        ASSERT( bytes != NULL );

        if ( len != MsgLenControlChange ) {
            return false;
        }

        if ((bytes[0] & StatusClassMask) != StatusClassControlChange) {
            return false;
        }

        if ( (bytes[1] != ChannelModeControllerResetAllControllers) || (bytes[2] != 0) ) {
            return false;
        }

        *channel = bytes[0] & ChannelMask;

        return MsgLenControlChange;
    }

    inline int packLocalControl( uint8_t * bytes, uint8_t channel, bool on ){
        ASSERT( bytes != NULL );
        ASSERT( (channel & ChannelMask) == channel );

        bytes[0] = StatusClassControlChange | (channel & ChannelMask);
        bytes[1] = ChannelModeControllerLocalControl;
        bytes[2] = on ? 127 : 0;

        return MsgLenControlChange;
    }

    inline int unpackLocalControl( uint8_t * bytes, int len, uint8_t * channel, bool * on ) {
        ASSERT( bytes != NULL );

        if ( len != MsgLenControlChange ) {
            return false;
        }

        if ((bytes[0] & StatusClassMask) != StatusClassControlChange) {
            return false;
        }

        if ( (bytes[1] != ChannelModeControllerLocalControl) || (bytes[2] != 0 && bytes[2] != 127) ) {
            return false;
        }

        *channel = bytes[0] & ChannelMask;
        *on = bytes[1] == 127;

        return MsgLenControlChange;
    }

    inline int packAllNotesOff( uint8_t * bytes, uint8_t channel ){
        ASSERT( bytes != NULL );
        ASSERT( (channel & ChannelMask) == channel );

        bytes[0] = StatusClassControlChange | (channel & ChannelMask);
        bytes[1] = ChannelModeControllerAllNotesOff;
        bytes[2] = 0;

        return MsgLenControlChange;
    }

    inline int unpackAllNotesOff( uint8_t * bytes, int len, uint8_t * channel ) {
        ASSERT( bytes != NULL );

        if ( len != MsgLenControlChange ) {
            return false;
        }

        if ((bytes[0] & StatusClassMask) != StatusClassControlChange) {
            return false;
        }

        if ( (bytes[1] != ChannelModeControllerAllNotesOff) || (bytes[2] != 0) ) {
            return false;
        }

        *channel = bytes[0] & ChannelMask;

        return MsgLenControlChange;
    }

    inline int packOmniMode( uint8_t * bytes, uint8_t channel, bool on ){
        ASSERT( bytes != NULL );
        ASSERT( (channel & ChannelMask) == channel );

        bytes[0] = StatusClassControlChange | (channel & ChannelMask);
        bytes[1] = on ? ChannelModeControllerOmniModeOn : ChannelModeControllerOmniModeOff;
        bytes[2] = 0;

        return MsgLenControlChange;
    }

    inline int unpackOmniMode( uint8_t * bytes, int len, uint8_t * channel, bool * on ) {
        ASSERT( bytes != NULL );

        if ( len != MsgLenControlChange ) {
            return false;
        }

        if ((bytes[0] & StatusClassMask) != StatusClassControlChange) {
            return false;
        }

        if ( (bytes[1] != ChannelModeControllerOmniModeOn && bytes[1] != ChannelModeControllerOmniModeOff) || (bytes[2] != 0) ) {
            return false;
        }

        *channel = bytes[0] & ChannelMask;
        *on = bytes[1] == ChannelModeControllerOmniModeOn;

        return MsgLenControlChange;
    }



    inline int packMonoMode( uint8_t * bytes, uint8_t channel, uint8_t numberOfChannels ){
        ASSERT( bytes != NULL );
        ASSERT( (channel & ChannelMask) == channel );

        bytes[0] = StatusClassControlChange | (channel & ChannelMask);
        bytes[1] = ChannelModeControllerMonoModeOn;
        bytes[2] = numberOfChannels;

        return MsgLenControlChange;
    }

//    inline bool isMonoMode

    inline int unpackMonoMode( uint8_t * bytes, int len, uint8_t * channel, uint8_t * numberOfChannels ) {
        ASSERT( bytes != NULL );

        if ( len != MsgLenControlChange ) {
            return false;
        }

        if ((bytes[0] & StatusClassMask) != StatusClassControlChange) {
            return false;
        }

        if ( (bytes[1] != ChannelModeControllerMonoModeOn) || ((bytes[2] & DataMask) != bytes[2]) ) {
            return false;
        }

        *channel = bytes[0] & ChannelMask;
        *numberOfChannels = bytes[2];

        return MsgLenControlChange;
    }



    inline int packPolyModeOn( uint8_t * bytes, uint8_t channel){
        ASSERT( bytes != NULL );
        ASSERT( (channel & ChannelMask) == channel );

        bytes[0] = StatusClassControlChange | (channel & ChannelMask);
        bytes[1] = ChannelModeControllerPolyModeOn;
        bytes[2] = 0;

        return MsgLenControlChange;
    }

    inline bool isPolyModeOn( uint8_t status, uint8_t controller, uint8_t value ){
        return ((status & StatusClassMask) == StatusClassControlChange) && (controller == ChannelModeControllerPolyModeOn) && (value == 0);
    }

    inline bool isPolyModeOn( uint8_t * bytes ){
        return isPolyModeOn( bytes[0] , bytes[1], bytes[2] );
    }

    inline bool isPolyModeOn( Message_t * msg ){
        return isPolyModeOn( msg->Status, msg->Data.ControlChange.Controller, msg->Data.ControlChange.Value );
    }



    inline int packMidiTimeCodeQuarterFrame( uint8_t * bytes, uint8_t messageType, uint8_t nibble ){
        ASSERT( bytes != NULL );

        bytes[0] = SystemMessageMidiTimeCodeQuarterFrame;
        bytes[1] = ((messageType & 0x05) << 4) | (nibble & 0x0f);

        return MsgLenMidiTimeCodeQuarterFrame;
    }

    inline bool unpackMidiTimeCodeQuarterFrame( uint8_t * bytes, int len, uint8_t * messageType, uint8_t * nibble ){
        ASSERT( bytes != NULL );

        if ( len != 2 ) {
            return false;
        }
        if (bytes[0] != SystemMessageMidiTimeCodeQuarterFrame) {
            return false;
        }
        if ((bytes[1] & DataMask) != bytes[1] ) {
            return false;
        }

        *messageType = (bytes[1] >> 4) & 0x05;
        *nibble = bytes[1] & 0x0f;

        return true;
    }

    inline int packSongPositionPointer (uint8_t * bytes, uint16_t position ){
        ASSERT( bytes != NULL );
        ASSERT( position <= MaxDoubleValue );

        bytes[0] = SystemMessageSongPositionPointer;
        packDoubleValue( &bytes[1], position );

        return MsgLenSongPositionPointer;
    }

    inline bool unpackSongPositionPointer( uint8_t * bytes, int len, uint16_t * position ){
        ASSERT( bytes != NULL );

        if (len != MsgLenSongPositionPointer){
            return false;
        }
        if (bytes[0] != SystemMessageSongPositionPointer) {
            return false;
        }

        unpackDoubleValue( &bytes[1], position );

        return true;
    }

    inline int packSongSelect( uint8_t * bytes, uint8_t song ){
        ASSERT( bytes != NULL );
        ASSERT( song & DataMask == song );

        bytes[0] = SystemMessageSystemExclusive | SystemMessageSongSelect;
        bytes[1] = song & DataMask;

        return MsgLenSongSelect;
    }

    inline bool unpackSongSelect( uint8_t * bytes, int len, uint8_t * song){
        ASSERT( bytes != NULL );
        ASSERT(bytes[0] == SystemMessageSongSelect);

        if (len != MsgLenSongSelect){
            return false;
        }
        if (bytes[0] != SystemMessageSongPositionPointer) {
            return false;
        }

        *song = bytes[1] & DataMask;

        return true;
    }

    inline int packSysExMidiTimeCodeFullMessage( uint8_t * bytes, uint8_t deviceId, uint8_t hour, uint8_t minute, uint8_t second, uint8_t frame, uint8_t fps ) {
        ASSERT( bytes != NULL );
        ASSERT((deviceId & DataMask) == deviceId );
        ASSERT( hour <= MaxHour );
        ASSERT( minute <= MaxMinute );
        ASSERT( second <= MaxSecond );
        ASSERT( isMidiTimeCodeFrameRate(fps) );
        ASSERT( frame <= MaxFps[fps] );

        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = ReservedSystemExclusiveIdRealTime;
        bytes[2] = deviceId & DataMask;
        bytes[3] = UniversalSysExRtMidiTimeCode;
        bytes[4] = UniversalSysExRtMidiTimeCodeFullMessage;
        bytes[5] = (fps << 4) | (hour & 0b00011111);
        bytes[6] = minute & 0b00111111;
        bytes[7] = second & 0b00111111;
        bytes[8] = frame & 0b00011111;
        bytes[9] = SystemMessageEndOfSystemExclusive;

        return MsgLenSysExMidiTimeCodeFullMessage;
    }

    inline bool unpackSysExMidiTimeCodeFullMessage( uint8_t * bytes, int len, uint8_t * deviceId, uint8_t * hour, uint8_t * minute, uint8_t * second, uint8_t * frame, uint8_t * fps ) {
        ASSERT( bytes != NULL );

        if ( len < 9 || 10 < len ){
            return false;
        }
        if (bytes[0] != StatusClassSystemMessage) {
            return false;
        }
        if (bytes[1] != ReservedSystemExclusiveIdRealTime ) {
            return false;
        }
        if ((bytes[2] & DataMask) != bytes[2] ) {
            return false;
        }
        if (bytes[3] != UniversalSysExRtMidiTimeCode) {
            return false;
        }
        if (bytes[4] != UniversalSysExRtMidiTimeCodeFullMessage){
            return false;
        }

        *fps = (bytes[5] >> 5) & 0b11;
        *hour = bytes[5] & 0b00011111;
        *minute = bytes[6] & 0b00111111;
        *second = bytes[7] & 0b00111111;
        *frame = bytes[8] & 0b00011111;

        return true;
    }

    /**
     * Extract Time Code from 8 QuarterFrames given in the order expected message type order.
     */
    inline void MidiTimeCodeFromQuarterFrames( MidiTimeCode_t * mtc, MidiTimeCodeQuarterFrame_t * frames) {
        ASSERT( mtc != NULL );
        ASSERT( frames != NULL );
        ASSERT( frames[0].MessageType == MidiTimeCodeQuarterFrameMessageTypeFrameLS );
        ASSERT( frames[1].MessageType == MidiTimeCodeQuarterFrameMessageTypeFrameMS );
        ASSERT( frames[2].MessageType == MidiTimeCodeQuarterFrameMessageTypeSecondLS );
        ASSERT( frames[3].MessageType == MidiTimeCodeQuarterFrameMessageTypeSecondMS );
        ASSERT( frames[4].MessageType == MidiTimeCodeQuarterFrameMessageTypeMinuteLS );
        ASSERT( frames[5].MessageType == MidiTimeCodeQuarterFrameMessageTypeMinuteMS );
        ASSERT( frames[6].MessageType == MidiTimeCodeQuarterFrameMessageTypeHourLS );
        ASSERT( frames[7].MessageType == MidiTimeCodeQuarterFrameMessageTypeHourMS );

        ASSERT( frames[0].Nibble & 0x0F == frames[0].Nibble );
        ASSERT( frames[1].Nibble & 0x0F == frames[1].Nibble );
        ASSERT( frames[2].Nibble & 0x0F == frames[2].Nibble );
        ASSERT( frames[3].Nibble & 0x0F == frames[3].Nibble );
        ASSERT( frames[4].Nibble & 0x0F == frames[4].Nibble );
        ASSERT( frames[5].Nibble & 0x0F == frames[5].Nibble );
        ASSERT( frames[6].Nibble & 0x0F == frames[6].Nibble );
        ASSERT( frames[7].Nibble & 0x0F == frames[7].Nibble );

        mtc->Frame = (frames[1].Nibble << 4) | (frames[0].Nibble);
        mtc->Second = (frames[3].Nibble << 4) | (frames[2].Nibble);
        mtc->Minute = (frames[5].Nibble << 4) | (frames[4].Nibble);
        mtc->FpsHour = (frames[7].Nibble << 4) | (frames[6].Nibble);
    }

    /**
     * Extract Time Code from 8 QuarterFrames given in the order expected message type order.
     */
    inline void MidiTimeCodeFromQuarterFrames( MidiTimeCode_t * mtc, uint8_t * nibbles) {
        ASSERT( nibbles[0] & 0x0F == nibbles[0] );
        ASSERT( nibbles[1] & 0x0F == nibbles[1] );
        ASSERT( nibbles[2] & 0x0F == nibbles[2] );
        ASSERT( nibbles[3] & 0x0F == nibbles[3] );
        ASSERT( nibbles[4] & 0x0F == nibbles[4] );
        ASSERT( nibbles[5] & 0x0F == nibbles[5] );
        ASSERT( nibbles[6] & 0x0F == nibbles[6] );
        ASSERT( nibbles[7] & 0x0F == nibbles[7] );

        mtc->Frame = (nibbles[1] << 4) | (nibbles[0]);
        mtc->Second = (nibbles[3] << 4) | (nibbles[2]);
        mtc->Minute = (nibbles[5] << 4) | (nibbles[4]);
        mtc->FpsHour = (nibbles[7] << 4) | (nibbles[6]);
    }

    inline uint8_t MidiQuarterFrameFromTimeCode( MidiTimeCode_t * mtc, MidiTimeCodeQuarterFrameMessageType_t type ) {
        ASSERT( mtc != NULL );
        ASSERT( isMidiTimeCodeQuarterMessageType(type) );

        switch(type){
            MidiTimeCodeQuarterFrameMessageTypeFrameLS:     return mtc->Frame & 0x0F;
            MidiTimeCodeQuarterFrameMessageTypeFrameMS:     return (mtc->Frame >> 4) & 0b1;
            MidiTimeCodeQuarterFrameMessageTypeSecondLS:    return mtc->Second & 0x0F;
            MidiTimeCodeQuarterFrameMessageTypeSecondMS:    return (mtc->Second >> 4) & 0b0011;
            MidiTimeCodeQuarterFrameMessageTypeMinuteLS:    return mtc->Minute & 0x0F;
            MidiTimeCodeQuarterFrameMessageTypeMinuteMS:    return (mtc->Minute >> 4) & 0b0011;
            MidiTimeCodeQuarterFrameMessageTypeHourLS:      return mtc->FpsHour & 0x0F;
            MidiTimeCodeQuarterFrameMessageTypeHourMS:      return (mtc->FpsHour >> 4) & 0b0111;
        }

        return (uint8_t) ~0;
    }


    /**
     *  Tries to pack a given midi <msg> into the corresponding sequence of raw bytes.
     *
     *  @param  bytes   destination byte array
     *  @param  msg     midi message to pack into array
     *  @return         number of relevant bytes, zero on invalid message
     */
    int pack( uint8_t * bytes, Message_t * msg );

    /**
     * Tries to parse/unpack a raw byte sequence assumed to be a midi message.
     *
     * NOTE: if the messaged parse is a SystemExclusive message (Status == SystemMessageSystemExclusive)
     * a buffer for the message contents is allocated which has to be freed (using freeMessage())
     *
     * @param   bytes   source byte array of midi message to parse
     * @param   len     length of bytes to parse
     * @param   msg     destination midi message struct
     * @return          true iff valid midi message was read
     * @see freeMessage()
     */
    bool unpack( uint8_t * bytes, int len, Message_t * msg );

#if SYSEX_MEMORY == SYSEX_MEMORY_STATIC

    /**
     * Dummy function for freeing SysEx memory
     *
     * Note: This function is only provided to allow simple chaning of the SYSEX_MEMORY strategy.
     *
     * @param   msg     midi message to be (supposedly freed)
     */
    inline void freeMessage( Message_t * msg ) {
        // do nothing
    }

#elif SYSEX_MEMORY == SYSEX_MEMORY_DYNAMIC
    /**
     * Frees SysEx memory (if set)
     *
     * Is safe to be called on any (correctly used) message, ie will only act if msg is has status == SystemMessageSystemExclusive
     * and has a data length of greater than zero.
     *
     * @param   msg     midi message to be freed
     */
    void freeMessage( Message_t * msg );
#endif

}

#endif //MIDIMESSAGE_H
