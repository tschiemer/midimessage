//
// Created by Philip Tschiemer on 19.07.19.
//

#ifndef MIDIMESSAGE_H
#define MIDIMESSAGE_H

#include <cstdint>

#ifndef USE_ASSERTS
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
// Several standards and best practices mention that MIDI messages are at most 128 bytes long.
#define SYSEX_MEMORY_STATIC_SIZE 128
#endif
#if SYSEX_MEMORY_STATIC_SIZE < 1
#warning If using static sysex memory please specify memory size
#endif
#endif // SYSEX_MEMORY == SYSEX_MEMORY_STATIC

// Require dynamic memory allocaction to support global parameter control
//#if SYSEX_MEMORY != SYSEX_MEMORY_DYNAMIC
//#ifndef DISABLE_GLOBAL_PARAMETER_CONTROL
//#define DISABLE_GLOBAL_PARAMETER_CONTROL
//#endif
//#endif

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

    inline void unpackDoubleValue( uint8_t * bytes, uint16_t * value ){
        ASSERT( (bytes[0] & DataMask) == bytes[0] );
        ASSERT( (bytes[1] & DataMask) == bytes[1] );

        *value = (((uint16_t)bytes[1]) << 7) | ((uint16_t)bytes[0]);
    }

    inline void packDoubleValue( uint8_t * bytes, uint16_t value ) {
        bytes[0] = value & DataMask;
        bytes[1] = (value >> 7) & DataMask;
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
    const int MsgLenMtcQuarterFrame                 = 2;
    const int MsgLenSongPositionPointer             = 3;
    const int MsgLenSongSelect                      = 2;

    const int MsgLenSysExNonRtMtcCueingSetupMessageMin  = 13;
    const int MsgLenSysExNonRtGenInfoIdentityRequest = 6;
    const int MsgLenSysExNonRtGenInfoIdentityReply = 15;
    const int MsgLenSysExNonRtDeviceControl = 8;

    const int MsgLenSysExRtMtcFullMessage    = 10;
    const int MsgLenSysExRtMtcUserBits       = 15;
    const int MsgLenSysExRtMtcCueingSetupMessageMin  = 8;
    const int MsgLenSysExRtMmcCommandWithoutInfo = 6;



    const uint16_t PitchCenter      = 0x2000;

    const uint16_t CoarseTuningCenter = 0x0040;
    const uint16_t CoarseTuningMax    = 0x007F;
    const uint16_t FineTuningCenter   = 0x2000; // 0xA440 when in two 7bit values
    const uint16_t FineTuningMax      = 0x3FFF; // 0x7F7F

    const uint8_t SysExDeviceIdBroadcast = 0x7F; // for  SysEx (Non-)Realtime Messages

    inline bool isSysExDeviceIdGroup( uint8_t value ){
        ASSERT( (value & DataMask) == value );

        return (0x70 <= value) && (value <= 0x7E);
    }

    inline uint8_t getSysExDevceIdGroup( uint8_t value ){
        ASSERT( isSysExDeviceIdGroup(value) );

        return (value >= 0x70) ? (value - 0x70) : 0x7F;
    }


    const uint8_t FpsMask       = 0b01100000;
    const uint8_t FpsOffset     = 5;
    const uint8_t HourMask      = 0b00011111;
    const uint8_t MinuteMask    = 0b00111111;
    const uint8_t SecondMask    = 0b00111111;
    const uint8_t FrameMask     = 0b00011111;
    const uint8_t FrameSignBit  = 0b01000000;
    const uint8_t FrameFinalByteIdBit   = 0b00100000;
    const uint8_t FractionalFrameMask   = 0b01111111;
    const uint8_t StatusCodeFlagBit     = 0b01000000;
    const uint8_t StatusInvalidCodeBit  = 0b00100000;
    const uint8_t StatusVideoIdBit      = 0b00010000;

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
        SystemMessageMtcQuarterFrame = 0xF1,
        SystemMessageSongPositionPointer = 0xF2,
        SystemMessageSongSelect = 0xF3,
        SystemMessageTuneRequest = 0xF6,
        SystemMessageEndOfExclusive = 0xF7,
        SystemMessageTimingClock = 0xF8,
        SystemMessageStart = 0xFA,
        SystemMessageContinue = 0xFB,
        SystemMessageStop = 0xFC,
        SystemMessageActiveSensing = 0xFE,
        SystemMessageReset = 0xFF
    } SystemMessage_t;

    inline bool isSystemMessage( uint8_t value ){
        return (value == SystemMessageSystemExclusive ||
                value == SystemMessageMtcQuarterFrame ||
                value == SystemMessageSongPositionPointer ||
                value == SystemMessageSongSelect ||
                value == SystemMessageTuneRequest ||
                value == SystemMessageEndOfExclusive ||
                value == SystemMessageTimingClock ||
                value == SystemMessageStart ||
                value == SystemMessageContinue ||
                value == SystemMessageStop ||
                value == SystemMessageActiveSensing ||
                value == SystemMessageReset);
    }

    inline bool isSystemCommonMessage( uint8_t value ){
        return (value == SystemMessageSystemExclusive ||
                value == SystemMessageMtcQuarterFrame ||
                value == SystemMessageSongPositionPointer ||
                value == SystemMessageSongSelect ||
                value == SystemMessageTuneRequest ||
                value == SystemMessageEndOfExclusive);
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
        SysExIdManufacturerExtension  = 0x00,
        SysExIdExperimental           = 0x7D,
        SysExIdNonRealTime            = 0x7E,
        SysExIdRealTime               = 0x7F
    } SysExId_t;

    inline bool isSysExId( uint8_t value ){
        return (value == SysExIdManufacturerExtension ||
                value == SysExIdExperimental ||
                value == SysExIdNonRealTime ||
                value == SysExIdRealTime);
    }

    inline bool isSysExManufacturerId( uint8_t byte0 ){
        return (byte0 != SysExIdExperimental &&
                byte0 != SysExIdNonRealTime &&
                byte0 != SysExIdRealTime);
    }
    inline bool isSysExManufacturerId( uint32_t id ){
        return isSysExManufacturerId( (uint8_t)((id >> 16) & DataMask));
    }

    inline bool is1ByteManufacturerId( uint8_t * bytes ){
        return bytes[0] == SysExIdManufacturerExtension;
    }

    inline bool is3ByteManufacturerId( uint8_t * bytes ){
        return !is1ByteManufacturerId( bytes );
    }
    inline bool is1ByteManufacturerId( uint32_t id ){
        return ((id >> 16) & DataMask) == SysExIdManufacturerExtension;
    }

    inline bool is3ByteManufacturerId( uint32_t id ){
        return !is1ByteManufacturerId(  id );
    }


    inline uint8_t packSysExId( uint8_t * bytes, uint32_t id ){
        ASSERT( bytes != NULL );

        bytes[0] = (id >> 16) & DataMask;

        if (bytes[0] == SysExIdManufacturerExtension){
            bytes[1] = (id >> 8) & DataMask;
            bytes[2] = id & DataMask;
            return 3;
        }
        
        return 1;
    }

    inline uint8_t unpackSysExId( uint8_t * bytes, uint32_t * id){
        ASSERT( bytes != NULL );

        *id = ((uint32_t)bytes[0]) << 16;

        if (bytes[0] == SysExIdManufacturerExtension){
            *id |= ((uint32_t)bytes[1]) << 8;
            *id |= ((uint32_t)bytes[2]);
            return 3;
        }

        return 1;
    }


    typedef enum {
        SysExNonRtSampleDumpHeader       = 0x01,
        SysExNonRtSampleDataPacket       = 0x02, // 
        SysExNonRtSampleDumpRequest      = 0x03, // 
        SysExNonRtMidiTimeCode           = 0x04, // SubId2 enum SysExNonRtMtc..
        SysExNonRtSampleDumpExtension    = 0x05, // SubId2 enum SysExNonRtSds..
        SysExNonRtGeneralInformation     = 0x06, // SubId2 enum SysExNonRtGenInfo..
        SysExNonRtFileDump               = 0x07, // SubId2 enum SysExNonRtFileDump..
        SysExNonRtMidiTuningStandard     = 0x08, // SubId2 enum SysExNonRtMts..
        SysExNonRtGeneralMidi            = 0x09, // SubId2 enum SysExNonRtGm..
        SysExNonRtDownloadableSounds     = 0x0A, // SubId2 enum SysExNonRtDls..
        SysExNonRtFileReferenceMessage   = 0x0B, // SubId2 enum SysExNonRtFileRef..
        SysExNonRtMidiVisualControl      = 0x0C, // SubId2 enum SysExNonRtMvc..
        SysExNonRtMidiCapabilityInquiry  = 0x0D, // SubId2 enum SysExNonRtCapInq..

        SysExNonRtEndOfFile              = 0x7B,
        SysExNonRtWait                   = 0x7C,
        SysExNonRtCancel                 = 0x7D,
        SysExNonRtNAK                    = 0x7E,
        SysExNonRtACK                    = 0x7F
    } SysExNonRt_t;
    
    inline bool isSysExNonRt( uint8_t value ){
        return (value == SysExNonRtSampleDumpHeader ||
                value == SysExNonRtSampleDataPacket ||
                value == SysExNonRtSampleDumpRequest ||
                value == SysExNonRtMidiTimeCode ||
                value == SysExNonRtSampleDumpExtension ||
                value == SysExNonRtGeneralInformation ||
                value == SysExNonRtFileDump ||
                value == SysExNonRtMidiTuningStandard ||
                value == SysExNonRtGeneralMidi ||
                value == SysExNonRtDownloadableSounds ||
                value == SysExNonRtFileReferenceMessage ||
                value == SysExNonRtMidiVisualControl ||
                value == SysExNonRtMidiCapabilityInquiry ||
                value == SysExNonRtEndOfFile ||
                value == SysExNonRtWait ||
                value == SysExNonRtCancel ||
                value == SysExNonRtNAK ||
                value == SysExNonRtACK);
    }

    typedef enum {
        SysExNonRtMtcSpecial                  = 0x00,
        SysExNonRtMtcPunchInPoint             = 0x01,
        SysExNonRtMtcPunchOutPoint            = 0x02,
        SysExNonRtMtcDeletePunchInPoint       = 0x03,
        SysExNonRtMtcDeletePunchOutPoint      = 0x04,
        SysExNonRtMtcEventStartPoint          = 0x05,
        SysExNonRtMtcEventStopPoint           = 0x06,
        SysExNonRtMtcEventStartPointsWithInfo = 0x07,
        SysExNonRtMtcEventStopPointsWithInfo  = 0x08,
        SysExNonRtMtcDeleteEventStartPoint    = 0x09,
        SysExNonRtMtcDeleteEventStopPoint     = 0x0A,
        SysExNonRtMtcCuePoints                = 0x0B,
        SysExNonRtMtcCuePointsWithInfo        = 0x0C,
        SysExNonRtMtcDeleteCuePoint           = 0x0D,
        SysExNonRtMtcEventNameInInfo          = 0x0E
    } SysExNonRtMtc_t;

    inline bool isSysExNonRtMtc( uint8_t value ){
        return (value == SysExNonRtMtcSpecial ||
                value == SysExNonRtMtcPunchInPoint ||
                value == SysExNonRtMtcPunchOutPoint ||
                value == SysExNonRtMtcDeletePunchInPoint ||
                value == SysExNonRtMtcDeletePunchOutPoint ||
                value == SysExNonRtMtcEventStartPoint ||
                value == SysExNonRtMtcEventStopPoint ||
                value == SysExNonRtMtcEventStartPointsWithInfo ||
                value == SysExNonRtMtcEventStopPointsWithInfo ||
                value == SysExNonRtMtcDeleteEventStartPoint ||
                value == SysExNonRtMtcDeleteEventStopPoint ||
                value == SysExNonRtMtcCuePoints ||
                value == SysExNonRtMtcCuePointsWithInfo ||
                value == SysExNonRtMtcDeleteCuePoint ||
                value == SysExNonRtMtcEventNameInInfo);
    }

    inline bool isSysExNonRtMtcWithAddInfo( uint8_t value ){
        return (value == SysExNonRtMtcEventStartPointsWithInfo ||
                value == SysExNonRtMtcEventStopPointsWithInfo ||
                value == SysExNonRtMtcCuePointsWithInfo ||
                value == SysExNonRtMtcEventNameInInfo);
    }

    typedef enum {
        SysExNonRtSdsLoopPointsTransmission    = 0x01,
        SysExNonRtSdsLoopPointsRequest         = 0x02,
        SysExNonRtSdsSampleNameTransmission    = 0x03,
        SysExNonRtSdsSampleNameRequest         = 0x04,
        SysExNonRtSdsExtendedDumpHeader        = 0x05,
        SysExNonRtSdsExtendedLoopPointsTransmission    = 0x06,
        SysExNonRtSdsExtendedLoopPointsRequest = 0x07
    } SysExNonRtSds_t;

    inline bool isSysExNonRtSds( uint8_t value ){
        return (value == SysExNonRtSdsLoopPointsTransmission ||
                value == SysExNonRtSdsLoopPointsRequest ||
                value == SysExNonRtSdsSampleNameTransmission ||
                value == SysExNonRtSdsSampleNameRequest ||
                value == SysExNonRtSdsExtendedDumpHeader ||
                value == SysExNonRtSdsExtendedLoopPointsTransmission ||
                value == SysExNonRtSdsExtendedLoopPointsRequest);
    }

    typedef enum {
        SysExNonRtGenInfoIdentityRequest    = 0x01,
        SysExNonRtGenInfoIdentityReply      = 0x02
    } SysExNonRtGenInfo_t;

    inline bool isSysExNonRtGeneralInformation( uint8_t value ){
        return (value == SysExNonRtGenInfoIdentityRequest ||
                value == SysExNonRtGenInfoIdentityReply);
    }

    typedef enum {
        SysExNonRtFileDumpHeader       = 0x01,
        SysExNonRtFileDumpDataPacket   = 0x02,
        SysExNonRtFileDumpRequest      = 0x03
    } SysExNonRtFileDump_t;

    inline bool isSysExNonRtFileDump( uint8_t value ){
        return (value == SysExNonRtFileDumpHeader ||
                value == SysExNonRtFileDumpDataPacket ||
                value == SysExNonRtFileDumpRequest);
    }

    typedef enum {
        SysExNonRtMtsBulkDumpRequest        = 0x00,
        SysExNonRtMtsBulkDumpReply          = 0x01,
        SysExNonRtMtsTuningDumpRequest      = 0x02,
        SysExNonRtMtsKeybasedTuningDump     = 0x03,
        SysExNonRtMtsScaleTuningDump1Byte   = 0x04,
        SysExNonRtMtsScaleTuningDump2Byte   = 0x05,
        SysExNonRtMtsSingleNoteTuningChange = 0x06,
        SysExNonRtMtsScaleTuning1Byte       = 0x07,
        SysExNonRtMtsScaleTuning2Byte       = 0x08
    } SysExNonRtMts_t;

    inline bool isSysExNonRtMts( uint8_t value ){
        return (value == SysExNonRtMtsBulkDumpRequest ||
                value == SysExNonRtMtsBulkDumpReply ||
                value == SysExNonRtMtsTuningDumpRequest ||
                value == SysExNonRtMtsKeybasedTuningDump ||
                value == SysExNonRtMtsScaleTuningDump1Byte ||
                value == SysExNonRtMtsSingleNoteTuningChange ||
                value == SysExNonRtMtsScaleTuning1Byte ||
                value == SysExNonRtMtsScaleTuning2Byte);
    }

    typedef enum {
        SysExNonRtGmSystemOn1     = 0x01,
        SysExNonRtGmSystemOff     = 0x02,
        SysExNonRtGmSystemOn2     = 0x03
    } SysExNonRtGm_t;

    inline bool isSysExNonRtGeneralMidi( uint8_t value ){
        return (value == SysExNonRtGmSystemOn1 ||
                value == SysExNonRtGmSystemOff ||
                value == SysExNonRtGmSystemOn2);
    }

    typedef enum {
        SysExNonRtDlsTurnDlsOn      = 0x01,
        SysExNonRtDlsTurnDlsOff     = 0x02,
        SysExNonRtDlsTurnDlsVoiceAllocOff = 0x03,
        SysExNonRtDlsTurnDlsVoiceAllocOn  = 0x04
    } SysExNonRtDls_t;

    inline bool isSysExNonRtDownloadableSounds( uint8_t value ){
        return (value == SysExNonRtDlsTurnDlsOn ||
                value == SysExNonRtDlsTurnDlsOff ||
                value == SysExNonRtDlsTurnDlsVoiceAllocOff ||
                value == SysExNonRtDlsTurnDlsVoiceAllocOn);
    }

    typedef enum {
        SysExNonRtFileRefOpenFile         = 0x01,
        SysExNonRtFileRefSelectContents   = 0x02,
        SysExNonRtFileRefOpenAndSelect    = 0x03,
        SysExNonRtFileRefCloseFile        = 0x04
    } SysExNonRtFileRef_t;

    inline bool isSysExNonRtFileReferenceMessage( uint8_t value ){
        return (value == SysExNonRtFileRefOpenFile ||
                value == SysExNonRtFileRefSelectContents ||
                value == SysExNonRtFileRefOpenAndSelect ||
                value == SysExNonRtFileRefCloseFile);
    }

    typedef enum {
        SysExNonRtMvcTODO //TODO
    } SysExNonRtMvc_t;

    inline bool isSysExNonRtMvc( uint8_t value ){
        return false; //TODO
    }

    typedef enum {
        SysExNonRtMidiCapInqTODO //TODO
    } SysExNonRtMidiCapInq_t;

    inline bool isSysExNonRtMidiCapInq( uint8_t value ){
        return false; //TODO
    }

    typedef enum {
        SysExRtMidiTimeCode                  = 0x01, // SubId2 enum SysExRtMtc..
        SysExRtMidiShowControl               = 0x02, // SubId2 enum SysExRtMsc.. 
        SysExRtDeviceControl                 = 0x04, // SubId2 enum SysExRtDc..
        SysExRtMidiTimeCodeCueing            = 0x05, // SubId2 enum SysExRtMtcCueing..
        SysExRtMidiMachineControlCommand     = 0x06, // SubId2 enum SysExRtMmcCommand..
        SysExRtMidiMachineControlResponse    = 0x07, // SubId2 enum SysExRtMmcResponse..
        SysExRtMidiTuningStandard            = 0x08, // SubId2 enum SysExRtMts..
        SysExRtControllerDestinationSetting  = 0x09, // SubId2 enum SysExRtCds..
        SysExRtKeybasedInstrumentControl     = 0x0A, // SubId2 enum SysExRtKeysControl..
        SysExRtScalablePolyphonyMidiMip      = 0x0B, // SubId2 enum SysExRtMip..
        SysExRtMobilePhoneControlMessage     = 0x0C  // SubId2 enum SysExRtMobile..
    } SysExRt_t;

    inline bool isSysExRt( uint8_t value ){
        return (value == SysExRtMidiTimeCode ||
                value == SysExRtMidiShowControl ||
                value == SysExRtDeviceControl ||
                value == SysExRtMidiTimeCodeCueing ||
                value == SysExRtMidiMachineControlCommand ||
                value == SysExRtMidiMachineControlResponse ||
                value == SysExRtMidiTuningStandard ||
                value == SysExRtControllerDestinationSetting ||
                value == SysExRtKeybasedInstrumentControl ||
                value == SysExRtScalablePolyphonyMidiMip ||
                value == SysExRtMobilePhoneControlMessage);
    }

    typedef enum {
        SysExRtMidiTimeCodeFullMessage           = 0x01,
        SysExRtMidiTimeCodeUserBits              = 0x02
    } SysExRtMidiTimeCode_t;

    inline bool isSysExRtMidiTimeCode( uint8_t value ){
        return (value == SysExRtMidiTimeCodeFullMessage ||
                value == SysExRtMidiTimeCodeUserBits);
    }

    typedef enum {
        SysExRtMscLighting                      = 0x01,
        SysExRtMscMovingLights                  = 0x02,
        SysExRtMscColorChangers                 = 0x03,
        SysExRtMscStrobes                       = 0x04,
        SysExRtMscLasers                        = 0x05,
        SysExRtMscChasers                       = 0x06,

        SysExRtMscSound                         = 0x10,
        SysExRtMscMusic                         = 0x11,
        SysExRtMscCdPlayers                     = 0x12,
        SysExRtMscEpromPlayback                 = 0x13,
        SysExRtMscAudioTapeMachines             = 0x14,
        SysExRtMscIntercoms                     = 0x15,
        SysExRtMscAmplifiers                    = 0x16,
        SysExRtMscAudioEffectsDevices           = 0x17,
        SysExRtMscEqualizers                    = 0x18,

        SysExRtMscMachinery                     = 0x20,
        SysExRtMscRigging                       = 0x21,
        SysExRtMscFlys                          = 0x22,
        SysExRtMscLifts                         = 0x23,
        SysExRtMscTurntables                    = 0x24,
        SysExRtMscTrusses                       = 0x25,
        SysExRtMscRobots                        = 0x26,
        SysExRtMscAnimation                     = 0x27,
        SysExRtMscFloats                        = 0x28,
        SysExRtMscBreakaways                    = 0x29,
        SysExRtMscBarges                        = 0x2A,

        SysExRtMscVideo                         = 0x30,
        SysExRtMscVideoTapeMachines             = 0x31,
        SysExRtMscVideoCassetteMachines         = 0x32,
        SysExRtMscVideoDiscPlayers              = 0x33,
        SysExRtMscVideoSwitchers                = 0x34,
        SysExRtMscVideoEffects                  = 0x35,
        SysExRtMscVideoCharacterGenerators      = 0x36,
        SysExRtMscVideoStillStores              = 0x37,
        SysExRtMscVideoMonitors                 = 0x38,

        SysExRtMscProjection                    = 0x40,
        SysExRtMscFilmProjectors                = 0x41,
        SysExRtMscSlideProjectors               = 0x42,
        SysExRtMscVideoProjectors               = 0x43,
        SysExRtMscDissolvers                    = 0x44,
        SysExRtMscShutterControls               = 0x45,

        SysExRtMscProcessControl                = 0x50,
        SysExRtMscHydraulicOil                  = 0x51,
        SysExRtMscH2O                           = 0x52,
        SysExRtMscWater                         = SysExRtMscH2O,
        SysExRtMscCO2                           = 0x53,
        SysExRtMscDryIce                        = SysExRtMscCO2,
        SysExRtMscCompressedAir                 = 0x54,
        SysExRtMscNaturalGas                    = 0x55,
        SysExRtMscFog                           = 0x56,
        SysExRtMscSmoke                         = 0x57,
        SysExRtMscCrackedHaze                   = 0x58,

        SysExRtMscPyro                          = 0x60,
        SysExRtMscFireworks                     = 0x61,
        SysExRtMscExplosions                    = 0x62,
        SysExRtMscFlame                         = 0x63,
        SysExRtMscSmokePots                     = 0x64,

        SysExRtMscAllTypes                      = 0x7F

    } SysExRtMsc_t;

    inline bool isSysExRtMidiShowControl( uint8_t value ){
        return (value == SysExRtMscLighting ||
                value == SysExRtMscMovingLights ||
                value == SysExRtMscColorChangers ||
                value == SysExRtMscStrobes ||
                value == SysExRtMscLasers ||
                value == SysExRtMscChasers ||
                value == SysExRtMscMusic ||
                value == SysExRtMscCdPlayers ||
                value == SysExRtMscEpromPlayback ||
                value == SysExRtMscAudioTapeMachines ||
                value == SysExRtMscIntercoms ||
                value == SysExRtMscAmplifiers ||
                value == SysExRtMscAudioEffectsDevices ||
                value == SysExRtMscEqualizers ||
                value == SysExRtMscMachinery ||
                value == SysExRtMscRigging ||
                value == SysExRtMscFlys ||
                value == SysExRtMscLifts ||
                value == SysExRtMscTurntables ||
                value == SysExRtMscTrusses ||
                value == SysExRtMscRobots ||
                value == SysExRtMscAnimation ||
                value == SysExRtMscFloats ||
                value == SysExRtMscBreakaways ||
                value == SysExRtMscBarges ||
                value == SysExRtMscVideo ||
                value == SysExRtMscVideoTapeMachines ||
                value == SysExRtMscVideoCassetteMachines ||
                value == SysExRtMscVideoDiscPlayers ||
                value == SysExRtMscVideoSwitchers ||
                value == SysExRtMscVideoEffects ||
                value == SysExRtMscVideoCharacterGenerators ||
                value == SysExRtMscVideoStillStores ||
                value == SysExRtMscVideoMonitors ||
                value == SysExRtMscProjection ||
                value == SysExRtMscFilmProjectors ||
                value == SysExRtMscSlideProjectors ||
                value == SysExRtMscVideoProjectors ||
                value == SysExRtMscDissolvers ||
                value == SysExRtMscShutterControls ||
                value == SysExRtMscProcessControl ||
                value == SysExRtMscHydraulicOil ||
                value == SysExRtMscH2O ||
                value == SysExRtMscCO2 ||
                value == SysExRtMscCompressedAir ||
                value == SysExRtMscNaturalGas ||
                value == SysExRtMscFog ||
                value == SysExRtMscSmoke ||
                value == SysExRtMscCrackedHaze ||
                value == SysExRtMscPyro ||
                value == SysExRtMscFireworks ||
                value == SysExRtMscExplosions ||
                value == SysExRtMscFlame ||
                value == SysExRtMscSmokePots ||
                value == SysExRtMscAllTypes);
    }

    inline uint8_t getSysExRtMscCategory( uint8_t value ){
        return (value & 0x70);
    }

    inline bool isSysExRtMsgCategory( uint8_t value, uint8_t category){
        return (value & 0x70) == category;
    }

    typedef enum {
        SysExRtNotationInformationBarNumber                = 0x01,
        SysExRtNotationInformationTimeSignatureImmediate   = 0x02,
        SysExRtNotationInformationTimeSignatureDelayed     = 0x03
    } SysExRtNotationInformation_t;

    inline bool isSysExRtNotationInformation( uint8_t value ){
        return (value == SysExRtNotationInformationBarNumber ||
                value == SysExRtNotationInformationTimeSignatureImmediate ||
                value == SysExRtNotationInformationTimeSignatureDelayed);
    }

    typedef enum {
        SysExRtDcMasterVolume           = 0x01,
        SysExRtDcMasterBalance          = 0x02,
        SysExRtDcMasterFineTuning       = 0x03,
        SysExRtDcMasterCoarseTuning     = 0x04,
        SysExRtDcGlobalParameterControl = 0x05
    } SysExRtDc_t;

    inline bool isSysExRtDeviceControl( uint8_t value ) {
        return (value == SysExRtDcMasterVolume ||
                value == SysExRtDcMasterBalance ||
                value == SysExRtDcMasterFineTuning ||
                value == SysExRtDcMasterCoarseTuning ||
                value == SysExRtDcGlobalParameterControl);
    }

    typedef enum {
        SysExRtMtcCueingSpecial                    = 0x00, // ??
        SysExRtMtcCueingPunchInPoints              = 0x01,
        SysExRtMtcCueingPunchOutPoints             = 0x02,
        // 3 - 4 reserved
        SysExRtMtcCueingEventStartPoints           = 0x05,
        SysExRtMtcCueingEventStopPoints            = 0x06,
        SysExRtMtcCueingEvenStartPointsWithInfo    = 0x07,
        SysExRtMtcCueingEventStopPointsWithInfo    = 0x08,
        // 9 - A reserved
        SysExRtMtcCueingCuePoints                  = 0x0B,
        SysExRtMtcCueingCuePointsWithInfo          = 0x0C,
        // D reserved
        SysExRtMtcCueingEventNameInInfo            = 0x0E
    } SysExRtMtcCueing_t;

    inline bool isSysExRtMtcCueing( uint8_t value ){
        return (value == SysExRtMtcCueingSpecial ||
                value == SysExRtMtcCueingPunchInPoints ||
                value == SysExRtMtcCueingPunchOutPoints ||
                value == SysExRtMtcCueingEventStartPoints ||
                value == SysExRtMtcCueingEventStopPoints ||
                value == SysExRtMtcCueingEvenStartPointsWithInfo ||
                value == SysExRtMtcCueingEventStopPointsWithInfo ||
                value == SysExRtMtcCueingCuePoints ||
                value == SysExRtMtcCueingCuePointsWithInfo ||
                value == SysExRtMtcCueingEventNameInInfo);
    }

    inline bool isSysExRtMtcCueingWithAddInfo( uint8_t value ){
        return (value == SysExRtMtcCueingEvenStartPointsWithInfo ||
                value == SysExRtMtcCueingEventStopPointsWithInfo ||
                value == SysExRtMtcCueingCuePointsWithInfo ||
                value == SysExRtMtcCueingEventNameInInfo);
    }


    typedef enum {
        SysExRtMmcCommandtop                = 0x01,
        SysExRtMmcCommandPlay               = 0x02,
        SysExRtMmcCommandDeferredPlay       = 0x03,
        SysExRtMmcCommandFastForward        = 0x04,
        SysExRtMmcCommandRewind             = 0x05,
        SysExRtMmcCommandRecordStrobe       = 0x06,
        SysExRtMmcCommandRecordExit         = 0x07,
        SysExRtMmcCommandRecordPause        = 0x08,
        SysExRtMmcCommandPause              = 0x09,
        SysExRtMmcCommandEject              = 0x0A,
        SysExRtMmcCommandChase              = 0x0B,
        SysExRtMmcCommandCommandErrorReset  = 0x0C,
        SysExRtMmcCommandMmcReset           = 0x0D,
        SysExRtMmcCommandWrite              = 0x40,
        SysExRtMmcCommandMaskedWrite        = 0x41,
        SysExRtMmcCommandRead               = 0x42,
        SysExRtMmcCommandUpdate             = 0x43,
        SysExRtMmcCommandLocate             = 0x44,
        SysExRtMmcCommandVariablePlay       = 0x45,
        SysExRtMmcCommandSearch             = 0x46,
        SysExRtMmcCommandhuttle             = 0x47,
        SysExRtMmcCommandStep               = 0x48,
        SysExRtMmcCommandAssignSystemMaster = 0x49,
        SysExRtMmcCommandGeneratorCommand   = 0x4A,
        SysExRtMmcCommandMtcCommand         = 0x4B,
        SysExRtMmcCommandMove               = 0x4C,
        SysExRtMmcCommandAdd                = 0x4D,
        SysExRtMmcCommandSubstract          = 0x4E,
        SysExRtMmcCommandDropFrameAdjust    = 0x4F,
        SysExRtMmcCommandProcedure          = 0x50,
        SysExRtMmcCommandEvent              = 0x51,
        SysExRtMmcCommandGroup              = 0x52,
        SysExRtMmcCommandCommandSegment     = 0x53,
        SysExRtMmcCommandDeferredVariablePlay = 0x54,
        SysExRtMmcCommandRecordStrobeVariable = 0x55,
        SysExRtMmcCommandWait               = 0x7C,
        SysExRtMmcCommandResume             = 0x7F
    } SysExRtMmcCommand_t;

    inline bool isSysExRtMmcCommand( uint8_t value ) {
        return (value == SysExRtMmcCommandtop ||
                value == SysExRtMmcCommandPlay ||
                value == SysExRtMmcCommandDeferredPlay ||
                value == SysExRtMmcCommandFastForward ||
                value == SysExRtMmcCommandRewind ||
                value == SysExRtMmcCommandRecordStrobe ||
                value == SysExRtMmcCommandRecordExit ||
                value == SysExRtMmcCommandRecordPause ||
                value == SysExRtMmcCommandPause ||
                value == SysExRtMmcCommandEject ||
                value == SysExRtMmcCommandChase ||
                value == SysExRtMmcCommandCommandErrorReset ||
                value == SysExRtMmcCommandMmcReset ||
                value == SysExRtMmcCommandWrite ||
                value == SysExRtMmcCommandMaskedWrite ||
                value == SysExRtMmcCommandRead ||
                value == SysExRtMmcCommandUpdate ||
                value == SysExRtMmcCommandLocate ||
                value == SysExRtMmcCommandVariablePlay ||
                value == SysExRtMmcCommandSearch ||
                value == SysExRtMmcCommandhuttle ||
                value == SysExRtMmcCommandStep ||
                value == SysExRtMmcCommandAssignSystemMaster ||
                value == SysExRtMmcCommandGeneratorCommand ||
                value == SysExRtMmcCommandMtcCommand ||
                value == SysExRtMmcCommandMove ||
                value == SysExRtMmcCommandAdd ||
                value == SysExRtMmcCommandSubstract ||
                value == SysExRtMmcCommandDropFrameAdjust ||
                value == SysExRtMmcCommandProcedure ||
                value == SysExRtMmcCommandEvent ||
                value == SysExRtMmcCommandGroup ||
                value == SysExRtMmcCommandCommandSegment ||
                value == SysExRtMmcCommandDeferredVariablePlay ||
                value == SysExRtMmcCommandRecordStrobeVariable ||
                value == SysExRtMmcCommandWait ||
                value == SysExRtMmcCommandResume);
    }


    inline bool isSysExRtMmcCommandCtrl( uint8_t value ) {
        return (value == SysExRtMmcCommandtop ||
                value == SysExRtMmcCommandPlay ||
                value == SysExRtMmcCommandDeferredPlay ||
                value == SysExRtMmcCommandFastForward ||
                value == SysExRtMmcCommandRewind ||
                value == SysExRtMmcCommandRecordStrobe ||
                value == SysExRtMmcCommandRecordExit ||
                value == SysExRtMmcCommandRecordPause ||
                value == SysExRtMmcCommandPause ||
                value == SysExRtMmcCommandEject ||
                value == SysExRtMmcCommandMmcReset ||
                value == SysExRtMmcCommandLocate ||
                value == SysExRtMmcCommandVariablePlay ||
                value == SysExRtMmcCommandSearch ||
                value == SysExRtMmcCommandhuttle ||
                value == SysExRtMmcCommandStep ||
                value == SysExRtMmcCommandDeferredVariablePlay ||
                value == SysExRtMmcCommandRecordStrobeVariable);
    }


    inline bool isSysExRtMmcCommandSync( uint8_t value ) {
        return (value == SysExRtMmcCommandChase ||
                value == SysExRtMmcCommandAssignSystemMaster);
    }


    inline bool isSysExRtMmcCommandIO( uint8_t value ) {
        return (value == SysExRtMmcCommandCommandErrorReset ||
                value == SysExRtMmcCommandWrite ||
                value == SysExRtMmcCommandMaskedWrite ||
                value == SysExRtMmcCommandRead ||
                value == SysExRtMmcCommandUpdate);
    }


    inline bool isSysExRtMmcCommandGen( uint8_t value ) {
        return (value == SysExRtMmcCommandGeneratorCommand);
    }

    inline bool isSysExRtMmcCommandMtc( uint8_t value ) {
        return (value == SysExRtMmcCommandMtcCommand);
    }

    inline bool isSysExRtMmcCommandMath( uint8_t value ) {
        return (value == SysExRtMmcCommandMove ||
                value == SysExRtMmcCommandAdd ||
                value == SysExRtMmcCommandSubstract ||
                value == SysExRtMmcCommandDropFrameAdjust);
    }

    inline bool isSysExRtMmcCommandProc( uint8_t value ) {
        return (value == SysExRtMmcCommandProcedure);
    }

    inline bool isSysExRtMmcCommandEvnt( uint8_t value ) {
        return (value == SysExRtMmcCommandEvent);
    }

    inline bool isSysExRtMmcCommandComm( uint8_t value ) {
        return (value == SysExRtMmcCommandGroup ||
                value == SysExRtMmcCommandCommandSegment ||
                value == SysExRtMmcCommandWait ||
                value == SysExRtMmcCommandResume);
    }

    typedef enum {
        SysExRtMmcResponseSelectedTimeCode                  = 0x01,
        SysExRtMmcResponseSelectedMasterCode                = 0x02,
        SysExRtMmcResponseRequestedOffset                   = 0x03,
        SysExRtMmcResponseActualOffset                      = 0x04,
        SysExRtMmcResponseLockDeviation                     = 0x05,
        SysExRtMmcResponseGeneratorTimeCode                 = 0x06,
        SysExRtMmcResponseMidiTimeCodeInput                 = 0x07,
        SysExRtMmcResponseGP0                               = 0x08,
        SysExRtMmcResponseLocatePoint                       = SysExRtMmcResponseGP0,
        SysExRtMmcResponseGP1                               = 0x09,
        SysExRtMmcResponseGP2                               = 0x0A,
        SysExRtMmcResponseGP3                               = 0x0B,
        SysExRtMmcResponseGP4                               = 0x0C,
        SysExRtMmcResponseGP5                               = 0x0D,
        SysExRtMmcResponseGP6                               = 0x0E,
        SysExRtMmcResponseGP7                               = 0x0F,
        SysExRtMmcResponseSignature                         = 0x40,
        SysExRtMmcResponseUpdateRate                        = 0x41,
        SysExRtMmcResponseResponseError                     = 0x42,
        SysExRtMmcResponseCommandError                      = 0x43,
        SysExRtMmcResponseCommandErrorLevel                 = 0x44,
        SysExRtMmcResponseTimeStandard                      = 0x45,
        SysExRtMmcResponseSelectedTimeCodeSource            = 0x46,
        SysExRtMmcResponseSelectedTimeCodeUserbits          = 0x47,
        SysExRtMmcResponseMotionControlTally                = 0x48,
        SysExRtMmcResponseVelocityTally                     = 0x49,
        SysExRtMmcResponseStopMode                          = 0x4A,
        SysExRtMmcResponseFastMode                          = 0x4B,
        SysExRtMmcResponseRecordMode                        = 0x4C,
        SysExRtMmcResponseRecordStatus                      = 0x4D,
        SysExRtMmcResponseTrackRecordStatus                 = 0x4E,
        SysExRtMmcResponseTrackRecordReady                  = 0x4F,
        SysExRtMmcResponseGlobalMonitor                     = 0x50,
        SysExRtMmcResponseRecordMonitor                     = 0x51,
        SysExRtMmcResponseTrackSyncMonitor                  = 0x52,
        SysExRtMmcResponseTrackInputMonitor                 = 0x53,
        SysExRtMmcResponseStepLength                        = 0x54,
        SysExRtMmcResponsePlaySpeedReference                = 0x55,
        SysExRtMmcResponseFixedSpeed                        = 0x56,
        SysExRtMmcResponseLifterDefeat                      = 0x57,
        SysExRtMmcResponseControlDisable                    = 0x58,
        SysExRtMmcResponseResolvedPlayMode                  = 0x59,
        SysExRtMmcResponseChaseMode                         = 0x5A,
        SysExRtMmcResponseGeneratorCommandTally             = 0x5B,
        SysExRtMmcResponseGeneratorSetup                    = 0x5C,
        SysExRtMmcResponseGeneratorUserbits                 = 0x5D,
        SysExRtMmcResponseMidiTimeCodeCommandTally          = 0x5E,
        SysExRtMmcResponseMidiTimeCodeSetup                 = 0x5F,
        SysExRtMmcResponseProcedureResponse                 = 0x60,
        SysExRtMmcResponseEventResponse                     = 0x61,
        SysExRtMmcResponseTrackMute                         = 0x62,
        SysExRtMmcResponseVitcInsertEnable                  = 0x63,
        SysExRtMmcResponseResponseSegment                   = 0x64,
        SysExRtMmcResponseFailure                           = 0x65,
        SysExRtMmcResponseWait                              = 0x7C,
        SysExRtMmcResponseResume                            = 0x7F
    } SysExRtMmcResponse_t;

    inline bool isSysExRtMidiMachineControlResponse( uint8_t value ) {
        return false; //TODO
    }

    typedef enum {
        SysExRtMtsSingleNoteTuningChange                = 0x01,
        SysExRtMtsSingleNoteTuningChangeWithBankSelect  = 0x02,
        SysExRtMtsScaleTuning1Byte                      = 0x03,
        SysExRtMtsScaleTuning2Byte                      = 0x04
    } SysExRtMts_t;

    inline bool isSysExRtMts( uint8_t value ) {
        return (value == SysExRtMtsSingleNoteTuningChange ||
                value == SysExRtMtsSingleNoteTuningChangeWithBankSelect ||
                value == SysExRtMtsScaleTuning1Byte ||
                value == SysExRtMtsScaleTuning2Byte);
    }

    typedef enum {
        SysExRtCdsChannelPressure         = 0x01,
        SysExRtCdsPolyphonicKeyPressure   = 0x02,
        SysExRtCdsController              = 0x03
    } SysExRtCds_t;

    inline bool isSysExRtControllerDestinationSetting( uint8_t value ) {
        return (value == SysExRtCdsChannelPressure ||
                value == SysExRtCdsPolyphonicKeyPressure ||
                value == SysExRtCdsController);
    }

    typedef enum {
        MtcQuarterFrameMessageTypeFrameLS      = 0b000,
        MtcQuarterFrameMessageTypeFrameMS      = 0b001,
        MtcQuarterFrameMessageTypeSecondLS     = 0b010,
        MtcQuarterFrameMessageTypeSecondMS     = 0b011,
        MtcQuarterFrameMessageTypeMinuteLS     = 0b100,
        MtcQuarterFrameMessageTypeMinuteMS     = 0b101,
        MtcQuarterFrameMessageTypeHourLS       = 0b110,
        MtcQuarterFrameMessageTypeHourMS       = 0b111
    } MtcQuarterFrameMessageType_t;

    inline bool isMtcQuarterMessageType( uint8_t value ){
        return (value == MtcQuarterFrameMessageTypeFrameLS ||
                value == MtcQuarterFrameMessageTypeFrameMS ||
                value == MtcQuarterFrameMessageTypeSecondLS ||
                value == MtcQuarterFrameMessageTypeSecondMS ||
                value == MtcQuarterFrameMessageTypeMinuteLS ||
                value == MtcQuarterFrameMessageTypeMinuteMS ||
                value == MtcQuarterFrameMessageTypeHourLS ||
                value == MtcQuarterFrameMessageTypeHourMS);
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
    } MtcQuarterFrame_t;


    typedef struct {
        uint8_t FpsHour;
        uint8_t Minute;
        uint8_t Second;
        uint8_t Frame;
        uint8_t FractionalFrame;
    } MidiTimeCode_t;


    typedef struct {
        uint8_t SlotPathLength;
        uint8_t ParameterIdWidth;
        uint8_t ValueWidth;
        uint8_t DataLength; // In principle, it is a stream terminated with an EOX
        uint8_t * Data;
    } GlobalParameterControl_t;

    inline uint8_t getIthGpcSlot( GlobalParameterControl_t * gpc, uint8_t i ){
        ASSERT( gpc != NULL );
        ASSERT( gpc->Data != NULL );
        ASSERT( i < gpc->SlotPathLength );

        return gpc->Data[i];
    }

    inline void setIthGpcSlot( GlobalParameterControl_t * gpc, uint8_t i, uint8_t value ){
        ASSERT( gpc != NULL );
        ASSERT( gpc->Data != NULL );
        ASSERT( i < gpc->SlotPathLength );
        ASSERT( (value & DataMask) == value );

        gpc->Data[i] = value & DataMask;
    }

    inline uint8_t * getIthGpcParameterIdAddr( GlobalParameterControl_t * gpc, uint8_t i ) {
        ASSERT( gpc != NULL );
        ASSERT( gpc->Data != NULL );
        ASSERT( gpc->SlotPathLength + i * (gpc->ParameterIdWidth + gpc->ValueWidth) < gpc->DataLength );

        return &gpc->Data[ gpc->SlotPathLength + i * (gpc->ParameterIdWidth + gpc->ValueWidth) ];
    }


    inline uint8_t * getIthGpcParameterValueAddr( GlobalParameterControl_t * gpc, uint8_t i ) {
        ASSERT( gpc != NULL );
        ASSERT( gpc->Data != NULL );
        ASSERT( gpc->SlotPathLength + i * (gpc->ParameterIdWidth + gpc->ValueWidth) + gpc->ParameterIdWidth < gpc->DataLength );

        return &gpc->Data[ gpc->SlotPathLength + i * (gpc->ParameterIdWidth + gpc->ValueWidth) + gpc->ParameterIdWidth ];
    }

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

                    struct {
                        uint32_t ManufacturerId;
                        uint16_t DeviceFamily;
                        uint16_t DeviceFamilyMember;
                        uint8_t SoftwareRevision[4];
                    } GeneralInfo;

                    uint16_t DeviceControlValue;

                    GlobalParameterControl_t GlobalParameterControl;

                } Data;

#if SYSEX_MEMORY == SYSEX_MEMORY_STATIC
                uint8_t ByteData[SYSEX_MEMORY_STATIC_SIZE];
#elif SYSEX_MEMORY == SYSEX_MEMORY_DYNAMIC
                uint8_t * ByteData;
#endif
            } SysEx;

            MtcQuarterFrame_t MtcQuarterFrame;
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


    inline bool isChannelModeMessage( uint8_t byte0, uint8_t byte1 ){
        return getStatusClass(byte0) == StatusClassControlChange && isChannelModeController(byte1);
    }

    inline bool isChannelModeMessage( Message_t * msg ){
        return isChannelModeMessage( msg->Status, msg->Data.ControlChange.Controller );
    }


    inline bool isChannelVoiceMessage( uint8_t byte0, uint8_t byte1 ) {
        return isStatusClass( (byte0 & StatusClassMask) )  &&
               (byte0 & StatusClassMask) != StatusClassSystemMessage &&
               !isChannelModeMessage(byte0, byte1);
    }

    inline bool isChannelVoiceMessage( Message_t * msg ){
        return isChannelVoiceMessage( msg->Status, msg->Data.ControlChange.Controller );
    }

    inline uint8_t getDeviceId( uint8_t * bytes ){
        ASSERT( bytes != NULL );
        ASSERT( bytes[0] == SystemMessageSystemExclusive );

        return bytes[2] & DataMask;
    }

    /**
     * Extract Time Code from 8 QuarterFrames given in the order expected message type order.
     */
    inline void MidiTimeCodeFromQuarterFrameNibbles( MidiTimeCode_t * mtc, uint8_t * nibbles ) {
        ASSERT( (nibbles[0] & NibbleMask) == nibbles[0] );
        ASSERT( (nibbles[1] & NibbleMask) == nibbles[1] );
        ASSERT( (nibbles[2] & NibbleMask) == nibbles[2] );
        ASSERT( (nibbles[3] & NibbleMask) == nibbles[3] );
        ASSERT( (nibbles[4] & NibbleMask) == nibbles[4] );
        ASSERT( (nibbles[5] & NibbleMask) == nibbles[5] );
        ASSERT( (nibbles[6] & NibbleMask) == nibbles[6] );
        ASSERT( (nibbles[7] & NibbleMask) == nibbles[7] );

        mtc->Frame = (nibbles[1] << 4) | (nibbles[0]);
        mtc->Second = (nibbles[3] << 4) | (nibbles[2]);
        mtc->Minute = (nibbles[5] << 4) | (nibbles[4]);
        mtc->FpsHour = (nibbles[7] << 4) | (nibbles[6]);
        mtc->FractionalFrame = 0;
    }

    inline uint8_t MidiQuarterFrameNibbleFromTimeCode( MidiTimeCode_t * mtc, MtcQuarterFrameMessageType_t type ) {
        ASSERT( mtc != NULL );
        ASSERT( isMtcQuarterMessageType(type) );

        switch(type){
            MtcQuarterFrameMessageTypeFrameLS:     return mtc->Frame & 0x0F;
            MtcQuarterFrameMessageTypeFrameMS:     return (mtc->Frame >> 4) & 0b1;
            MtcQuarterFrameMessageTypeSecondLS:    return mtc->Second & 0x0F;
            MtcQuarterFrameMessageTypeSecondMS:    return (mtc->Second >> 4) & 0b0011;
            MtcQuarterFrameMessageTypeMinuteLS:    return mtc->Minute & 0x0F;
            MtcQuarterFrameMessageTypeMinuteMS:    return (mtc->Minute >> 4) & 0b0011;
            MtcQuarterFrameMessageTypeHourLS:      return mtc->FpsHour & 0x0F;
            MtcQuarterFrameMessageTypeHourMS:      return (mtc->FpsHour >> 4) & 0b0111;
        }

        return (uint8_t) ~0;
    }



    inline uint8_t packNoteOff( uint8_t * bytes, uint8_t channel, uint8_t key, uint8_t velocity ){
        ASSERT( bytes != NULL );
        ASSERT( (channel & ChannelMask) == channel );
        ASSERT( (key & DataMask) == key );
        ASSERT( (velocity & DataMask) == velocity );

        bytes[0] = StatusClassNoteOff | (channel & ChannelMask);
        bytes[1] = key & DataMask;
        bytes[2] = velocity & DataMask;

        return MsgLenNoteOff;
    }

    inline bool unpackNoteOff( uint8_t * bytes, uint8_t len, uint8_t * channel, uint8_t * key, uint8_t  * velocity ){
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


    inline uint8_t packNoteOn( uint8_t * bytes, uint8_t channel, uint8_t key, uint8_t velocity ){
        ASSERT( bytes != NULL );
        ASSERT( (channel & ChannelMask) == channel );
        ASSERT( (key & DataMask) == key );
        ASSERT( (velocity & DataMask) == velocity );

        bytes[0] = StatusClassNoteOn | (channel & ChannelMask);
        bytes[1] = key & DataMask;
        bytes[2] = velocity & DataMask;

        return MsgLenNoteOn;
    }

    inline bool unpackNoteOn( uint8_t * bytes,uint8_t len, uint8_t * channel, uint8_t * key, uint8_t * velocity ){
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


    inline uint8_t packPolyphonicKeyPressure( uint8_t * bytes, uint8_t channel, uint8_t key, uint8_t pressure ){
        ASSERT( bytes != NULL );
        ASSERT( (channel & ChannelMask) == channel );
        ASSERT( (key & DataMask) == key );
        ASSERT( (pressure & DataMask) == pressure );

        bytes[0] = StatusClassPolyphonicKeyPressure | (channel & ChannelMask);
        bytes[1] = key & DataMask;
        bytes[2] = pressure & DataMask;

        return MsgLenPolyphonicKeyPressure;
    }

    inline bool unpackPolyphonicKeyPressure( uint8_t * bytes,uint8_t len, uint8_t * channel, uint8_t * key, uint8_t * pressure ){
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


    inline uint8_t packControlChange( uint8_t * bytes, uint8_t channel, uint8_t controller, uint8_t value ){
        ASSERT( bytes != NULL );
        ASSERT( (channel & ChannelMask) == channel );
        ASSERT((controller & DataMask) == controller );
        ASSERT((value & DataMask) == value );

        bytes[0] = StatusClassControlChange | (channel & ChannelMask);
        bytes[1] = controller & DataMask;
        bytes[2] = value & DataMask;

        return MsgLenControlChange;
    }

    inline bool unpackControlChange( uint8_t * bytes,uint8_t len, uint8_t * channel, uint8_t * controller, uint8_t * value ){
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


    inline uint8_t packProgramChange( uint8_t * bytes, uint8_t channel, uint8_t program ){
        ASSERT( bytes != NULL );
        ASSERT( (channel & ChannelMask) == channel );
        ASSERT((program & DataMask) == program );

        bytes[0] = StatusClassProgramChange | (channel & ChannelMask);
        bytes[1] = program & DataMask;

        return MsgLenProgramChange;
    }

    inline bool unpackProgramChange( uint8_t * bytes,uint8_t len, uint8_t * channel, uint8_t * program ){
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


    inline uint8_t packChannelPressure( uint8_t * bytes, uint8_t channel, uint8_t pressure ){
        ASSERT( bytes != NULL );
        ASSERT( (channel & ChannelMask) == channel );

        bytes[0] = StatusClassChannelPressure | (channel & ChannelMask);
        bytes[1] = pressure & DataMask;

        return MsgLenChannelPressure;
    }

    inline bool unpackChannelPressure( uint8_t * bytes,uint8_t len, uint8_t * channel, uint8_t * pressure ){
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


    inline uint8_t packPitchBendChange( uint8_t * bytes, uint8_t channel, uint16_t pitch ){
        ASSERT( bytes != NULL );
        ASSERT( (channel & ChannelMask) == channel );
        ASSERT( pitch <= MaxDoubleValue );

        bytes[0] = StatusClassPitchBendChange | (channel & ChannelMask);
        packDoubleValue( &bytes[1], pitch );

        return MsgLenPitchBendChange;
    }

    inline bool unpackPitchBendChange( uint8_t * bytes,uint8_t len, uint8_t * channel, uint16_t * pitch ){
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



    inline uint8_t packAllSoundOff( uint8_t * bytes, uint8_t channel ){
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


    inline uint8_t packResetAllControllers( uint8_t * bytes, uint8_t channel ){
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


    inline uint8_t packLocalControl( uint8_t * bytes, uint8_t channel, bool on ){
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


    inline uint8_t packAllNotesOff( uint8_t * bytes, uint8_t channel ){
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


    inline uint8_t packOmniMode( uint8_t * bytes, uint8_t channel, bool on ){
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



    inline uint8_t packMonoMode( uint8_t * bytes, uint8_t channel, uint8_t numberOfChannels ){
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



    inline uint8_t packPolyModeOn( uint8_t * bytes, uint8_t channel){
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



    inline uint8_t packMtcQuarterFrame( uint8_t * bytes, uint8_t messageType, uint8_t nibble ){
        ASSERT( bytes != NULL );

        bytes[0] = SystemMessageMtcQuarterFrame;
        bytes[1] = ((messageType & 0x05) << 4) | (nibble & 0x0f);

        return MsgLenMtcQuarterFrame;
    }

    inline bool unpackMtcQuarterFrame( uint8_t * bytes,uint8_t len, uint8_t * messageType, uint8_t * nibble ){
        ASSERT( bytes != NULL );
        ASSERT( messageType != NULL );
        ASSERT( nibble != NULL );

        if ( len != MsgLenMtcQuarterFrame ) {
            return false;
        }
        if (bytes[0] != SystemMessageMtcQuarterFrame) {
            return false;
        }
        if ((bytes[1] & DataMask) != bytes[1] ) {
            return false;
        }

        *messageType = (bytes[1] >> 4) & 0x05;
        *nibble = bytes[1] & NibbleMask;

        return true;
    }


    inline uint8_t packSongPositionPointer (uint8_t * bytes, uint16_t position ){
        ASSERT( bytes != NULL );
        ASSERT( position <= MaxDoubleValue );

        bytes[0] = SystemMessageSongPositionPointer;
        packDoubleValue( &bytes[1], position );

        return MsgLenSongPositionPointer;
    }

    inline bool unpackSongPositionPointer( uint8_t * bytes,uint8_t len, uint16_t * position ){
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

    inline uint8_t packSongSelect( uint8_t * bytes, uint8_t song ){
        ASSERT( bytes != NULL );
        ASSERT( (song & DataMask) == song );

        bytes[0] = SystemMessageSystemExclusive | SystemMessageSongSelect;
        bytes[1] = song & DataMask;

        return MsgLenSongSelect;
    }

    inline bool unpackSongSelect( uint8_t * bytes,uint8_t len, uint8_t * song){
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

    inline uint8_t packSysExMidiTimeCodeFullMessage( uint8_t * bytes, uint8_t deviceId, uint8_t hour, uint8_t minute, uint8_t second, uint8_t frame, uint8_t fps ) {
        ASSERT( bytes != NULL );
        ASSERT( (deviceId & DataMask) == deviceId );
        ASSERT( isMidiTimeCodeFrameRate(fps) );
        ASSERT( hour <= MaxHour );
        ASSERT( minute <= MaxMinute );
        ASSERT( second <= MaxSecond );
        ASSERT( frame <= MaxFps[fps] );

        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = SysExIdRealTime;
        bytes[2] = deviceId & DataMask;
        bytes[3] = SysExRtMidiTimeCode;
        bytes[4] = SysExRtMidiTimeCodeFullMessage;
        bytes[5] = ((fps << FpsOffset) & FpsMask) | (hour & HourMask);
        bytes[6] = minute & MinuteMask;
        bytes[7] = second & SecondMask;
        bytes[8] = frame & FrameMask;
        bytes[9] = SystemMessageEndOfExclusive;

        return MsgLenSysExRtMtcFullMessage;
    }

    inline uint8_t packSysExMidiTimeCodeFullMessage( uint8_t * bytes, uint8_t deviceId, MidiTimeCode_t * mtc){
        ASSERT( bytes != NULL );
        ASSERT( mtc != NULL );

        return packSysExMidiTimeCodeFullMessage( bytes, deviceId, getFps(mtc->FpsHour), getHour(mtc->FpsHour), mtc->Minute, mtc->Second, mtc->Frame );
    }

    inline bool unpackSysExMidiTimeCodeFullMessage( uint8_t * bytes,uint8_t len, uint8_t * deviceId, uint8_t * fps, uint8_t * hour, uint8_t * minute, uint8_t * second, uint8_t * frame ) {
        ASSERT( bytes != NULL );
        ASSERT( deviceId != NULL );
        ASSERT( fps != NULL );
        ASSERT( hour != NULL );
        ASSERT( minute != NULL );
        ASSERT( second != NULL );
        ASSERT( frame != NULL );

        if ( (len !=  MsgLenSysExRtMtcFullMessage - 1) && (len != MsgLenSysExRtMtcFullMessage || bytes[MsgLenSysExRtMtcFullMessage-1] != SystemMessageEndOfExclusive) ){
            return false;
        }
        if (bytes[0] != SystemMessageSystemExclusive) {
            return false;
        }
        if (bytes[1] != SysExIdRealTime ) {
            return false;
        }
        if ((bytes[2] & DataMask) != bytes[2] ) {
            return false;
        }
        if (bytes[3] != SysExRtMidiTimeCode) {
            return false;
        }
        if (bytes[4] != SysExRtMidiTimeCodeFullMessage){
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

    inline bool unpackSysExMidiTimeCodeFullMessage( uint8_t * bytes,uint8_t len, uint8_t * deviceId, MidiTimeCode_t * mtc ){
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

    inline uint8_t packSysExMidiTimeCodeUserBits( uint8_t * bytes, uint8_t deviceId, uint8_t * userBits ) {
        ASSERT( bytes != NULL );
        ASSERT((deviceId & DataMask) == deviceId );
        ASSERT( userBits != NULL );

        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = SysExIdRealTime;
        bytes[2] = deviceId & DataMask;
        bytes[3] = SysExRtMidiTimeCode;
        bytes[4] = SysExRtMidiTimeCodeUserBits;
        bytes[5] = getLsNibble(userBits[3]);
        bytes[6] = getMsNibble(userBits[3]);
        bytes[7] = getLsNibble(userBits[2]);
        bytes[8] = getMsNibble(userBits[2]);
        bytes[9] = getLsNibble(userBits[1]);
        bytes[10] = getMsNibble(userBits[1]);
        bytes[11] = getLsNibble(userBits[0]);
        bytes[12] = getMsNibble(userBits[0]);
        bytes[13] = userBits[4] & 0b00000011; //TODO is this right???
        bytes[14] = SystemMessageEndOfExclusive;

        return MsgLenSysExRtMtcUserBits;
    }

    inline uint32_t unpackSysExMidiTimeCodeUserBits( uint8_t * bytes,uint8_t len, uint8_t * deviceId, uint8_t * userBits ) {
        ASSERT( bytes != NULL );
        ASSERT( deviceId != NULL );
        ASSERT( userBits != NULL );

        if ( (len !=  MsgLenSysExRtMtcUserBits - 1) && (len != MsgLenSysExRtMtcUserBits || bytes[MsgLenSysExRtMtcUserBits-1] != SystemMessageEndOfExclusive) ){
            return false;
        }
        if (bytes[0] != SystemMessageSystemExclusive) {
            return false;
        }
        if (bytes[1] != SysExIdRealTime ) {
            return false;
        }
        if ((bytes[2] & DataMask) != bytes[2] ) {
            return false;
        }
        if (bytes[3] != SysExRtMidiTimeCode) {
            return false;
        }
        if (bytes[4] != SysExRtMidiTimeCodeUserBits){
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

    inline uint8_t packSysExNonRtMtcCueingSetupMessage( SysExNonRtMtc_t msgType, uint8_t * bytes, uint8_t deviceId, uint8_t fps, uint8_t hour, uint8_t minute, uint8_t second, uint8_t frame, uint8_t fractionalFrame, uint16_t eventNumber, uint8_t * addInfo, uint8_t addInfoLen) {
        ASSERT( isSysExNonRtMtc( msgType ) );
        ASSERT( !isSysExNonRtMtcWithAddInfo( msgType ) || addInfoLen == 0 );
        ASSERT( bytes != NULL );
        ASSERT( (deviceId & DataMask) == deviceId );
        ASSERT( isMidiTimeCodeFrameRate(fps) );
        ASSERT( hour <= MaxHour );
        ASSERT( minute <= MaxMinute );
        ASSERT( second <= MaxSecond );
        ASSERT( frame <= MaxFps[fps] );
        ASSERT( fractionalFrame <= MaxFractionalFrame );
        ASSERT( eventNumber <= MaxEventNumber );

        int msgLen = MsgLenSysExNonRtMtcCueingSetupMessageMin - 1; // min message length (w/o EOX)

        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = SysExIdNonRealTime;
        bytes[2] = deviceId & DataMask;
        bytes[3] = SysExNonRtMidiTimeCode;
        bytes[4] = msgType;
        bytes[5] = ((fps << FpsOffset) & FpsMask) | (hour & HourMask);
        bytes[6] = minute & MinuteMask;
        bytes[7] = second & SecondMask;
        bytes[8] = frame & FrameMask;
        bytes[9] = fractionalFrame & FractionalFrameMask;

        packDoubleValue( &bytes[10], eventNumber );

        if ( isSysExNonRtMtcWithAddInfo( msgType ) ) {
            msgLen += nibblize(&bytes[7], addInfo, addInfoLen);
        }

        bytes[msgLen++] = SystemMessageEndOfExclusive;

        return msgLen;
    }

    inline uint8_t packSysExNonRtMtcCueingSetupMessage(  uint8_t * bytes, SysExNonRtMtc_t msgType, uint8_t deviceId, MidiTimeCode_t * mtc, uint16_t eventNumber, uint8_t * addInfo, uint8_t addInfoLen ){
        ASSERT( bytes != NULL );
        ASSERT( mtc != NULL );

        return packSysExNonRtMtcCueingSetupMessage( msgType, bytes, deviceId, getFps(mtc->FpsHour), mtc->FpsHour & HourMask, mtc->Minute, mtc->Second, mtc->Frame, mtc->FractionalFrame, eventNumber, addInfo, addInfoLen );
    }

    inline bool unpackSysExNonRtMtcCueingSetupMessage( uint8_t * bytes,uint8_t len, SysExNonRtMtc_t * msgType, uint8_t * deviceId, uint8_t * fps, uint8_t * hour, uint8_t * minute, uint8_t * second, uint8_t * frame, uint8_t  * fractionalFrame, uint16_t  * eventNumber, uint8_t * addInfo, uint8_t * addInfoLen ) {
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


        if ( len <  MsgLenSysExNonRtMtcCueingSetupMessageMin - 1 ){
            return false;
        }
        if ( bytes[0] != SystemMessageSystemExclusive ) {
            return false;
        }
        if ( bytes[1] != SysExIdNonRealTime ) {
            return false;
        }
        if ( (bytes[2] & DataMask) != bytes[2] ) {
            return false;
        }
        if ( bytes[3] != SysExNonRtMidiTimeCode ) {
            return false;
        }
        if ( ! isSysExNonRtMtc( bytes[4] ) ){
            return false;
        }

        *deviceId = bytes[2] & DataMask;
        *msgType = (SysExNonRtMtc_t)(bytes[4]);
        *fps = (bytes[5] >> 5) & 0b11;
        *hour = bytes[5] & HourMask;
        *minute = bytes[6] & MinuteMask;
        *second = bytes[7] & SecondMask;
        *frame = bytes[8] & FrameMask;
        *fractionalFrame = bytes[9] & FractionalFrameMask;

        unpackDoubleValue( &bytes[10], eventNumber );

        if ( isSysExNonRtMtc(bytes[4]) ){

            if (bytes[len - 1] != SystemMessageEndOfExclusive){
                len++;
            }

            *addInfoLen = denibblize( addInfo, &bytes[12], len - MsgLenSysExNonRtMtcCueingSetupMessageMin );
        }

        return true;
    }

    inline bool unpackSysExNonRtMtcCueingSetupMessage( uint8_t * bytes,uint8_t len, SysExNonRtMtc_t * msgType, uint8_t * deviceId, MidiTimeCode_t * mtc, uint16_t  * eventNumber, uint8_t * addInfo, uint8_t * addInfoLen  ){
        ASSERT( bytes != NULL );
        ASSERT( mtc != NULL );

        MidiTimeCodeFrameRate_t fps = MidiTimeCodeFrameRate24fps;

        if ( ! unpackSysExNonRtMtcCueingSetupMessage( bytes, len, msgType, deviceId, (uint8_t*)&fps, &mtc->FpsHour, &mtc->Minute, &mtc->Second, &mtc->Frame, &mtc->FractionalFrame, eventNumber, addInfo, addInfoLen ) ) {
            return false;
        }

        mtc->FpsHour |= setFps(fps);

        return true;
    }


    inline uint8_t packSysExRtMtcCueingSetupMessage( uint8_t * bytes, SysExRtMtcCueing_t msgType, uint8_t deviceId, uint16_t eventNumber, uint8_t * addInfo, uint32_t addInfoLen) {
        ASSERT( isSysExRtMtcCueing( msgType ) );
        ASSERT( !isSysExRtMtcCueing( msgType ) || addInfoLen == 0 );
        ASSERT( bytes != NULL );
        ASSERT( (deviceId & DataMask) == deviceId );
        ASSERT( eventNumber <= MaxEventNumber );

        int msgLen = MsgLenSysExRtMtcCueingSetupMessageMin - 1; // min message length (w/o EOX)

        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = SysExIdRealTime;
        bytes[2] = deviceId & DataMask;
        bytes[3] = SysExNonRtMidiTimeCode;
        bytes[4] = msgType;

        packDoubleValue( &bytes[5], eventNumber );

        if ( isSysExRtMtcCueingWithAddInfo( msgType ) ) {
            msgLen += nibblize(&bytes[7], addInfo, addInfoLen);
        }

        bytes[msgLen++] = SystemMessageEndOfExclusive;

        return msgLen;
    }


    inline bool unpackSysExRtMtcCueingSetupMessage( uint8_t * bytes,uint8_t len, SysExRtMtcCueing_t * msgType, uint8_t * deviceId, uint16_t  * eventNumber, uint8_t * addInfo, uint8_t * addInfoLen ) {
        ASSERT( bytes != NULL );
        ASSERT( deviceId != NULL );
        ASSERT( eventNumber != NULL );
        ASSERT( addInfo != NULL );
        ASSERT( addInfoLen != NULL );


        if ( len <  MsgLenSysExRtMtcCueingSetupMessageMin - 1 ){
            return false;
        }
        if (bytes[0] != SystemMessageSystemExclusive) {
            return false;
        }
        if (bytes[1] != SysExIdRealTime ) {
            return false;
        }
        if ((bytes[2] & DataMask) != bytes[2] ) {
            return false;
        }
        if (bytes[3] != SysExRtMidiTimeCode) {
            return false;
        }
        if ( ! isSysExRtMtcCueing( bytes[4] ) ){
            return false;
        }


        *deviceId = bytes[2] & DataMask;
        *msgType = (SysExRtMtcCueing_t)(bytes[4]);

        unpackDoubleValue( &bytes[10], eventNumber );

        if ( isSysExRtMtcCueingWithAddInfo(bytes[4]) ){

            if (bytes[len - 1] != SystemMessageEndOfExclusive){
                len++;
            }

            *addInfoLen = denibblize( addInfo, &bytes[12], len - MsgLenSysExRtMtcCueingSetupMessageMin );
        }

        return true;
    }

    inline uint8_t packGeneralInformationIdentityRequest( uint8_t * bytes, uint8_t deviceId ){
        ASSERT( bytes != NULL );
        ASSERT( (deviceId & DataMask) == deviceId );

        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = SysExIdNonRealTime;
        bytes[2] = deviceId & DataMask;
        bytes[3] = SysExNonRtGeneralInformation;
        bytes[4] = SysExNonRtGenInfoIdentityRequest;
        bytes[5] = SystemMessageEndOfExclusive;

        return MsgLenSysExNonRtGenInfoIdentityRequest;
    }

//    inline bool unpackGeneralInformationIdentityRequest( uint8_t * bytes,uint8_t len, uint8_t * deviceId ){
//        ASSERT( buffer != NULL );
//
//        if ( (len !=  MsgLenSysExNonRtGenInfoIdentityRequest - 1) && (len != MsgLenSysExNonRtGenInfoIdentityRequest || bytes[MsgLenSysExNonRtGenInfoIdentityRequest-1] != SystemMessageEndOfExclusive ) ){
//            return false;
//        }
//        if (bytes[0] != SystemMessageSystemExclusive) {
//            return false;
//        }
//        if (bytes[1] != SysExIdNonRealTime ) {
//            return false;
//        }
//        if ((bytes[2] & DataMask) != bytes[2] ) {
//            return false;
//        }
//        if (bytes[3] != SysExNonRtGeneralInformation) {
//            return false;
//        }
//        if ( bytes[4] != SysExNonRtGenInfoIdentityRequest ){
//            return false;
//        }
//
//        *deviceId = bytes[2];
//
//        return true;
//    }

    inline uint8_t packGeneralInformationIdentityReply( uint8_t * bytes, uint8_t deviceId, uint32_t manufacturerId, uint16_t deviceFamily, uint16_t deviceFamilyMember, uint8_t * softwareRevision ){
        ASSERT( bytes != NULL );
        ASSERT( (deviceId & DataMask) == deviceId );
        ASSERT( deviceFamily <= MaxDoubleValue );
        ASSERT( deviceFamilyMember <= MaxDoubleValue );

       uint32_t len = 5;

        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = SysExIdNonRealTime;
        bytes[2] = deviceId & DataMask;
        bytes[3] = SysExNonRtGeneralInformation;
        bytes[4] = SysExNonRtGenInfoIdentityReply;

        len += packSysExId( &bytes[5], manufacturerId );

        packDoubleValue( &bytes[len], deviceFamily );
        len += 2;

        packDoubleValue( &bytes[len], deviceFamilyMember );
        len += 2;

        bytes[len++] = softwareRevision[0];
        bytes[len++] = softwareRevision[1];
        bytes[len++] = softwareRevision[2];
        bytes[len++] = softwareRevision[3];

        bytes[len] = SystemMessageEndOfExclusive;

        return len;
    }

    inline bool unpackGeneralInformationIdentityReply( uint8_t * bytes,uint8_t len, uint8_t * deviceId, uint32_t * manufacturerId, uint16_t * deviceFamily, uint16_t * deviceFamilyMember, uint8_t * softwareRevision ){
        ASSERT( bytes != NULL );
        ASSERT( deviceId != NULL );
        ASSERT( manufacturerId != NULL );
        ASSERT( deviceFamily != NULL );
        ASSERT( deviceFamilyMember != NULL );
        ASSERT( softwareRevision != NULL );


        if (len < 13) {
            return false;
        }
        if ( bytes[0] != SystemMessageSystemExclusive ){
            return false;
        }
        if ( bytes[1] != SysExIdNonRealTime ){
            return false;
        }
        if ((bytes[2] & DataMask) != bytes[2] ) {
            return false;
        }
        if (bytes[3] != SysExNonRtGeneralInformation) {
            return false;
        }
        if ( bytes[4] != SysExNonRtGenInfoIdentityReply ){
            return false;
        }

        int l = 4;

        *deviceId = bytes[2] & DataMask;

        l += unpackSysExId( &bytes[5], manufacturerId );

        unpackDoubleValue( &bytes[l], deviceFamily );
        l += 2;

        unpackDoubleValue( &bytes[l], deviceFamilyMember );
        l += 2;

        softwareRevision[0] = bytes[l++];
        softwareRevision[1] = bytes[l++];
        softwareRevision[2] = bytes[l++];
        softwareRevision[3] = bytes[l++];

        return true;
    }

    inline uint8_t packDeviceControl( uint8_t * bytes, uint8_t deviceId, SysExRtDc_t type, uint16_t value ){
        ASSERT( bytes != NULL );
        ASSERT( isSysExRtDeviceControl(type) && type != SysExRtDcGlobalParameterControl );
        ASSERT( type != SysExRtDcMasterVolume || value <= MaxDoubleValue );
        ASSERT( type != SysExRtDcMasterBalance || value <= MaxDoubleValue );
        ASSERT( type != SysExRtDcMasterCoarseTuning || value <= CoarseTuningMax ); // MaxDoubleValue
        ASSERT( type != SysExRtDcMasterFineTuning || value <= FineTuningMax );

        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = SysExIdRealTime;
        bytes[2] = deviceId & DataMask;
        bytes[3] = SysExRtDeviceControl;
        bytes[4] = type;

        packDoubleValue( &bytes[5], value );

        bytes[7] = SystemMessageEndOfExclusive;

        return MsgLenSysExNonRtDeviceControl;
    }

    inline bool unpackDeviceControl( uint8_t * bytes, uint8_t len, uint8_t * deviceId, SysExRtDc_t * type, uint16_t * value ){
        ASSERT( bytes != NULL );
        ASSERT( deviceId != NULL );
        ASSERT( type != NULL );
        ASSERT( value != NULL );

        if ( (len != MsgLenSysExNonRtDeviceControl - 1) && (len != MsgLenSysExNonRtDeviceControl || bytes[len-1] == SystemMessageEndOfExclusive) ){
            return false;
        }
        if ( bytes[0] != SystemMessageSystemExclusive ){
            return false;
        }
        if ( bytes[1] != SysExIdRealTime ){
            return false;
        }
        if ((bytes[2] & DataMask) != bytes[2] ) {
            return false;
        }
        if (bytes[3] != SysExRtDeviceControl) {
            return false;
        }
        if ( ! isSysExRtDeviceControl( bytes[4] ) || bytes[4] == SysExRtDcGlobalParameterControl ){
            return false;
        }

        *deviceId = bytes[2];
        *type = (SysExRtDc_t)bytes[4];

        unpackDoubleValue( &bytes[5], value );

        return true;
    }

    inline uint8_t packGlobalParameterControl( uint8_t * bytes, uint8_t deviceId, GlobalParameterControl_t * gpc ){
        ASSERT( bytes != NULL );
        ASSERT( (deviceId & DataMask) == deviceId );
        ASSERT( gpc != NULL );
        ASSERT( gpc->Data != NULL );

        uint32_t len = 8;

        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = SysExIdRealTime;
        bytes[2] = deviceId & DataMask;
        bytes[3] = SysExRtDeviceControl;
        bytes[4] = SysExRtDcGlobalParameterControl;
        bytes[5] = gpc->SlotPathLength;
        bytes[6] = gpc->ParameterIdWidth;
        bytes[7] = gpc->ValueWidth;

        for (int i = 0; i < gpc->SlotPathLength; i++){
            bytes[len++] = getMsNibble( gpc->Data[i] );
            bytes[len++] = getLsNibble( gpc->Data[i] );
        }

        for (int i = gpc->SlotPathLength; i < gpc->DataLength; i++){
            bytes[len++] = gpc->Data[i];
        }

        bytes[len++] = SystemMessageEndOfExclusive;

        return len;
    }

    inline bool unpackGlobalParameterControl( uint8_t * bytes, uint8_t len, uint8_t * deviceId, GlobalParameterControl_t * gpc ){
        ASSERT( bytes != NULL );
        ASSERT( deviceId != NULL );
        ASSERT( gpc != NULL );

        if ( len < 8 ){
            return false;
        }

        if ( bytes[0] != SystemMessageSystemExclusive ){
            return false;
        }
        if ( bytes[1] != SysExIdRealTime ){
            return false;
        }
        if ((bytes[2] & DataMask) != bytes[2] ) {
            return false;
        }
        if (bytes[3] != SysExRtDeviceControl) {
            return false;
        }
        if ( bytes[4] != SysExRtDcGlobalParameterControl ){
            return false;
        }

        *deviceId = bytes[2];
        gpc->SlotPathLength = bytes[5];
        gpc->ParameterIdWidth = bytes[6];
        gpc->ValueWidth = bytes[7];

        uint32_t l = 0;

        // s acts merely as counter (identical to l..)
        for( int s = 0, i = 8; s < gpc->SlotPathLength; s++, l++ ){
            gpc->Data[l] = (bytes[i++] & NibbleMask) << 4;
            gpc->Data[l] |= bytes[i++] & NibbleMask;
        }

        for (int i = 8 + 2 * gpc->SlotPathLength; i < len && bytes[i] != SystemMessageEndOfExclusive; i++, l++){
            gpc->Data[l] = bytes[i];
        }

        gpc->DataLength = l;

        return true;
    }

    inline uint8_t packMmcCommand( uint8_t * bytes, uint8_t deviceId, SysExRtMmcCommand_t command, uint8_t * data, uint8_t dataLen ){
        ASSERT( bytes != NULL );
        ASSERT( (deviceId & DataMask) == deviceId );
        ASSERT( isSysExRtMmcCommand( command ) );
        ASSERT( (dataLen == 0) || (data != NULL) );

        uint32_t len = 5;

        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = SysExIdRealTime;
        bytes[2] = deviceId & DataMask;
        bytes[3] = SysExRtMidiMachineControlCommand;
        bytes[4] = command;

        for(int i = 0; i < dataLen; i++){
            bytes[len++] = data[i];
        }

        bytes[len++] = SystemMessageEndOfExclusive;

        return len;
    }

    inline bool unpackMmcCommand( uint8_t * bytes, uint8_t len, uint8_t * deviceId,  SysExRtMmcCommand_t * command, uint8_t * data, uint8_t * dataLen ) {
        ASSERT( bytes != NULL );
        ASSERT( deviceId != NULL );
        ASSERT( command != NULL );
        ASSERT( data != NULL );
        ASSERT( dataLen != NULL );


        if ( len < MsgLenSysExRtMmcCommandWithoutInfo - 1 ){
            return false;
        }
        if ( bytes[0] != SystemMessageSystemExclusive ){
            return false;
        }
        if ( bytes[1] != SysExIdRealTime ){
            return false;
        }
        if ( (bytes[2] & DataMask) != bytes[2] ) {
            return false;
        }
        if ( bytes[3] != SysExRtMidiMachineControlCommand ) {
            return false;
        }
        if ( ! isSysExRtMmcCommand(bytes[4]) ){
            return false;
        }

        *deviceId = bytes[2];
        *command = (SysExRtMmcCommand_t)bytes[4];

        if (bytes[len-1] == SystemMessageEndOfExclusive){
            len--;
        }
        if ( len > 5 ){
            *dataLen = 0;
            for(int i = 5; i < len; i++, (*dataLen)++){
                data[(*dataLen)++] = bytes[i];
            }
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
    uint8_t pack( uint8_t * bytes, Message_t * msg );

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
    bool unpack( uint8_t * bytes,uint8_t len, Message_t * msg );

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
