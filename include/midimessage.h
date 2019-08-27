/**
 * \file
 */

#ifndef MIDIMESSAGE_H
#define MIDIMESSAGE_H

#include <cstdint>

/**
 * \def USE_ASSERTS
 * \brief If set enables asserts.
 */
#define USE_ASSERTS

/**
 * \def ASSERT
 * \brief Assert function macro. Can be overriden. Requires USE_ASSERTS define.
 */

#ifdef USE_ASSERTS
#ifndef ASSERT
#include <cassert>
#define ASSERT(x) assert(x)
#endif // ASSERT
#else
#define ASSERT(x)
#endif


namespace MidiMessage {

    // Constants & Enumerations
    /////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////

    /**
     * Non-Data Bytes always have a non-zero match
     */
    const uint8_t ControlByteMask = 0x80;

    /**
     * Macro to check wether the given byte is considered a control byte (as opposed to a data byte)
     */
    inline bool isControlByte( uint8_t byte ) {
        return (byte & ControlByteMask) != 0;
    }


    /**
     * Data bytes may only have these bits set
     */
    const uint8_t DataMask = 0b01111111; // = 127

    /**
     *
     */
    const uint16_t DoubleValueMask  = 0b0111111101111111;

    /**
     * Mask for Byte-Nibbles (half-bytes)
     */
    const uint8_t NibbleMask = 0b00001111;

    /**
     * Data-byte max value
     */
    const uint8_t MaxValue          = 127;

    /**
     * Double-data-byte max value
     */
    const uint16_t MaxDoubleValue   = 0x3FFF;

    const uint32_t MaxTripleValue  = 0x1FFFFF;

    /**
     * Is it a data byte?
     */
    inline bool isDataByte( uint8_t byte ){
        return (byte & DataMask) == byte;
    }

    /**
     * Get Data-byte value (safe)
     */
    inline uint8_t getData( uint8_t byte ){
        return byte & DataMask;
    }

    inline void packDoubleValue( uint8_t * bytes, uint16_t value ) {
        bytes[0] = value & DataMask;
        bytes[1] = (value >> 7) & DataMask;
    }

    inline uint16_t unpackDoubleValue( uint8_t * bytes ){
        ASSERT( (bytes[0] & DataMask) == bytes[0] );
        ASSERT( (bytes[1] & DataMask) == bytes[1] );

        return (((uint16_t)bytes[1]) << 7) | ((uint16_t)bytes[0]);
    }

    inline void packTripleValue( uint8_t * bytes, uint32_t value ) {
        bytes[0] = value & DataMask;
        bytes[1] = (value >> 7) & DataMask;
        bytes[2] = (value >> 14) & DataMask;
    }

    inline uint32_t unpackTripleValue( uint8_t * bytes ){
        ASSERT( (bytes[0] & DataMask) == bytes[0] );
        ASSERT( (bytes[1] & DataMask) == bytes[1] );
        ASSERT( (bytes[2] & DataMask) == bytes[2] );

        return (((uint32_t)bytes[2]) << 14) | (((uint32_t)bytes[1]) << 7) | ((uint32_t)bytes[0]);
    }



    /**
     * Get Least-Significant Nibble.
     * @see NibbleMask
     */
    inline uint8_t getLsNibble( uint8_t byte ){
        return byte & NibbleMask;
    }

    /**
     * Get Most-Significant Nibble
     */
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

    const uint8_t StatusClassMask = 0xF0;
    const uint8_t ChannelMask     = 0x0F;

    inline uint8_t getChannel( uint8_t value ){
        return value & ChannelMask;
    }


    const uint8_t SysExDeviceIdBroadcast = 0x7F; // for  SysEx (Non-)Realtime Messages

    inline bool isSysExDeviceIdGroup( uint8_t value ){
        ASSERT( (value & DataMask) == value );

        return (0x70 <= value) && (value <= 0x7E);
    }

    inline uint8_t getSysExDevceIdGroup( uint8_t value ){
        ASSERT( isSysExDeviceIdGroup(value) );

        return (value >= 0x70) ? (value - 0x70) : 0x7F;
    }


    const uint8_t MtcFpsMask       = 0b01100000;
    const uint8_t MtcFpsOffset     = 5;
    const uint8_t MtcHourMask      = 0b00011111;
    const uint8_t MtcMinuteMask    = 0b00111111;
    const uint8_t MtcSecondMask    = 0b00111111;
    const uint8_t MtcFrameMask     = 0b00011111;
    const uint8_t MtcFrameSignBit  = 0b01000000;
    const uint8_t MtcFrameFinalByteIdBit   = 0b00100000;
    const uint8_t MtcFractionalFrameMask   = 0b01111111;

    const uint8_t MtcMaxHour = 23;
    const uint8_t MtcMaxMinute = 59;
    const uint8_t MtcMaxSecond = 59;
    const uint8_t MtcMaxFps[] = {23,24,29,29}; // According to index given by FrameRate enum @see MtcFrameRate_t
    const uint8_t MtcMaxFractionalFrame = 99;

    /**
     * Status code flag bit as used in MIDI Show Control (MSC)
     */
    const uint8_t StatusCodeFlagBit     = 0b01000000;

    /**
     * Status invalid code bit as used in MIDI Show Control (MSC)
     */
    const uint8_t StatusInvalidCodeBit  = 0b00100000;

    /**
     * Status video id bit as used in MIDI Show Control (MSC)
     */
    const uint8_t StatusVideoIdBit      = 0b00010000;

    /**
     * Max event number as used in MIDI Time Code Cueing
     * @see packSysExNonRtMtcCueingSetupMessage()
     * @see unpackSysExNonRtMtcCueingSetupMessage()
     * @see packSysExRtMtcCueingSetupMessage()
     * @see unpackSysExRtMtcCueingSetupMessage()
     */
    const uint16_t MaxEventNumber = 0x3FFF; // 14 bit value

    /**
     * Pitch center of Pitch Bend Message
     * @see MaxDoubleValue
     * @see packPitchBendChange()
     * @see unpackPitchBendChange()
     */
    const uint16_t PitchCenter      = 0x2000;

    /**
     * As used in
     */
    const uint16_t CoarseTuningCenter = 0x0040;
    const uint16_t CoarseTuningMax    = 0x007F;
    const uint16_t FineTuningCenter   = 0x2000; // 0xA440 when in two 7bit values
    const uint16_t FineTuningMax      = 0x3FFF; // 0x7F7F


    const uint8_t MsgLenNoteOff                         = 3;
    const uint8_t MsgLenNoteOn                          = 3;
    const uint8_t MsgLenPolyphonicKeyPressure           = 3;
    const uint8_t MsgLenControlChange                   = 3;
    const uint8_t MsgLenProgramChange                   = 2;
    const uint8_t MsgLenChannelPressure                 = 2;
    const uint8_t MsgLenPitchBendChange                 = 3;
    const uint8_t MsgLenMtcQuarterFrame                 = 2;
    const uint8_t MsgLenSongPositionPointer             = 3;
    const uint8_t MsgLenSongSelect                      = 2;

    const uint8_t MsgLenSysExNonRtGeneralHandshaking    = 6;
    const uint8_t MsgLenSysExNonRtGeneralMidi           = 5;
    const uint8_t MsgLenSysExNonRtMtcCueingSetupMessageMin  = 13;
    const uint8_t MsgLenSysExNonRtGenInfoIdentityRequest = 6;
    const uint8_t MsgLenSysExNonRtGenInfoIdentityReply = 15;
    const uint8_t MsgLenSysExNonRtDeviceControl = 8;

    const uint8_t MsgLenSysExRtMtcFullMessage    = 10;
    const uint8_t MsgLenSysExRtMtcUserBits       = 15;
    const uint8_t MsgLenSysExRtMtcCueingSetupMessageMin  = 8;
    const uint8_t MsgLenSysExRtMmcCommandWithoutInfo = 6;

    const uint8_t MsgLenSysExNonRtSampleDumpHeader = 21;
    const uint8_t MsgLenSysExNonRtSampleDumpRequest = 7;


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

    #define MidiMessage_RunningStatusNotSet 0

    inline uint8_t isRunningStatus( uint8_t state ){
        return isStatusClass( state & StatusClassMask ) && (state & StatusClassMask) != StatusClassSystemMessage;
    }

    inline uint8_t nextRunningStatus( uint8_t state, uint8_t nextStatus ){
        // identical messages do not change the state.
        if (state == nextStatus){
            return state;
        }
        // system common messages break the state
        if (isSystemCommonMessage(nextStatus)){
            return MidiMessage_RunningStatusNotSet;
        }
        // system real time message to not change the state
        if (isSystemRealTimeMessage(nextStatus)){
            return state;
        }

        // this should always be true (unless there is some unknown status byte)
        if (isRunningStatus(nextStatus)){
            return nextStatus;
        }

        return MidiMessage_RunningStatusNotSet;
    }

    inline bool updateRunningStatus( uint8_t *state, uint8_t nextStatus ){

        uint8_t previous = *state;
        uint8_t next = nextRunningStatus( *state, nextStatus );

        *state = next;

        return previous == next && isRunningStatus(next);
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


    /**
     * Single byte SysEx Ids (as per MIDI spec)
     *
     * @see unifySysExId()
     * @see deunifySysExId()
     */
    typedef enum {
        SysExIdManufacturerExtensionByte  = 0x00,
        SysExIdExperimentalByte           = 0x7D,
        SysExIdNonRealTimeByte            = 0x7E,
        SysExIdRealTimeByte               = 0x7F
    } SysExIdByte_t;

    /**
     * Type check
     */
    inline bool isSysExIdByte( uint8_t value ){
        return (value == SysExIdManufacturerExtensionByte ||
                value == SysExIdExperimentalByte ||
                value == SysExIdNonRealTimeByte ||
                value == SysExIdRealTimeByte);
    }

    /**
     * Test wether given byte belongs to a non-manufacturer id.
     */
    inline bool isSysExManufacturerIdByte( uint8_t byte0 ){
        return (byte0 != SysExIdExperimentalByte &&
                byte0 != SysExIdNonRealTimeByte &&
                byte0 != SysExIdRealTimeByte);
    }

    /**
     * Unified SysEx Id
     *
     * @see MidiMessage::ManufacturerId_t
     */
    typedef enum {
        SysExIdManufacturerExtension  = 0x00000000,
        SysExIdExperimental           = 0x007D0000,
        SysExIdNonRealTime            = 0x007E0000,
        SysExIdRealTime               = 0x007F0000
    } SysExId_t;

    inline bool isSysExId( uint32_t value ){
        return (value == SysExIdManufacturerExtension ||
                value == SysExIdExperimental ||
                value == SysExIdNonRealTime ||
                value == SysExIdRealTime);
    }

    inline bool isSysExManufacturerId( uint32_t value ){
        return (value != SysExIdExperimental &&
                value != SysExIdNonRealTime &&
                value != SysExIdRealTime);
    }

    /**
     * Turn a single byte id into a unified id.
     */
    inline uint32_t unifySysExId( uint8_t byte ){
        return ((uint32_t)byte) << 16;
    }

    /**
     * Turn a unified id into a single byte id
     */
//    inline uint8_t deunifySysExId( uint32_t value ){
//        return (value >> 16) & 0x7F;
//    }


//    inline bool is1ByteManufacturerId( uint8_t * bytes ){
//        return bytes[0] == SysExIdManufacturerExtension;
//    }
//
//    inline bool is3ByteManufacturerId( uint8_t * bytes ){
//        return !is1ByteManufacturerId( bytes );
//    }
//    inline bool is1ByteManufacturerId( uint32_t id ){
//        return ((id >> 16) & DataMask) == SysExIdManufacturerExtension;
//    }
//
//    inline bool is3ByteManufacturerId( uint32_t id ){
//        return !is1ByteManufacturerId(  id );
//    }


    inline uint8_t packSysExId( uint8_t * bytes, uint32_t id ){
        ASSERT( bytes != NULL );

        bytes[0] = (id >> 16) & DataMask;

        if (bytes[0] == SysExIdManufacturerExtensionByte){
            bytes[1] = (id >> 8) & DataMask;
            bytes[2] = id & DataMask;
            return 3;
        }
        
        return 1;
    }

    inline uint8_t unpackSysExId( uint8_t * bytes, uint32_t * id){
        ASSERT( bytes != NULL );

        *id = ((uint32_t)bytes[0]) << 16;

        if (bytes[0] == SysExIdManufacturerExtensionByte){
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
        SysExRtMtcFullMessage           = 0x01,
        SysExRtMtcUserBits              = 0x02
    } SysExRtMtc_t;

    inline bool isSysExRtMtc( uint8_t value ){
        return (value == SysExRtMtcFullMessage ||
                value == SysExRtMtcUserBits);
    }

    /**
     * MIDI Show Control
     *
     * F0 7F <device_ID> <msc> <command_format> <command> <data> F7
     *
     * <msc>                SubId1 := 0x02
     * <command_format>     SysExRtMscCmdFmt_t
     * <command>            SysExRtMscCmd_t
     *
     * Notes:
     * 1. No more than once command can be transmitted in a SysEx.
     * 2. The total number of byte sin a Show Control message should not exceed 128.
     * 3. SysEx messages must always be closed with an F7H as soon as all currently prepared information has been transmitted.
     *
     * <command_format> and <command> have two possible extension sets that are indicated by 0x00 which indicates that the next byte is part of the
     *
     * Que Numbers
     *
     * <Q_number> 00 <Q_list> 00 <Q_path> F7 (or alternatively)
     * <Q_number> 00 <Q_list>  F7 (or alternatively)
     * <Q_number> F7
     *
     * <Q_number> | <Q_list> | <Q_path> are formatted as as ASCII number 0 - 9 (0x30 - 0x39) and can be sectioned using ASCII decimal point . (0x2E), ex
     *
     * "235.6" == 0x3233352E36
     *
     * Standard MIDI Time Code is used.
     */

    typedef enum {
        SysExRtMscCmdFmtExtension                     = 0x00,
        SysExRtMscCmdFmtLighting                      = 0x01,
        SysExRtMscCmdFmtMovingLights                  = 0x02,
        SysExRtMscCmdFmtColorChangers                 = 0x03,
        SysExRtMscCmdFmtStrobes                       = 0x04,
        SysExRtMscCmdFmtLasers                        = 0x05,
        SysExRtMscCmdFmtChasers                       = 0x06,

        SysExRtMscCmdFmtSound                         = 0x10,
        SysExRtMscCmdFmtMusic                         = 0x11,
        SysExRtMscCmdFmtCdPlayers                     = 0x12,
        SysExRtMscCmdFmtEpromPlayback                 = 0x13,
        SysExRtMscCmdFmtAudioTapeMachines             = 0x14,
        SysExRtMscCmdFmtIntercoms                     = 0x15,
        SysExRtMscCmdFmtAmplifiers                    = 0x16,
        SysExRtMscCmdFmtAudioEffectsDevices           = 0x17,
        SysExRtMscCmdFmtEqualizers                    = 0x18,

        SysExRtMscCmdFmtMachinery                     = 0x20,
        SysExRtMscCmdFmtRigging                       = 0x21,
        SysExRtMscCmdFmtFlys                          = 0x22,
        SysExRtMscCmdFmtLifts                         = 0x23,
        SysExRtMscCmdFmtTurntables                    = 0x24,
        SysExRtMscCmdFmtTrusses                       = 0x25,
        SysExRtMscCmdFmtRobots                        = 0x26,
        SysExRtMscCmdFmtAnimation                     = 0x27,
        SysExRtMscCmdFmtFloats                        = 0x28,
        SysExRtMscCmdFmtBreakaways                    = 0x29,
        SysExRtMscCmdFmtBarges                        = 0x2A,

        SysExRtMscCmdFmtVideo                         = 0x30,
        SysExRtMscCmdFmtVideoTapeMachines             = 0x31,
        SysExRtMscCmdFmtVideoCassetteMachines         = 0x32,
        SysExRtMscCmdFmtVideoDiscPlayers              = 0x33,
        SysExRtMscCmdFmtVideoSwitchers                = 0x34,
        SysExRtMscCmdFmtVideoEffects                  = 0x35,
        SysExRtMscCmdFmtVideoCharacterGenerators      = 0x36,
        SysExRtMscCmdFmtVideoStillStores              = 0x37,
        SysExRtMscCmdFmtVideoMonitors                 = 0x38,

        SysExRtMscCmdFmtProjection                    = 0x40,
        SysExRtMscCmdFmtFilmProjectors                = 0x41,
        SysExRtMscCmdFmtSlideProjectors               = 0x42,
        SysExRtMscCmdFmtVideoProjectors               = 0x43,
        SysExRtMscCmdFmtDissolvers                    = 0x44,
        SysExRtMscCmdFmtShutterControls               = 0x45,

        SysExRtMscCmdFmtProcessControl                = 0x50,
        SysExRtMscCmdFmtHydraulicOil                  = 0x51,
        SysExRtMscCmdFmtH2O                           = 0x52,
        SysExRtMscCmdFmtWater                         = SysExRtMscCmdFmtH2O,
        SysExRtMscCmdFmtCO2                           = 0x53,
        SysExRtMscCmdFmtDryIce                        = SysExRtMscCmdFmtCO2,
        SysExRtMscCmdFmtCompressedAir                 = 0x54,
        SysExRtMscCmdFmtNaturalGas                    = 0x55,
        SysExRtMscCmdFmtFog                           = 0x56,
        SysExRtMscCmdFmtSmoke                         = 0x57,
        SysExRtMscCmdFmtCrackedHaze                   = 0x58,

        SysExRtMscCmdFmtPyro                          = 0x60,
        SysExRtMscCmdFmtFireworks                     = 0x61,
        SysExRtMscCmdFmtExplosions                    = 0x62,
        SysExRtMscCmdFmtFlame                         = 0x63,
        SysExRtMscCmdFmtSmokePots                     = 0x64,

        SysExRtMscCmdFmtAllTypes                      = 0x7F

    } SysExRtMscCmdFmt_t;

    inline bool isSysExRtMscCmdFmt( uint8_t value ){
        return (value == SysExRtMscCmdFmtLighting ||
                value == SysExRtMscCmdFmtMovingLights ||
                value == SysExRtMscCmdFmtColorChangers ||
                value == SysExRtMscCmdFmtStrobes ||
                value == SysExRtMscCmdFmtLasers ||
                value == SysExRtMscCmdFmtChasers ||
                value == SysExRtMscCmdFmtMusic ||
                value == SysExRtMscCmdFmtCdPlayers ||
                value == SysExRtMscCmdFmtEpromPlayback ||
                value == SysExRtMscCmdFmtAudioTapeMachines ||
                value == SysExRtMscCmdFmtIntercoms ||
                value == SysExRtMscCmdFmtAmplifiers ||
                value == SysExRtMscCmdFmtAudioEffectsDevices ||
                value == SysExRtMscCmdFmtEqualizers ||
                value == SysExRtMscCmdFmtMachinery ||
                value == SysExRtMscCmdFmtRigging ||
                value == SysExRtMscCmdFmtFlys ||
                value == SysExRtMscCmdFmtLifts ||
                value == SysExRtMscCmdFmtTurntables ||
                value == SysExRtMscCmdFmtTrusses ||
                value == SysExRtMscCmdFmtRobots ||
                value == SysExRtMscCmdFmtAnimation ||
                value == SysExRtMscCmdFmtFloats ||
                value == SysExRtMscCmdFmtBreakaways ||
                value == SysExRtMscCmdFmtBarges ||
                value == SysExRtMscCmdFmtVideo ||
                value == SysExRtMscCmdFmtVideoTapeMachines ||
                value == SysExRtMscCmdFmtVideoCassetteMachines ||
                value == SysExRtMscCmdFmtVideoDiscPlayers ||
                value == SysExRtMscCmdFmtVideoSwitchers ||
                value == SysExRtMscCmdFmtVideoEffects ||
                value == SysExRtMscCmdFmtVideoCharacterGenerators ||
                value == SysExRtMscCmdFmtVideoStillStores ||
                value == SysExRtMscCmdFmtVideoMonitors ||
                value == SysExRtMscCmdFmtProjection ||
                value == SysExRtMscCmdFmtFilmProjectors ||
                value == SysExRtMscCmdFmtSlideProjectors ||
                value == SysExRtMscCmdFmtVideoProjectors ||
                value == SysExRtMscCmdFmtDissolvers ||
                value == SysExRtMscCmdFmtShutterControls ||
                value == SysExRtMscCmdFmtProcessControl ||
                value == SysExRtMscCmdFmtHydraulicOil ||
                value == SysExRtMscCmdFmtH2O ||
                value == SysExRtMscCmdFmtCO2 ||
                value == SysExRtMscCmdFmtCompressedAir ||
                value == SysExRtMscCmdFmtNaturalGas ||
                value == SysExRtMscCmdFmtFog ||
                value == SysExRtMscCmdFmtSmoke ||
                value == SysExRtMscCmdFmtCrackedHaze ||
                value == SysExRtMscCmdFmtPyro ||
                value == SysExRtMscCmdFmtFireworks ||
                value == SysExRtMscCmdFmtExplosions ||
                value == SysExRtMscCmdFmtFlame ||
                value == SysExRtMscCmdFmtSmokePots ||
                value == SysExRtMscCmdFmtAllTypes);
    }

    inline uint8_t getSysExRtMscCmdFmtCategory( uint8_t value ){
        return (value & 0x70);
    }

    inline bool isSysExRtMsgCategory( uint8_t value, uint8_t category ){
        return (value & 0x70) == category;
    }


    /**
     * Recommended minimum sets
     *
     * #1   Simple Controlled Device, no time code, basic data
     * #2   No time code, full data capability
     * #3   Full time code, full data capability
     * #4   Two phase commit methodology
     */
    typedef enum {
        SysExRtMscCmdExtension          = 0x00,

        // General Commands
        SysExRtMscCmdGo                 = 0x01, // 123-
        SysExRtMscCmdStop               = 0x02, // 123-
        SysExRtMscCmdResume             = 0x03, // 123-
        SysExRtMscCmdTimedGo            = 0x04, // -23-
        SysExRtMscCmdLoad               = 0x05, // -23-
        SysExRtMscCmdSet                = 0x06, // -23-
        SysExRtMscCmdFire               = 0x07, // -23-
        SysExRtMscCmdAllOff             = 0x08, // -23-
        SysExRtMscCmdRestore            = 0x09, // -23-
        SysExRtMscCmdReset              = 0x0A, // -23-
        SysExRtMscCmdGoOff              = 0x0B, // -23-

        // Sound Commands
        SysExRtMscCmdGo_JamLock         = 0x10, // --3-
        SysExRtMscCmdStandbyPlus        = 0x11, // -23-
        SysExRtMscCmdStandbyMinus       = 0x12, // -23-
        SysExRtMscCmdSequencePlus       = 0x13, // -23-
        SysExRtMscCmdSequenceMinus      = 0x14, // -23-
        SysExRtMscCmdStartClock         = 0x15, // --3-
        SysExRtMscCmdStopClock          = 0x16, // --3-
        SysExRtMscCmdZeroClock          = 0x17, // --3-
        SysExRtMscCmdSetClock           = 0x18, // --3-
        SysExRtMscCmdMtcChaseOn         = 0x19, // --3-
        SysExRtMscCmdMtcChaseOff        = 0x1A, // --3-
        SysExRtMscCmdOpenCueList        = 0x1B, // -23-
        SysExRtMscCmdCloseCueList       = 0x1C, // -23-
        SysExRtMscCmdOpenCuePath        = 0x1D, // -23-
        SysExRtMscCmdCloseCuePath       = 0x1E,  // -23-

        // Two-phase Commands
        SysExRtMscCmdStandby            = 0x20, // ---4
        SysExRtMscCmdStandingBy         = 0x21, // ---4
        SysExRtMscCmdGo2Pc              = 0x22, // ---4
        SysExRtMscCmdComplete           = 0x23, // ---4
        SysExRtMscCmdCancel             = 0x24, // ---4
        SysExRtMscCmdCancelled          = 0x25, // ---4
        SysExRtMscCmdAbort              = 0x26  // ---4
    } SysExRtMscCmd_t;

    inline bool isSysExRtMscCmd( uint8_t value ){
        return (value == SysExRtMscCmdGo ||
                value == SysExRtMscCmdStop ||
                value == SysExRtMscCmdResume ||
                value == SysExRtMscCmdTimedGo ||
                value == SysExRtMscCmdLoad ||
                value == SysExRtMscCmdSet ||
                value == SysExRtMscCmdFire ||
                value == SysExRtMscCmdAllOff ||
                value == SysExRtMscCmdRestore ||
                value == SysExRtMscCmdReset ||
                value == SysExRtMscCmdGoOff ||
                value == SysExRtMscCmdGo_JamLock ||
                value == SysExRtMscCmdStandbyPlus ||
                value == SysExRtMscCmdStandbyMinus ||
                value == SysExRtMscCmdSequencePlus ||
                value == SysExRtMscCmdSequenceMinus ||
                value == SysExRtMscCmdStartClock ||
                value == SysExRtMscCmdStopClock ||
                value == SysExRtMscCmdZeroClock ||
                value == SysExRtMscCmdSetClock ||
                value == SysExRtMscCmdMtcChaseOn ||
                value == SysExRtMscCmdMtcChaseOff ||
                value == SysExRtMscCmdOpenCueList ||
                value == SysExRtMscCmdCloseCueList ||
                value == SysExRtMscCmdOpenCuePath ||
                value == SysExRtMscCmdCloseCuePath ||
                value == SysExRtMscCmdStandby ||
                value == SysExRtMscCmdStandingBy ||
                value == SysExRtMscCmdGo2Pc ||
                value == SysExRtMscCmdComplete ||
                value == SysExRtMscCmdCancel ||
                value == SysExRtMscCmdCancelled ||
                value == SysExRtMscCmdAbort);
    }

    typedef enum {
        SysExRtMscStatusUndefinedError                      = 0x0000,

        SysExRtMscStatus1                                   = 0x1004, // meaning is dependent on Command_Format
        SysExRtMscStatus2                                   = 0x1008,
        SysExRtMscStatus3                                   = 0x100C,
        SysExRtMscStatus4                                   = 0x1010,
        SysExRtMscStatus5                                   = 0x1014,
        SysExRtMscStatus6                                   = 0x1018,
        SysExRtMscStatus7                                   = 0x101C,
        SysExRtMscStatus8                                   = 0x1104,
        SysExRtMscStatus9                                   = 0x1108,
        SysExRtMscStatus10                                  = 0x1204,

        SysExRtMscStatusChecksumError                       = 0x8000,
        SysExRtMscStatusCompleting                          = 0x8004,
        SysExRtMscStatusPaused                              = 0x8008,
        SysExRtMscStatusTerminated                          = 0x800C,
        SysExRtMscStatusReversed                            = 0x8010,
        SysExRtMscStatusTimeout                             = 0x8020,
        SysExRtMscStatusNotStandingBy                       = 0x8024,
        SysExRtMscStatusManualOverrideInitiated             = 0x8028,
        SysExRtMscStatusManualOverrideInProgress            = 0x8030, // Or possibly 0x8028??? The specs are contradictory CANCELLED vs ABORT
        SysExRtMscStatusDeadmanInterlockNotEstablished      = 0x8040,
        SysExRtMscStatusRequiredSafetyInterlockNotEstablished = 0x8044,
        SysExRtMscStatusUnknownQNumber                       = 0x8050,
        SysExRtMscStatusUnknownQList                        = 0x8054,
        SysExRtMscStatusUnknownQPath                        = 0x8058,
        SysExRtMscStatusTooManyCuesActive                   = 0x805C,
        SysExRtMscStatusCueOutOfSequence                    = 0x8060,
        SysExRtMscStatusInvalidData1Value                   = 0x8064,
        SysExRtMscStatusInvalidData2Value                   = 0x8068,
        SysExRtMscStatusInvalidData3Value                   = 0x806C,
        SysExRtMscStatusInvalidData4Value                   = 0x8070,
        SysExRtMscStatusManualCueingOfPlaybackMediumRequired = 0x8090,
        SysExRtMscStatusPowerFailureInControlledSubsystem   = 0x80A0,
        SysExRtMscStatusReadingNewShowCuesFromDisk          = 0x80B0,
    } SysExRtMscStatus_t;

    inline bool isSysExRtMscStatus( uint16_t value ){
        return (value == SysExRtMscStatusUndefinedError ||
                value == SysExRtMscStatus1 ||
                value == SysExRtMscStatus2 ||
                value == SysExRtMscStatus3 ||
                value == SysExRtMscStatus4 ||
                value == SysExRtMscStatus5 ||
                value == SysExRtMscStatus6 ||
                value == SysExRtMscStatus7 ||
                value == SysExRtMscStatus8 ||
                value == SysExRtMscStatus9 ||
                value == SysExRtMscStatus10 ||
                value == SysExRtMscStatusChecksumError ||
                value == SysExRtMscStatusCompleting ||
                value == SysExRtMscStatusPaused ||
                value == SysExRtMscStatusTerminated ||
                value == SysExRtMscStatusReversed ||
                value == SysExRtMscStatusTimeout ||
                value == SysExRtMscStatusNotStandingBy ||
                value == SysExRtMscStatusManualOverrideInitiated ||
                value == SysExRtMscStatusManualOverrideInProgress ||
                value == SysExRtMscStatusDeadmanInterlockNotEstablished ||
                value == SysExRtMscStatusRequiredSafetyInterlockNotEstablished ||
                value == SysExRtMscStatusUnknownQNumber ||
                value == SysExRtMscStatusUnknownQList ||
                value == SysExRtMscStatusUnknownQPath ||
                value == SysExRtMscStatusTooManyCuesActive ||
                value == SysExRtMscStatusCueOutOfSequence ||
                value == SysExRtMscStatusInvalidData1Value ||
                value == SysExRtMscStatusInvalidData2Value ||
                value == SysExRtMscStatusInvalidData3Value ||
                value == SysExRtMscStatusInvalidData4Value ||
                value == SysExRtMscStatusManualCueingOfPlaybackMediumRequired ||
                value == SysExRtMscStatusPowerFailureInControlledSubsystem ||
                value == SysExRtMscStatusReadingNewShowCuesFromDisk);
    }

    inline uint8_t packSysExRtMscStatus( uint8_t * bytes, uint16_t status ){
        bytes[0] = (status >> 2) & DataMask;
        bytes[1] = (status >> 10) & DataMask;
        return 2;
    }

    inline uint8_t unpackSysExRtMscStatus( uint8_t * bytes, uint16_t * status ){
        *status = (((uint16_t)bytes[0]) << 2) + (((uint16_t)bytes[1]) << 10);
        return 2;
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


    /**
     * Guideline sets:
     * #1   Simple transport, no time code, open loop communications only
     * #2   Basic transport, no time code reader, closed loop communications possible
     * #3   Advanced transport, time code reader included, closed loop communications, event triggering, track by track record control
     * #4   Basic Synchronizer, closed loop communications
     */
    typedef enum {
        SysExRtMmcCommandStop               = 0x01, // 1234 (guideline min sets)
        SysExRtMmcCommandPlay               = 0x02, // -234
        SysExRtMmcCommandDeferredPlay       = 0x03, // 1234
        SysExRtMmcCommandFastForward        = 0x04, // 1234
        SysExRtMmcCommandRewind             = 0x05, // 1234
        SysExRtMmcCommandRecordStrobe       = 0x06, // 1234
        SysExRtMmcCommandRecordExit         = 0x07, // 1234
        SysExRtMmcCommandRecordPause        = 0x08, // ----
        SysExRtMmcCommandPause              = 0x09, // ----
        SysExRtMmcCommandEject              = 0x0A, // ----
        SysExRtMmcCommandChase              = 0x0B, // ---4
        SysExRtMmcCommandCommandErrorReset  = 0x0C, // -234
        SysExRtMmcCommandMmcReset           = 0x0D, // 1234
        SysExRtMmcCommandWrite              = 0x40, // 1234
        SysExRtMmcCommandMaskedWrite        = 0x41, // --3-
        SysExRtMmcCommandRead               = 0x42, // -234
        SysExRtMmcCommandUpdate             = 0x43, // -234
        SysExRtMmcCommandLocate             = 0x44, // 1234
        SysExRtMmcCommandVariablePlay       = 0x45, // -234
        SysExRtMmcCommandSearch             = 0x46, // --34
        SysExRtMmcCommandhuttle             = 0x47, // ----
        SysExRtMmcCommandStep               = 0x48, // ----
        SysExRtMmcCommandAssignSystemMaster = 0x49, // ----
        SysExRtMmcCommandGeneratorCommand   = 0x4A, // ----
        SysExRtMmcCommandMtcCommand         = 0x4B, // ----
        SysExRtMmcCommandMove               = 0x4C, // 1234
        SysExRtMmcCommandAdd                = 0x4D, // -234
        SysExRtMmcCommandSubstract          = 0x4E, // -234
        SysExRtMmcCommandDropFrameAdjust    = 0x4F, // --34
        SysExRtMmcCommandProcedure          = 0x50, // --34
        SysExRtMmcCommandEvent              = 0x51, // --34
        SysExRtMmcCommandGroup              = 0x52, // -234
        SysExRtMmcCommandCommandSegment     = 0x53, // -234
        SysExRtMmcCommandDeferredVariablePlay = 0x54, // -234
        SysExRtMmcCommandRecordStrobeVariable = 0x55, // ----
        SysExRtMmcCommandWait               = 0x7C, // -234
        SysExRtMmcCommandResume             = 0x7F // -234
    } SysExRtMmcCommand_t;

    inline bool isSysExRtMmcCommand( uint8_t value ) {
        return (value == SysExRtMmcCommandStop ||
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


    typedef enum {
        SysExRtMmcResponseSelectedTimeCode                  = 0x01, // 1234
        SysExRtMmcResponseSelectedMasterCode                = 0x02, // ---4
        SysExRtMmcResponseRequestedOffset                   = 0x03, // ---4
        SysExRtMmcResponseActualOffset                      = 0x04, // ---4
        SysExRtMmcResponseLockDeviation                     = 0x05, // ---4
        SysExRtMmcResponseGeneratorTimeCode                 = 0x06, // ----
        SysExRtMmcResponseMidiTimeCodeInput                 = 0x07, // ----
        SysExRtMmcResponseGP0                               = 0x08, // 1234
        SysExRtMmcResponseLocatePoint                       = SysExRtMmcResponseGP0,
        SysExRtMmcResponseGP1                               = 0x09, // -234
        SysExRtMmcResponseGP2                               = 0x0A, // -234
        SysExRtMmcResponseGP3                               = 0x0B, // -234
        SysExRtMmcResponseGP4                               = 0x0C, // ----
        SysExRtMmcResponseGP5                               = 0x0D, // ----
        SysExRtMmcResponseGP6                               = 0x0E, // ----
        SysExRtMmcResponseGP7                               = 0x0F, // ----
        // 21 thru 2F SHORT forms of 01 thru 0F (?)
        SysExRtMmcResponseSignature                         = 0x40, // -234
        SysExRtMmcResponseUpdateRate                        = 0x41, // -234
        SysExRtMmcResponseResponseError                     = 0x42, // -234
        SysExRtMmcResponseCommandError                      = 0x43, // -234
        SysExRtMmcResponseCommandErrorLevel                 = 0x44, // -234
        SysExRtMmcResponseTimeStandard                      = 0x45, // -234
        SysExRtMmcResponseSelectedTimeCodeSource            = 0x46, // ----
        SysExRtMmcResponseSelectedTimeCodeUserbits          = 0x47, // ----
        SysExRtMmcResponseMotionControlTally                = 0x48, // -234
        SysExRtMmcResponseVelocityTally                     = 0x49, // ----
        SysExRtMmcResponseStopMode                          = 0x4A, // ----
        SysExRtMmcResponseFastMode                          = 0x4B, // ----
        SysExRtMmcResponseRecordMode                        = 0x4C, // -234
        SysExRtMmcResponseRecordStatus                      = 0x4D, // -234
        SysExRtMmcResponseTrackRecordStatus                 = 0x4E, // --3-
        SysExRtMmcResponseTrackRecordReady                  = 0x4F, // --3-
        SysExRtMmcResponseGlobalMonitor                     = 0x50, // --3-
        SysExRtMmcResponseRecordMonitor                     = 0x51, // ----
        SysExRtMmcResponseTrackSyncMonitor                  = 0x52, // ----
        SysExRtMmcResponseTrackInputMonitor                 = 0x53, // ----
        SysExRtMmcResponseStepLength                        = 0x54, // ----
        SysExRtMmcResponsePlaySpeedReference                = 0x55, // -23-
        SysExRtMmcResponseFixedSpeed                        = 0x56, // ----
        SysExRtMmcResponseLifterDefeat                      = 0x57, // ----
        SysExRtMmcResponseControlDisable                    = 0x58, // ---4
        SysExRtMmcResponseResolvedPlayMode                  = 0x59, // ---4
        SysExRtMmcResponseChaseMode                         = 0x5A, // ---4
        SysExRtMmcResponseGeneratorCommandTally             = 0x5B, // ----
        SysExRtMmcResponseGeneratorSetup                    = 0x5C, // ----
        SysExRtMmcResponseGeneratorUserbits                 = 0x5D, // ----
        SysExRtMmcResponseMidiTimeCodeCommandTally          = 0x5E, // ----
        SysExRtMmcResponseMidiTimeCodeSetup                 = 0x5F, // ----
        SysExRtMmcResponseProcedureResponse                 = 0x60, // --34
        SysExRtMmcResponseEventResponse                     = 0x61, // --34
        SysExRtMmcResponseTrackMute                         = 0x62, // --3-
        SysExRtMmcResponseVitcInsertEnable                  = 0x63, // ----
        SysExRtMmcResponseResponseSegment                   = 0x64, // -234
        SysExRtMmcResponseFailure                           = 0x65, // -234
        SysExRtMmcResponseWait                              = 0x7C, // -234
        SysExRtMmcResponseResume                            = 0x7F  // -234
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
        MtcFrameRate24fps      = 0b00,
        MtcFrameRate25fps      = 0b01,
        MtcFrameRate29_97fps   = 0b10, // 30fps Drop-Frame
        MtcFrameRate30fpsDropFrame = MtcFrameRate29_97fps,
        MtcFrameRate30fps      = 0b11  // 30fps Non-Drop
    } MtcFrameRate_t;

    inline bool isMtcFrameRate( uint8_t fps ) {
        return (fps == MtcFrameRate24fps ||
                fps == MtcFrameRate25fps ||
                fps == MtcFrameRate29_97fps ||
                fps == MtcFrameRate30fps);
    }

    inline MtcFrameRate_t getFps( uint8_t fpsHour ){
        return (MtcFrameRate_t)((fpsHour & MtcFpsMask) >> MtcFpsOffset);
    }

    inline uint8_t setFps( MtcFrameRate_t fps ){
        return (((uint8_t)fps) << MtcFpsOffset) & MtcFpsMask;
    }

    inline uint8_t getHour( uint8_t fpsHour ){
        return fpsHour & MtcHourMask;
    }


    // Structures
    /////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////


    /**
     * Container for MIDI Time Code Quarter Frames
     */
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


    typedef union {
        uint16_t Value;
        GlobalParameterControl_t GlobalParameterControl;
    } DeviceControlData_t;


    typedef union {
        uint32_t Value;
        uint8_t Bytes[3];
    } MscExtensibleCommandField_t;


    typedef struct {
        uint8_t * Number;
        uint8_t * List;
        uint8_t * Path;
    } MscCueNumber_t;

    inline bool isMscCueNumberChar( uint8_t value ){
        return ( '0' <= value && value <= '9') || value == '.';
    }


    typedef struct {
        MscExtensibleCommandField_t CommandFormat;
        MscExtensibleCommandField_t Command;
        MscCueNumber_t CueNumber;
        MidiTimeCode_t MidiTimeCode;
        uint16_t Controller;
        uint16_t Value;
        uint8_t MacroNumber;
        uint16_t Checksum;
        uint16_t SequenceNumber;
        uint16_t Status;
        uint8_t Data[4];
    } MidiShowControlData_t;

    typedef struct {
        MidiTimeCode_t MidiTimeCode;
        uint16_t EventNumber;
    } MtcCueingData_t;

    typedef struct {
        uint32_t ManufacturerId;
        uint16_t DeviceFamily;
        uint16_t DeviceFamilyMember;
        uint8_t SoftwareRevision[4];
    } GeneralInformationData_t;


    typedef enum {
        SampleDumpLoopTypeForwardOnly       = 0x00,
        SampleDumpLoopTypeBackwardForward   = 0x01,
        SampleDumpLoopTypeLoopOff           = 0x7F
    } SampleDumpLoopType_t;

    inline bool isSampleDumpLoopType( uint8_t value ){
        return (value == SampleDumpLoopTypeForwardOnly ||
                value == SampleDumpLoopTypeBackwardForward ||
                value == SampleDumpLoopTypeLoopOff);
    }

    typedef struct {
        uint16_t SampleNumber;
        uint8_t SampleFormat; // # of significant bits from 8 - 28
        uint32_t SamplePeriod; // in nsec (3 bytes)
        uint32_t SampleLength; // in words (3 bytes)
        uint32_t LoopStartPoint; // Word number (3 bytes)
        uint32_t LoopEndPoint; // Word number (3 bytes)
        uint8_t LoopType; //
    } SampleDumpHeaderData_t;

    typedef struct {
        uint16_t RequestedSample;
    } SampleDumpRequestData_t;

    typedef struct {
        uint8_t RunningPacketCount;
        uint8_t Length;
        uint8_t * Data;
        uint8_t Checksum; // XOR of complete message to end of payload
    } SampleDumpDataPacket_t;

    inline uint8_t xorChecksum( uint8_t * bytes, uint8_t length ){
        uint8_t chk = bytes[0];

        for( uint8_t i = 1; i < length; i++){
            chk ^= bytes[i];
        }

        return chk;
    }

    typedef enum {
        SampleDumpExtLoopTypeForward                    = 0x00,
        SampleDumpExtLoopTypeForwardBackward            = 0x01,
        SampleDumpExtLoopTypeForwardWithRelease         = 0x02,
        SampleDumpExtLoopTypeForwardBackwardWithRelease = 0x03,
        SampleDumpExtLoopTypeBackward                   = 0x40,
        SampleDumpExtLoopTypeBackwardForward            = 0x41,
        SampleDumpExtLoopTypeBackwardWithRelease        = 0x42,
        SampleDumpExtLoopTypeBackwardForwardWithRelease = 0x43,
        SampleDumpExtLoopTypeBackwardOneShot            = 0x7E,
        SampleDumpExtLoopTypeForwardOneShot             = 0x7F
    } SampleDumpExtLoopType_t;

    typedef struct {
        uint16_t SampleNumber;
        uint8_t SampleFormat; // # of significant bits from 8 - 28
        uint32_t SampleRateIntegerPortion; // in Hz
        uint32_t SampleRateFractionalPortion; // in Hz
        uint64_t SampleLength; // in words
        uint64_t SustainLoopStart; // word number
        uint64_t SustainLoopEnd; // word number
        uint8_t LoopType;
        uint8_t NumberofChannels;
    } SampleDumpExtLoopPointHeaderData_t;

    typedef struct {
        uint16_t Number;
        uint16_t LoopNumber; // 7f7f delete all loops
        uint8_t LoopType;
        uint64_t LoopStartAddress;
        uint64_t LoopEndAddress;
    } SampleDumpExtLoopPointTransmissionData_t;

    typedef struct {
        uint16_t SampleNumber;
        uint16_t LoopNumber; // 7F7F = all loops
    } SampleDumpExtLoopPointRequestData_t;

    typedef struct {
        uint16_t SampleNumber;
        uint8_t LanguageTagLength;
        uint8_t * LanguageTag;
        uint8_t NameLength;
        uint8_t * Name;
    } SampleDumpExtNameTransmissionData_t;

    typedef struct {
        uint16_t SampleNumber;
    } SampleDumpExtNameRequestData_t;

    typedef struct {
        StatusClass_t StatusClass;
        uint8_t SystemMessage;
        uint8_t Channel; // Also DeviceId for SysEx messages
        union {
            struct {
                uint8_t Key;
                uint8_t Velocity;
            } Note;
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
            MtcQuarterFrame_t MtcQuarterFrame;
            struct {
                uint16_t Position;
            } SongPositionPointer;
            struct {
                uint8_t Song;
            } SongSelect;
            struct {
                uint32_t Id;
                uint8_t Length;
                uint8_t SubId1;
                uint8_t SubId2;
                union {
                    uint8_t PacketNumber;

                    MidiTimeCode_t MidiTimeCode;

                    MtcCueingData_t Cueing;

                    GeneralInformationData_t GeneralInfo;

                    DeviceControlData_t DeviceControl;

                    MidiShowControlData_t MidiShowControl;

                    union {
                        SampleDumpHeaderData_t Header;
                        SampleDumpRequestData_t Request;
                        SampleDumpDataPacket_t Data;
                    } SampleDump;

                    union {
                        SampleDumpExtLoopPointHeaderData_t Header;
                        SampleDumpExtLoopPointTransmissionData_t Transmission;
                        SampleDumpExtLoopPointRequestData_t Request;
                        SampleDumpExtNameTransmissionData_t NameTransmission;
                        SampleDumpExtNameRequestData_t NameRequest;
                    } SampleDumpExt;

                } Data;

                uint8_t * ByteData;
            } SysEx;

        } Data;
    } Message_t;


    /**
     * Test wether message is a system common message.
     */
    inline bool isSystemCommonMessage( Message_t * msg ){
        return isSystemCommonMessage( msg->StatusClass );
    }

    /**
     * Test wether message is a real time system message.
     */
    inline bool isSystemRealTimeMessage( Message_t * msg ){
        return isSystemRealTimeMessage( msg->StatusClass );
    }

    /**
     * Test wether message is a channel mode message (ie specific cc message)
     */
    inline bool isChannelModeMessage( uint8_t byte0, uint8_t byte1 ){
        return getStatusClass(byte0) == StatusClassControlChange && isChannelModeController(byte1);
    }

    /**
     * Test wether message is a channel mode message (ie specific cc message)
     */
    inline bool isChannelModeMessage( Message_t * msg ){
        return isChannelModeMessage( msg->StatusClass, msg->Data.ControlChange.Controller );
    }


    /**
     * Test wether message is a channel voice message (ie specific cc message)
     */
    inline bool isChannelVoiceMessage( uint8_t byte0, uint8_t byte1 ) {
        return isStatusClass( (byte0 & StatusClassMask) )  &&
               (byte0 & StatusClassMask) != StatusClassSystemMessage &&
               !isChannelModeMessage(byte0, byte1);
    }

    /**
     * Test wether message is a channel voice message (ie specific cc message)
     */
    inline bool isChannelVoiceMessage( Message_t * msg ){
        return isChannelVoiceMessage( msg->StatusClass, msg->Data.ControlChange.Controller );
    }

    /**
     * Get device Id of a sysex byte stream
     */
    inline uint8_t getDeviceId( uint8_t * bytes ){
        ASSERT( bytes != NULL );
        ASSERT( bytes[0] == SystemMessageSystemExclusive );

        return bytes[2] & DataMask;
    }

    /**
     * Extract complete MIDI Time Code from 8 QuarterFrames given in the expected message order.
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

    /**
     * Extract Quarter Frame nibble from given MIDI Time Code.
     */
    inline uint8_t MidiQuarterFrameNibbleFromTimeCode( MidiTimeCode_t * mtc, MtcQuarterFrameMessageType_t type ) {
        ASSERT( mtc != NULL );
        ASSERT( isMtcQuarterMessageType(type) );

        switch(type){
            MtcQuarterFrameMessageTypeFrameLS: return mtc->Frame & 0x0F;
            MtcQuarterFrameMessageTypeFrameMS:     return (mtc->Frame >> 4) & 0b1;
            MtcQuarterFrameMessageTypeSecondLS:    return mtc->Second & 0x0F;
            MtcQuarterFrameMessageTypeSecondMS:    return (mtc->Second >> 4) & 0b0011;
            MtcQuarterFrameMessageTypeMinuteLS:    return mtc->Minute & 0x0F;
            MtcQuarterFrameMessageTypeMinuteMS:    return (mtc->Minute >> 4) & 0b0011;
            MtcQuarterFrameMessageTypeHourLS:      return mtc->FpsHour & 0x0F;
            MtcQuarterFrameMessageTypeHourMS:      return (mtc->FpsHour >> 4) & 0b0111;
            default:
                break;
        }

        return (uint8_t) ~0;
    }



    inline uint8_t packMidiTimeCode( uint8_t * bytes, MidiTimeCode_t * mtc ){
        ASSERT( bytes != NULL );
        ASSERT( mtc != NULL );

        bytes[0] = mtc->FpsHour;
        bytes[1] = mtc->Minute;
        bytes[2] = mtc->Second;
        bytes[3] = mtc->Frame;

        return 4;
    }

    inline uint8_t unpackMidiTimeCode( uint8_t * bytes, MidiTimeCode_t * mtc ){
        ASSERT( bytes != NULL );
        ASSERT( mtc != NULL );

        mtc->FpsHour = bytes[0];
        mtc->Minute = bytes[1];
        mtc->Second = bytes[2];
        mtc->Frame = bytes[3];

        return 4;
    }

    inline uint8_t packMidiTimeCodeLong( uint8_t * bytes, MidiTimeCode_t * mtc ){
        ASSERT( bytes != NULL );
        ASSERT( mtc != NULL );

        bytes[0] = mtc->FpsHour;
        bytes[1] = mtc->Minute;
        bytes[2] = mtc->Second;
        bytes[3] = mtc->Frame;
        bytes[4] = mtc->FractionalFrame;

        return 5;
    }

    inline uint8_t unpackMidiTimeCodeLong( uint8_t * bytes, MidiTimeCode_t * mtc ){
        ASSERT( bytes != NULL );
        ASSERT( mtc != NULL );

        mtc->FpsHour = bytes[0];
        mtc->Minute = bytes[1];
        mtc->Second = bytes[2];
        mtc->Frame = bytes[3];
        mtc->FractionalFrame = bytes[4];

        return 5;
    }

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

    inline bool isValidMscCueNumberPart( uint8_t * str ){
        if (str == NULL || str[0] == '\0'){
            return true;
        }

        for (uint8_t i = 0; str[i] != '\0'; i++){
            if ( ! isMscCueNumberChar(str[i]) ) {
                return false;
            }
        }

        return true;
    }

    inline uint8_t packMscCueNumber( uint8_t * bytes, uint8_t * number, uint8_t * list, uint8_t * path ){
        ASSERT( bytes != NULL );
        ASSERT( isValidMscCueNumberPart(number) && number != NULL && isMscCueNumberChar(number[0]) );
        ASSERT( isValidMscCueNumberPart(list) );
        ASSERT( isValidMscCueNumberPart(path) );

        int len = 0;

        for(uint8_t i = 0; number[i] != '\0'; i++){
            bytes[len++] = number[i];
        }

        if (list == NULL || ! isMscCueNumberChar(list[0]) ){
            return len;
        }

        bytes[len++] = '\0';

        for ( uint8_t i = 0; list[i] != '\0'; i++){
            bytes[len++] = list[i];
        }

        if (path == NULL || ! isMscCueNumberChar(path[0]) ) {
            return len;
        }

        bytes[len++] = '\0';

        for ( uint8_t i = 0; path[i] != '\0'; i++){
            bytes[len++] = list[i];
        }

        return len;
    }

    inline bool unpackMscCueNumber( uint8_t * bytes, uint8_t len, uint8_t ** number, uint8_t ** list, uint8_t ** path ){
        ASSERT( bytes != NULL );
        ASSERT( number != NULL );
        ASSERT( list != NULL );
        ASSERT( path != NULL );

        if (len < 1){
            return false;
        }

        uint8_t l = 0;

        *number = bytes;

        for(; l < len; l++){
            if (bytes[l] == '\0'){
                break;
            }
            if (bytes[l] == SystemMessageEndOfExclusive){
                bytes[l] = '\0';
                *list = NULL;
                *path = NULL;
                return true;
            }
            if ( ! isMscCueNumberChar(bytes[l]) ){
                return false;
            }
        }

        l++;

        if ( l >= len || ! isMscCueNumberChar(bytes[l]) ){
            *list = NULL;
            *path = NULL;
            return true;
        }

        *list = &bytes[l];

        for(; l < len; l++){
            if (bytes[l] == '\0'){
                break;
            }
            if (bytes[l] == SystemMessageEndOfExclusive){
                bytes[l] = '\0';
                *path = NULL;
                return true;
            }
            if ( ! isMscCueNumberChar(bytes[l]) ){
                return false;
            }
        }

        l++;

        if ( l >= len || ! isMscCueNumberChar(bytes[l]) ){
            *path = NULL;
            return true;
        }


        for(; l < len; l++){
            if (bytes[l] == '\0') {
                // by specification Cue numbers must end with an EOX
                return false;
            }
            if (bytes[l] == SystemMessageEndOfExclusive){
                bytes[l] = '\0';
                return true;
            }
            if ( ! isMscCueNumberChar(bytes[l]) ){
                return false;
            }
        }

        return false;
    }


    inline uint8_t copyMscExtensibleCommandField( uint8_t * dst, uint8_t * src ) {
        ASSERT(dst != NULL);
        ASSERT(src != NULL);

        dst[0] = src[0];
        if (src[0] != SysExRtMscCmdExtension) {
            return 1;
        }

        dst[1] = src[1];
        if (src[1] != SysExRtMscCmdExtension) {
            return 2;
        }

        dst[2] = src[2];

        return 3;
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

}

#endif //MIDIMESSAGE_H
