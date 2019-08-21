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

    const uint8_t NibbleMask = 0b00001111;

    const uint8_t MaxValue          = 127;
    const uint16_t MaxDoubleValue   = 0x3FFF;

    inline bool isDataByte( uint8_t byte ){
        return byte & DataMask;
    }

    inline uint8_t getData( uint8_t byte ){
        return byte & DataMask;
    }

    inline uint8_t getLsNibble( uint8_t byte ){
        return byte & NibbleMask;
    }

    inline uint8_t getMsNibble( uint8_t byte ){
        return (byte >> 4) & NibbleMask;
    }

    inline int nibblize( uint8_t * dst, uint8_t * src, int len ){
        for (int i = 0, j = 0; i < len; i++) {
            dst[j++] = getLsNibble( src[i] );
            dst[j++] = getMsNibble( src[i] );
        }
        return 2 * len;
    }

    inline int denibblize( uint8_t * dst, uint8_t * src, int len ){
        ASSERT( len % 2 == 0 );
        for (int i = 0, j = 0; i < len; ){
            dst[j] = src[i++] & NibbleMask;
            dst[j] |= (src[i++] & NibbleMask) << 4;
        }
        return len / 2;
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
    const int MsgLenSysExMidiTimeCodeUserBits       = 15;
    const int MsgLenSysExNonRtMidiCueingSetupMessageMin  = 13;
    const int MsgLenSysExRtMidiCueingSetupMessageMin  = 8;


    const uint16_t PitchCenter      = 0x2000;


    const uint8_t SysExBroadcastDeviceId = 0x7F; // for Universal SysEx (Non-)Realtime Messages


    const uint8_t FpsMask       = 0b01100000;
    const uint8_t FpsOffset     = 5;
    const uint8_t HourMask      = 0b00011111;
    const uint8_t MinuteMask    = 0b00111111;
    const uint8_t SecondMask    = 0b00111111;
    const uint8_t FrameMask     = 0b00011111;
    const uint8_t FractionalFrameMask = 0b01111111;

    const uint8_t MaxHour = 23;
    const uint8_t MaxMinute = 59;
    const uint8_t MaxSecond = 59;
    const uint8_t MaxFps[] = {23,24,29,29}; // According to index given by FrameRate enum @see MidiTimeCodeFrameRate_t
    const uint8_t MaxFractionalFrame = 99;

    const uint16_t MaxEventNumber = 0x3FFF; // 14 bit value

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
        ReservedSysExIdManufacturerExtension  = 0x00,
        ReservedSysExIdExperimental           = 0x7D,
        ReservedSysExIdNonRealTime            = 0x7E,
        ReservedSysExIdRealTime               = 0x7F
    } ReservedSysExId_t;

    inline bool isReservedSysExId( uint8_t value ){
        return (value == ReservedSysExIdManufacturerExtension ||
                value == ReservedSysExIdExperimental ||
                value == ReservedSysExIdNonRealTime ||
                value == ReservedSysExIdRealTime);
    }

    inline bool isManufacturerSysExId( uint8_t byte0 ){
        return (byte0 != ReservedSysExIdExperimental &&
                byte0 != ReservedSysExIdNonRealTime &&
                byte0 != ReservedSysExIdRealTime);
    }
    inline bool isManufacturerSysExId( uint32_t id ){
        return isManufacturerSysExId( (uint8_t)((id >> 16) & DataMask));
    }

    inline bool is1ByteSysExId( uint8_t * bytes ){
        return bytes[0] == ReservedSysExIdManufacturerExtension;
    }

    inline bool is3ByteSysExId( uint8_t * bytes ){
        return !is1ByteSysExId( bytes );
    }
    inline bool is1ByteSysExId( uint32_t id ){
        return ((id >> 16) & DataMask) == ReservedSysExIdManufacturerExtension;
    }

    inline bool is3ByteSysExId( uint32_t id ){
        return !is1ByteSysExId(  id );
    }


    inline int packSysExId( uint8_t * bytes, uint32_t id ){
        ASSERT( bytes != NULL );

        bytes[0] = (id >> 16) & DataMask;

        if (bytes[0] == ReservedSysExIdManufacturerExtension){
            bytes[1] = (id >> 8) & DataMask;
            bytes[2] = id & DataMask;
            return 3;
        }
        
        return 1;
    }

    inline uint32_t unpackSysExId( uint8_t * bytes ){
        ASSERT( bytes != NULL );

        uint32_t id = ((uint32_t)bytes[0]) << 16;

        if (bytes[0] == ReservedSysExIdManufacturerExtension){
            id |= ((uint32_t)bytes[1]) << 8;
            id |= ((uint32_t)bytes[2]);
        }

        return id;
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

    inline bool isUniversalSysExNonRtMidiTimeCodeWithAddInfo( uint8_t value ){
        return (value == UniversalSysExNonRtMidiTimeCodeEventStartPointsWithInfo ||
                value == UniversalSysExNonRtMidiTimeCodeEventStopPointsWithInfo ||
                value == UniversalSysExNonRtMidiTimeCodeCuePointsWithInfo ||
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
        UniversalSysExRtMidiTimeCodeCueing             = 0x05,
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
                value == UniversalSysExRtMidiTimeCodeCueing ||
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
        UniversalSysExRtMidiTimeCodeCueingSpecial                    = 0x00, // ??
        UniversalSysExRtMidiTimeCodeCueingPunchInPoints              = 0x01,
        UniversalSysExRtMidiTimeCodeCueingPunchOutPoints             = 0x02,
        // 3 - 4 reserved
        UniversalSysExRtMidiTimeCodeCueingEventStartPoints           = 0x05,
        UniversalSysExRtMidiTimeCodeCueingEventStopPoints            = 0x06,
        UniversalSysExRtMidiTimeCodeCueingEvenStartPointsWithInfo    = 0x07,
        UniversalSysExRtMidiTimeCodeCueingEventStopPointsWithInfo    = 0x08,
        // 9 - A reserved
        UniversalSysExRtMidiTimeCodeCueingCuePoints                  = 0x0B,
        UniversalSysExRtMidiTimeCodeCueingCuePointsWithInfo          = 0x0C,
        // D reserved
        UniversalSysExRtMidiTimeCodeCueingEventNameInInfo            = 0x0E
    } UniversalSysExRtMidiTimeCodeCueing_t;

    inline bool isUniversalSysExRtMidiTimeCodeCueing( uint8_t value ){
        return (value == UniversalSysExRtMidiTimeCodeCueingSpecial ||
                value == UniversalSysExRtMidiTimeCodeCueingPunchInPoints ||
                value == UniversalSysExRtMidiTimeCodeCueingPunchOutPoints ||
                value == UniversalSysExRtMidiTimeCodeCueingEventStartPoints ||
                value == UniversalSysExRtMidiTimeCodeCueingEventStopPoints ||
                value == UniversalSysExRtMidiTimeCodeCueingEvenStartPointsWithInfo ||
                value == UniversalSysExRtMidiTimeCodeCueingEventStopPointsWithInfo ||
                value == UniversalSysExRtMidiTimeCodeCueingCuePoints ||
                value == UniversalSysExRtMidiTimeCodeCueingCuePointsWithInfo ||
                value == UniversalSysExRtMidiTimeCodeCueingEventNameInInfo);
    }

    inline bool isUniversalSysExRtMidiTimeCodeCueingWithAddInfo( uint8_t value ){
        return (value == UniversalSysExRtMidiTimeCodeCueingEvenStartPointsWithInfo ||
                value == UniversalSysExRtMidiTimeCodeCueingEventStopPointsWithInfo ||
                value == UniversalSysExRtMidiTimeCodeCueingCuePointsWithInfo ||
                value == UniversalSysExRtMidiTimeCodeCueingEventNameInInfo);
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


    inline MidiTimeCodeFrameRate_t getFps( uint8_t fpsHour ){
        return (MidiTimeCodeFrameRate_t)((fpsHour & FpsMask) >> FpsOffset);
    }

    inline uint8_t setFps( MidiTimeCodeFrameRate_t fps ){
        return (((uint8_t)fps) << FpsOffset) & FpsMask;
    }

    inline uint8_t getHour( uint8_t fpsHour ){
        return fpsHour & HourMask;
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
        uint8_t FractionalFrame;
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
                union {
                    MidiTimeCode_t MidiTimeCode;

                    struct {
                        MidiTimeCode_t MidiTimeCode;
                        uint16_t EventNumber;
                    } Cueing;
                } Data;

#if SYSEX_MEMORY == SYSEX_MEMORY_STATIC
                uint8_t ByteData[SYSEX_MEMORY_STATIC_SIZE];
#elif SYSEX_MEMORY == SYSEX_MEMORY_DYNAMIC
                uint8_t * ByteData;
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


    /**
     * Extract Time Code from 8 QuarterFrames given in the order expected message type order.
     */
    inline void MidiTimeCodeFromQuarterFrameNibbles( MidiTimeCode_t * mtc, uint8_t * nibbles ) {
        ASSERT( nibbles[0] & NibbleMask == nibbles[0] );
        ASSERT( nibbles[1] & NibbleMask == nibbles[1] );
        ASSERT( nibbles[2] & NibbleMask == nibbles[2] );
        ASSERT( nibbles[3] & NibbleMask == nibbles[3] );
        ASSERT( nibbles[4] & NibbleMask == nibbles[4] );
        ASSERT( nibbles[5] & NibbleMask == nibbles[5] );
        ASSERT( nibbles[6] & NibbleMask == nibbles[6] );
        ASSERT( nibbles[7] & NibbleMask == nibbles[7] );

        mtc->Frame = (nibbles[1] << 4) | (nibbles[0]);
        mtc->Second = (nibbles[3] << 4) | (nibbles[2]);
        mtc->Minute = (nibbles[5] << 4) | (nibbles[4]);
        mtc->FpsHour = (nibbles[7] << 4) | (nibbles[6]);
        mtc->FractionalFrame = 0;
    }

    inline uint8_t MidiQuarterFrameNibbleFromTimeCode( MidiTimeCode_t * mtc, MidiTimeCodeQuarterFrameMessageType_t type ) {
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

    inline bool unpackNoteOff( uint8_t * bytes, int len, uint8_t * channel, uint8_t * key, uint8_t  * velocity ){
        ASSERT( bytes != NULL );
        ASSERT( channel != NULL );
        ASSERT( key != NULL );
        ASSERT( velocity != NULL );

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

        return true;
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

    inline bool unpackNoteOn( uint8_t * bytes, int len, uint8_t * channel, uint8_t * key, uint8_t * velocity ){
        ASSERT( bytes != NULL );
        ASSERT( channel != NULL );
        ASSERT( key != NULL );
        ASSERT( velocity != NULL );

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

        return true;
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

    inline bool unpackPolyphonicKeyPressure( uint8_t * bytes, int len, uint8_t * channel, uint8_t * key, uint8_t * pressure ){
        ASSERT( bytes != NULL );
        ASSERT( channel != NULL );
        ASSERT( key != NULL );
        ASSERT( pressure != NULL );

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

        return true;
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

    inline bool unpackControlChange( uint8_t * bytes, int len, uint8_t * channel, uint8_t * controller, uint8_t * value ){
        ASSERT( bytes != NULL );
        ASSERT( channel != NULL );
        ASSERT( controller != NULL );
        ASSERT( value != NULL );

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

        return true;
    }


    inline int packProgramChange( uint8_t * bytes, uint8_t channel, uint8_t program ){
        ASSERT( bytes != NULL );
        ASSERT( (channel & ChannelMask) == channel );
        ASSERT((program & DataMask) == program );

        bytes[0] = StatusClassProgramChange | (channel & ChannelMask);
        bytes[1] = program & DataMask;

        return MsgLenProgramChange;
    }

    inline bool unpackProgramChange( uint8_t * bytes, int len, uint8_t * channel, uint8_t * program ){
        ASSERT( bytes != NULL );
        ASSERT( channel != NULL );
        ASSERT( program != NULL );

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

        return true;
    }


    inline int packChannelPressure( uint8_t * bytes, uint8_t channel, uint8_t pressure ){
        ASSERT( bytes != NULL );
        ASSERT( (channel & ChannelMask) == channel );

        bytes[0] = StatusClassChannelPressure | (channel & ChannelMask);
        bytes[1] = pressure & DataMask;

        return MsgLenChannelPressure;
    }

    inline bool unpackChannelPressure( uint8_t * bytes, int len, uint8_t * channel, uint8_t * pressure ){
        ASSERT( bytes != NULL );
        ASSERT( channel != NULL );
        ASSERT( pressure != NULL );

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

        return true;
    }


    inline int packPitchBendChange( uint8_t * bytes, uint8_t channel, uint16_t pitch ){
        ASSERT( bytes != NULL );
        ASSERT( (channel & ChannelMask) == channel );
        ASSERT( pitch <= MaxDoubleValue );

        bytes[0] = StatusClassPitchBendChange | (channel & ChannelMask);
        packDoubleValue( &bytes[1], pitch );

        return MsgLenPitchBendChange;
    }

    inline bool unpackPitchBendChange( uint8_t * bytes, int len, uint8_t * channel, uint16_t * pitch ){
        ASSERT( bytes != NULL );
        ASSERT( channel != NULL );
        ASSERT( pitch != NULL );

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

        return true;
    }



    inline int packAllSoundOff( uint8_t * bytes, uint8_t channel ){
        ASSERT( bytes != NULL );
        ASSERT( (channel & ChannelMask) == channel );

        bytes[0] = StatusClassControlChange | (channel & ChannelMask);
        bytes[1] = ChannelModeControllerAllSoundOff;
        bytes[2] = 0;

        return MsgLenControlChange;
    }

    inline bool isAllSoundOff( uint8_t status, uint8_t controller, uint8_t value ){
        return ((status & StatusClassMask) == StatusClassControlChange) && (controller == ChannelModeControllerAllSoundOff) && (value == 0);
    }

    inline bool isAllSoundOff( uint8_t * bytes ){
        ASSERT( bytes != NULL );

        return isAllSoundOff( bytes[0], bytes[1], bytes[2] );
    }

    inline bool isAllSoundOff( Message_t * msg ){
        ASSERT( msg != NULL );

        return isAllSoundOff( msg->StatusClass, msg->Data.ControlChange.Controller, msg->Data.ControlChange.Value );
    }


    inline int packResetAllControllers( uint8_t * bytes, uint8_t channel ){
        ASSERT( bytes != NULL );
        ASSERT( (channel & ChannelMask) == channel );

        bytes[0] = StatusClassControlChange | (channel & ChannelMask);
        bytes[1] = ChannelModeControllerResetAllControllers;
        bytes[2] = 0;

        return MsgLenControlChange;
    }

    inline bool isResetAllControllers( uint8_t status, uint8_t controller, uint8_t value ){
        return ((status & StatusClassMask) == StatusClassControlChange) && (controller == ChannelModeControllerResetAllControllers) && (value == 0);
    }

    inline bool isResetAllControllers( uint8_t * bytes ){
        ASSERT( bytes != NULL );

        return isResetAllControllers( bytes[0], bytes[1], bytes[2] );
    }

    inline bool isResetAllControllers( Message_t * msg ){
        ASSERT( msg != NULL );

        return isResetAllControllers( msg->StatusClass, msg->Data.ControlChange.Controller, msg->Data.ControlChange.Value );
    }


    inline int packLocalControl( uint8_t * bytes, uint8_t channel, bool on ){
        ASSERT( bytes != NULL );
        ASSERT( (channel & ChannelMask) == channel );

        bytes[0] = StatusClassControlChange | (channel & ChannelMask);
        bytes[1] = ChannelModeControllerLocalControl;
        bytes[2] = on ? 127 : 0;

        return MsgLenControlChange;
    }

    inline bool isLocalControl( uint8_t status, uint8_t controller ){
        return ((status & StatusClassMask) == StatusClassControlChange) && (controller == ChannelModeControllerLocalControl);
    }

    inline bool isLocalControl( uint8_t * bytes ){
        ASSERT( bytes != NULL );

        return isLocalControl( bytes[0], bytes[1] );
    }

    inline bool isLocalControl( Message_t * msg ){
        ASSERT( msg != NULL );

        return isLocalControl( msg->StatusClass, msg->Data.ControlChange.Controller );
    }

    inline bool getLocalControlState( uint8_t * bytes ){
        ASSERT( bytes != NULL );
        ASSERT( isLocalControl(bytes) );

        return (bytes[3] & DataMask) > 0;
    }

    inline bool getLocalControlState( Message_t * msg ){
        ASSERT( msg != NULL );
        ASSERT( isLocalControl(msg) );

        return msg->Data.ControlChange.Value > 0;
    }


    inline int packAllNotesOff( uint8_t * bytes, uint8_t channel ){
        ASSERT( bytes != NULL );
        ASSERT( (channel & ChannelMask) == channel );

        bytes[0] = StatusClassControlChange | (channel & ChannelMask);
        bytes[1] = ChannelModeControllerAllNotesOff;
        bytes[2] = 0;

        return MsgLenControlChange;
    }

    inline bool isAllNotesOff( uint8_t status, uint8_t controller, uint8_t value ){
        return ((status & StatusClassMask) == StatusClassControlChange) && (controller == ChannelModeControllerAllNotesOff) && (value == 0);
    }

    inline bool isAllNotesOff( uint8_t * bytes ){
        ASSERT( bytes != NULL );

        return isAllNotesOff( bytes[0], bytes[1], bytes[2] );
    }

    inline bool isAllNotesOff( Message_t * msg ){
        ASSERT( msg != NULL );

        return isAllNotesOff( msg->StatusClass, msg->Data.ControlChange.Controller, msg->Data.ControlChange.Value );
    }


    inline int packOmniMode( uint8_t * bytes, uint8_t channel, bool on ){
        ASSERT( bytes != NULL );
        ASSERT( (channel & ChannelMask) == channel );

        bytes[0] = StatusClassControlChange | (channel & ChannelMask);
        bytes[1] = on ? ChannelModeControllerOmniModeOn : ChannelModeControllerOmniModeOff;
        bytes[2] = 0;

        return MsgLenControlChange;
    }

    inline bool isOmniMode( uint8_t status, uint8_t controller ){
        return ((status & StatusClassMask) == StatusClassControlChange) && (controller == ChannelModeControllerOmniModeOn || controller == ChannelModeControllerOmniModeOff);
    }

    inline bool isOmniMode( uint8_t * bytes ){
        ASSERT( bytes != NULL );

        return isOmniMode( bytes[0], bytes[1] );
    }

    inline bool isOmniMode( Message_t * msg ){
        ASSERT( msg != NULL );

        return isOmniMode( msg->StatusClass, msg->Data.ControlChange.Controller );
    }

    inline bool getOmniModeState( uint8_t * bytes ){
        ASSERT( bytes != NULL );
        ASSERT( isOmniMode(bytes) );

        return bytes[3] == ChannelModeControllerOmniModeOn;
    }

    inline bool getOmniModeState( Message_t * msg ){
        ASSERT( msg != NULL );
        ASSERT( isOmniMode(msg) );

        return msg->Data.ControlChange.Value == ChannelModeControllerOmniModeOn;
    }



    inline int packMonoMode( uint8_t * bytes, uint8_t channel, uint8_t numberOfChannels ){
        ASSERT( bytes != NULL );
        ASSERT( (channel & ChannelMask) == channel );

        bytes[0] = StatusClassControlChange | (channel & ChannelMask);
        bytes[1] = ChannelModeControllerMonoModeOn;
        bytes[2] = numberOfChannels;

        return MsgLenControlChange;
    }

    inline bool isMonoMode( uint8_t status, uint8_t controller ){
        return ((status & StatusClassMask) == StatusClassControlChange) && (controller == ChannelModeControllerMonoModeOn);
    }

    inline bool isMonoMode( uint8_t * bytes ){
        ASSERT( bytes != NULL );

        return isMonoMode( bytes[0], bytes[1] );
    }

    inline bool isMonoMode( Message_t * msg ){
        ASSERT( msg != NULL );

        return isMonoMode( msg->StatusClass, msg->Data.ControlChange.Controller );
    }

    inline uint8_t getMonoModeNumberOfChannels( uint8_t * bytes ){
        ASSERT( bytes != NULL );
        ASSERT( isMonoMode(bytes[0], bytes[1]) );

        return bytes[2] & DataMask;
    }

    inline uint8_t getMonoModeNumberOfChannels( Message_t * msg ){
        ASSERT( msg != NULL );
        ASSERT( isMonoMode(msg) );

        return msg->Data.ControlChange.Value;
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
        ASSERT( bytes != NULL );

        return isPolyModeOn( bytes[0] , bytes[1], bytes[2] );
    }

    inline bool isPolyModeOn( Message_t * msg ){
        ASSERT( msg != NULL );

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
        ASSERT( messageType != NULL );
        ASSERT( nibble != NULL );

        if ( len != MsgLenMidiTimeCodeQuarterFrame ) {
            return false;
        }
        if (bytes[0] != SystemMessageMidiTimeCodeQuarterFrame) {
            return false;
        }
        if ((bytes[1] & DataMask) != bytes[1] ) {
            return false;
        }

        *messageType = (bytes[1] >> 4) & 0x05;
        *nibble = bytes[1] & NibbleMask;

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
        ASSERT( position != NULL );

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
        ASSERT( song != NULL );

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
        ASSERT( (deviceId & DataMask) == deviceId );
        ASSERT( isMidiTimeCodeFrameRate(fps) );
        ASSERT( hour <= MaxHour );
        ASSERT( minute <= MaxMinute );
        ASSERT( second <= MaxSecond );
        ASSERT( frame <= MaxFps[fps] );

        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = ReservedSysExIdRealTime;
        bytes[2] = deviceId & DataMask;
        bytes[3] = UniversalSysExRtMidiTimeCode;
        bytes[4] = UniversalSysExRtMidiTimeCodeFullMessage;
        bytes[5] = ((fps << FpsOffset) & FpsMask) | (hour & HourMask);
        bytes[6] = minute & MinuteMask;
        bytes[7] = second & SecondMask;
        bytes[8] = frame & FrameMask;
        bytes[9] = SystemMessageEndOfSystemExclusive;

        return MsgLenSysExMidiTimeCodeFullMessage;
    }

    inline int packSysExMidiTimeCodeFullMessage( uint8_t * bytes, uint8_t deviceId, MidiTimeCode_t * mtc){
        ASSERT( bytes != NULL );
        ASSERT( mtc != NULL );

        return packSysExMidiTimeCodeFullMessage( bytes, deviceId, getFps(mtc->FpsHour), getHour(mtc->FpsHour), mtc->Minute, mtc->Second, mtc->Frame );
    }

    inline bool unpackSysExMidiTimeCodeFullMessage( uint8_t * bytes, int len, uint8_t * deviceId, uint8_t * fps, uint8_t * hour, uint8_t * minute, uint8_t * second, uint8_t * frame ) {
        ASSERT( bytes != NULL );
        ASSERT( deviceId != NULL );
        ASSERT( fps != NULL );
        ASSERT( hour != NULL );
        ASSERT( minute != NULL );
        ASSERT( second != NULL );
        ASSERT( frame != NULL );

        if ( (len !=  MsgLenSysExMidiTimeCodeFullMessage - 1) && (len != MsgLenSysExMidiTimeCodeFullMessage || bytes[MsgLenSysExMidiTimeCodeFullMessage-1] == SystemMessageEndOfSystemExclusive) ){
            return false;
        }
        if (bytes[0] != SystemMessageSystemExclusive) {
            return false;
        }
        if (bytes[1] != ReservedSysExIdRealTime ) {
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

        *deviceId = bytes[2] & DataMask;
        *fps = (bytes[5] >> 5) & 0b11;
        *hour = bytes[5] & 0b00011111;
        *minute = bytes[6] & 0b00111111;
        *second = bytes[7] & 0b00111111;
        *frame = bytes[8] & 0b00011111;


        return true;
    }

    inline bool unpackSysExMidiTimeCodeFullMessage( uint8_t * bytes, int len, uint8_t * deviceId, MidiTimeCode_t * mtc ){
        ASSERT( bytes != NULL );
        ASSERT( deviceId != NULL );
        ASSERT( mtc != NULL );

        MidiTimeCodeFrameRate_t fps = MidiTimeCodeFrameRate24fps;

        if ( ! unpackSysExMidiTimeCodeFullMessage( bytes, len, deviceId, (uint8_t*)&fps, &mtc->FpsHour, &mtc->Minute, &mtc->Second, &mtc->Frame) ) {
            return false;
        }

        mtc->FpsHour |= setFps(fps);
        mtc->FractionalFrame = 0;

        return true;
    }

    inline int packSysExMidiTimeCodeUserBits( uint8_t * bytes, uint8_t deviceId, uint8_t * userBits ) {
        ASSERT( bytes != NULL );
        ASSERT((deviceId & DataMask) == deviceId );
        ASSERT( userBits != NULL );

        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = ReservedSysExIdRealTime;
        bytes[2] = deviceId & DataMask;
        bytes[3] = UniversalSysExRtMidiTimeCode;
        bytes[4] = UniversalSysExRtMidiTimeCodeUserBits;
        bytes[5] = getLsNibble(userBits[3]);
        bytes[6] = getMsNibble(userBits[3]);
        bytes[7] = getLsNibble(userBits[2]);
        bytes[8] = getMsNibble(userBits[2]);
        bytes[9] = getLsNibble(userBits[1]);
        bytes[10] = getMsNibble(userBits[1]);
        bytes[11] = getLsNibble(userBits[0]);
        bytes[12] = getMsNibble(userBits[0]);
        bytes[13] = userBits[4] & 0b00000011; //TODO is this right???
        bytes[14] = SystemMessageEndOfSystemExclusive;

        return MsgLenSysExMidiTimeCodeUserBits;
    }

    inline int unpackSysExMidiTimeCodeUserBits( uint8_t * bytes, int len, uint8_t * deviceId, uint8_t * userBits ) {
        ASSERT( bytes != NULL );
        ASSERT( deviceId != NULL );
        ASSERT( userBits != NULL );

        if ( (len !=  MsgLenSysExMidiTimeCodeUserBits - 1) && (len != MsgLenSysExMidiTimeCodeUserBits || bytes[MsgLenSysExMidiTimeCodeUserBits-1] == SystemMessageEndOfSystemExclusive) ){
            return false;
        }
        if (bytes[0] != SystemMessageSystemExclusive) {
            return false;
        }
        if (bytes[1] != ReservedSysExIdRealTime ) {
            return false;
        }
        if ((bytes[2] & DataMask) != bytes[2] ) {
            return false;
        }
        if (bytes[3] != UniversalSysExRtMidiTimeCode) {
            return false;
        }
        if (bytes[4] != UniversalSysExRtMidiTimeCodeUserBits){
            return false;
        }

        *deviceId = bytes[2] & DataMask;
        userBits[3] = ((bytes[5] & NibbleMask) << 4) | (bytes[6] & NibbleMask);
        userBits[2] = ((bytes[7] & NibbleMask) << 4) | (bytes[8] & NibbleMask);
        userBits[1] = ((bytes[9] & NibbleMask) << 4) | (bytes[10] & NibbleMask);
        userBits[0] = ((bytes[11] & NibbleMask) << 4) | (bytes[12] & NibbleMask);
        userBits[4] = bytes[13] & 0b00000011; //TODO is this right??

        return len;
    }

    inline int packSysExNonRtMidiCueingSetupMessage( UniversalSysExNonRtMidiTimeCode_t msgType, uint8_t * bytes, uint8_t deviceId, uint8_t fps, uint8_t hour, uint8_t minute, uint8_t second, uint8_t frame, uint8_t fractionalFrame, uint16_t eventNumber, uint8_t * addInfo, uint8_t addInfoLen) {
        ASSERT( isUniversalSysExNonRtMidiTimeCode( msgType ) );
        ASSERT( !isUniversalSysExNonRtMidiTimeCodeWithAddInfo( msgType ) || addInfoLen == 0 );
        ASSERT( bytes != NULL );
        ASSERT( (deviceId & DataMask) == deviceId );
        ASSERT( isMidiTimeCodeFrameRate(fps) );
        ASSERT( hour <= MaxHour );
        ASSERT( minute <= MaxMinute );
        ASSERT( second <= MaxSecond );
        ASSERT( frame <= MaxFps[fps] );
        ASSERT( fractionalFrame <= MaxFractionalFrame );
        ASSERT( eventNumber <= MaxEventNumber );

        int msgLen = MsgLenSysExNonRtMidiCueingSetupMessageMin - 1; // min message length (w/o EOX)

        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = ReservedSysExIdNonRealTime;
        bytes[2] = deviceId & DataMask;
        bytes[3] = UniversalSysExNonRtMidiTimeCode;
        bytes[4] = msgType;
        bytes[5] = ((fps << FpsOffset) & FpsMask) | (hour & HourMask);
        bytes[6] = minute & MinuteMask;
        bytes[7] = second & SecondMask;
        bytes[8] = frame & FrameMask;
        bytes[9] = fractionalFrame & FractionalFrameMask;

        packDoubleValue( &bytes[10], eventNumber );

        if ( isUniversalSysExNonRtMidiTimeCodeWithAddInfo( msgType ) ) {
            msgLen += nibblize(&bytes[7], addInfo, addInfoLen);
        }

        bytes[msgLen++] = SystemMessageEndOfSystemExclusive;

        return msgLen;
    }

    inline int packSysExNonRtMidiCueingSetupMessage(  uint8_t * bytes, UniversalSysExNonRtMidiTimeCode_t msgType, uint8_t deviceId, MidiTimeCode_t * mtc, uint16_t eventNumber, uint8_t * addInfo, uint8_t addInfoLen ){
        ASSERT( bytes != NULL );
        ASSERT( mtc != NULL );

        return packSysExNonRtMidiCueingSetupMessage( msgType, bytes, deviceId, getFps(mtc->FpsHour), mtc->FpsHour & HourMask, mtc->Minute, mtc->Second, mtc->Frame, mtc->FractionalFrame, eventNumber, addInfo, addInfoLen );
    }

    inline bool unpackSysExNonRtMidiCueingSetupMessage( uint8_t * bytes, int len, UniversalSysExNonRtMidiTimeCode_t * msgType, uint8_t * deviceId, uint8_t * fps, uint8_t * hour, uint8_t * minute, uint8_t * second, uint8_t * frame, uint8_t  * fractionalFrame, uint16_t  * eventNumber, uint8_t * addInfo, uint8_t * addInfoLen ) {
        ASSERT( bytes != NULL );
        ASSERT( deviceId != NULL );
        ASSERT( fps != NULL );
        ASSERT( hour != NULL );
        ASSERT( minute != NULL );
        ASSERT( second != NULL );
        ASSERT( frame != NULL );
        ASSERT( fractionalFrame != NULL );
        ASSERT( eventNumber != NULL );
        ASSERT( addInfo != NULL );
        ASSERT( addInfoLen != NULL );


        if ( len <  MsgLenSysExNonRtMidiCueingSetupMessageMin - 1 ){
            return false;
        }
        if (bytes[0] != SystemMessageSystemExclusive) {
            return false;
        }
        if (bytes[1] != ReservedSysExIdNonRealTime ) {
            return false;
        }
        if ((bytes[2] & DataMask) != bytes[2] ) {
            return false;
        }
        if (bytes[3] != UniversalSysExNonRtMidiTimeCode) {
            return false;
        }
        if ( ! isUniversalSysExNonRtMidiTimeCode( bytes[4] ) ){
            return false;
        }

        *deviceId = bytes[2] & DataMask;
        *msgType = (UniversalSysExNonRtMidiTimeCode_t)(bytes[4]);
        *fps = (bytes[5] >> 5) & 0b11;
        *hour = bytes[5] & HourMask;
        *minute = bytes[6] & MinuteMask;
        *second = bytes[7] & SecondMask;
        *frame = bytes[8] & FrameMask;
        *fractionalFrame = bytes[9] & FractionalFrameMask;

        unpackDoubleValue( &bytes[10], eventNumber );

        if (isUniversalSysExNonRtMidiTimeCodeWithAddInfo(bytes[4])){

            if (bytes[len - 1] != SystemMessageEndOfSystemExclusive){
                len++;
            }

            *addInfoLen = denibblize( addInfo, &bytes[12], len - MsgLenSysExNonRtMidiCueingSetupMessageMin );
        }

        return true;
    }

    inline bool unpackSysExNonRtMidiCueingSetupMessage( uint8_t * bytes, int len, UniversalSysExNonRtMidiTimeCode_t * msgType, uint8_t * deviceId, MidiTimeCode_t * mtc, uint16_t  * eventNumber, uint8_t * addInfo, uint8_t * addInfoLen  ){
        ASSERT( bytes != NULL );
        ASSERT( mtc != NULL );

        MidiTimeCodeFrameRate_t fps = MidiTimeCodeFrameRate24fps;

        if ( ! unpackSysExNonRtMidiCueingSetupMessage( bytes, len, msgType, deviceId, (uint8_t*)&fps, &mtc->FpsHour, &mtc->Minute, &mtc->Second, &mtc->Frame, &mtc->FractionalFrame, eventNumber, addInfo, addInfoLen ) ) {
            return false;
        }

        mtc->FpsHour |= setFps(fps);

        return true;
    }


    inline int packSysExRtMidiCueingSetupMessage( uint8_t * bytes, UniversalSysExRtMidiTimeCodeCueing_t msgType, uint8_t deviceId, uint16_t eventNumber, uint8_t * addInfo, uint8_t addInfoLen) {
        ASSERT( isUniversalSysExRtMidiTimeCodeCueing( msgType ) );
        ASSERT( !isUniversalSysExRtMidiTimeCodeCueing( msgType ) || addInfoLen == 0 );
        ASSERT( bytes != NULL );
        ASSERT( (deviceId & DataMask) == deviceId );
        ASSERT( eventNumber <= MaxEventNumber );

        int msgLen = MsgLenSysExRtMidiCueingSetupMessageMin - 1; // min message length (w/o EOX)

        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = ReservedSysExIdRealTime;
        bytes[2] = deviceId & DataMask;
        bytes[3] = UniversalSysExNonRtMidiTimeCode;
        bytes[4] = msgType;

        packDoubleValue( &bytes[5], eventNumber );

        if ( isUniversalSysExRtMidiTimeCodeCueingWithAddInfo( msgType ) ) {
            msgLen += nibblize(&bytes[7], addInfo, addInfoLen);
        }

        bytes[msgLen++] = SystemMessageEndOfSystemExclusive;

        return msgLen;
    }


    inline bool unpackSysExRtMidiCueingSetupMessage( uint8_t * bytes, int len, UniversalSysExRtMidiTimeCodeCueing_t * msgType, uint8_t * deviceId, uint16_t  * eventNumber, uint8_t * addInfo, uint8_t * addInfoLen ) {
        ASSERT( bytes != NULL );
        ASSERT( deviceId != NULL );
        ASSERT( eventNumber != NULL );
        ASSERT( addInfo != NULL );
        ASSERT( addInfoLen != NULL );


        if ( len <  MsgLenSysExRtMidiCueingSetupMessageMin - 1 ){
            return false;
        }
        if (bytes[0] != SystemMessageSystemExclusive) {
            return false;
        }
        if (bytes[1] != ReservedSysExIdRealTime ) {
            return false;
        }
        if ((bytes[2] & DataMask) != bytes[2] ) {
            return false;
        }
        if (bytes[3] != UniversalSysExRtMidiTimeCode) {
            return false;
        }
        if ( ! isUniversalSysExRtMidiTimeCodeCueing( bytes[4] ) ){
            return false;
        }


        *deviceId = bytes[2] & DataMask;
        *msgType = (UniversalSysExRtMidiTimeCodeCueing_t)(bytes[4]);

        unpackDoubleValue( &bytes[10], eventNumber );

        if ( isUniversalSysExRtMidiTimeCodeCueingWithAddInfo(bytes[4]) ){

            if (bytes[len - 1] != SystemMessageEndOfSystemExclusive){
                len++;
            }

            *addInfoLen = denibblize( addInfo, &bytes[12], len - MsgLenSysExRtMidiCueingSetupMessageMin );
        }

        return true;
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
