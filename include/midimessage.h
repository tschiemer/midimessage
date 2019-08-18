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

    const uint8_t StatusMaskByte  = 0x80;

    const uint8_t StatusMask      = 0xF0;
    const uint8_t ChannelMask     = 0x0F;

    const uint8_t ValueMask         = 0b01111111; // = 127
    const uint16_t DoubleValueMask  = 0b0111111101111111;

    const uint8_t MaxValue          = 127;
    const uint16_t MaxDoubleValue   = 0x3FFF;

    const int MessageLengthNoteOff                  = 3;
    const int MessageLengthNoteOn                   = 3;
    const int MessageLengthPolyphonicKeyPressure    = 3;
    const int MessageLengthControlChange            = 3;
    const int MessageLengthProgramChange            = 2;
    const int MessageLengthChannelPressure          = 2;
    const int MessageLengthPitchBendChange          = 3;

    const uint16_t PitchCenter      = 0x2000;

    const uint8_t BroadcastDeviceId = 0x7F; // for Universal SysEx (Non-)Realtime Messages

    typedef enum {
        SystemExclusive                             = 0x00,
        SystemExclusiveMIDITimeCodeQuarterFrame     = 0x01,
        SystemExclusiveSongPositionPointer          = 0x02,
        SystemExclusiveSongSelect                   = 0x03,
        SystemExclusiveTuneRequest                  = 0x06,
        SystemExclusiveEndOfExclusive               = 0x07,
        SystemExclusiveTimingClock                  = 0x08,
        SystemExclusiveStart                        = 0x0A,
        SystemExclusiveContinue                     = 0x0B,
        SystemExclusiveStop                         = 0x0C,
        SystemExclusiveActiveSensing                = 0x0E,
        SystemExclusiveReset                        = 0x0F
    } SystemExclusive_t;

    typedef enum {
        StatusNoteOff               = 0x80,
        StatusNoteOn                = 0x90,
        StatusPolyphonicKeyPressure = 0xA0,
        StatusControlChange         = 0xB0,
        StatusProgramChange         = 0xC0,
        StatusChannelPressure       = 0xD0,
        StatusPitchBendChange       = 0xE0,
        StatusSystemExclusive       = 0xF0,
        StatusSystemExclusiveMIDITimeCodeQuarterFrame       = StatusSystemExclusive + SystemExclusiveMIDITimeCodeQuarterFrame,
        StatusSystemExclusiveSongPositionPointer            = StatusSystemExclusive + SystemExclusiveSongPositionPointer,
        StatusSystemExclusiveSongSelect                     = StatusSystemExclusive + SystemExclusiveSongSelect,
        StatusSystemExclusiveTuneRequest                    = StatusSystemExclusive + SystemExclusiveTuneRequest,
        StatusSystemExclusiveEndOfExclusive                 = StatusSystemExclusive + SystemExclusiveEndOfExclusive,
        StatusSystemExclusiveTimingClock                    = StatusSystemExclusive + SystemExclusiveTimingClock,
        StatusSystemExclusiveStart                          = StatusSystemExclusive + SystemExclusiveStart,
        StatusSystemExclusiveContinue                       = StatusSystemExclusive + SystemExclusiveContinue,
        StatusSystemExclusiveStop                           = StatusSystemExclusive + SystemExclusiveStop,
        StatusSystemExclusiveActiveSensing                  = StatusSystemExclusive + SystemExclusiveActiveSensing,
        StatusSystemExclusiveReset                          = StatusSystemExclusive + SystemExclusiveReset
    } Status_t;

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

    typedef enum {
        ReservedSystemExclusiveIdManufacturerExtension  = 0x00,
        ReservedSystemExclusiveIdExperimental           = 0x7D,
        ReservedSystemExclusiveIdNonRealTime            = 0x7E,
        ReservedSystemExclusiveIdRealTime               = 0x7F
    } ReservedSystemExclusiveId_t;

    typedef enum {
        UniversalSysExNonRTSampleDumpHeader       = 0x01,
        UniversalSysExNonRTSampleDataPacket       = 0x02,
        UniversalSysExNonRTSampleDumpRequest      = 0x03,
        UniversalSysExNonRTMidiTimeCode           = 0x04,
        UniversalSysExNonRTSampleDumpExtension    = 0x05,
        UniversalSysExNonRTGeneralInformation     = 0x06,
        UniversalSysExNonRTFileDump               = 0x07,
        UniversalSysExNonRTMidiTuningStandard     = 0x08,
        UniversalSysExNonRTGeneralMidi            = 0x09,
        UniversalSysExNonRTDownloadableSounds     = 0x0A,
        UniversalSysExNonRTFileReferenceMessage   = 0x0B,
        UniversalSysExNonRTMidiVisualControl      = 0x0C,
        UniversalSysExNonRTMidiCapabilityInquiry  = 0x0D,

        UniversalSysExNonRTEndOfFile              = 0x7B,
        UniversalSysExNonRTWait                   = 0x7C,
        UniversalSysExNonRTCancel                 = 0x7D,
        UniversalSysExNonRTNAK                    = 0x7E,
        UniversalSysExNonRTACK                    = 0x7F
    } UniversalSysExNonRT_t;

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

    typedef enum {
        UniversalSysExNonRtSampleDumpExtensionLoopPointsTransmission    = 0x01,
        UniversalSysExNonRtSampleDumpExtensionLoopPointsRequest         = 0x02,
        UniversalSysExNonRtSampleDumpExtensionSampleNameTransmission    = 0x03,
        UniversalSysExNonRtSampleDumpExtensionSampleNameRequest         = 0x04,
        UniversalSysExNonRtSampleDumpExtensionExtendedDumpHeader        = 0x05,
        UniversalSysExNonRtSampleDumpExtensionExtendedLoopPointsTransmission    = 0x06,
        UniversalSysExNonRtSampleDumpExtensionExtendedLoopPointsRequest = 0x07
    } UniversalSysExNonRtSampleDumpExtension_t;

    typedef enum {
        UniversalSysExNonRtGeneralInformationIdentityRequest    = 0x01,
        UniversalSysExNonRtGeneralInformationIdentityReply      = 0x02
    } UniversalSysExNonRtGeneralInformation_t;

    typedef enum {
        UniversalSysExNonRtFileDumpHeader       = 0x01,
        UniversalSysExNonRtFileDumpDataPacket   = 0x02,
        UniversalSysExNonRtFileDumpRequest      = 0x03
    } UniversalSysExNonRtFileDump_t;

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

    typedef enum {
        UniversalSysExNonRtGeneralMidi1SystemOn     = 0x01,
        UniversalSysExNonRtGeneralMidiSystemOff     = 0x02,
        UniversalSysExNonRtGeneralMidi2SystemOn     = 0x03
    } UniversalSysExNonRtGeneralMidi_t;

    typedef enum {
        UniversalSysExNonRtDownloadableSoundsTurnDlsOn      = 0x01,
        UniversalSysExNonRtDownloadableSoundsTurnDlsOff     = 0x02,
        UniversalSysExNonRtDownloadableSoundsTurnDlsVoiceAllocOff = 0x03,
        UniversalSysExNonRtDownloadableSoundsTurnDlsVoiceAllocOn  = 0x04
    } UniversalSysExNonRtDownloadableSounds_t;

    typedef enum {
        UniversalSysExNonRtFileReferenceMessageOpenFile         = 0x01,
        UniversalSysExNonRtFileReferenceMessageSelectContents   = 0x02,
        UniversalSysExNonRtFileReferenceMessageOpenAndSelect    = 0x03,
        UniversalSysExNonRtFileReferenceMessageCloseFile        = 0x04
    } UniversalSysExNonRtFileReferenceMessage_t;

    typedef enum {
        UniversalSysExNonRtMidiVisualControlTODO //TODO
    } UniversalSysExNonRtMidiVisualControl_t;

    typedef enum {
        UniversalSysExNonRtMidiCapabilityInquiryTODO //TODO
    } UniversalSysExNonRtMidiCapabilityInquiry_t;

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

    typedef enum {
        UniversalSysExRtTimeCodeFullMessage           = 0x01,
        UniversalSysExRtTimeCodeUserBits              = 0x02
    } UniversalSysExRtTimeCode_t;

    typedef enum {
        UniversalSysExRtMidiShowControlTODO //TODO
    } UniversalSysExRtMidiShowControl_t;

    typedef enum {
        UniversalSysExRtNotationInformationBarNumber                = 0x01,
        UniversalSysExRtNotationInformationTimeSignatureImmediate   = 0x02,
        UniversalSysExRtNotationInformationTimeSignatureDelayed     = 0x03
    } UniversalSysExRtNotationInformation_t;

    typedef enum {
        UniversalSysExRtDeviceControlMasterVolume           = 0x01,
        UniversalSysExRtDeviceControlMasterBalance          = 0x02,
        UniversalSysExRtDeviceControlMasterFineTuning       = 0x03,
        UniversalSysExRtDeviceControlMasterCoarseTuning     = 0x04,
        UniversalSysExRtDeviceControlGlobalParameterControl = 0x05
    } UniversalSysExRtDeviceControl_t;

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

    typedef enum {
        UniversalSysExRtMidiMachineControlCommandsTODO //TODO
    } UniversalSysExRtMidiMachineControlCommands_t;

    typedef enum {
        UniversalSysExRtMidiMachineControlResponsesTODO //TODO
    } UniversalSysExRtMidiMachineControlResponses_t;

    typedef enum {
        UniversalSysExRtMidiTuningStandardSingleNoteTuningChange                = 0x01,
        UniversalSysExRtMidiTuningStandardSingleNoteTuningChangeWithBankSelect  = 0x02,
        UniversalSysExRtMidiTuningStandardScaleTuning1Byte                      = 0x03,
        UniversalSysExRtMidiTuningStandardScaleTuning2Byte                      = 0x04
    } UniversalSysExRtMidiTuningStandard_t;

    typedef enum {
        UniversalSysExRtControllerDestinationSettingChannelPressure         = 0x01,
        UniversalSysExRtControllerDestinationSettingPolyphonicKeyPressure   = 0x02,
        UniversalSysExRtControllerDestinationSettingController              = 0x03
    } UniversalSysExRtControllerDestinationSetting_t;


    typedef struct {
        Status_t Status;
        uint8_t Channel;
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
            union {
                struct {
                    uint32_t Id;
                    uint8_t Length;
#if SYSEX_MEMORY == SYSEX_MEMORY_STATIC
                    uint8_t Data[SYSEX_MEMORY_STATIC_SIZE];
#elif SYSEX_MEMORY == SYSEX_MEMORY_DYNAMIC
                    void * Data;
#endif
                } Custom;
                struct {
                    uint8_t MessageType;
                    uint8_t Values;
                } MIDITimeCodeQuarterFrame;
                struct {
                    uint16_t Position;
                } SongPositionPointer;
                struct {
                    uint8_t Song;
                } SongSelect;
            } SysEx;
        } Data;
    } Message_t;


    inline bool isValidStatusByte( uint8_t byte ){
        return (byte & StatusMaskByte) == StatusMaskByte;
    }


    inline Status_t getStatus( uint8_t byte ){
        return (Status_t)(byte & StatusMask);
    }

    inline uint8_t getChannel( uint8_t byte ){
        return (byte & ChannelMask);
    }

    inline uint8_t getValue( uint8_t byte ){
        return (byte & ValueMask);
    }

    inline void unpackDoubleValue( uint8_t * bytes, uint16_t * value ){
        ASSERT( bytes[0] & ValueMask == bytes[0] );
        ASSERT( bytes[1] & ValueMask == bytes[1] );

        *value = (((uint16_t)bytes[1]) << 8) | ((uint16_t)bytes[0]);
    }

    inline void packDoubleValue( uint8_t * bytes, uint16_t value ) {
        bytes[0] = value & ValueMask;
        bytes[1] = (value >> 7) & ValueMask;
    }


    inline bool isChannelModeMessage( uint8_t status, uint8_t controller ){
        return (status & StatusMask) == StatusControlChange  &&
               ((controller & ChannelModeControllerAllSoundOff) == ChannelModeControllerResetAllControllers ||
                (controller & ChannelModeControllerAllSoundOff) == ChannelModeControllerLocalControl ||
                (controller & ChannelModeControllerAllSoundOff) == ChannelModeControllerAllNotesOff ||
                (controller & ChannelModeControllerAllSoundOff) == ChannelModeControllerOmniModeOff ||
                (controller & ChannelModeControllerAllSoundOff) == ChannelModeControllerOmniModeOn ||
                (controller & ChannelModeControllerAllSoundOff) == ChannelModeControllerMonoModeOn ||
                (controller & ChannelModeControllerAllSoundOff) == ChannelModeControllerPolyModeOn);
    }

    inline bool isChannelModeMessage( uint8_t * bytes ){
        return isChannelModeMessage( bytes[0], bytes[1] );
    }

    inline bool isChannelModeMessage( Message_t * msg ){
        return isChannelModeMessage( msg->Status, msg->Data.ControlChange.Controller );
    }

    inline bool isChannelVoiceMessage( uint8_t status, uint8_t byte1 ){
        return ((status & StatusMask) == StatusNoteOff ||
                (status & StatusMask) == StatusNoteOn ||
                (status & StatusMask) == StatusPolyphonicKeyPressure ||
                (status & StatusMask) == StatusControlChange ||
                (status & StatusMask) == StatusProgramChange ||
                (status & StatusMask) == StatusChannelPressure ||
                (status & StatusMask) == StatusPitchBendChange) &&
               !isChannelModeMessage(status, byte1);
    }

    inline bool isChannelVoiceMessage( uint8_t * bytes ){
        return isChannelVoiceMessage( bytes[0], bytes[1] );
    }

    inline bool isChannelVoiceMessage( Message_t * msg ){
        return isChannelVoiceMessage( msg->Status, msg->Data.ControlChange.Controller );
    }


    inline bool isSystemExclusive( Status_t status ){
        return (status & StatusMask) == StatusSystemExclusive;
    }

    inline bool isSystemCommonMessage( uint8_t status ){
        return ((status & StatusMask) == StatusSystemExclusive) &&
               ((status & ChannelMask) == SystemExclusive ||
                (status & ChannelMask) == SystemExclusiveMIDITimeCodeQuarterFrame ||
                (status & ChannelMask) == SystemExclusiveSongPositionPointer ||
                (status & ChannelMask) == SystemExclusiveSongSelect ||
                (status & ChannelMask) == SystemExclusiveTuneRequest ||
                (status & ChannelMask) == SystemExclusiveEndOfExclusive);
    }

    inline bool isSystemCommonMessage( uint8_t * bytes ){
        return isSystemCommonMessage( bytes[0] );
    }

    inline bool isSystemCommonMessage( Message_t * msg ){
        return isSystemCommonMessage( msg->Status );
    }


    inline bool isSystemRealTimeMessage( uint8_t status ){
        return ((status & StatusMask) == StatusSystemExclusive) &&
               ((status & ChannelMask) == SystemExclusiveTimingClock ||
                (status & ChannelMask) == SystemExclusiveStart ||
                (status & ChannelMask) == SystemExclusiveContinue ||
                (status & ChannelMask) == SystemExclusiveStop ||
                (status & ChannelMask) == SystemExclusiveActiveSensing ||
                (status & ChannelMask) == SystemExclusiveReset);
    }

    inline bool isSystemRealTimeMessage( uint8_t * bytes ){
        return isSystemRealTimeMessage( bytes[0] );
    }

    inline bool isSystemRealTimeMessage( Message_t * msg ){
        return isSystemRealTimeMessage( msg->Status );
    }



    inline int packNoteOff( uint8_t * bytes, uint8_t channel, uint8_t key, uint8_t velocity ){
        ASSERT( channel & ChannelMask == channel );

        bytes[0] = StatusNoteOff | (channel & ChannelMask);
        bytes[1] = key & ValueMask;
        bytes[2] = velocity & ValueMask;

        return MessageLengthNoteOff;
    }

    inline bool unpackNoteOff( uint8_t * bytes, int len, uint8_t * channel, uint8_t * key, uint8_t  * velocity ){
        ASSERT( bytes[0] & StatusMask == StatusNoteOff );

        if (len != MessageLengthNoteOff){
            return false;
        }

        *channel = bytes[0] & ChannelMask;
        *key = bytes[1] & ValueMask;
        *velocity = bytes[2] & ValueMask;

        return true;
    }


    inline int packNoteOn( uint8_t * bytes, uint8_t channel, uint8_t key, uint8_t velocity ){
        ASSERT( channel & ChannelMask == channel );

        bytes[0] = StatusNoteOn | (channel & ChannelMask);
        bytes[1] = key & ValueMask;
        bytes[2] = velocity & ValueMask;

        return MessageLengthNoteOn;
    }

    inline bool unpackNoteOn( uint8_t * bytes, int len, uint8_t * channel, uint8_t * key, uint8_t * velocity ){
        ASSERT( bytes[0] & StatusMask == StatusNoteOn );

        if (len != MessageLengthNoteOn){
            return false;
        }

        *channel = bytes[0] & ChannelMask;
        *key = bytes[1] & ValueMask;
        *velocity = bytes[2] & ValueMask;

        return true;
    }


    inline int packPolyphonicKeyPressure( uint8_t * bytes, uint8_t channel, uint8_t key, uint8_t pressure ){
        ASSERT( channel & ChannelMask == channel );

        bytes[0] = StatusPolyphonicKeyPressure | (channel & ChannelMask);
        bytes[1] = key & ValueMask;
        bytes[2] = pressure & ValueMask;

        return MessageLengthPolyphonicKeyPressure;
    }

    inline bool unpackPolyphonicKeyPressure( uint8_t * bytes, int len, uint8_t * channel, uint8_t * key, uint8_t * pressure ){
        ASSERT( bytes[0] & StatusMask == StatusPolyphonicKeyPressure );

        if (len != MessageLengthPolyphonicKeyPressure){
            return false;
        }

        *channel = bytes[0] & ChannelMask;
        *key = bytes[1] & ValueMask;
        *pressure = bytes[2] & ValueMask;

        return true;
    }


    inline int packControlChange( uint8_t * bytes, uint8_t channel, uint8_t controller, uint8_t value ){
        ASSERT( channel & ChannelMask == channel );

        bytes[0] = StatusControlChange | (channel & ChannelMask);
        bytes[1] = controller & ValueMask;
        bytes[2] = value & ValueMask;

        return MessageLengthControlChange;
    }

    inline bool unpackControlChange( uint8_t * bytes, int len, uint8_t * channel, uint8_t * controller, uint8_t * value ){
        ASSERT( bytes[0] & StatusMask == StatusControlChange );

        if (len != MessageLengthControlChange){
            return false;
        }

        *channel = bytes[0] & ChannelMask;
        *controller = bytes[1] & ValueMask;
        *value = bytes[2] & ValueMask;

        return true;
    }


    inline int packProgramChange( uint8_t * bytes, uint8_t channel, uint8_t program ){
        ASSERT( channel & ChannelMask == channel );

        bytes[0] = StatusProgramChange | (channel & ChannelMask);
        bytes[1] = program & ValueMask;

        return MessageLengthProgramChange;
    }

    inline bool unpackProgramChange( uint8_t * bytes, int len, uint8_t * channel, uint8_t * program ){
        ASSERT( bytes[0] & StatusMask == StatusProgramChange );

        if (len != MessageLengthProgramChange){
            return false;
        }

        *channel = bytes[0] & ChannelMask;
        *program = bytes[1] & ValueMask;

        return true;
    }


    inline int packChannelPressure( uint8_t * bytes, uint8_t channel, uint8_t pressure ){
        ASSERT( channel & ChannelMask == channel );

        bytes[0] = StatusChannelPressure | (channel & ChannelMask);
        bytes[1] = pressure & ValueMask;

        return MessageLengthChannelPressure;
    }

    inline bool unpackChannelPressure( uint8_t * bytes, int len, uint8_t * channel, uint8_t * pressure ){
        ASSERT( bytes[0] & StatusMask == StatusChannelPressure );

        if (len != MessageLengthChannelPressure){
            return false;
        }

        *channel = bytes[0] & ChannelMask;
        *pressure = bytes[1] & ValueMask;

        return true;
    }


    inline int packPitchBendChange( uint8_t * bytes, uint8_t channel, uint16_t pitch ){
        ASSERT( channel & ChannelMask == channel );
        ASSERT( pitch <= MaxDoubleValue );

        bytes[0] = StatusPitchBendChange | (channel & ChannelMask);
        packDoubleValue( &bytes[1], pitch );

        return MessageLengthPitchBendChange;
    }

    inline bool unpackPitchBendChange( uint8_t * bytes, int len, uint8_t * channel, uint16_t * pitch ){
        ASSERT( bytes[0] & StatusMask == StatusPitchBendChange );

        if (len != MessageLengthPitchBendChange){
            return false;
        }

        *channel = bytes[0] & ChannelMask;
        unpackDoubleValue( &bytes[1], pitch );

        return false;
    }



    inline int packAllSoundOff( uint8_t * bytes, uint8_t channel ){
        ASSERT( channel & ChannelMask == channel );

        bytes[0] = StatusControlChange | (channel & ChannelMask);
        bytes[1] = ChannelModeControllerAllSoundOff;
        bytes[2] = 0;

        return 3;
    }

    inline int packResetAllControllers( uint8_t * bytes, uint8_t channel ){
        ASSERT( channel & ChannelMask == channel );

        bytes[0] = StatusControlChange | (channel & ChannelMask);
        bytes[1] = ChannelModeControllerResetAllControllers;
        bytes[2] = 0;

        return 3;
    }

    inline int packLocalControl( uint8_t * bytes, uint8_t channel, bool on ){
        ASSERT( channel & ChannelMask == channel );

        bytes[0] = StatusControlChange | (channel & ChannelMask);
        bytes[1] = ChannelModeControllerLocalControl;
        bytes[2] = on ? 127 : 0;

        return 3;
    }

    inline int packAllNotesOff( uint8_t * bytes, uint8_t channel ){
        ASSERT( channel & ChannelMask == channel );

        bytes[0] = StatusControlChange | (channel & ChannelMask);
        bytes[1] = ChannelModeControllerAllNotesOff;
        bytes[2] = 0;

        return 3;
    }

    inline int packOmniMode( uint8_t * bytes, uint8_t channel, bool on ){
        ASSERT( channel & ChannelMask == channel );

        bytes[0] = StatusControlChange | (channel & ChannelMask);
        bytes[1] = on ? ChannelModeControllerOmniModeOn : ChannelModeControllerOmniModeOff;
        bytes[2] = 0;

        return 3;
    }



    inline int packMonoMode( uint8_t * bytes, uint8_t channel, uint8_t numberOfChannels ){
        ASSERT( channel & ChannelMask == channel );

        bytes[0] = StatusControlChange | (channel & ChannelMask);
        bytes[1] = ChannelModeControllerMonoModeOn;
        bytes[2] = numberOfChannels;

        return 3;
    }


    inline bool isPolyModeOn( uint8_t status, uint8_t controller, uint8_t value ){
        return ((status & StatusMask) == StatusControlChange) && (controller == ChannelModeControllerPolyModeOn) && (value == 0);
    }

    inline bool isPolyModeOn( uint8_t * bytes ){
        return isPolyModeOn( bytes[0] , bytes[1], bytes[2] );
    }

    inline bool isPolyModeOn( Message_t * msg ){
        return isPolyModeOn( msg->Status, msg->Data.ControlChange.Controller, msg->Data.ControlChange.Value );
    }

    inline void packPolyModeOn( uint8_t * bytes, uint8_t channel){
        ASSERT( channel & ChannelMask == channel );

        bytes[0] = StatusControlChange | (channel & ChannelMask);
        bytes[1] = ChannelModeControllerPolyModeOn;
        bytes[2] = 0;
    }



    inline int packMIDITimeCodeQuarterFrame( uint8_t * bytes, uint8_t messageType, uint8_t values ){

        bytes[0] = StatusSystemExclusive | SystemExclusiveMIDITimeCodeQuarterFrame;
        bytes[1] = ((messageType & 0x05) << 4) | (values & 0x0f);

        return 2;
    }

    inline bool unpackMIDITimeCodeQuarterFrame( uint8_t * bytes, int len, uint8_t * messageType, uint8_t * values ){
        ASSERT( bytes[0] == StatusSystemExclusiveMIDITimeCodeQuarterFrame );

        if (len != 2){
            return false;
        }

        *messageType = (bytes[1] >> 4) & 0x05;
        *values = bytes[1] & 0x0f;

        return true;
    }

    inline int packSongPositionPointer (uint8_t * bytes, uint16_t position ){

        bytes[0] = StatusSystemExclusive | SystemExclusiveSongPositionPointer;
        packDoubleValue( &bytes[1], position );

        return 2;
    }

    inline bool unpackSongPositionPointer( uint8_t * bytes, int len, uint16_t * position ){
        ASSERT( bytes[0] == StatusSystemExclusiveSongPositionPointer );

        if (len != 3){
            return false;
        }

        unpackDoubleValue( &bytes[1], position );

        return true;
    }

    inline int packSongSelect( uint8_t * bytes, uint8_t song ){

        bytes[0] = StatusSystemExclusive | StatusSystemExclusiveSongSelect;
        bytes[1] = song & ValueMask;

        return 2;
    }

    inline bool unpackSongSelect( uint8_t * bytes, int len, uint8_t * song){
        ASSERT( bytes[0] == StatusSystemExclusiveSongSelect );

        if (len != 2){
            return false;
        }

        *song = bytes[1] & ValueMask;

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
     * NOTE: if the messaged parse is a SystemExclusive message (Status == StatusSystemExclusive)
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
     * Is safe to be called on any (correctly used) message, ie will only act if msg is has status == StatusSystemExclusive
     * and has a data length of greater than zero.
     *
     * @param   msg     midi message to be freed
     */
    void freeMessage( Message_t * msg );
#endif

}

#endif //MIDIMESSAGE_H
