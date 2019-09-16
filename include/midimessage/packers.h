/**
 * \file
 */

#ifndef MIDIMESSAGE_PACKERS_H
#define MIDIMESSAGE_PACKERS_H

#include "midimessage.h"

#ifdef __cplusplus
namespace MidiMessage {
    extern "C" {
#endif


///////////// Voice Channel Messages                /////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

    inline uint8_t packNoteOff(uint8_t *bytes, uint8_t channel, uint8_t key, uint8_t velocity) {
        ASSERT(bytes != NULL);
        ASSERT((channel & ChannelMask) == channel);
        ASSERT((key & DataMask) == key);
        ASSERT((velocity & DataMask) == velocity);

        bytes[0] = StatusClassNoteOff | (channel & ChannelMask);
        bytes[1] = key & DataMask;
        bytes[2] = velocity & DataMask;

        return MsgLenNoteOff;
    }

    inline uint8_t packNoteOffObj(uint8_t *bytes, Message_t *msg) {
        ASSERT(msg != NULL);

        return packNoteOff(bytes, msg->Channel, msg->Data.Note.Key, msg->Data.Note.Velocity);
    }

    inline bool unpackNoteOff(uint8_t *bytes, uint8_t len, uint8_t *channel, uint8_t *key, uint8_t *velocity) {
        ASSERT(bytes != NULL);
        ASSERT(channel != NULL);
        ASSERT(key != NULL);
        ASSERT(velocity != NULL);

        if (len != MsgLenNoteOff) {
            return false;
        }
        if ((bytes[0] & StatusClassMask) != StatusClassNoteOff) {
            return false;
        }
        if (((bytes[1] & DataMask) != bytes[1]) || ((bytes[2] & DataMask) != bytes[2])) {
            return false;
        }

        *channel = bytes[0] & ChannelMask;
        *key = bytes[1];
        *velocity = bytes[2];

        return true;
    }

    inline bool unpackNoteOffObj(uint8_t *bytes, uint8_t len, Message_t *msg) {
        ASSERT(msg != NULL);

        if (unpackNoteOff(bytes, len, &msg->Channel, &msg->Data.Note.Key, &msg->Data.Note.Velocity)) {
            msg->StatusClass = StatusClassNoteOff;
            msg->SystemMessage = 0;
            return true;
        }

        return false;
    }


    inline uint8_t packNoteOn(uint8_t *bytes, uint8_t channel, uint8_t key, uint8_t velocity) {
        ASSERT(bytes != NULL);
        ASSERT((channel & ChannelMask) == channel);
        ASSERT((key & DataMask) == key);
        ASSERT((velocity & DataMask) == velocity);

        bytes[0] = StatusClassNoteOn | (channel & ChannelMask);
        bytes[1] = key & DataMask;
        bytes[2] = velocity & DataMask;

        return MsgLenNoteOn;
    }

    inline uint8_t packNoteOnObj(uint8_t *bytes, Message_t *msg) {
        ASSERT(msg != NULL);

        return packNoteOn(bytes, msg->Channel, msg->Data.Note.Key, msg->Data.Note.Velocity);
    }

    inline bool unpackNoteOn(uint8_t *bytes, uint8_t len, uint8_t *channel, uint8_t *key, uint8_t *velocity) {
        ASSERT(bytes != NULL);
        ASSERT(channel != NULL);
        ASSERT(key != NULL);
        ASSERT(velocity != NULL);

        if (len != MsgLenNoteOn) {
            return false;
        }
        if ((bytes[0] & StatusClassMask) != StatusClassNoteOn) {
            return false;
        }
        if (((bytes[1] & DataMask) != bytes[1]) || ((bytes[2] & DataMask) != bytes[2])) {
            return false;
        }

        *channel = bytes[0] & ChannelMask;
        *key = bytes[1];
        *velocity = bytes[2];

        return true;
    }

    inline bool unpackNoteOnObj(uint8_t *bytes, uint8_t len, Message_t *msg) {
        ASSERT(msg != NULL);

        if (unpackNoteOn(bytes, len, &msg->Channel, &msg->Data.Note.Key, &msg->Data.Note.Velocity)) {
            msg->StatusClass = StatusClassNoteOn;
            msg->SystemMessage = 0;
            return true;
        }

        return false;
    }


    inline uint8_t packNote(uint8_t *bytes, bool on, uint8_t channel, uint8_t key, uint8_t velocity) {
        if (on) {
            return packNoteOn(bytes, channel, key, velocity);
        } else {
            return packNoteOff(bytes, channel, key, velocity);
        }
    }

    inline uint8_t packNoteObj(uint8_t *bytes, Message_t *msg) {
        ASSERT(msg != NULL);

        if (msg->StatusClass == StatusClassNoteOff) {
            return packNoteOffObj(bytes, msg);
        }
        if (msg->StatusClass == StatusClassNoteOn) {
            return packNoteOnObj(bytes, msg);
        }

        ASSERT(false);

        return 0;
    }

    inline bool unpackNote(uint8_t *bytes, uint8_t len, bool *on, uint8_t *channel, uint8_t *key, uint8_t *velocity) {
        if ((bytes[0] & StatusClassMask) == StatusClassNoteOn) {
            *on = true;
            return unpackNoteOn(bytes, len, channel, key, velocity);
        }
        if ((bytes[0] & StatusClassMask) == StatusClassNoteOff) {
            *on = true;
            return unpackNoteOn(bytes, len, channel, key, velocity);
        }
        return false;
    }

    inline bool unpackNoteObj(uint8_t *bytes, uint8_t len, Message_t *msg) {
        ASSERT(msg != NULL);

        if (unpackNoteOffObj(bytes, len, msg)) {
            return true;
        }

        if (unpackNoteOnObj(bytes, len, msg)) {
            return true;
        }

        return false;
    }


    inline uint8_t packPolyphonicKeyPressure(uint8_t *bytes, uint8_t channel, uint8_t key, uint8_t pressure) {
        ASSERT(bytes != NULL);
        ASSERT((channel & ChannelMask) == channel);
        ASSERT((key & DataMask) == key);
        ASSERT((pressure & DataMask) == pressure);

        bytes[0] = StatusClassPolyphonicKeyPressure | (channel & ChannelMask);
        bytes[1] = key & DataMask;
        bytes[2] = pressure & DataMask;

        return MsgLenPolyphonicKeyPressure;
    }

    inline uint8_t packPolyphonicKeyPressureObj(uint8_t *bytes, Message_t *msg) {
        ASSERT(msg != NULL);

        return packPolyphonicKeyPressure(bytes, msg->Channel, msg->Data.PolyphonicKeyPressure.Key,
                                         msg->Data.PolyphonicKeyPressure.Pressure);
    }

    inline bool unpackPolyphonicKeyPressure(uint8_t *bytes, uint8_t len, uint8_t *channel, uint8_t *key,
                                            uint8_t *pressure) {
        ASSERT(bytes != NULL);
        ASSERT(channel != NULL);
        ASSERT(key != NULL);
        ASSERT(pressure != NULL);

        if (len != MsgLenPolyphonicKeyPressure) {
            return false;
        }
        if ((bytes[0] & StatusClassMask) != StatusClassPolyphonicKeyPressure) {
            return false;
        }
        if (((bytes[1] & DataMask) != bytes[1]) || ((bytes[2] & DataMask) != bytes[2])) {
            return false;
        }

        *channel = bytes[0] & ChannelMask;
        *key = bytes[1];
        *pressure = bytes[2];

        return true;
    }

    inline bool unpackPolyphonicKeyPressureObj(uint8_t *bytes, uint8_t len, Message_t *msg) {
        ASSERT(msg != NULL);

        if (unpackPolyphonicKeyPressure(bytes, len, &msg->Channel, &msg->Data.PolyphonicKeyPressure.Key,
                                        &msg->Data.PolyphonicKeyPressure.Pressure)) {
            msg->StatusClass = StatusClassPolyphonicKeyPressure;
            msg->SystemMessage = 0;
            return true;
        }

        return false;
    }


    inline uint8_t packControlChange(uint8_t *bytes, uint8_t channel, uint8_t controller, uint8_t value) {
        ASSERT(bytes != NULL);
        ASSERT((channel & ChannelMask) == channel);
        ASSERT((controller & DataMask) == controller);
        ASSERT(value <= MaxU7);

        bytes[0] = StatusClassControlChange | (channel & ChannelMask);
        bytes[1] = controller & DataMask;
        bytes[2] = value & DataMask;

        return MsgLenControlChange;
    }

    inline uint8_t packControlChangeObj(uint8_t *bytes, Message_t *msg) {
        ASSERT(msg != NULL);

        return packControlChange(bytes, msg->Channel, msg->Data.ControlChange.Controller,
                                 msg->Data.ControlChange.Value);
    }

    inline bool unpackControlChange(uint8_t *bytes, uint8_t len, uint8_t *channel, uint8_t *controller,
                                    uint8_t *value) {
        ASSERT(bytes != NULL);
        ASSERT(channel != NULL);
        ASSERT(controller != NULL);
        ASSERT(value != NULL);

        if (len != MsgLenControlChange) {
            return false;
        }
        if ((bytes[0] & StatusClassMask) != StatusClassControlChange) {
            return false;
        }
        if (((bytes[1] & DataMask) != bytes[1]) || ((bytes[2] & DataMask) != bytes[2])) {
            return false;
        }

        *channel = bytes[0] & ChannelMask;
        *controller = bytes[1];
        *value = bytes[2];

        return true;
    }

    inline bool unpackControlChangeObj(uint8_t *bytes, uint8_t len, Message_t *msg) {
        ASSERT(msg != NULL);

        if (unpackControlChange(bytes, len, &msg->Channel, &msg->Data.ControlChange.Controller,
                                &msg->Data.ControlChange.Value)) {
            msg->StatusClass = StatusClassControlChange;
            msg->SystemMessage = 0;
            return true;
        }

        return false;
    }


    inline uint8_t packProgramChange(uint8_t *bytes, uint8_t channel, uint8_t program) {
        ASSERT(bytes != NULL);
        ASSERT((channel & ChannelMask) == channel);
        ASSERT((program & DataMask) == program);

        bytes[0] = StatusClassProgramChange | (channel & ChannelMask);
        bytes[1] = program & DataMask;

        return MsgLenProgramChange;
    }


    inline uint8_t packProgramChangeObj(uint8_t *bytes, Message_t *msg) {
        ASSERT(msg != NULL);

        return packProgramChange(bytes, msg->Channel, msg->Data.ProgramChange.Program);
    }


    inline bool unpackProgramChange(uint8_t *bytes, uint8_t len, uint8_t *channel, uint8_t *program) {
        ASSERT(bytes != NULL);
        ASSERT(channel != NULL);
        ASSERT(program != NULL);

        if (len != MsgLenProgramChange) {
            return false;
        }
        if ((bytes[0] & StatusClassMask) != StatusClassProgramChange) {
            return false;
        }
        if ((bytes[1] & DataMask) != bytes[1]) {
            return false;
        }

        *channel = bytes[0] & ChannelMask;
        *program = bytes[1];

        return true;
    }

    inline bool unpackProgramChangeObj(uint8_t *bytes, uint8_t len, Message_t *msg) {
        ASSERT(msg != NULL);

        if (unpackProgramChange(bytes, len, &msg->Channel, &msg->Data.ProgramChange.Program)) {
            msg->StatusClass = StatusClassProgramChange;
            msg->SystemMessage = 0;
            return true;
        }

        return false;
    }


    inline uint8_t packChannelPressure(uint8_t *bytes, uint8_t channel, uint8_t pressure) {
        ASSERT(bytes != NULL);
        ASSERT((channel & ChannelMask) == channel);

        bytes[0] = StatusClassChannelPressure | (channel & ChannelMask);
        bytes[1] = pressure & DataMask;

        return MsgLenChannelPressure;
    }

    inline uint8_t packChannelPressureObj(uint8_t *bytes, Message_t *msg) {
        ASSERT(msg != NULL);

        return packChannelPressure(bytes, msg->Channel, msg->Data.ChannelPressure.Pressure);
    }

    inline bool unpackChannelPressure(uint8_t *bytes, uint8_t len, uint8_t *channel, uint8_t *pressure) {
        ASSERT(bytes != NULL);
        ASSERT(channel != NULL);
        ASSERT(pressure != NULL);

        if (len != MsgLenChannelPressure) {
            return false;
        }
        if ((bytes[0] & StatusClassMask) != StatusClassChannelPressure) {
            return false;
        }
        if ((bytes[1] & DataMask) != bytes[1]) {
            return false;
        }

        *channel = bytes[0] & ChannelMask;
        *pressure = bytes[1];

        return true;
    }

    inline bool unpackChannelPressureObj(uint8_t *bytes, uint8_t len, Message_t *msg) {
        ASSERT(msg != NULL);

        if (unpackChannelPressure(bytes, len, &msg->Channel, &msg->Data.ChannelPressure.Pressure)) {
            msg->StatusClass = StatusClassChannelPressure;
            msg->SystemMessage = 0;
            return true;
        }

        return false;
    }


    inline uint8_t packPitchBendChange(uint8_t *bytes, uint8_t channel, uint16_t pitch) {
        ASSERT(bytes != NULL);
        ASSERT((channel & ChannelMask) == channel);
        ASSERT(pitch <= MaxU14);

        bytes[0] = StatusClassPitchBendChange | (channel & ChannelMask);
        packU14(&bytes[1], pitch);

        return MsgLenPitchBendChange;
    }

    inline uint8_t packPitchBendChangeObj(uint8_t *bytes, Message_t *msg) {
        ASSERT(msg != NULL);

        return packPitchBendChange(bytes, msg->Channel, msg->Data.PitchBendChange.Pitch);
    }

    inline bool unpackPitchBendChange(uint8_t *bytes, uint8_t len, uint8_t *channel, uint16_t *pitch) {
        ASSERT(bytes != NULL);
        ASSERT(channel != NULL);
        ASSERT(pitch != NULL);

        if (len != MsgLenPitchBendChange) {
            return false;
        }
        if ((bytes[0] & StatusClassMask) != StatusClassPitchBendChange) {
            return false;
        }
        if (((bytes[1] & DataMask) != bytes[1]) || ((bytes[2] & DataMask) != bytes[2])) {
            return false;
        }

        *channel = bytes[0] & ChannelMask;
        *pitch = unpackU14(&bytes[1]);

        return true;
    }

    inline bool unpackPitchBendChangeObj(uint8_t *bytes, uint8_t len, Message_t *msg) {
        ASSERT(msg != NULL);

        if (unpackPitchBendChange(bytes, len, &msg->Channel, &msg->Data.PitchBendChange.Pitch)) {
            msg->StatusClass = StatusClassPitchBendChange;
            msg->SystemMessage = 0;
            return true;
        }

        return false;
    }




///////////// Channel Mode Messages (ie special CC) /////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////


    inline uint8_t packAllSoundOff(uint8_t *bytes, uint8_t channel) {
        ASSERT(bytes != NULL);
        ASSERT((channel & ChannelMask) == channel);

        bytes[0] = StatusClassControlChange | (channel & ChannelMask);
        bytes[1] = ChannelModeControllerAllSoundOff;
        bytes[2] = 0;

        return MsgLenControlChange;
    }

    inline bool isAllSoundOff(uint8_t status, uint8_t controller, uint8_t value) {
        return ((status & StatusClassMask) == StatusClassControlChange) &&
               (controller == ChannelModeControllerAllSoundOff) && (value == 0);
    }

    inline bool isAllSoundOffArray(uint8_t *bytes) {
        ASSERT(bytes != NULL);

        return isAllSoundOff(bytes[0], bytes[1], bytes[2]);
    }

    inline bool isAllSoundOffObj(Message_t *msg) {
        ASSERT(msg != NULL);

        return isAllSoundOff(msg->StatusClass, msg->Data.ControlChange.Controller, msg->Data.ControlChange.Value);
    }


    inline uint8_t packResetAllControllers(uint8_t *bytes, uint8_t channel) {
        ASSERT(bytes != NULL);
        ASSERT((channel & ChannelMask) == channel);

        bytes[0] = StatusClassControlChange | (channel & ChannelMask);
        bytes[1] = ChannelModeControllerResetAllControllers;
        bytes[2] = 0;

        return MsgLenControlChange;
    }

    inline bool isResetAllControllers(uint8_t status, uint8_t controller, uint8_t value) {
        return ((status & StatusClassMask) == StatusClassControlChange) &&
               (controller == ChannelModeControllerResetAllControllers) && (value == 0);
    }

    inline bool isResetAllControllersArray(uint8_t *bytes) {
        ASSERT(bytes != NULL);

        return isResetAllControllers(bytes[0], bytes[1], bytes[2]);
    }

    inline bool isResetAllControllersObj(Message_t *msg) {
        ASSERT(msg != NULL);

        return isResetAllControllers(msg->StatusClass, msg->Data.ControlChange.Controller,
                                     msg->Data.ControlChange.Value);
    }


    inline uint8_t packLocalControl(uint8_t *bytes, uint8_t channel, bool on) {
        ASSERT(bytes != NULL);
        ASSERT((channel & ChannelMask) == channel);

        bytes[0] = StatusClassControlChange | (channel & ChannelMask);
        bytes[1] = ChannelModeControllerLocalControl;
        bytes[2] = on ? 127 : 0;

        return MsgLenControlChange;
    }

    inline bool isLocalControl(uint8_t status, uint8_t controller) {
        return ((status & StatusClassMask) == StatusClassControlChange) &&
               (controller == ChannelModeControllerLocalControl);
    }

    inline bool isLocalControlArray(uint8_t *bytes) {
        ASSERT(bytes != NULL);

        return isLocalControl(bytes[0], bytes[1]);
    }

    inline bool isLocalControlObj(Message_t *msg) {
        ASSERT(msg != NULL);

        return isLocalControl(msg->StatusClass, msg->Data.ControlChange.Controller);
    }

    inline bool getLocalControlState(uint8_t *bytes) {
        ASSERT(bytes != NULL);
        ASSERT(isLocalControlArray(bytes));

        return (bytes[3] & DataMask) > 0;
    }

    inline bool getLocalControlStateObj(Message_t *msg) {
        ASSERT(msg != NULL);
        ASSERT(isLocalControlObj(msg));

        return msg->Data.ControlChange.Value > 0;
    }


    inline uint8_t packAllNotesOff(uint8_t *bytes, uint8_t channel) {
        ASSERT(bytes != NULL);
        ASSERT((channel & ChannelMask) == channel);

        bytes[0] = StatusClassControlChange | (channel & ChannelMask);
        bytes[1] = ChannelModeControllerAllNotesOff;
        bytes[2] = 0;

        return MsgLenControlChange;
    }

    inline bool isAllNotesOff(uint8_t status, uint8_t controller, uint8_t value) {
        return ((status & StatusClassMask) == StatusClassControlChange) &&
               (controller == ChannelModeControllerAllNotesOff) && (value == 0);
    }

    inline bool isAllNotesOffArray(uint8_t *bytes) {
        ASSERT(bytes != NULL);

        return isAllNotesOff(bytes[0], bytes[1], bytes[2]);
    }

    inline bool isAllNotesOffObj(Message_t *msg) {
        ASSERT(msg != NULL);

        return isAllNotesOff(msg->StatusClass, msg->Data.ControlChange.Controller, msg->Data.ControlChange.Value);
    }


    inline uint8_t packOmniMode(uint8_t *bytes, uint8_t channel, bool on) {
        ASSERT(bytes != NULL);
        ASSERT((channel & ChannelMask) == channel);

        bytes[0] = StatusClassControlChange | (channel & ChannelMask);
        bytes[1] = on ? ChannelModeControllerOmniModeOn : ChannelModeControllerOmniModeOff;
        bytes[2] = 0;

        return MsgLenControlChange;
    }

    inline bool isOmniMode(uint8_t status, uint8_t controller) {
        return ((status & StatusClassMask) == StatusClassControlChange) &&
               (controller == ChannelModeControllerOmniModeOn || controller == ChannelModeControllerOmniModeOff);
    }

    inline bool isOmniModeArray(uint8_t *bytes) {
        ASSERT(bytes != NULL);

        return isOmniMode(bytes[0], bytes[1]);
    }

    inline bool isOmniModeObj(Message_t *msg) {
        ASSERT(msg != NULL);

        return isOmniMode(msg->StatusClass, msg->Data.ControlChange.Controller);
    }

    inline bool getOmniModeStateArray(uint8_t *bytes) {
        ASSERT(bytes != NULL);
        ASSERT(isOmniModeArray(bytes));

        return bytes[3] == ChannelModeControllerOmniModeOn;
    }

    inline bool getOmniModeStateObj(Message_t *msg) {
        ASSERT(msg != NULL);
        ASSERT(isOmniModeObj(msg));

        return msg->Data.ControlChange.Value == ChannelModeControllerOmniModeOn;
    }


    inline uint8_t packMonoMode(uint8_t *bytes, uint8_t channel, uint8_t numberOfChannels) {
        ASSERT(bytes != NULL);
        ASSERT((channel & ChannelMask) == channel);

        bytes[0] = StatusClassControlChange | (channel & ChannelMask);
        bytes[1] = ChannelModeControllerMonoModeOn;
        bytes[2] = numberOfChannels;

        return MsgLenControlChange;
    }

    inline bool isMonoMode(uint8_t status, uint8_t controller) {
        return ((status & StatusClassMask) == StatusClassControlChange) &&
               (controller == ChannelModeControllerMonoModeOn);
    }

    inline bool isMonoModeArray(uint8_t *bytes) {
        ASSERT(bytes != NULL);

        return isMonoMode(bytes[0], bytes[1]);
    }

    inline bool isMonoModeObj(Message_t *msg) {
        ASSERT(msg != NULL);

        return isMonoMode(msg->StatusClass, msg->Data.ControlChange.Controller);
    }

    inline uint8_t getMonoModeNumberOfChannels(uint8_t *bytes) {
        ASSERT(bytes != NULL);
        ASSERT(isMonoMode(bytes[0], bytes[1]));

        return bytes[2] & DataMask;
    }

    inline uint8_t getMonoModeNumberOfChannelsObj(Message_t *msg) {
        ASSERT(msg != NULL);
        ASSERT(isMonoModeObj(msg));

        return msg->Data.ControlChange.Value;
    }


    inline uint8_t packPolyModeOn(uint8_t *bytes, uint8_t channel) {
        ASSERT(bytes != NULL);
        ASSERT((channel & ChannelMask) == channel);

        bytes[0] = StatusClassControlChange | (channel & ChannelMask);
        bytes[1] = ChannelModeControllerPolyModeOn;
        bytes[2] = 0;

        return MsgLenControlChange;
    }

    inline bool isPolyModeOn(uint8_t status, uint8_t controller, uint8_t value) {
        return ((status & StatusClassMask) == StatusClassControlChange) &&
               (controller == ChannelModeControllerPolyModeOn) && (value == 0);
    }

    inline bool isPolyModeOnArray(uint8_t *bytes) {
        ASSERT(bytes != NULL);

        return isPolyModeOn(bytes[0], bytes[1], bytes[2]);
    }

    inline bool isPolyModeOnObj(Message_t *msg) {
        ASSERT(msg != NULL);

        return isPolyModeOn(msg->SystemMessage, msg->Data.ControlChange.Controller, msg->Data.ControlChange.Value);
    }



///////////// System Common Messages                /////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////


    inline uint8_t packMtcQuarterFrame(uint8_t *bytes, uint8_t messageType, uint8_t nibble) {
        ASSERT(bytes != NULL);

        bytes[0] = SystemMessageMtcQuarterFrame;
        bytes[1] = ((messageType & 0x05) << 4) | (nibble & 0x0f);

        return MsgLenMtcQuarterFrame;
    }

    inline uint8_t packMtcQuarterFrameObj(uint8_t *bytes, Message_t *msg) {
        ASSERT(msg != NULL);

        return (packMtcQuarterFrame(bytes, msg->Data.MtcQuarterFrame.MessageType, msg->Data.MtcQuarterFrame.Nibble));
    }

    inline bool unpackMtcQuarterFrame(uint8_t *bytes, uint8_t len, uint8_t *messageType, uint8_t *nibble) {
        ASSERT(bytes != NULL);
        ASSERT(messageType != NULL);
        ASSERT(nibble != NULL);

        if (len != MsgLenMtcQuarterFrame) {
            return false;
        }
        if (bytes[0] != SystemMessageMtcQuarterFrame) {
            return false;
        }
        if ((bytes[1] & DataMask) != bytes[1]) {
            return false;
        }

        *messageType = (bytes[1] >> 4) & 0x05;
        *nibble = bytes[1] & NibbleMask;

        return true;
    }

    inline bool unpackMtcQuarterFrameObj(uint8_t *bytes, uint8_t len, Message_t *msg) {
        ASSERT(msg != NULL);

        if (unpackMtcQuarterFrame(bytes, len, &msg->Data.MtcQuarterFrame.MessageType,
                                  &msg->Data.MtcQuarterFrame.Nibble)) {
            msg->StatusClass = StatusClassSystemMessage;
            msg->SystemMessage = SystemMessageMtcQuarterFrame;
            return true;
        }

        return false;
    }


    inline uint8_t packSongPositionPointer(uint8_t *bytes, uint16_t position) {
        ASSERT(bytes != NULL);
        ASSERT(position <= MaxU14);

        bytes[0] = SystemMessageSongPositionPointer;
        packU14(&bytes[1], position);

        return MsgLenSongPositionPointer;
    }

    inline uint8_t packSongPositionPointerObj(uint8_t *bytes, Message_t *msg) {
        ASSERT(msg != NULL);

        return packSongPositionPointer(bytes, msg->Data.SongPositionPointer.Position);
    }

    inline bool unpackSongPositionPointer(uint8_t *bytes, uint8_t len, uint16_t *position) {
        ASSERT(bytes != NULL);
        ASSERT(position != NULL);

        if (len != MsgLenSongPositionPointer) {
            return false;
        }
        if (bytes[0] != SystemMessageSongPositionPointer) {
            return false;
        }

        *position = unpackU14(&bytes[1]);

        return true;
    }

    inline bool unpackSongPositionPointerObj(uint8_t *bytes, uint8_t len, Message_t *msg) {
        ASSERT(msg != NULL);

        if (unpackSongPositionPointer(bytes, len, &msg->Data.SongPositionPointer.Position)) {
            msg->StatusClass = StatusClassSystemMessage;
            msg->SystemMessage = SystemMessageSongPositionPointer;
            return true;
        }

        return false;
    }

    inline uint8_t packSongSelect(uint8_t *bytes, uint8_t song) {
        ASSERT(bytes != NULL);
        ASSERT((song & DataMask) == song);

        bytes[0] = SystemMessageSystemExclusive | SystemMessageSongSelect;
        bytes[1] = song & DataMask;

        return MsgLenSongSelect;
    }

    inline uint8_t packSongSelectObj(uint8_t *bytes, Message_t *msg) {
        ASSERT(msg != NULL);

        return packSongSelect(bytes, msg->Data.SongSelect.Song);
    }

    inline bool unpackSongSelect(uint8_t *bytes, uint8_t len, uint8_t *song) {
        ASSERT(bytes != NULL);
        ASSERT(song != NULL);

        if (len != MsgLenSongSelect) {
            return false;
        }
        if (bytes[0] != SystemMessageSongSelect) {
            return false;
        }

        *song = bytes[1] & DataMask;

        return true;
    }

    inline bool unpackSongSelectObj(uint8_t *bytes, uint8_t len, Message_t *msg) {
        ASSERT(msg != NULL);

        if (unpackSongSelect(bytes, len, &msg->Data.SongSelect.Song)) {
            msg->StatusClass = StatusClassSystemMessage;
            msg->SystemMessage = SystemMessageSongSelect;
            return true;
        }

        return false;
    }

    inline uint8_t packSystemMessage( uint8_t *bytes, uint8_t systemMessage ){
        ASSERT( bytes != NULL );
        ASSERT( isSystemMessage(systemMessage) && systemMessage != SystemMessageSystemExclusive );

        bytes[0] = systemMessage;

        return MsgLenSystemMessage;
    }

    inline uint8_t packSystemMessageObj( uint8_t *bytes, Message_t * msg){
        ASSERT( msg != NULL);
        ASSERT(msg->StatusClass == StatusClassSystemMessage);

        return packSystemMessage(bytes, msg->SystemMessage);
    }

    inline bool unpackSystemMessage( uint8_t *bytes, uint8_t length, uint8_t *systemMessage){
        ASSERT(bytes != NULL);

        if (length != MsgLenSystemMessage || bytes[0] == SystemMessageSystemExclusive || !isSystemMessage(bytes[0])){
            return false;
        }

        *systemMessage = bytes[0];

        return true;
    }

    inline bool unpackSystemMessageObj( uint8_t *bytes, uint8_t length, Message_t *msg){
        ASSERT( msg != NULL);

        if (unpackSystemMessage(bytes, length, &msg->SystemMessage)){
            msg->StatusClass = StatusClassSystemMessage;
            return true;
        }

        return false;
    }

///////////// SysEx: (General) Handshakes           /////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////


    inline uint8_t packSysExNonRtHandshake(uint8_t *bytes, uint8_t deviceId, uint8_t subId1, uint8_t packetNumber){
        ASSERT( bytes != NULL );
        ASSERT( deviceId <= MaxU7);
        ASSERT( isSysExNonRtHandshake(subId1) );
        ASSERT( packetNumber <= MaxU7);

        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = SysExIdNonRealTimeByte;
        bytes[2] = deviceId;
        bytes[3] = subId1;
        bytes[4] = packetNumber;
        bytes[5] = SystemMessageEndOfExclusive;

        return MsgLenSysExNonRtGeneralHandshaking;
    }

    inline uint8_t packSysExNonRtHandshakeObj(uint8_t *bytes, Message_t *msg){
        ASSERT( msg != NULL );
        ASSERT( msg->StatusClass == StatusClassSystemMessage );
        ASSERT( msg->SystemMessage == SystemMessageSystemExclusive );
        ASSERT( msg->Data.SysEx.Id == SysExIdNonRealTime);

        return packSysExNonRtHandshake(bytes, msg->Channel, msg->Data.SysEx.SubId1, msg->Data.SysEx.Data.PacketNumber );
    }

    inline bool unpackSysExNonRtHandshake(uint8_t *bytes, uint8_t length, uint8_t *deviceId, uint8_t *subId1, uint8_t *packetNumber){
        ASSERT(bytes != NULL);
        ASSERT( deviceId != NULL);
        ASSERT( subId1 != NULL);
        ASSERT( packetNumber != NULL);

        if (length != MsgLenSysExNonRtGeneralHandshaking){
            return false;
        }
        if (bytes[0] != SystemMessageSystemExclusive || bytes[1] != SysExIdNonRealTimeByte || !isSysExNonRtHandshake(bytes[3]) ){
            return false;
        }

        *deviceId = bytes[2];
        *subId1 = bytes[3];
        *packetNumber = bytes[3];

        return true;
    }

    inline bool unpackSysExNonRtHandshakeObj(uint8_t *bytes, uint8_t length, Message_t *msg){
        ASSERT(msg != NULL);

        if (unpackSysExNonRtHandshake(bytes, length, &msg->Channel, &msg->Data.SysEx.SubId1, &msg->Data.SysEx.Data.PacketNumber)){
            msg->StatusClass = StatusClassSystemMessage;
            msg->SystemMessage = SystemMessageSystemExclusive;
            msg->Data.SysEx.Id = SysExIdNonRealTime;
            return true;
        }

        return false;
    }

///////////// SysEx: Experimental Messages          /////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

    inline uint8_t packSysExExperimentalMessage(uint8_t *bytes, uint8_t *data, uint8_t dataLen) {
        ASSERT(bytes != NULL);
        ASSERT(dataLen == 0 || data != NULL);

        uint8_t len = 2;

        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = SysExIdExperimentalByte;

        for (uint8_t i = 0; i < dataLen; i++) {
            bytes[len++] = data[i];
        }

        bytes[len++] = SystemMessageEndOfExclusive;

        return len;
    }

    inline uint8_t packSysExExperimentalMessageObj(uint8_t *bytes, Message_t *msg) {
        ASSERT(msg != NULL);

        return packSysExExperimentalMessage(bytes, msg->Data.SysEx.ByteData, msg->Data.SysEx.Length);
    }

    inline bool unpackSysExExperimentalMessage(uint8_t *bytes, uint8_t len, uint8_t *data, uint8_t *dataLen) {
        ASSERT(bytes != NULL);
        ASSERT(len > 2 || data != NULL);
        ASSERT(dataLen != NULL);

        if (len < 3 || !isControlByte(bytes[len-1]) ) {
            return false;
        }
//        ASSERT(false);
        if (bytes[0] != SystemMessageSystemExclusive) {
            return false;
        }
        if (bytes[1] != SysExIdExperimentalByte) {
            return false;
        }

//        if (bytes[len - 1] == SystemMessageEndOfExclusive) {
            len--;
//        }

        if (len <= 2) {
            *dataLen = 0;
        } else {
            for (uint8_t i = 2, j = 0; i < len; i++, j++) {
                data[j] = bytes[i];
            }
            *dataLen = len - 2;
        }

        return true;
    }

    inline bool unpackSysExExperimentalMessageObj(uint8_t *bytes, uint8_t len, Message_t *msg) {
        ASSERT(msg != NULL);

        if (unpackSysExExperimentalMessage(bytes, len, msg->Data.SysEx.ByteData, &msg->Data.SysEx.Length)) {
            msg->StatusClass = StatusClassSystemMessage;
            msg->SystemMessage = SystemMessageSystemExclusive;
            msg->Data.SysEx.Id = SysExIdExperimental;
            return true;
        }

        return false;
    }

///////////// SysEx: Manufacturer Messages          /////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

    inline uint8_t packSysExManufacturerMessage(uint8_t *bytes, uint32_t manufacturerId, uint8_t *data,
                                                uint8_t dataLen) {
        ASSERT(bytes != NULL);
        ASSERT(isSysExManufacturerId(manufacturerId));
        ASSERT(dataLen == 0 || data != NULL);

        uint8_t len = 1;

        bytes[0] = SystemMessageSystemExclusive;
        len += packSysExId(&bytes[1], manufacturerId);

        for (uint8_t i = 0; i < dataLen; i++) {
            bytes[len++] = data[i];
        }

        bytes[len++] = SystemMessageEndOfExclusive;

        return len;
    }

    inline uint8_t packSysExManufacturerMessageObj( uint8_t *bytes, Message_t * msg ){
        ASSERT( msg != NULL );

        return packSysExManufacturerMessage( bytes, msg->Data.SysEx.Id, msg->Data.SysEx.ByteData, msg->Data.SysEx.Length );
    }


    inline bool unpackSysExManufacturerMessage(uint8_t *bytes, uint8_t len, uint32_t *manufacturerId, uint8_t *data,
                                               uint8_t *dataLen) {
        ASSERT(bytes != NULL);
        ASSERT(manufacturerId != NULL);
        ASSERT(data != NULL);
        ASSERT(dataLen != NULL);

        uint8_t p = 1;

        if ((len < 2 && bytes[1] != SysExIdManufacturerExtensionByte) ||
            (len < 4 && bytes[1] == SysExIdManufacturerExtensionByte) ||
                ! isControlByte(bytes[len-1])) {
            return false;
        }
        if (bytes[0] != SystemMessageSystemExclusive) {
            return false;
        }
        if (!isSysExManufacturerIdByte(bytes[1])) {
            return false;
        }

        p += unpackSysExId(&bytes[1], manufacturerId);

//        if (bytes[len - 1] == SystemMessageEndOfExclusive) {
            len--;
//        }

        if (len <= p) {
            *dataLen = 0;
        } else {
            for (uint8_t i = p, j = 0; i < len; i++, j++) {
                data[j] = bytes[i];
            }
            *dataLen = len - p;
        }

        return true;
    }

    inline bool unpackSysExManufacturerMessageObj( uint8_t *bytes, uint8_t length, Message_t * msg ){
        ASSERT( msg != NULL );

        if ( unpackSysExManufacturerMessage(bytes, length, &msg->Data.SysEx.Id, msg->Data.SysEx.ByteData, &msg->Data.SysEx.Length) ){
            msg->StatusClass = StatusClassSystemMessage;
            msg->SystemMessage = SystemMessageSystemExclusive;
            return true;
        }

        return false;
    }

///////////// SysEx: MIDI Time Code + Cueing         ////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////


    inline uint8_t packSysExRtMtcFullMessage(uint8_t *bytes, uint8_t deviceId, uint8_t fps, uint8_t hour, uint8_t minute,
                                           uint8_t second, uint8_t frame) {
        ASSERT(bytes != NULL);
        ASSERT(deviceId <= MaxU7);
        ASSERT(isMtcFrameRate(fps));
        ASSERT(hour <= MtcMaxHour);
        ASSERT(minute <= MtcMaxMinute);
        ASSERT(second <= MtcMaxSecond);
        ASSERT(frame <= MtcMaxFps[fps]);

        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = SysExIdRealTimeByte;
        bytes[2] = deviceId & DataMask;
        bytes[3] = SysExRtMidiTimeCode;
        bytes[4] = SysExRtMtcFullMessage;
        bytes[5] = ((fps << MtcFpsOffset) & MtcFpsMask) | (hour & MtcHourMask);
        bytes[6] = minute & MtcMinuteMask;
        bytes[7] = second & MtcSecondMask;
        bytes[8] = frame & MtcFrameMask;
        bytes[9] = SystemMessageEndOfExclusive;

        return MsgLenSysExRtMtcFullMessage;
    }

    inline uint8_t packSysExRtMtcFullMessageObjMtc(uint8_t *bytes, uint8_t deviceId, MidiTimeCode_t *mtc) {
        ASSERT(bytes != NULL);
        ASSERT(mtc != NULL);

        return packSysExRtMtcFullMessage(bytes, deviceId, mtc->Fps, mtc->Hour, mtc->Minute,
                                       mtc->Second, mtc->Frame);
    }

    inline uint8_t packSysExRtMtcFullMessageObj(uint8_t *bytes, Message_t *msg) {
        ASSERT(msg != NULL);

        return packSysExRtMtcFullMessageObjMtc(bytes, msg->Channel, &msg->Data.SysEx.Data.MidiTimeCode);
    }

    inline bool unpackSysExRtMtcFullMessage(uint8_t *bytes, uint8_t len, uint8_t *deviceId, uint8_t *fps, uint8_t *hour,
                                          uint8_t *minute, uint8_t *second, uint8_t *frame) {
        ASSERT(bytes != NULL);
        ASSERT(deviceId != NULL);
        ASSERT(fps != NULL);
        ASSERT(hour != NULL);
        ASSERT(minute != NULL);
        ASSERT(second != NULL);
        ASSERT(frame != NULL);

        if ((len != MsgLenSysExRtMtcFullMessage - 1) && (len != MsgLenSysExRtMtcFullMessage ||
                                                         bytes[MsgLenSysExRtMtcFullMessage - 1] !=
                                                         SystemMessageEndOfExclusive)) {
            return false;
        }
        if (bytes[0] != SystemMessageSystemExclusive) {
            return false;
        }
        if (bytes[1] != SysExIdRealTimeByte) {
            return false;
        }
        if ((bytes[2] & DataMask) != bytes[2]) {
            return false;
        }
        if (bytes[3] != SysExRtMidiTimeCode) {
            return false;
        }
        if (bytes[4] != SysExRtMtcFullMessage) {
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

    inline bool unpackSysExRtMtcFullMessageObjMtc(uint8_t *bytes, uint8_t len, uint8_t *deviceId, MidiTimeCode_t *mtc) {
        ASSERT(bytes != NULL);
        ASSERT(deviceId != NULL);
        ASSERT(mtc != NULL);

        if (unpackSysExRtMtcFullMessage(bytes, len, deviceId, &mtc->Fps, &mtc->Hour, &mtc->Minute,
                                       &mtc->Second, &mtc->Frame)) {
            mtc->FractionalFrame = 0;
            return true;
        }

        return false;
    }

    inline bool unpackSysExRtMtcFullMessageObj(uint8_t *bytes, uint8_t len, Message_t *msg) {
        ASSERT(msg != NULL);

        if (unpackSysExRtMtcFullMessageObjMtc(bytes, len, &msg->Channel, &msg->Data.SysEx.Data.MidiTimeCode)) {
            msg->StatusClass = StatusClassSystemMessage;
            msg->SystemMessage = SystemMessageSystemExclusive;
            msg->Data.SysEx.Id = SysExIdRealTime;
            msg->Data.SysEx.SubId1 = SysExRtMidiTimeCode;
            msg->Data.SysEx.SubId2 = SysExRtMtcFullMessage;
            return true;
        }

        return false;
    }

    inline uint8_t packSysExRtMtcUserBits(uint8_t *bytes, uint8_t deviceId, uint8_t *userBits) {
        ASSERT(bytes != NULL);
        ASSERT(deviceId <= MaxU7);
        ASSERT(userBits != NULL);

        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = SysExIdRealTimeByte;
        bytes[2] = deviceId & DataMask;
        bytes[3] = SysExRtMidiTimeCode;
        bytes[4] = SysExRtMtcUserBits;
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

    inline uint8_t packSysExRtMtcUserBitsObj(uint8_t *bytes, Message_t *msg) {
        ASSERT(msg != NULL);

        return packSysExRtMtcUserBits(bytes, msg->Channel, msg->Data.SysEx.ByteData);
    }

    inline bool unpackSysExRtMtcUserBits(uint8_t *bytes, uint8_t len, uint8_t *deviceId, uint8_t *userBits) {
        ASSERT(bytes != NULL);
        ASSERT(deviceId != NULL);
        ASSERT(userBits != NULL);

        if ( len != MsgLenSysExRtMtcUserBits && !isControlByte(bytes[len-1]) ) {
            return false;
        }
        if (bytes[0] != SystemMessageSystemExclusive) {
            return false;
        }
        if (bytes[1] != SysExIdRealTimeByte) {
            return false;
        }
        if ((bytes[2] & DataMask) != bytes[2]) {
            return false;
        }
        if (bytes[3] != SysExRtMidiTimeCode) {
            return false;
        }
        if (bytes[4] != SysExRtMtcUserBits) {
            return false;
        }

        *deviceId = bytes[2] & DataMask;
        userBits[3] = ((bytes[6] & NibbleMask) << 4) | (bytes[5] & NibbleMask);
        userBits[2] = ((bytes[8] & NibbleMask) << 4) | (bytes[7] & NibbleMask);
        userBits[1] = ((bytes[10] & NibbleMask) << 4) | (bytes[9] & NibbleMask);
        userBits[0] = ((bytes[12] & NibbleMask) << 4) | (bytes[11] & NibbleMask);
        userBits[4] = bytes[13] & 0b00000011; //TODO is this right??

        return true;
    }

    inline bool unpackSysExRtMtcUserBitsObj(uint8_t *bytes, uint8_t len, Message_t *msg) {
        ASSERT(msg != NULL);

        if (unpackSysExRtMtcUserBits(bytes, len, &msg->Channel, msg->Data.SysEx.ByteData)) {
            msg->StatusClass = StatusClassSystemMessage;
            msg->SystemMessage = SystemMessageSystemExclusive;
            msg->Data.SysEx.Id = SysExIdRealTime;
            msg->Data.SysEx.SubId1 = SysExRtMidiTimeCode;
            msg->Data.SysEx.SubId2 = SysExRtMtcUserBits;
            msg->Data.SysEx.Length = 5;
            return true;
        }

        return false;
    }

    inline uint8_t packSysExRtMidiTimeCodeObj( uint8_t *bytes, Message_t *msg){
        ASSERT( msg != NULL );


        switch (msg->Data.SysEx.SubId2) {

            case SysExRtMtcFullMessage:
                return packSysExRtMtcFullMessageObj(bytes, msg);

            case SysExRtMtcUserBits:
                return packSysExRtMtcUserBitsObj(bytes, msg);

        }

        ASSERT( false ); // should never reach here

        return 0;
    }

    inline bool unpackSysExRtMidiTimeCodeObj(uint8_t * bytes, uint8_t length, Message_t *msg){
        ASSERT( msg != NULL );

        if (length < 5) {
            return false;
        }

        switch (bytes[4]) {
            case SysExRtMtcFullMessage:
                return unpackSysExRtMtcFullMessageObj( bytes, length, msg );

            case SysExRtMtcUserBits:
                return unpackSysExRtMtcUserBitsObj(bytes, length, msg);
        }
        return false;
    }


    inline uint8_t packSysExNonRtMtcCueingSetupMessage(uint8_t * bytes, uint8_t msgType, uint8_t deviceId,
                                                       uint8_t fps, uint8_t hour, uint8_t minute, uint8_t second,
                                                       uint8_t frame, uint8_t fractionalFrame, uint16_t eventNumber,
                                                       uint8_t * addInfo, uint8_t addInfoLen) {
        ASSERT(isSysExNonRtMtc(msgType));
        ASSERT(!isSysExNonRtMtcWithAddInfo(msgType) || addInfoLen > 0);
        ASSERT(bytes != NULL);
        ASSERT(deviceId <= MaxU7);
        ASSERT(isMtcFrameRate(fps));
        ASSERT(hour <= MtcMaxHour);
        ASSERT(minute <= MtcMaxMinute);
        ASSERT(second <= MtcMaxSecond);
        ASSERT(frame <= MtcMaxFps[fps]);
        ASSERT(fractionalFrame <= MtcMaxFractionalFrame);
        ASSERT(eventNumber <= MaxEventNumber);
        ASSERT( addInfoLen == 0 || addInfo != NULL );

        int msgLen = MsgLenSysExNonRtMtcCueingSetupMessageMin - 1; // min message length (w/o EOX)

        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = SysExIdNonRealTimeByte;
        bytes[2] = deviceId & DataMask;
        bytes[3] = SysExNonRtMidiTimeCode;
        bytes[4] = msgType;

        if (msgType == SysExNonRtMtcSpecial){
            // time is ignored, so let's simplify things
            bytes[5] = 0;
            bytes[6] = 0;
            bytes[7] = 0;
            bytes[8] = 0;
            bytes[9] = 0;
        } else {
            bytes[5] = ((fps << MtcFpsOffset) & MtcFpsMask) | (hour & MtcHourMask);
            bytes[6] = minute & MtcMinuteMask;
            bytes[7] = second & MtcSecondMask;
            bytes[8] = frame & MtcFrameMask;
            bytes[9] = fractionalFrame & MtcFractionalFrameMask;
        }

        packU14(&bytes[10], eventNumber);

        if (isSysExNonRtMtcWithAddInfo(msgType)) {
            msgLen += nibblize(&bytes[12], addInfo, addInfoLen);
        }

        bytes[msgLen++] = SystemMessageEndOfExclusive;

        return msgLen;
    }

    inline uint8_t packSysExNonRtMtcCueingSetupMessageObjCue(uint8_t * bytes, uint8_t msgType, uint8_t deviceId,
                                                       MtcCueingData_t * cueing, uint8_t * addInfo,
                                                       uint8_t addInfoLen) {
        ASSERT( cueing != NULL );

        return packSysExNonRtMtcCueingSetupMessage(bytes, msgType, deviceId, cueing->MidiTimeCode.Fps,
                                                   cueing->MidiTimeCode.Hour, cueing->MidiTimeCode.Minute, cueing->MidiTimeCode.Second, cueing->MidiTimeCode.Frame,
                                                   cueing->MidiTimeCode.FractionalFrame, cueing->EventNumber, addInfo, addInfoLen);
    }

    inline uint8_t packSysExNonRtMtcCueingSetupMessageObj( uint8_t * bytes, Message_t * msg ){
        ASSERT( msg != NULL );
        ASSERT( msg->StatusClass == StatusClassSystemMessage);
        ASSERT( msg->SystemMessage == SystemMessageSystemExclusive);
        ASSERT( msg->Data.SysEx.Id == SysExIdNonRealTime );
        ASSERT( msg->Data.SysEx.SubId1 == SysExNonRtMidiTimeCode );
        ASSERT( isSysExNonRtMtc(msg->Data.SysEx.SubId2));

        return packSysExNonRtMtcCueingSetupMessageObjCue( bytes, msg->Data.SysEx.SubId2, msg->Channel, &msg->Data.SysEx.Data.Cueing, msg->Data.SysEx.ByteData, msg->Data.SysEx.Length );
    }

    inline bool unpackSysExNonRtMtcCueingSetupMessage(uint8_t * bytes, uint8_t len,
                                                      uint8_t * deviceId, uint8_t  * msgType, uint8_t * fps, uint8_t * hour, uint8_t * minute,
                                                      uint8_t * second, uint8_t * frame, uint8_t * fractionalFrame,
                                                      uint16_t * eventNumber, uint8_t * addInfo, uint8_t * addInfoLen) {
        ASSERT(bytes != NULL);
        ASSERT(deviceId != NULL);
        ASSERT(msgType != NULL);
        ASSERT(fps != NULL);
        ASSERT(hour != NULL);
        ASSERT(minute != NULL);
        ASSERT(second != NULL);
        ASSERT(frame != NULL);
        ASSERT(fractionalFrame != NULL);
        ASSERT(eventNumber != NULL);
        ASSERT(addInfo != NULL);
        ASSERT(addInfoLen != NULL);


        if (len < MsgLenSysExNonRtMtcCueingSetupMessageMin - 1 || ! isControlByte(bytes[len-1])) {
            return false;
        }
        if (bytes[0] != SystemMessageSystemExclusive || bytes[1] != SysExIdNonRealTimeByte || bytes[3] != SysExNonRtMidiTimeCode || !isSysExNonRtMtc(bytes[4])) {
            return false;
        }

        *deviceId = bytes[2] & DataMask;
        *msgType = bytes[4];
        *fps = (bytes[5] >> 5) & 0b11;
        *hour = bytes[5] & MtcHourMask;
        *minute = bytes[6] & MtcMinuteMask;
        *second = bytes[7] & MtcSecondMask;
        *frame = bytes[8] & MtcFrameMask;
        *fractionalFrame = bytes[9] & MtcFractionalFrameMask;

        *eventNumber = unpackU14(&bytes[10]);

        if (len > MsgLenSysExNonRtMtcCueingSetupMessageMin) {
            *addInfoLen = denibblize(addInfo, &bytes[12], len - MsgLenSysExNonRtMtcCueingSetupMessageMin);
        }

        return true;
    }

    inline bool unpackSysExNonRtMtcCueingSetupMessageObjCue( uint8_t * bytes, uint8_t len, uint8_t * deviceId, uint8_t * msgType, MtcCueingData_t * cueing, uint8_t *addInfo, uint8_t *addInfoLen){
        ASSERT( cueing != NULL );

        return unpackSysExNonRtMtcCueingSetupMessage( bytes, len, deviceId, msgType, &cueing->MidiTimeCode.Fps, &cueing->MidiTimeCode.Hour, &cueing->MidiTimeCode.Minute, &cueing->MidiTimeCode.Second, &cueing->MidiTimeCode.Frame, &cueing->MidiTimeCode.FractionalFrame, &cueing->EventNumber, addInfo, addInfoLen);
    }

    inline bool unpackSysExNonRtMtcCueingSetupMessageObj(uint8_t * bytes, uint8_t len, Message_t * msg ){
        ASSERT( msg != NULL );

        if ( unpackSysExNonRtMtcCueingSetupMessageObjCue(bytes, len, &msg->Channel, &msg->Data.SysEx.SubId2, &msg->Data.SysEx.Data.Cueing, msg->Data.SysEx.ByteData, &msg->Data.SysEx.Length ) ){
            msg->StatusClass = StatusClassSystemMessage;
            msg->SystemMessage = SystemMessageSystemExclusive;
            msg->Data.SysEx.Id = SysExIdNonRealTime;
            msg->Data.SysEx.SubId1 = SysExNonRtMidiTimeCode;
            return true;
        }

        return false;
    }


    inline uint8_t packSysExRtMtcCueingSetupMessage(uint8_t *bytes, uint8_t deviceId, uint8_t msgType,
                                                    uint16_t eventNumber, uint8_t *addInfo, uint32_t addInfoLen) {
        ASSERT( isSysExRtMtcCueing(msgType) );
        ASSERT( !isSysExRtMtcCueingWithAddInfo(msgType) || addInfoLen > 0  );
        ASSERT( bytes != NULL );
        ASSERT( deviceId <= MaxU7 );
        ASSERT( eventNumber <= MaxEventNumber );

        int len = 7; // min message length (w/o EOX)

        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = SysExIdRealTimeByte;
        bytes[2] = deviceId;
        bytes[3] = SysExRtMidiTimeCodeCueing;
        bytes[4] = msgType;

        packU14(&bytes[5], eventNumber);

        if (isSysExRtMtcCueingWithAddInfo(msgType)) {
            len += nibblize(&bytes[7], addInfo, addInfoLen);
        }

        bytes[len++] = SystemMessageEndOfExclusive;

        return len;
    }

    inline uint8_t packSysExRtMtcCueingSetupMessageObj( uint8_t * bytes, Message_t * msg ){
        ASSERT( msg != NULL );

        return packSysExRtMtcCueingSetupMessage( bytes, msg->Channel, msg->Data.SysEx.SubId2, msg->Data.SysEx.Data.Cueing.EventNumber, msg->Data.SysEx.ByteData, msg->Data.SysEx.Length );
    }


    inline bool unpackSysExRtMtcCueingSetupMessage(uint8_t *bytes, uint8_t len, uint8_t *deviceId,
                                                   uint8_t *msgType, uint16_t *eventNumber, uint8_t *addInfo,
                                                   uint8_t *addInfoLen) {
        ASSERT(bytes != NULL);
        ASSERT(deviceId != NULL);
        ASSERT(eventNumber != NULL);
        ASSERT(addInfo != NULL);
        ASSERT(addInfoLen != NULL);


        if (len < MsgLenSysExRtMtcCueingSetupMessageMin || ! isControlByte(bytes[len-1])) {
            return false;
        }
        if (bytes[0] != SystemMessageSystemExclusive || bytes[1] != SysExIdRealTimeByte || bytes[3] != SysExRtMidiTimeCodeCueing || !isSysExRtMtcCueing(bytes[4])) {
            return false;
        }

        *deviceId = bytes[2] & DataMask;
        *msgType = bytes[4];

        *eventNumber = unpackU14(&bytes[5]);

        if (isSysExRtMtcCueingWithAddInfo(bytes[4])) {
            *addInfoLen = denibblize(addInfo, &bytes[7], len - MsgLenSysExRtMtcCueingSetupMessageMin);
        }

        return true;
    }

    inline bool unpackSysExRtMtcCueingSetupMessageObj(uint8_t *bytes, uint8_t length, Message_t * msg){
        ASSERT( msg != NULL );

        if (unpackSysExRtMtcCueingSetupMessage( bytes, length, &msg->Channel, &msg->Data.SysEx.SubId2, &msg->Data.SysEx.Data.Cueing.EventNumber, msg->Data.SysEx.ByteData, &msg->Data.SysEx.Length) ){
            msg->StatusClass = StatusClassSystemMessage;
            msg->SystemMessage = SystemMessageSystemExclusive;
            msg->Data.SysEx.Id = SysExIdRealTime;
            msg->Data.SysEx.SubId1 = SysExRtMidiTimeCodeCueing;
            return true;
        }

        return false;
    }

///////////// SysEx: General Information            /////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

    inline uint8_t packSysExNonRtGenInfoIdentityRequest(uint8_t *bytes, uint8_t deviceId) {
        ASSERT(bytes != NULL);
        ASSERT(deviceId <= MaxU7);

        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = SysExIdNonRealTimeByte;
        bytes[2] = deviceId & DataMask;
        bytes[3] = SysExNonRtGeneralInformation;
        bytes[4] = SysExNonRtGenInfoIdentityRequest;
        bytes[5] = SystemMessageEndOfExclusive;

        return MsgLenSysExNonRtGenInfoIdentityRequest;
    }


    inline uint8_t packSysExNonRtGenInfoIdentityReply(uint8_t *bytes, uint8_t deviceId, uint32_t manufacturerId,
                                                       uint16_t deviceFamily, uint16_t deviceFamilyMember,
                                                       uint8_t *softwareRevision) {
        ASSERT(bytes != NULL);
        ASSERT(deviceId <= MaxU7);
        ASSERT(deviceFamily <= MaxU14);
        ASSERT(deviceFamilyMember <= MaxU14);

        uint32_t len = 5;

        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = SysExIdNonRealTimeByte;
        bytes[2] = deviceId & DataMask;
        bytes[3] = SysExNonRtGeneralInformation;
        bytes[4] = SysExNonRtGenInfoIdentityReply;

        len += packSysExId(&bytes[5], manufacturerId);

        packU14(&bytes[len], deviceFamily);
        len += 2;

        packU14(&bytes[len], deviceFamilyMember);
        len += 2;

        bytes[len++] = softwareRevision[0];
        bytes[len++] = softwareRevision[1];
        bytes[len++] = softwareRevision[2];
        bytes[len++] = softwareRevision[3];

        bytes[len] = SystemMessageEndOfExclusive;

        return len;
    }


    inline uint8_t packSysExNonRtGeneralInformationObj( uint8_t *bytes, Message_t *msg ){
        ASSERT( msg != NULL );

        switch(msg->Data.SysEx.SubId2) {
            case SysExNonRtGenInfoIdentityRequest:
                return packSysExNonRtGenInfoIdentityRequest( bytes, msg->Channel );

            case SysExNonRtGenInfoIdentityReply:
                return packSysExNonRtGenInfoIdentityReply( bytes, msg->Channel, msg->Data.SysEx.Data.GeneralInfo.ManufacturerId, msg->Data.SysEx.Data.GeneralInfo.DeviceFamily, msg->Data.SysEx.Data.GeneralInfo.DeviceFamilyMember, msg->Data.SysEx.Data.GeneralInfo.SoftwareRevision );
        }

        return false;
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

    inline bool unpackSysExNonRtGenInfoIdentityReply(uint8_t *bytes, uint8_t len, uint8_t *deviceId,
                                                      uint32_t *manufacturerId, uint16_t *deviceFamily,
                                                      uint16_t *deviceFamilyMember, uint8_t *softwareRevision) {
        ASSERT(bytes != NULL);
        ASSERT(deviceId != NULL);
        ASSERT(manufacturerId != NULL);
        ASSERT(deviceFamily != NULL);
        ASSERT(deviceFamilyMember != NULL);
        ASSERT(softwareRevision != NULL);


        if (len != 16 || isControlByte(bytes[len-1]) ) {
            return false;
        }
        if (bytes[0] != SystemMessageSystemExclusive) {
            return false;
        }
        if (bytes[1] != SysExIdNonRealTimeByte) {
            return false;
        }
        if ((bytes[2] & DataMask) != bytes[2]) {
            return false;
        }
        if (bytes[3] != SysExNonRtGeneralInformation) {
            return false;
        }
        if (bytes[4] != SysExNonRtGenInfoIdentityReply) {
            return false;
        }

        int l = 5;

        *deviceId = bytes[2] & DataMask;

        l += unpackSysExId(&bytes[5], manufacturerId);

        *deviceFamily = unpackU14(&bytes[l]);
        l += 2;

        *deviceFamilyMember = unpackU14(&bytes[l]);
        l += 2;

        softwareRevision[0] = bytes[l++];
        softwareRevision[1] = bytes[l++];
        softwareRevision[2] = bytes[l++];
        softwareRevision[3] = bytes[l++];

        return true;
    }

    inline bool unpackSysExNonRtGeneralInformationObj( uint8_t *bytes, uint8_t length, Message_t *msg){
        ASSERT( msg != NULL );

        if (length < 5 || isControlByte(bytes[length-1])){
            return false;
        }

        if (bytes[0] != SystemMessageSystemExclusive) {
            return false;
        }
        if (bytes[1] != SysExIdNonRealTimeByte) {
            return false;
        }
        if ((bytes[2] & DataMask) != bytes[2]) {
            return false;
        }
        if (bytes[3] != SysExNonRtGeneralInformation) {
            return false;
        }

        switch(bytes[4]) {
            case SysExNonRtGenInfoIdentityRequest:
                msg->StatusClass = StatusClassSystemMessage;
                msg->SystemMessage = SystemMessageSystemExclusive;
                msg->Channel = bytes[2];
                msg->Data.SysEx.Id = SysExIdNonRealTime;
                msg->Data.SysEx.SubId1 = SysExNonRtGeneralInformation;
                msg->Data.SysEx.SubId2 = SysExNonRtGenInfoIdentityRequest;
                return true;

            case SysExNonRtGenInfoIdentityReply:
                if (unpackSysExNonRtGenInfoIdentityReply( bytes, length, &msg->Channel, &msg->Data.SysEx.Data.GeneralInfo.ManufacturerId, &msg->Data.SysEx.Data.GeneralInfo.DeviceFamily, &msg->Data.SysEx.Data.GeneralInfo.DeviceFamilyMember, msg->Data.SysEx.Data.GeneralInfo.SoftwareRevision )){
                    msg->StatusClass = StatusClassSystemMessage;
                    msg->SystemMessage = SystemMessageSystemExclusive;
                    msg->Data.SysEx.Id = SysExIdNonRealTime;
                    msg->Data.SysEx.SubId1 = SysExNonRtGeneralInformation;
                    msg->Data.SysEx.SubId2 = SysExNonRtGenInfoIdentityReply;
                    return true;
                }
        }

        return false;
    }

///////////// SysEx: Device Control                  ////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

    inline uint8_t packSysExRtDcBasic(uint8_t *bytes, uint8_t deviceId, SysExRtDc_t type, uint16_t value) {
        ASSERT(bytes != NULL);
        ASSERT(isSysExRtDeviceControl(type) && type != SysExRtDcGlobalParameterControl);
        ASSERT(type != SysExRtDcMasterVolume || value <= MaxU14);
        ASSERT(type != SysExRtDcMasterBalance || value <= MaxU14);
        ASSERT(type != SysExRtDcMasterCoarseTuning || value <= CoarseTuningMax); // MaxU14
        ASSERT(type != SysExRtDcMasterFineTuning || value <= FineTuningMax);

        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = SysExIdRealTimeByte;
        bytes[2] = deviceId & DataMask;
        bytes[3] = SysExRtDeviceControl;
        bytes[4] = type;

        packU14(&bytes[5], value);

        bytes[7] = SystemMessageEndOfExclusive;

        return MsgLenSysExNonRtDeviceControl;
    }

    inline uint8_t packSysExRtDcGlobalParameterControl(uint8_t *bytes, uint8_t deviceId, GlobalParameterControl_t *gpc) {
        ASSERT(bytes != NULL);
        ASSERT(deviceId <= MaxU7);
        ASSERT(gpc != NULL);
        ASSERT(gpc->Data != NULL);

        uint32_t len = 8;

        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = SysExIdRealTimeByte;
        bytes[2] = deviceId & DataMask;
        bytes[3] = SysExRtDeviceControl;
        bytes[4] = SysExRtDcGlobalParameterControl;
        bytes[5] = gpc->SlotPathLength;
        bytes[6] = gpc->ParameterIdWidth;
        bytes[7] = gpc->ValueWidth;

//        for (int i = 0; i < gpc->SlotPathLength; i++) {
//            bytes[len++] = getMsNibble(gpc->Data[i]);
//            bytes[len++] = getLsNibble(gpc->Data[i]);
//        }

        for (int i = 0; i < gpc->DataLength; i++) {
            bytes[len++] = gpc->Data[i];
        }

        bytes[len++] = SystemMessageEndOfExclusive;

        return len;
    }

    inline uint8_t packSysExRtDeviceControlObj(uint8_t *bytes, Message_t * msg ){
        ASSERT( msg != NULL );

        switch(msg->Data.SysEx.SubId2) {
            case SysExRtDcMasterVolume:
            case SysExRtDcMasterBalance:
            case SysExRtDcMasterCoarseTuning:
            case SysExRtDcMasterFineTuning:
                return packSysExRtDcBasic( bytes, msg->Channel, (SysExRtDc_t)msg->Data.SysEx.SubId2, msg->Data.SysEx.Data.DeviceControl.Value );

            case SysExRtDcGlobalParameterControl:
                return packSysExRtDcGlobalParameterControl( bytes, msg->Channel, &msg->Data.SysEx.Data.DeviceControl.GlobalParameterControl);

        }

        return false;
    }

    inline bool unpackSysExRtDcBasic(uint8_t *bytes, uint8_t len, uint8_t *deviceId, uint8_t *type,
                                    uint16_t *value) {
        ASSERT(bytes != NULL);
        ASSERT(deviceId != NULL);
        ASSERT(type != NULL);
        ASSERT(value != NULL);

        if ((len != MsgLenSysExNonRtDeviceControl - 1) &&
            (len != MsgLenSysExNonRtDeviceControl || bytes[len - 1] == SystemMessageEndOfExclusive)) {
            return false;
        }
        if (bytes[0] != SystemMessageSystemExclusive) {
            return false;
        }
        if (bytes[1] != SysExIdRealTimeByte) {
            return false;
        }
        if ((bytes[2] & DataMask) != bytes[2]) {
            return false;
        }
        if (bytes[3] != SysExRtDeviceControl) {
            return false;
        }
        if (!isSysExRtDeviceControl(bytes[4]) || bytes[4] == SysExRtDcGlobalParameterControl) {
            return false;
        }

        *deviceId = bytes[2];
        *type = bytes[4];

        *value = unpackU14(&bytes[5]);

        return true;
    }


    inline bool unpackSysExRtDcGlobalParameterControl(uint8_t *bytes, uint8_t len, uint8_t *deviceId,
                                             GlobalParameterControl_t *gpc) {
        ASSERT(bytes != NULL);
        ASSERT(deviceId != NULL);
        ASSERT(gpc != NULL);

        if (len < 9 || ! isControlByte(bytes[len -1])) {
            return false;
        }

        if (bytes[0] != SystemMessageSystemExclusive) {
            return false;
        }
        if (bytes[1] != SysExIdRealTimeByte) {
            return false;
        }
        if ((bytes[2] & DataMask) != bytes[2]) {
            return false;
        }
        if (bytes[3] != SysExRtDeviceControl) {
            return false;
        }
        if (bytes[4] != SysExRtDcGlobalParameterControl) {
            return false;
        }

        *deviceId = bytes[2];
        gpc->SlotPathLength = bytes[5];
        gpc->ParameterIdWidth = bytes[6];
        gpc->ValueWidth = bytes[7];

        uint32_t l = 0;

        // s acts merely as counter (identical to l..)
//        for (int s = 0, i = 8; s < gpc->SlotPathLength; s++, l++) {
//            gpc->Data[l] = (bytes[i++] & NibbleMask) << 4;
//            gpc->Data[l] |= bytes[i++] & NibbleMask;
//        }

        for (int i = 8 ; i < len && bytes[i] != SystemMessageEndOfExclusive; i++, l++) {
            gpc->Data[l] = bytes[i];
        }

        gpc->DataLength = l;

        return true;
    }

    inline bool unpackSysExRtDeviceControlObj(uint8_t *bytes, uint8_t length, Message_t *msg){
        ASSERT( msg != NULL );

        if (length < 5){
            return false;
        }

        switch(bytes[4]) {
            case SysExRtDcMasterVolume:
            case SysExRtDcMasterBalance:
            case SysExRtDcMasterCoarseTuning:
            case SysExRtDcMasterFineTuning:
                if (unpackSysExRtDcBasic( bytes, length, &msg->Channel, &msg->Data.SysEx.SubId2, &msg->Data.SysEx.Data.DeviceControl.Value )){
                    msg->StatusClass = StatusClassSystemMessage;
                    msg->SystemMessage = SystemMessageSystemExclusive;
                    msg->Data.SysEx.Id = SysExIdRealTime;
                    msg->Data.SysEx.SubId1 = SysExRtDeviceControl;
                    msg->Data.SysEx.SubId2 = bytes[4];
                    return true;
                }

            case SysExRtDcGlobalParameterControl:

                // use sysex raw byte data container for gpc
                msg->Data.SysEx.Data.DeviceControl.GlobalParameterControl.Data = msg->Data.SysEx.ByteData;

                if ( unpackSysExRtDcGlobalParameterControl( bytes, length, &msg->Channel, &msg->Data.SysEx.Data.DeviceControl.GlobalParameterControl )){
                    msg->StatusClass = StatusClassSystemMessage;
                    msg->SystemMessage = SystemMessageSystemExclusive;
                    msg->Data.SysEx.Id = SysExIdRealTime;
                    msg->Data.SysEx.SubId1 = SysExRtDeviceControl;
                    msg->Data.SysEx.SubId2 = SysExRtDcGlobalParameterControl;
                    return true;
                }

        }

        return false;
    }

///////////// SysEx: Controller Destination          ////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

    inline uint8_t packSysExRtControllerDestinationSetting( uint8_t * bytes, uint8_t deviceId, uint8_t subId2, uint8_t channel, uint8_t controller, uint8_t * data, uint8_t dataLen){
        ASSERT( bytes != NULL );
        ASSERT( deviceId <= MaxU7 );
        ASSERT( isSysExRtCds(subId2) );
        ASSERT(channel <= MaxU4 );
        ASSERT(controller <= MaxU7);
        ASSERT( data != NULL);
//        ASSERT(dataLen > 0); // require some data?
        ASSERT(dataLen % 2 == 0);


        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = SysExIdRealTimeByte;
        bytes[2] = deviceId;
        bytes[3] = SysExRtControllerDestinationSetting;
        bytes[4] = subId2;
        bytes[5] = channel;

        uint8_t length = 0;

        if (subId2 == SysExRtCdsController){
            bytes[6] = controller;
            length = 7;
        } else {
            length = 6;
        }

        for(uint8_t i = 0; i < dataLen; i++){
            bytes[length++] = data[i];
        }

        bytes[length++] = SystemMessageEndOfExclusive;

        return length;
    }

    inline uint8_t packSysExRtControllerDestinationSettingObj( uint8_t * bytes, Message_t * msg){
        return packSysExRtControllerDestinationSetting(bytes, msg->Channel, msg->Data.SysEx.SubId2, msg->Data.SysEx.Data.ControllerDestinationSetting.Channel, msg->Data.SysEx.Data.ControllerDestinationSetting.Controller, msg->Data.SysEx.ByteData, msg->Data.SysEx.Length);
    }

    inline bool unpackSysExRtControllerDestinationSetting( uint8_t * bytes, uint8_t length, uint8_t * deviceId, uint8_t *subId2, uint8_t * channel, uint8_t * controller, uint8_t * data, uint8_t * dataLen){
        ASSERT( bytes != NULL );
        ASSERT( deviceId != NULL );
        ASSERT( channel != NULL );
        ASSERT( controller != NULL );
        ASSERT( data != NULL );
        ASSERT( dataLen != NULL );

        if (length < 7 || ! isControlByte(bytes[length-1])){
            return false;
        }
        if (bytes[0] != SystemMessageSystemExclusive || bytes[1] != SysExIdRealTimeByte || bytes[3] != SysExRtControllerDestinationSetting || !isSysExRtCds(bytes[4])){
            return false;
        }

        *deviceId = bytes[2];
        *subId2 = bytes[4];
        *channel = bytes[5];
        *controller = bytes[6];

        uint8_t p;

        if (bytes[4] == SysExRtCdsController){
            p = 7;
        } else {
            p = 6;
        }

        length--; // skip trailing control byte
        uint8_t l = 0;

        while(p < length){
            data[l++] = bytes[p++];
        }

        *dataLen = l;

        return true;
    }

    inline bool unpackSysExRtControllerDestinationSettingObj( uint8_t * bytes, uint8_t length, Message_t * msg){
        ASSERT(msg != NULL);

        if (unpackSysExRtControllerDestinationSetting(bytes, length, &msg->Channel, &msg->Data.SysEx.SubId2, &msg->Data.SysEx.Data.ControllerDestinationSetting.Channel, &msg->Data.SysEx.Data.ControllerDestinationSetting.Controller, msg->Data.SysEx.ByteData, &msg->Data.SysEx.Length)){
            msg->StatusClass = StatusClassSystemMessage;
            msg->SystemMessage = SystemMessageSystemExclusive;
            msg->Data.SysEx.Id = SysExIdRealTime;
            msg->Data.SysEx.SubId1 = SysExRtControllerDestinationSetting;
            return true;
        }

        return false;
    }

///////////// SysEx: Keybased Instrument Control     ////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

    inline uint8_t packSysExRtKeybasedInstrumentControl( uint8_t * bytes, uint8_t deviceId, uint8_t channel, uint8_t key, uint8_t * data, uint8_t dataLen){
        ASSERT( bytes != NULL );
        ASSERT( deviceId <= MaxU7 );
        ASSERT(channel <= MaxU4 );
        ASSERT(key <= MaxU7 );
        ASSERT( data != NULL);
//        ASSERT(dataLen > 0); // require some data?
        ASSERT(dataLen % 2 == 0);


        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = SysExIdRealTimeByte;
        bytes[2] = deviceId;
        bytes[3] = SysExRtKeybasedInstrumentControl;
        bytes[4] = SysExRtKeysBasicMessage;
        bytes[5] = channel;
        bytes[6] = key;

        uint8_t length = 7;

        for(uint8_t i = 0; i < dataLen; i++){
            bytes[length++] = data[i];
        }

        bytes[length++] = SystemMessageEndOfExclusive;

        return length;
    }

    inline uint8_t packSysExRtKeybasedInstrumentControlObj( uint8_t * bytes, Message_t * msg){
        return packSysExRtKeybasedInstrumentControl(bytes, msg->Channel, msg->Data.SysEx.Data.ControllerDestinationSetting.Channel, msg->Data.SysEx.Data.ControllerDestinationSetting.Controller, msg->Data.SysEx.ByteData, msg->Data.SysEx.Length);
    }

    inline bool unpackSysExRtKeybasedInstrumentControl( uint8_t * bytes, uint8_t length, uint8_t * deviceId, uint8_t *channel, uint8_t * key, uint8_t * data, uint8_t * dataLen){
        ASSERT( bytes != NULL );
        ASSERT( deviceId != NULL );
        ASSERT( channel != NULL );
        ASSERT( key != NULL );
        ASSERT( data != NULL );
        ASSERT( dataLen != NULL );

        if (length < 7 || ! isControlByte(bytes[length-1])){
            return false;
        }
        if (bytes[0] != SystemMessageSystemExclusive || bytes[1] != SysExIdRealTimeByte || bytes[3] != SysExRtKeybasedInstrumentControl || !isSysExRtCds(bytes[4])){
            return false;
        }

        *deviceId = bytes[2];
        *channel = bytes[5];
        *key = bytes[6];

        uint8_t p = 7;

        length--; // skip trailing control byte
        uint8_t l = 0;

        while(p < length){
            data[l++] = bytes[p++];
        }

        *dataLen = l;

        return true;
    }

    inline bool unpackSysExRtKeybasedInstrumentControlObj( uint8_t * bytes, uint8_t length, Message_t * msg){
        ASSERT(msg != NULL);

        if (unpackSysExRtKeybasedInstrumentControl(bytes, length, &msg->Channel, &msg->Data.SysEx.Data.KeybasedInstrumentControl.Channel, &msg->Data.SysEx.Data.KeybasedInstrumentControl.Key, msg->Data.SysEx.ByteData, &msg->Data.SysEx.Length)){
            msg->StatusClass = StatusClassSystemMessage;
            msg->SystemMessage = SystemMessageSystemExclusive;
            msg->Data.SysEx.Id = SysExIdRealTime;
            msg->Data.SysEx.SubId1 = SysExRtKeybasedInstrumentControl;
            return true;
        }

        return false;
    }

///////////// SysEx: General MIDI                    ////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

    inline uint8_t packSysExNonRtGeneralMidi(uint8_t *bytes, uint8_t deviceId, uint8_t subId2){
        ASSERT( bytes != NULL );
        ASSERT( deviceId <= MaxU7 );
        ASSERT( isSysExNonRtGeneralMidi(subId2) );

        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = SysExIdNonRealTimeByte;
        bytes[2] = deviceId;
        bytes[3] = SysExNonRtGeneralMidi;
        bytes[4] = subId2;
        bytes[5] = SystemMessageEndOfExclusive;

        return MsgLenSysExNonRtGeneralMidi;
    }

    inline uint8_t packSysExNonRtGeneralMidiObj(uint8_t *bytes, Message_t *msg){
        ASSERT( msg != NULL );
        ASSERT( msg->StatusClass == StatusClassSystemMessage);
        ASSERT( msg->SystemMessage == SystemMessageSystemExclusive);
        ASSERT( msg->Data.SysEx.Id == SysExIdNonRealTime);
        ASSERT( msg->Data.SysEx.SubId1 == SysExNonRtGeneralMidi);

        return packSysExNonRtGeneralMidi(bytes, msg->Channel, msg->Data.SysEx.SubId2);
    }

    inline bool unpackSysExNonRtGeneralMidi(uint8_t *bytes, uint8_t length, uint8_t *deviceId, uint8_t *subId2){
        ASSERT( bytes != NULL );
        ASSERT(deviceId != NULL);
        ASSERT( subId2 != NULL);

        if (length != MsgLenSysExNonRtGeneralMidi || !isControlByte((bytes[length-1]))){
            return false;
        }
        if (bytes[0] != SystemMessageSystemExclusive || bytes[1] != SysExIdNonRealTimeByte || bytes[3] != SysExNonRtGeneralMidi || !isSysExNonRtGeneralMidi(bytes[4]) ){
            return false;
        }

        *deviceId = bytes[2];
        *subId2 = bytes[4];

        return true;
    }

    inline bool unpackSysExNonRtGeneralMidiObj(uint8_t *bytes, uint8_t length, Message_t *msg){
        ASSERT( msg != NULL );

        if (unpackSysExNonRtGeneralMidi(bytes, length, &msg->Channel, &msg->Data.SysEx.SubId2)){
            msg->StatusClass = StatusClassSystemMessage;
            msg->SystemMessage = SystemMessageSystemExclusive;
            msg->Data.SysEx.Id = SysExIdNonRealTime;
            msg->Data.SysEx.SubId1 = SysExNonRtGeneralMidi;
            return true;
        }

        return false;
    }

///////////// SysEx: MIDI Machine Control            ////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

    /**
     * MMC Commands allow for multiple commands in a sysex message
     *
     * if command == 0, it is assumed the data structure is taken care of externally by the caller. That is, any commands must be
     * given by <data> where of <data[0]> would be the first (and possibly only) command.
     */
    inline uint8_t packSysExRtMmcCommandMessage(uint8_t *bytes, uint8_t deviceId, uint8_t *data,
                                         uint8_t dataLen) {
        ASSERT(bytes != NULL);
        ASSERT(deviceId <= MaxU7);
        ASSERT((dataLen == 0) || (data != NULL));
        ASSERT(dataLen <= MaxU7);
        ASSERT(isSysExRtMmcCommand(data[0]));

        uint32_t len = 4;

        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = SysExIdRealTimeByte;
        bytes[2] = deviceId & DataMask;
        bytes[3] = SysExRtMidiMachineControlCommand;

        for (uint8_t i = 0; i < dataLen; i++) {
            bytes[len++] = data[i];
        }

        bytes[len++] = SystemMessageEndOfExclusive;

        ASSERT( len <= MsgLenSysExRtMmcCommandMax );

        return len;
    }

    inline uint8_t packSysExRtMmcCommandMessageObj( uint8_t *bytes, Message_t * msg ){
        ASSERT( msg != NULL );

        return packSysExRtMmcCommandMessage( bytes, msg->Channel, msg->Data.SysEx.ByteData, msg->Data.SysEx.Length );
    }

    inline bool unpackSysExRtMmcCommandMessage(uint8_t *bytes, uint8_t len, uint8_t *deviceId,
                                        uint8_t *data, uint8_t *dataLen) {
        ASSERT(bytes != NULL);
        ASSERT(deviceId != NULL);
        ASSERT(data != NULL);
        ASSERT(dataLen != NULL);


        if (len <= MsgLenSysExRtMmcCommandWithoutData || ! isControlByte(bytes[len-1])) {
            return false;
        }
        if (bytes[0] != SystemMessageSystemExclusive) {
            return false;
        }
        if (bytes[1] != SysExIdRealTimeByte) {
            return false;
        }
        if ((bytes[2] & DataMask) != bytes[2]) {
            return false;
        }
        if (bytes[3] != SysExRtMidiMachineControlCommand) {
            return false;
        }
        if (!isSysExRtMmcCommand(bytes[4])) {
            return false;
        }

        *deviceId = bytes[2];

        uint8_t l = len - MsgLenSysExRtMmcCommandWithoutData;

        for (uint8_t i = 4, j = 0; j < l; i++, j++) {
            data[j] = bytes[i];
        }

        *dataLen = l;

        return true;
    }


    inline bool unpackSysExRtMmcCommandMessageObj(uint8_t *bytes, uint8_t length, Message_t * msg){
        ASSERT( msg != NULL );

        if ( unpackSysExRtMmcCommandMessage( bytes, length, &msg->Channel, msg->Data.SysEx.ByteData, &msg->Data.SysEx.Length) ){
            msg->StatusClass = StatusClassSystemMessage;
            msg->SystemMessage = SystemMessageSystemExclusive;
            msg->Data.SysEx.Id = SysExIdRealTime;
            msg->Data.SysEx.SubId1 = SysExRtMidiMachineControlCommand;
            msg->Data.SysEx.SubId2 = 0; // see data (mmc commands can be a stream of commands and need not only be one)
            return true;
        }

        return false;
    }

    inline uint8_t packSysExRtMmcCommand( uint8_t * bytes, SysExRtMmcCommandData_t * cmd ){
        ASSERT( bytes != NULL );
        ASSERT( cmd != NULL );
        ASSERT( isSysExRtMmcCommand(cmd->Command.Bytes[0]) );

        uint8_t len = 0;

        len += copyMmcExtensibleCommand( bytes, cmd->Command.Bytes );

        switch(cmd->Command.Bytes[0]){

            case SysExRtMmcCommandStop:
            case SysExRtMmcCommandPlay:
            case SysExRtMmcCommandDeferredPlay:
            case SysExRtMmcCommandFastForward:
            case SysExRtMmcCommandRewind:
            case SysExRtMmcCommandRecordStrobe:
            case SysExRtMmcCommandRecordExit:
            case SysExRtMmcCommandRecordPause:
            case SysExRtMmcCommandPause:
            case SysExRtMmcCommandEject:
            case SysExRtMmcCommandChase:
            case SysExRtMmcCommandCommandErrorReset:
            case SysExRtMmcCommandMmcReset:
            case SysExRtMmcCommandWait:
            case SysExRtMmcCommandResume:
                // no data
                return len;

            case SysExRtMmcCommandVariablePlay:
            case SysExRtMmcCommandSearch:
            case SysExRtMmcCommandShuttle:
            case SysExRtMmcCommandDeferredVariablePlay:
            case SysExRtMmcCommandRecordStrobeVariable:
                bytes[len++] = 0x03;
                len += packSysExRtMmcStandardSpeedObj(&bytes[2], &cmd->Data.StandardSpeed);
                return len;

            case SysExRtMmcCommandStep:
                bytes[len++] = 0x01;
                bytes[len++] = cmd->Data.S7 & DataMask;
                return len;


            case SysExRtMmcCommandLocate:
                ASSERT( isSysExRtMmcCommandLocateSubCommand(cmd->Data.Locate.SubCommand));

                if (cmd->Data.Locate.SubCommand == SysExRtMmcCommandLocateSubCommandInformationField){
                    bytes[len++] = 2; // length
                    bytes[len++] = SysExRtMmcCommandLocateSubCommandInformationField;
                    bytes[len++] = cmd->Data.Locate.InformationField;
                    return len;
                }
                else if (cmd->Data.Locate.SubCommand == SysExRtMmcCommandLocateSubCommandTarget){
                    bytes[len++] = 6; // length
                    bytes[len++] = SysExRtMmcCommandLocateSubCommandTarget;
                    len += packMidiTimeCodeLong( &bytes[len], &cmd->Data.Locate.MidiTimeCode);
                    return len;
                }
                else {
                    ASSERT(false);
                    return 0;
                }

            case SysExRtMmcCommandAssignSystemMaster:
                bytes[len++] = 1; // length
                bytes[len++] = cmd->Data.U7;
                return len;

            case SysExRtMmcCommandWrite: ////////

            case SysExRtMmcCommandMaskedWrite:
            case SysExRtMmcCommandRead:
            case SysExRtMmcCommandUpdate:
            case SysExRtMmcCommandGeneratorCommand:
            case SysExRtMmcCommandMtcCommand:
            case SysExRtMmcCommandMove:
            case SysExRtMmcCommandAdd:
            case SysExRtMmcCommandSubstract:
            case SysExRtMmcCommandDropFrameAdjust:
            case SysExRtMmcCommandProcedure:
            case SysExRtMmcCommandEvent:
            case SysExRtMmcCommandGroup:
            case SysExRtMmcCommandCommandSegment:
                //TODO

            default:
                return 0;

        }
    }

    inline bool unpackSysExRtMmcCommand( uint8_t * bytes, uint8_t length, SysExRtMmcCommandData_t * cmd ){
        ASSERT( bytes != NULL );
        ASSERT( cmd != NULL );

        if (length < 1){
            return false;
        }
        if ( ! isSysExRtMmcCommand(bytes[0]) ){
            return false;
        }

        cmd->Command.Value = 0;


        uint8_t len = 0;

        len += copyMmcExtensibleCommand( cmd->Command.Bytes, bytes );

        switch(cmd->Command.Bytes[0]){

            case SysExRtMmcCommandStop:
            case SysExRtMmcCommandPlay:
            case SysExRtMmcCommandDeferredPlay:
            case SysExRtMmcCommandFastForward:
            case SysExRtMmcCommandRewind:
            case SysExRtMmcCommandRecordStrobe:
            case SysExRtMmcCommandRecordExit:
            case SysExRtMmcCommandRecordPause:
            case SysExRtMmcCommandPause:
            case SysExRtMmcCommandEject:
            case SysExRtMmcCommandChase:
            case SysExRtMmcCommandCommandErrorReset:
            case SysExRtMmcCommandMmcReset:
            case SysExRtMmcCommandWait:
            case SysExRtMmcCommandResume:
                // no data
                return true;

            case SysExRtMmcCommandVariablePlay:
            case SysExRtMmcCommandSearch:
            case SysExRtMmcCommandShuttle:
            case SysExRtMmcCommandDeferredVariablePlay:
            case SysExRtMmcCommandRecordStrobeVariable:
                if (length < 5 || bytes[1] != 0x03){
                    return false;
                }
                unpackSysExRtMmcStandardSpeedObj(&bytes[2], &cmd->Data.StandardSpeed);
                return true;

            case SysExRtMmcCommandStep:
                if (length < 3 || bytes[1] != 0x01){
                    return false;
                }
                cmd->Data.S7 = bytes[2];

                if (bytes[2] & 0x40){
                    cmd->Data.S7 |= 0x80; // set sign byte
                }
                return true;

            case SysExRtMmcCommandLocate:
                if ( ! isSysExRtMmcCommandLocateSubCommand(bytes[2])){
                    return false;
                }

                cmd->Data.Locate.SubCommand = bytes[2];

                if (cmd->Data.Locate.SubCommand == SysExRtMmcCommandLocateSubCommandInformationField){
                    if (length < 3 || bytes[1] != 2){
                        return false;
                    }
                    cmd->Data.Locate.InformationField = bytes[3];
                    return true;
                }
                else if (cmd->Data.Locate.SubCommand == SysExRtMmcCommandLocateSubCommandTarget){
                    if (length < 7 || bytes[1] != 6){
                        return false;
                    }
                    unpackMidiTimeCodeLong( &bytes[3], &cmd->Data.Locate.MidiTimeCode);
                    return true;
                }

            case SysExRtMmcCommandAssignSystemMaster:
                if (length < 3 || bytes[1] != 1){
                    return false;
                }
                cmd->Data.U7 = bytes[2];
                return true;

            case SysExRtMmcCommandWrite: ////////

            case SysExRtMmcCommandMaskedWrite:
            case SysExRtMmcCommandRead:
            case SysExRtMmcCommandUpdate:
            case SysExRtMmcCommandGeneratorCommand:
            case SysExRtMmcCommandMtcCommand:
            case SysExRtMmcCommandMove:
            case SysExRtMmcCommandAdd:
            case SysExRtMmcCommandSubstract:
            case SysExRtMmcCommandDropFrameAdjust:
            case SysExRtMmcCommandProcedure:
            case SysExRtMmcCommandEvent:
            case SysExRtMmcCommandGroup:
            case SysExRtMmcCommandCommandSegment:
                //TODO

            default:
                return false;

        }
    }

    /**
     * MMC Responses allow for multiple responses in a sysex message
     */
    inline uint8_t packSysExRtMmcResponseMessage(uint8_t *bytes, uint8_t deviceId, uint8_t *data,
                                         uint8_t dataLen) {
        ASSERT(bytes != NULL);
        ASSERT(deviceId <= MaxU7);
        ASSERT((dataLen == 0) || (data != NULL));
        ASSERT(dataLen <= MaxU7);
        ASSERT(isSysExRtMmcCommand(data[0]));

        uint32_t len = 5;

        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = SysExIdRealTimeByte;
        bytes[2] = deviceId & DataMask;
        bytes[3] = SysExRtMidiMachineControlResponse;

        for (uint8_t i = 0; i < dataLen; i++) {
            bytes[len++] = data[i];
        }

        bytes[len++] = SystemMessageEndOfExclusive;

        ASSERT( len <= MsgLenSysExRtMmcCommandMax );

        return len;
    }

    inline uint8_t packSysExRtMmcResponseMessageObj( uint8_t *bytes, Message_t * msg ){
        ASSERT( msg != NULL );

        return packSysExRtMmcResponseMessage( bytes, msg->Channel, msg->Data.SysEx.ByteData, msg->Data.SysEx.Length );
    }

    inline bool unpackSysExRtMmcResponseMessage(uint8_t *bytes, uint8_t len, uint8_t *deviceId,
                                        uint8_t *data, uint8_t *dataLen) {
        ASSERT(bytes != NULL);
        ASSERT(deviceId != NULL);
        ASSERT(data != NULL);
        ASSERT(dataLen != NULL);


        if (len <= MsgLenSysExRtMmcResponseWithoutData || ! isControlByte(bytes[len-1])) {
            return false;
        }
        if (bytes[0] != SystemMessageSystemExclusive) {
            return false;
        }
        if (bytes[1] != SysExIdRealTimeByte) {
            return false;
        }
        if ((bytes[2] & DataMask) != bytes[2]) {
            return false;
        }
        if (bytes[3] != SysExRtMidiMachineControlResponse) {
            return false;
        }
        if (!isSysExRtMmcResponse(bytes[4])) {
            return false;
        }

        *deviceId = bytes[2];

        uint8_t l = len - MsgLenSysExRtMmcResponseWithoutData;

        for (uint8_t i = 4, j = 0; j < l; i++, j++) {
            data[j] = bytes[i];
        }

        *dataLen = l;

        return true;
    }


    inline bool unpackSysExRtMmcResponseMessageObj(uint8_t *bytes, uint8_t length, Message_t * msg){
        ASSERT( msg != NULL );

        if ( unpackSysExRtMmcResponseMessage( bytes, length, &msg->Channel, msg->Data.SysEx.ByteData, &msg->Data.SysEx.Length) ){
            msg->StatusClass = StatusClassSystemMessage;
            msg->SystemMessage = SystemMessageSystemExclusive;
            msg->Data.SysEx.Id = SysExIdRealTimeByte;
            msg->Data.SysEx.SubId1 = SysExRtMidiMachineControlResponse;
            msg->Data.SysEx.SubId2 = 0; // see data (mmc responses can be a stream of commands and need not only be one)
            return true;
        }

        return false;
    }

///////////// SysEx: MIDI Show Control               ////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

    inline uint8_t packSysExRtMidiShowControl( uint8_t * bytes, uint8_t deviceId, MidiShowControlData_t * msc ){
        ASSERT( bytes != NULL );
        ASSERT( deviceId <= MaxU7 );
        ASSERT( msc != NULL );
        ASSERT( isSysExRtMscCmdFmt(msc->CommandFormat.Bytes[0]) );
        ASSERT( isSysExRtMscCmd(msc->Command.Bytes[0]) );

        uint8_t len = 4;

        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = SysExIdRealTimeByte;
        bytes[2] = deviceId & DataMask;
        bytes[3] = SysExRtMidiShowControl;

        len += copyMscExtensibleCommandField( &bytes[len], msc->CommandFormat.Bytes );

        len += copyMscExtensibleCommandField( &bytes[len], msc->Command.Bytes );

        switch( msc->Command.Bytes[0] ){
            case SysExRtMscCmdGo:
            case SysExRtMscCmdStop:
            case SysExRtMscCmdResume:
            case SysExRtMscCmdLoad:

            case SysExRtMscCmdGoOff:
            case SysExRtMscCmdGo_JamLock:
                len += packMscCueNumber( &bytes[len], msc->CueNumber.Number, msc->CueNumber.List, msc->CueNumber.Path );
                break;

            case SysExRtMscCmdTimedGo:
                len += packMidiTimeCodeLong( &bytes[len], &msc->MidiTimeCode );
                len += packMscCueNumber( &bytes[len], msc->CueNumber.Number, msc->CueNumber.List, msc->CueNumber.Path );
                break;

            case SysExRtMscCmdSet:
                packU14( &bytes[len], msc->Controller );
                len += 2;
                packU14( &bytes[len], msc->Value );
                len += 2;
                len += packMidiTimeCodeLong( &bytes[len], &msc->MidiTimeCode );
                break;

            case SysExRtMscCmdFire:
                bytes[len++] = msc->MacroNumber;
                break;

            case SysExRtMscCmdStandbyPlus:
            case SysExRtMscCmdStandbyMinus:
            case SysExRtMscCmdSequencePlus:
            case SysExRtMscCmdSequenceMinus:
            case SysExRtMscCmdStartClock:
            case SysExRtMscCmdStopClock:
            case SysExRtMscCmdZeroClock:
            case SysExRtMscCmdMtcChaseOn:
            case SysExRtMscCmdMtcChaseOff:
            case SysExRtMscCmdOpenCueList:
            case SysExRtMscCmdCloseCueList:
                len += packMscCueNumber( &bytes[len], msc->CueNumber.List, NULL, NULL );
                break;

            case SysExRtMscCmdOpenCuePath:
            case SysExRtMscCmdCloseCuePath:
                len += packMscCueNumber( &bytes[len], msc->CueNumber.Path, NULL, NULL );
                break;

            case SysExRtMscCmdSetClock:
                len += packMidiTimeCodeLong( &bytes[len], &msc->MidiTimeCode );
                len += packMscCueNumber( &bytes[len], msc->CueNumber.List, NULL, NULL );
                break;

            case SysExRtMscCmdStandby:
            case SysExRtMscCmdGo2Pc:
                packU14( &bytes[len], msc->Checksum );
                len += 2;
                packU14( &bytes[len], msc->SequenceNumber );
                len += 2;
                bytes[len++] = msc->Data[0];
                bytes[len++] = msc->Data[1];
                bytes[len++] = msc->Data[2];
                bytes[len++] = msc->Data[3];
                len += packMscCueNumber( &bytes[len], msc->CueNumber.Number, msc->CueNumber.List, msc->CueNumber.Path );
                break;

            case SysExRtMscCmdStandingBy:
                packU14( &bytes[len], msc->Checksum );
                len += 2;
                packU14( &bytes[len], msc->SequenceNumber );
                len += 2;
                len += packMidiTimeCodeLong( &bytes[len], &msc->MidiTimeCode );
                len += packMscCueNumber( &bytes[len], msc->CueNumber.Number, msc->CueNumber.List, msc->CueNumber.Path );
                break;

            case SysExRtMscCmdComplete:
            case SysExRtMscCmdCancel:
                packU14( &bytes[len], msc->Checksum );
                len += 2;
                packU14( &bytes[len], msc->SequenceNumber );
                len += 2;
                len += packMscCueNumber( &bytes[len], msc->CueNumber.Number, msc->CueNumber.List, msc->CueNumber.Path );
                break;

            case SysExRtMscCmdCancelled:
            case SysExRtMscCmdAbort:
                packU14( &bytes[len], msc->Checksum );
                len += 2;
                len += packSysExRtMscStatus( &bytes[len], msc->Status );
                packU14( &bytes[len], msc->SequenceNumber );
                len += 2;
                len += packMscCueNumber( &bytes[len], msc->CueNumber.Number, msc->CueNumber.List, msc->CueNumber.Path );
                break;


            default:
            case SysExRtMscCmdAllOff:
            case SysExRtMscCmdRestore:
            case SysExRtMscCmdReset:
                break;


        }

        bytes[len++] = SystemMessageEndOfExclusive;

        return len;
    }

    inline uint8_t packSysExRtMidiShowControlObj( uint8_t * bytes, Message_t * msg ){
        ASSERT( msg != NULL );

        return packSysExRtMidiShowControl( bytes, msg->Channel, &msg->Data.SysEx.Data.MidiShowControl );
    }


    inline bool unpackSysExRtMidiShowControl(uint8_t * bytes, uint8_t len, uint8_t * deviceId, MidiShowControlData_t * msc ) {
        ASSERT( bytes != NULL );
        ASSERT( deviceId != NULL );
        ASSERT( msc != NULL );


        if (len < 5) {
            return false;
        }
        if (bytes[0] != SystemMessageSystemExclusive) {
            return false;
        }
        if (bytes[1] != SysExIdRealTimeByte) {
            return false;
        }
        if ((bytes[2] & DataMask) != bytes[2]) {
            return false;
        }
        if (bytes[3] != SysExRtMidiShowControl) {
            return false;
        }
        if ( ! isSysExRtMscCmdFmt(bytes[4]) ) {
            return false;
        }

        uint8_t pos = 4;

        *deviceId = bytes[2];

        pos += copyMscExtensibleCommandField( msc->CommandFormat.Bytes, &bytes[pos] );

        if ( ! isSysExRtMscCmd(msc->CommandFormat.Bytes[0]) ){
            return false;
        }

//        ASSERT(false);
        pos += copyMscExtensibleCommandField( msc->Command.Bytes, &bytes[pos] );


        switch( msc->Command.Bytes[0] ){
            case SysExRtMscCmdGo:
            case SysExRtMscCmdStop:
            case SysExRtMscCmdResume:
            case SysExRtMscCmdLoad:

            case SysExRtMscCmdGoOff:
            case SysExRtMscCmdGo_JamLock:
                pos += unpackMscCueNumber( &bytes[pos], len - pos, &msc->CueNumber.Number, &msc->CueNumber.List, &msc->CueNumber.Path );
                break;

            case SysExRtMscCmdTimedGo:
                pos += unpackMidiTimeCodeLong( &bytes[len], &msc->MidiTimeCode );
                pos += unpackMscCueNumber( &bytes[pos], len - pos, &msc->CueNumber.Number, &msc->CueNumber.List, &msc->CueNumber.Path );
                break;

            case SysExRtMscCmdSet:
                msc->Controller = unpackU14( &bytes[pos] );
                pos += 2;
                msc->Value = unpackU14( &bytes[pos] );
                pos += 2;
                pos += unpackMidiTimeCodeLong( &bytes[pos], &msc->MidiTimeCode );
                break;

            case SysExRtMscCmdFire:
                msc->MacroNumber = bytes[pos++];
                break;

            case SysExRtMscCmdStandbyPlus:
            case SysExRtMscCmdStandbyMinus:
            case SysExRtMscCmdSequencePlus:
            case SysExRtMscCmdSequenceMinus:
            case SysExRtMscCmdStartClock:
            case SysExRtMscCmdStopClock:
            case SysExRtMscCmdZeroClock:
            case SysExRtMscCmdMtcChaseOn:
            case SysExRtMscCmdMtcChaseOff:
            case SysExRtMscCmdOpenCueList:
            case SysExRtMscCmdCloseCueList:
                pos += unpackMscCueNumber( &bytes[pos], len - pos, &msc->CueNumber.List, NULL, NULL );
                break;

            case SysExRtMscCmdOpenCuePath:
            case SysExRtMscCmdCloseCuePath:
                pos += unpackMscCueNumber( &bytes[pos], len - pos, &msc->CueNumber.Path, NULL, NULL );
                break;

            case SysExRtMscCmdSetClock:
                pos += unpackMidiTimeCodeLong( &bytes[pos], &msc->MidiTimeCode );
                pos += unpackMscCueNumber( &bytes[pos], len - pos, &msc->CueNumber.List, NULL, NULL );
                break;

            case SysExRtMscCmdStandby:
            case SysExRtMscCmdGo2Pc:
                msc->Checksum = unpackU14( &bytes[pos] );
                pos += 2;
                msc->SequenceNumber = unpackU14( &bytes[pos] );
                pos += 2;
                msc->Data[0] = bytes[pos++];
                msc->Data[1] = bytes[pos++];
                msc->Data[2] = bytes[pos++];
                msc->Data[3] = bytes[pos++];
                pos += unpackMscCueNumber( &bytes[pos], len - pos, &msc->CueNumber.Number, &msc->CueNumber.List, &msc->CueNumber.Path );
                break;

            case SysExRtMscCmdStandingBy:
                msc->Checksum = unpackU14( &bytes[pos] );
                pos += 2;
                msc->SequenceNumber = unpackU14( &bytes[pos]);
                pos += 2;
                pos += unpackMidiTimeCodeLong( &bytes[pos], &msc->MidiTimeCode );
                pos += unpackMscCueNumber( &bytes[pos], len - pos, &msc->CueNumber.Number, &msc->CueNumber.List, &msc->CueNumber.Path );
                break;

            case SysExRtMscCmdComplete:
            case SysExRtMscCmdCancel:
                msc->Checksum = unpackU14( &bytes[pos]);
                pos += 2;
                msc->SequenceNumber = unpackU14( &bytes[pos]);
                pos += 2;
                pos += unpackMscCueNumber( &bytes[pos], len - pos, &msc->CueNumber.Number, &msc->CueNumber.List, &msc->CueNumber.Path );
                break;

            case SysExRtMscCmdCancelled:
            case SysExRtMscCmdAbort:
                msc->Checksum = unpackU14( &bytes[pos]);
                pos += 2;
                pos += unpackSysExRtMscStatus( &bytes[pos], &msc->Status );
                msc->SequenceNumber = unpackU14( &bytes[pos]);
                pos += 2;
                pos += unpackMscCueNumber( &bytes[len], len - pos, &msc->CueNumber.Number, &msc->CueNumber.List, &msc->CueNumber.Path );
                break;

            case SysExRtMscCmdAllOff:
            case SysExRtMscCmdRestore:
            case SysExRtMscCmdReset:
                break;


            default:
                return false;

        }

        return true;
    }

    inline bool unpackSysExRtMidiShowControlObj(uint8_t * bytes, uint8_t length, Message_t * msg ) {
        ASSERT( msg != NULL );

        if ( unpackSysExRtMidiShowControl(bytes, length, &msg->Channel, &msg->Data.SysEx.Data.MidiShowControl) ){
            msg->StatusClass = StatusClassSystemMessage;
            msg->SystemMessage = SystemMessageSystemExclusive;
            msg->Data.SysEx.Id = SysExIdRealTime;
            msg->Data.SysEx.SubId1 = SysExRtMidiShowControl;
            return true;
        }

        return false;
    }

///////////// SysEx: Sample Dump (Extension)         ////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

    inline uint8_t packSysExNonRtSdsHeader(uint8_t *bytes, uint8_t deviceId, uint16_t sampleNumber, uint8_t sampleFormat, uint32_t samplePeriod, uint32_t sampleLength, uint32_t loopStartPoint, uint32_t loopEndPoint, uint8_t loopType ){

        ASSERT( bytes!= NULL );
        ASSERT( deviceId <= MaxU7 );
        ASSERT( sampleNumber <= MaxU14 );
        ASSERT( sampleFormat <= MaxU7 );
        ASSERT( samplePeriod <= MaxU21 );
        ASSERT( sampleLength <= MaxU21 );
        ASSERT( loopEndPoint <= MaxU21 );
        ASSERT( loopStartPoint <= MaxU21 );
        ASSERT( isSysExNonRtSdsLoopType(loopType) );


        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = SysExIdNonRealTimeByte;
        bytes[2] = deviceId;
        bytes[3] = SysExNonRtSampleDumpHeader;

        packU14( &bytes[4], sampleNumber );

        bytes[6] = sampleFormat;

        packU21( &bytes[7], samplePeriod );
        packU21( &bytes[10], sampleLength );
        packU21( &bytes[13], loopStartPoint );
        packU21( &bytes[16], loopEndPoint );

        bytes[19] = (uint8_t)loopType;

        bytes[20] = SystemMessageEndOfExclusive;

        return MsgLenSysExNonRtSdsHeader;
    }

    inline uint8_t packSysExNonRtSdsHeaderObjSds(uint8_t *bytes, uint8_t deviceId, SysExNonRtSdsHeaderData_t *header ){
        ASSERT( header != NULL );

        return packSysExNonRtSdsHeader(bytes, deviceId, header->SampleNumber, (uint8_t)header->SampleFormat, header->SampleLength, header->SampleLength, header->LoopStartPoint, header->LoopEndPoint, header->LoopType );
    }

    inline uint8_t packSysExNonRtSdsHeaderObj(uint8_t * bytes, Message_t *msg){
        ASSERT( msg != NULL);

        return packSysExNonRtSdsHeaderObjSds(bytes, msg->Channel, &msg->Data.SysEx.Data.SampleDump.Header);
    }

    inline bool unpackSysExNonRtSdsHeader(uint8_t *bytes, uint8_t length, uint8_t *deviceId, uint16_t *sampleNumber, uint8_t *sampleFormat, uint32_t *samplePeriod, uint32_t *sampleLength, uint32_t *loopStartPoint, uint32_t *loopEndPoint, uint8_t *loopType ){
        ASSERT( bytes!= NULL );
        ASSERT( deviceId != NULL );
        ASSERT( sampleNumber != NULL );
        ASSERT( sampleFormat != NULL );
        ASSERT( samplePeriod != NULL );
        ASSERT( sampleLength != NULL );
        ASSERT( loopEndPoint != NULL );
        ASSERT( loopStartPoint != NULL );
        ASSERT( loopType!= NULL );

        if (length != MsgLenSysExNonRtSdsHeader || ! isControlByte(bytes[length-1])){
            return false;
        }
        if (bytes[0] != SystemMessageSystemExclusive || bytes[1] != SysExIdNonRealTimeByte || bytes[3] != SysExNonRtSampleDumpHeader){
            return false;
        }

        *deviceId = bytes[2];

        *sampleNumber = unpackU14( &bytes[4] );

        *sampleFormat = bytes[6];

        *samplePeriod = unpackU21( &bytes[7] );
        *sampleLength = unpackU21( &bytes[10] );
        *loopStartPoint = unpackU21( &bytes[13]);
        *loopEndPoint = unpackU21( &bytes[16] );

        *loopType = bytes[19];

        return true;
    }

    inline bool unpackSysExNonRtSdsHeaderObjSds(uint8_t *bytes, uint8_t length, uint8_t *deviceId, SysExNonRtSdsHeaderData_t *header ){
        ASSERT( header != NULL );

        return unpackSysExNonRtSdsHeader(bytes, length, deviceId, &header->SampleNumber, &header->SampleFormat, &header->SampleLength, &header->SampleLength, &header->LoopStartPoint, &header->LoopEndPoint, &header->LoopType );
    }

    inline bool unpackSysExNonRtSdsHeaderObj(uint8_t * bytes, uint8_t length, Message_t *msg){
        ASSERT( msg != NULL);

        if (unpackSysExNonRtSdsHeaderObjSds(bytes, length, &msg->Channel, &msg->Data.SysEx.Data.SampleDump.Header)){
            msg->StatusClass = StatusClassSystemMessage;
            msg->SystemMessage = SystemMessageSystemExclusive;
            msg->Data.SysEx.Id = SysExIdNonRealTime;
            msg->Data.SysEx.SubId1 = SysExNonRtSampleDumpHeader;
            return true;
        }

        return false;
    }



    inline uint8_t packSysExNonRtSdsDataPacket(uint8_t *bytes, uint8_t deviceId, uint8_t runningPacketCount, uint8_t * data, uint8_t dataLen ){

        ASSERT( bytes!= NULL );
        ASSERT( deviceId <= MaxU7 );
        ASSERT( runningPacketCount <= MaxU7 );
        ASSERT( dataLen == 0 || data != NULL );
        ASSERT( dataLen <= MaxU7 );

        uint8_t length = 5;

        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = SysExIdNonRealTimeByte;
        bytes[2] = deviceId;
        bytes[3] = SysExNonRtSampleDataPacket;

        bytes[4] = runningPacketCount;

        for (uint8_t i = 0; i < dataLen; i++, length++){
            bytes[length] = data[i];
        }

        bytes[length] = xorChecksum( bytes, length );
        length++;

        bytes[length++] = SystemMessageEndOfExclusive;

        return length; // MsgLenSysExNonRtSdsPacketMin
    }

    inline uint8_t packSysExNonRtSdsDataPacketObjSds(uint8_t *bytes, uint8_t deviceId, SysExNonRtSdsDataPacket_t * data){
        ASSERT( data != NULL );

        return packSysExNonRtSdsDataPacket(bytes, deviceId, data->RunningPacketCount, data->Data, data->Length );
    }

    inline uint8_t packSysExNonRtSdsDataPacketObj(uint8_t * bytes, Message_t *msg){
        ASSERT( msg != NULL);

        return packSysExNonRtSdsDataPacketObjSds(bytes, msg->Channel, &msg->Data.SysEx.Data.SampleDump.DataPacket);
    }


    inline bool unpackSysExNonRtSdsDataPacket(uint8_t *bytes, uint8_t length, uint8_t *deviceId, uint8_t *runningPacketCount, uint8_t * data, uint8_t * dataLen, uint8_t *checksumData, uint8_t *checksumComputed ){

        ASSERT( bytes!= NULL );
        ASSERT( deviceId != NULL );
        ASSERT( runningPacketCount != NULL );
        ASSERT( data != NULL );
        ASSERT( dataLen != NULL );
        ASSERT( checksumData != NULL );
        ASSERT( checksumComputed != NULL );


        if (length != MsgLenSysExNonRtSdsHeader || ! isControlByte(bytes[length-1]) ){
            return false;
        }
        if ( bytes[0] != SystemMessageSystemExclusive || bytes[1] != SysExIdNonRealTimeByte || bytes[3] != SysExNonRtSampleDataPacket){
            return false;
        }

        *deviceId = bytes[2];

        *runningPacketCount = bytes[4];

        *dataLen = 0;
        if (length > MsgLenSysExNonRtSdsDataPacketMin){
            *dataLen = length - MsgLenSysExNonRtSdsDataPacketMin;
            for(uint8_t p = 5, i = 0; i < *dataLen; i++, p++){
                data[i] = bytes[p];
            }
        }

        *checksumData = bytes[length-2];

        *checksumComputed =  xorChecksum( bytes, length - 2 );

        return true;
    }

    inline bool unpackSysExNonRtSampleDataPacket(uint8_t *bytes, uint8_t length, uint8_t *deviceId, SysExNonRtSdsDataPacket_t * data ){
        ASSERT( data != NULL );

        return unpackSysExNonRtSdsDataPacket(bytes, length, deviceId, &data->RunningPacketCount, data->Data, &data->Length, &data->ChecksumData, &data->ChecksumComputed );
    }

    inline bool unpackSysExNonRtSdsDataPacketObj(uint8_t * bytes, uint8_t length, Message_t *msg){
        ASSERT( msg != NULL);

        if (unpackSysExNonRtSampleDataPacket(bytes, length, &msg->Channel, &msg->Data.SysEx.Data.SampleDump.DataPacket)){
            msg->StatusClass = StatusClassSystemMessage;
            msg->SystemMessage = SystemMessageSystemExclusive;
            msg->Data.SysEx.Id = SysExIdNonRealTime;
            msg->Data.SysEx.SubId1 = SysExNonRtSampleDataPacket;
            return true;
        }

        return false;
    }

    inline uint8_t packSysExNonRtSdsRequest(uint8_t *bytes, uint8_t deviceId, uint16_t requestedSample ){

        ASSERT( bytes!= NULL );
        ASSERT( deviceId <= MaxU7 );
        ASSERT( requestedSample <= MaxU14 );

        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = SysExIdNonRealTimeByte;
        bytes[2] = deviceId;
        bytes[3] = SysExNonRtSampleDumpRequest;

        packU14( &bytes[4], requestedSample );

        bytes[6] = SystemMessageEndOfExclusive;

        return MsgLenSysExNonRtSdsRequest;
    }

    inline uint8_t packSysExNonRtSdsRequestObj(uint8_t * bytes, Message_t *msg){
        ASSERT( msg != NULL);

        return packSysExNonRtSdsRequest(bytes, msg->Channel, msg->Data.SysEx.Data.SampleDump.Request.RequestedSample);
    }


    inline bool unpackSysExNonRtSdsRequest(uint8_t *bytes, uint8_t length, uint8_t * deviceId, uint16_t * requestedSample ){

        ASSERT( bytes!= NULL );
        ASSERT( deviceId != NULL );
        ASSERT( requestedSample != NULL );

        if (length != MsgLenSysExNonRtSdsRequest || ! isControlByte(bytes[length-1]) ){
            return false;
        }
        if (bytes[0] != SystemMessageSystemExclusive || bytes[1] != SysExIdNonRealTimeByte || bytes[3] != SysExNonRtSampleDumpRequest){
            return false;
        }

        *deviceId = bytes[2];
        *requestedSample = unpackU14( &bytes[4] );

        return true;
    }

    inline bool unpackSysExNonRtSdsRequestObj(uint8_t * bytes, uint8_t length, Message_t *msg){
        ASSERT( msg != NULL);

        if (unpackSysExNonRtSdsRequest(bytes, length, &msg->Channel, &msg->Data.SysEx.Data.SampleDump.Request.RequestedSample)){
            msg->StatusClass = StatusClassSystemMessage;
            msg->SystemMessage = SystemMessageSystemExclusive;
            msg->Data.SysEx.Id = SysExIdNonRealTime;
            msg->Data.SysEx.SubId1 = SysExNonRtSampleDumpRequest;
            return true;
        }

        return false;
    }

    inline uint8_t packSysExNonRtSdsExtHeader(uint8_t *bytes, uint8_t deviceId, uint16_t sampleNumber, uint8_t sampleFormat, uint32_t sampleRateIntegerPortion, uint32_t sampleRateFractionalPortion, uint64_t sampleLength, uint64_t sustainLoopStart, uint64_t sustainLoopEnd, uint8_t loopType, uint8_t numberofChannels){
        ASSERT( bytes != NULL );
        ASSERT( deviceId <= MaxU7);
        ASSERT(sampleNumber <= MaxU14);
        ASSERT(sampleFormat <= MaxU7);
        ASSERT(sampleRateIntegerPortion <= MaxU28);
        ASSERT(sampleRateFractionalPortion <= MaxU28);
        ASSERT( sampleLength <= MaxU35);
        ASSERT(sustainLoopStart<= MaxU35);
        ASSERT(sustainLoopEnd <= MaxU35);
        ASSERT( loopType <= MaxU7);
        ASSERT( numberofChannels <= MaxU7);

        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = SysExIdNonRealTimeByte;
        bytes[2] = deviceId;
        bytes[3] = SysExNonRtSampleDumpExtension;
        bytes[4] = SysExNonRtSdsExtendedDumpHeader;

        packU14( &bytes[5], sampleNumber);

        bytes[7] = sampleFormat;

        packU28( &bytes[8], sampleRateIntegerPortion );
        packU28( &bytes[12], sampleRateFractionalPortion );

        packU35( &bytes[16], sampleLength );
        packU35( &bytes[21], sustainLoopStart );
        packU35( &bytes[26], sustainLoopEnd );

        bytes[31] = loopType;
        bytes[32] = numberofChannels;

        bytes[33] = SystemMessageEndOfExclusive;

        return MsgLenSysExNonRtSdsHeader;
    }

    inline uint8_t packSysExNonRtSdsExtHeaderObjSds(uint8_t *bytes, uint8_t deviceId, SysExNonRtSdsExtHeaderData_t *header){
        ASSERT(header!=NULL);

        return packSysExNonRtSdsExtHeader(bytes, deviceId, header->SampleNumber, header->SampleFormat, header->SampleRateIntegerPortion, header->SampleRateFractionalPortion, header->SampleLength, header->SustainLoopStart, header->SustainLoopEnd, header->LoopType, header->NumberofChannels);
    }

    inline uint8_t packSysExNonRtSdsExtHeaderObj(uint8_t *bytes, Message_t * msg){
        ASSERT( msg != NULL );
        ASSERT(msg->StatusClass == StatusClassSystemMessage);
        ASSERT(msg->SystemMessage == SystemMessageSystemExclusive);
        ASSERT(msg->Data.SysEx.Id == SysExIdNonRealTimeByte);
        ASSERT(msg->Data.SysEx.SubId1 == SysExNonRtSampleDumpExtension);
        ASSERT(msg->Data.SysEx.SubId2 == SysExNonRtSdsExtendedDumpHeader);

        return packSysExNonRtSdsExtHeaderObjSds(bytes, msg->Channel, &msg->Data.SysEx.Data.SampleDumpExt.Header);
    }

    inline bool unpackSysExNonRtSdsExtHeader(uint8_t *bytes, uint8_t length, uint8_t *deviceId, uint16_t *sampleNumber, uint8_t *sampleFormat, uint32_t *sampleRateIntegerPortion, uint32_t *sampleRateFractionalPortion, uint64_t *sampleLength, uint64_t *sustainLoopStart, uint64_t *sustainLoopEnd, uint8_t *loopType, uint8_t *numberofChannels){
        ASSERT( bytes != NULL );
        ASSERT( deviceId != NULL);
        ASSERT(sampleNumber != NULL);
        ASSERT(sampleFormat != NULL);
        ASSERT(sampleRateIntegerPortion != NULL);
        ASSERT(sampleRateFractionalPortion != NULL);
        ASSERT( sampleLength != NULL);
        ASSERT(sustainLoopStart!= NULL);
        ASSERT(sustainLoopEnd != NULL);
        ASSERT( loopType != NULL);
        ASSERT( numberofChannels != NULL);

        if (length != MsgLenSysExNonRtSdsHeader || ! isControlByte(bytes[length-1])){
            return false;
        }
        if (bytes[0] != SystemMessageSystemExclusive || bytes[1] != SysExIdNonRealTimeByte || bytes[3] != SysExNonRtSampleDumpExtension || bytes[4] != SysExNonRtSdsExtendedDumpHeader){
            return false;
        }

        *sampleNumber = unpackU14( &bytes[5]);

        *sampleFormat = bytes[7];

        *sampleRateIntegerPortion  = unpackU28( &bytes[8]);
        *sampleRateFractionalPortion = unpackU28( &bytes[12]);

        *sampleLength = unpackU35( &bytes[16]);
        *sustainLoopStart = unpackU35( &bytes[21]);
        *sustainLoopEnd = unpackU35( &bytes[26]);

        *loopType = bytes[31];
        *numberofChannels = bytes[32];

        return true;
    }

    inline bool unpackSysExNonRtSdsExtHeaderObjSds(uint8_t *bytes, uint8_t length, uint8_t *deviceId, SysExNonRtSdsExtHeaderData_t *header){
        ASSERT(header!=NULL);

        return unpackSysExNonRtSdsExtHeader(bytes, length, deviceId, &header->SampleNumber, &header->SampleFormat, &header->SampleRateIntegerPortion, &header->SampleRateFractionalPortion, &header->SampleLength, &header->SustainLoopStart, &header->SustainLoopEnd, &header->LoopType, &header->NumberofChannels);
    }

    inline bool unpackSysExNonRtSdsExtHeaderObj(uint8_t *bytes, uint8_t length, Message_t * msg){
        ASSERT( msg != NULL );

        if (unpackSysExNonRtSdsExtHeaderObjSds(bytes, length, &msg->Channel, &msg->Data.SysEx.Data.SampleDumpExt.Header)){
            msg->StatusClass = StatusClassSystemMessage;
            msg->SystemMessage = SystemMessageSystemExclusive;
            msg->Data.SysEx.Id = SysExIdNonRealTime;
            msg->Data.SysEx.SubId1 = SysExNonRtSampleDumpExtension;
            msg->Data.SysEx.SubId1 = SysExNonRtSdsExtendedDumpHeader;
            return true;
        }

        return false;
    }

    inline uint8_t packSysExNonRtSdsExtLoopPointTransmission(uint8_t *bytes, uint8_t deviceId, uint16_t sampleNumber, uint16_t loopNumber, uint8_t loopType, uint64_t loopStartAddress, uint64_t loopEndAddress){
        ASSERT(bytes!=NULL);
        ASSERT(deviceId<=MaxU7);
        ASSERT(sampleNumber <= MaxU14);
        ASSERT(loopNumber <= MaxU14);
        ASSERT( loopType <= MaxU7);
        ASSERT(loopStartAddress <= MaxU35);
        ASSERT(loopEndAddress <= MaxU35);

        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = SysExIdNonRealTimeByte;
        bytes[2] = deviceId;
        bytes[3] = SysExNonRtSampleDumpExtension;
        bytes[4] = SysExNonRtSdsExtendedLoopPointsTransmission;

        packU14( &bytes[5], sampleNumber );
        packU14( &bytes[7], loopNumber );

        bytes[9] = loopType;

        packU35( &bytes[14], loopStartAddress);
        packU35( &bytes[19], loopEndAddress);

        bytes[24] = SystemMessageEndOfExclusive;

        return MsgLenSysExNonRtSdsExtLoopPointTransmission;
    }

    inline uint8_t packSysExNonRtSdsExtLoopPointTransmissionObjSds(uint8_t *bytes, uint8_t deviceId, SysExNonRtSdsExtLoopPointTransmissionData_t * data){
        ASSERT(data!=NULL);

        return packSysExNonRtSdsExtLoopPointTransmission(bytes, deviceId, data->SampleNumber, data->LoopNumber, data->LoopType, data->LoopStartAddress, data->LoopEndAddress);
    }

    inline uint8_t packSysExNonRtSdsExtLoopPointTransmissionObj(uint8_t *bytes, Message_t *msg){
        ASSERT( msg != NULL);
        ASSERT( msg->StatusClass == StatusClassSystemMessage);
        ASSERT(msg->SystemMessage == SystemMessageSystemExclusive);
        ASSERT(msg->Data.SysEx.Id == SysExIdNonRealTime);
        ASSERT(msg->Data.SysEx.SubId1 == SysExNonRtSampleDumpExtension);
        ASSERT(msg->Data.SysEx.SubId2 == SysExNonRtSdsExtendedLoopPointsTransmission);

        return packSysExNonRtSdsExtLoopPointTransmissionObjSds(bytes, msg->Channel, &msg->Data.SysEx.Data.SampleDumpExt.LoopPointTransmission);
    }


    inline bool unpackSysExNonRtSdsExtLoopPointTransmission(uint8_t *bytes, uint8_t length, uint8_t *deviceId, uint16_t *sampleNumber, uint16_t *loopNumber, uint8_t *loopType, uint64_t *loopStartAddress, uint64_t *loopEndAddress){
        ASSERT(bytes!=NULL);
        ASSERT(deviceId!=NULL);
        ASSERT(sampleNumber !=NULL);
        ASSERT(loopNumber !=NULL);
        ASSERT( loopType !=NULL);
        ASSERT(loopStartAddress !=NULL);
        ASSERT(loopEndAddress !=NULL);


        if (length != MsgLenSysExNonRtSdsExtLoopPointTransmission || ! isControlByte(bytes[length-1])){
            return false;
        }
        if (bytes[0] != SystemMessageSystemExclusive || bytes[1] != SysExIdNonRealTimeByte || bytes[3] != SysExNonRtSampleDumpExtension || bytes[4] != SysExNonRtSdsExtendedLoopPointsTransmission){
            return false;
        }

        *sampleNumber = unpackU14( &bytes[5]);
        *loopNumber = unpackU14( &bytes[7]);

        *loopType = bytes[9];

        *loopStartAddress = unpackU35( &bytes[14]);
        *loopEndAddress = unpackU35( &bytes[19]);

        return true;
    }

    inline bool unpackSysExNonRtSdsExtLoopPointTransmissionObjSds(uint8_t *bytes, uint8_t length, uint8_t *deviceId, SysExNonRtSdsExtLoopPointTransmissionData_t * data){
        ASSERT(data!=NULL);

        return unpackSysExNonRtSdsExtLoopPointTransmission(bytes, length, deviceId, &data->SampleNumber, &data->LoopNumber, &data->LoopType, &data->LoopStartAddress, &data->LoopEndAddress);
    }

    inline bool unpackSysExNonRtSdsExtLoopPointTransmissionObj(uint8_t *bytes, uint8_t length,  Message_t *msg){
        ASSERT( msg != NULL);

        if (unpackSysExNonRtSdsExtLoopPointTransmissionObjSds(bytes, length, &msg->Channel, &msg->Data.SysEx.Data.SampleDumpExt.LoopPointTransmission)){
            msg->StatusClass = StatusClassSystemMessage;
            msg->SystemMessage = SystemMessageSystemExclusive;
            msg->Data.SysEx.Id = SysExIdNonRealTime;
            msg->Data.SysEx.SubId1 = SysExNonRtSampleDumpExtension;
            msg->Data.SysEx.SubId2 = SysExNonRtSdsExtendedLoopPointsTransmission;
            return true;
        }

        return false;
    }

    inline uint8_t packSysExNonRtSdsExtLoopPointRequest(uint8_t *bytes, uint8_t deviceId, uint16_t sampleNumber, uint16_t loopNumber){
        ASSERT( bytes != NULL);
        ASSERT(deviceId <= MaxU7);
        ASSERT(sampleNumber <= MaxU14);
        ASSERT(loopNumber <= MaxU14);

        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = SysExIdNonRealTimeByte;
        bytes[2] = deviceId;
        bytes[3] = SysExNonRtSampleDumpExtension;
        bytes[4] = SysExNonRtSdsLoopPointsRequest;

        packU14( &bytes[5], sampleNumber );
        packU14( &bytes[7], loopNumber );

        bytes[9] = SystemMessageEndOfExclusive;

        return MsgLenSysExNonRtSdsExtLoopPointRequest;
    }

    inline uint8_t packSysExNonRtSdsExtLoopPointRequestObj(uint8_t *bytes, Message_t *msg){
        ASSERT(msg != NULL);
        ASSERT(msg->StatusClass == StatusClassSystemMessage);
        ASSERT(msg->SystemMessage == SystemMessageSystemExclusive);
        ASSERT(msg->Data.SysEx.Id == SysExIdNonRealTime);
        ASSERT(msg->Data.SysEx.SubId1 == SysExNonRtSampleDumpExtension);
        ASSERT(msg->Data.SysEx.SubId2 == SysExNonRtSdsLoopPointsRequest);

        return packSysExNonRtSdsExtLoopPointRequest( bytes, msg->Channel, msg->Data.SysEx.Data.SampleDumpExt.LoopPointRequest.SampleNumber, msg->Data.SysEx.Data.SampleDumpExt.LoopPointRequest.LoopNumber);
    }


    inline bool unpackSysExNonRtSdsExtLoopPointRequest(uint8_t *bytes, uint8_t length, uint8_t *deviceId, uint16_t *sampleNumber, uint16_t *loopNumber){
        ASSERT( bytes != NULL);
        ASSERT(deviceId != NULL);
        ASSERT(sampleNumber != NULL);
        ASSERT(loopNumber != NULL);

        if (length != MsgLenSysExNonRtSdsExtLoopPointRequest || ! isControlByte(bytes[length-1]) ){
            return false;
        }
        if (bytes[0] != SystemMessageSystemExclusive || bytes[1] != SysExIdNonRealTimeByte || bytes[3] != SysExNonRtSampleDumpExtension || bytes[4] != SysExNonRtSdsLoopPointsRequest){
            return false;
        }

        *deviceId = bytes[2];

        *sampleNumber = unpackU14( &bytes[5]);
        *loopNumber = unpackU14( &bytes[7]);
        
        return true;
    }

    inline bool unpackSysExNonRtSdsExtLoopPointRequestObj(uint8_t *bytes, uint8_t length, Message_t *msg){
        ASSERT(msg != NULL);

        if (unpackSysExNonRtSdsExtLoopPointRequest( bytes, length, &msg->Channel, &msg->Data.SysEx.Data.SampleDumpExt.LoopPointRequest.SampleNumber, &msg->Data.SysEx.Data.SampleDumpExt.LoopPointRequest.LoopNumber)){
            msg->StatusClass = StatusClassSystemMessage;
            msg->SystemMessage = SystemMessageSystemExclusive;
            msg->Data.SysEx.Id = SysExIdNonRealTime;
            msg->Data.SysEx.SubId1 = SysExNonRtSampleDumpExtension;
            msg->Data.SysEx.SubId2 = SysExNonRtSdsLoopPointsRequest;
            return true;
        }

        return false;
    }

    inline uint8_t packSysExNonRtSdsExtSampleNameTransmission(uint8_t *bytes, uint8_t deviceId, uint16_t sampleNumber, uint8_t languageTagLength, uint8_t * languageTag, uint8_t nameLength, uint8_t *name){
        ASSERT(bytes!=NULL);
        ASSERT(deviceId<=MaxU7);
        ASSERT(sampleNumber <= MaxU14);
        ASSERT(languageTagLength <= MaxU7);
        ASSERT( languageTagLength == 0 || languageTag != NULL);
        ASSERT(nameLength <= MaxU7);
        ASSERT(nameLength == 0 || name != NULL);

        uint8_t length = 7;

        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = SysExIdNonRealTimeByte;
        bytes[2] = deviceId;
        bytes[3] = SysExNonRtSampleDumpExtension;
        bytes[4] = SysExNonRtSdsSampleNameTransmission;

        packU14( &bytes[5], sampleNumber );

        bytes[6] = languageTagLength;

        for(uint8_t i = 0; i < languageTagLength; i++){
            bytes[length++] = languageTag[i];
        }

        bytes[length++] = nameLength;

        for(uint8_t i = 0; i < nameLength; i++){
            bytes[length++] = name[i];
        }

        bytes[length++] = SystemMessageEndOfExclusive;

        return length;
    }

    inline uint8_t packSysExNonRtSdsExtSampleNameTransmissionObjSds(uint8_t *bytes, uint8_t deviceId, SysExNonRtSdsExtNameTransmissionData_t * data){
        ASSERT(data!=NULL);

        return packSysExNonRtSdsExtSampleNameTransmission(bytes, deviceId, data->SampleNumber, data->LanguageTagLength, data->LanguageTag, data->NameLength, data->Name);
    }

    inline uint8_t packSysExNonRtSdsExtSampleNameTransmissionObj(uint8_t *bytes, Message_t *msg){
        ASSERT( msg != NULL);
        ASSERT( msg->StatusClass == StatusClassSystemMessage);
        ASSERT(msg->SystemMessage == SystemMessageSystemExclusive);
        ASSERT(msg->Data.SysEx.Id == SysExIdNonRealTime);
        ASSERT(msg->Data.SysEx.SubId1 == SysExNonRtSampleDumpExtension);
        ASSERT(msg->Data.SysEx.SubId2 == SysExNonRtSdsSampleNameTransmission);

        return packSysExNonRtSdsExtSampleNameTransmissionObjSds(bytes, msg->Channel, &msg->Data.SysEx.Data.SampleDumpExt.NameTransmission);
    }

    inline bool unpackSysExNonRtSdsExtSampleNameTransmission(uint8_t *bytes, uint8_t length, uint8_t *deviceId, uint16_t *sampleNumber, uint8_t *languageTagLength, uint8_t * languageTag, uint8_t *nameLength, uint8_t *name){
        ASSERT(bytes!=NULL);
        ASSERT(deviceId!= NULL);
        ASSERT(sampleNumber != NULL);
        ASSERT(languageTagLength != NULL);
        ASSERT(languageTag != NULL);
        ASSERT(nameLength != NULL);
        ASSERT(name != NULL);

        uint8_t pos = 7;

        if (length < 8 || ! isControlByte(bytes[length-1])){
            return false;
        }
        if (bytes[0] != SystemMessageSystemExclusive || bytes[1] != SysExIdNonRealTimeByte || bytes[3] != SysExNonRtSampleDumpExtension || bytes[4] != SysExNonRtSdsSampleNameTransmission){
            return false;
        }

        *deviceId = bytes[2];
        *sampleNumber = unpackU14( &bytes[5]);

        *languageTagLength = bytes[6];

        for(uint8_t i = 0; i < *languageTagLength; i++){
            languageTag[i] = bytes[pos++];
        }

        *nameLength = bytes[pos++];

        for(uint8_t i = 0; i < *nameLength; i++){
            name[i] = bytes[pos++];
        }

        return true;
    }

    inline bool unpackSysExNonRtSdsExtSampleNameTransmissionObjSds(uint8_t *bytes, uint8_t length, uint8_t * deviceId, SysExNonRtSdsExtNameTransmissionData_t * data){
        ASSERT(data!=NULL);

        return unpackSysExNonRtSdsExtSampleNameTransmission(bytes, length, deviceId, &data->SampleNumber, &data->LanguageTagLength, data->LanguageTag, &data->NameLength, data->Name);
    }

    inline bool unpackSysExNonRtSdsExtSampleNameTransmissionObj(uint8_t *bytes,uint8_t length, Message_t *msg){
        ASSERT( msg != NULL);

        if (unpackSysExNonRtSdsExtSampleNameTransmissionObjSds(bytes, length, &msg->Channel, &msg->Data.SysEx.Data.SampleDumpExt.NameTransmission)){
            msg->StatusClass = StatusClassSystemMessage;
            msg->SystemMessage = SystemMessageSystemExclusive;
            msg->Data.SysEx.Id = SysExIdNonRealTime;
            msg->Data.SysEx.SubId1 = SysExNonRtSampleDumpExtension;
            msg->Data.SysEx.SubId2 = SysExNonRtSdsSampleNameTransmission;
            return true;
        }

        return false;
    }


    inline uint8_t packSysExNonRtSdsExtSampleNameRequest(uint8_t *bytes, uint8_t deviceId, uint16_t sampleNumber){
        ASSERT( bytes != NULL);
        ASSERT(deviceId <= MaxU7);
        ASSERT(sampleNumber <= MaxU14);

        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = SysExIdNonRealTimeByte;
        bytes[2] = deviceId;
        bytes[3] = SysExNonRtSampleDumpExtension;
        bytes[4] = SysExNonRtSdsSampleNameRequest;

        packU14( &bytes[5], sampleNumber );

        bytes[7] = SystemMessageEndOfExclusive;

        return MsgLenSysExNonRtSdsExtSampleNameRequest;
    }

    inline uint8_t packSysExNonRtSdsExtSampleNameRequestObj(uint8_t *bytes, Message_t *msg){
        ASSERT(msg != NULL);
        ASSERT(msg->StatusClass == StatusClassSystemMessage);
        ASSERT(msg->SystemMessage == SystemMessageSystemExclusive);
        ASSERT(msg->Data.SysEx.Id == SysExIdNonRealTime);
        ASSERT(msg->Data.SysEx.SubId1 == SysExNonRtSampleDumpExtension);
        ASSERT(msg->Data.SysEx.SubId2 == SysExNonRtSdsSampleNameRequest);

        return packSysExNonRtSdsExtSampleNameRequest( bytes, msg->Channel, msg->Data.SysEx.Data.SampleDumpExt.NameRequest.SampleNumber);
    }

    inline bool unpackSysExNonRtSdsExtSampleNameRequest(uint8_t *bytes, uint8_t length, uint8_t *deviceId, uint16_t *sampleNumber){
        ASSERT( bytes != NULL);
        ASSERT(deviceId != NULL);
        ASSERT(sampleNumber != NULL);

        if (length != MsgLenSysExNonRtSdsExtSampleNameRequest || ! isControlByte(bytes[length-1])){
            return false;
        }
        if (bytes[0] != SystemMessageSystemExclusive || bytes[1] != SysExIdNonRealTimeByte || bytes[3] != SysExNonRtSampleDumpExtension || bytes[4] != SysExNonRtSdsSampleNameRequest){
            return false;
        }

        *deviceId = bytes[2];

        *sampleNumber = unpackU14( &bytes[5]);

        return true;
    }

    inline bool unpackSysExNonRtSdsExtSampleNameRequestObj(uint8_t *bytes, uint8_t length, Message_t *msg){
        ASSERT(msg != NULL);

        if (unpackSysExNonRtSdsExtSampleNameRequest( bytes, length, &msg->Channel, &msg->Data.SysEx.Data.SampleDumpExt.NameRequest.SampleNumber)){
            msg->StatusClass = StatusClassSystemMessage;
            msg->SystemMessage = SystemMessageSystemExclusive;
            msg->Data.SysEx.Id = SysExIdNonRealTime;
            msg->Data.SysEx.SubId1 = SysExNonRtSampleDumpExtension;
            msg->Data.SysEx.SubId2 = SysExNonRtSdsSampleNameRequest;
            return true;
        }

        return false;
    }


    inline uint8_t packSysExNonRtSdsExtObj(uint8_t *bytes, Message_t *msg){
        ASSERT( msg != NULL );
        ASSERT( isSysExNonRtSds(msg->Data.SysEx.SubId2) );

        switch(msg->Data.SysEx.SubId2){
            case SysExNonRtSdsExtendedDumpHeader:
                return packSysExNonRtSdsHeaderObj(bytes, msg);

            case SysExNonRtSdsSampleNameTransmission:
                return packSysExNonRtSdsExtLoopPointTransmissionObj( bytes, msg);

            case SysExNonRtSdsSampleNameRequest:
                return packSysExNonRtSdsExtLoopPointRequestObj(bytes, msg);

            case SysExNonRtSdsExtendedLoopPointsTransmission:
                return packSysExNonRtSdsExtSampleNameTransmissionObj(bytes, msg);

            case SysExNonRtSdsExtendedLoopPointsRequest:
                return packSysExNonRtSdsExtSampleNameRequestObj(bytes, msg);

        }

        return 0;
    }


    inline uint8_t unpackSysExNonRtSdsExtObj(uint8_t *bytes, uint8_t length, Message_t *msg){
        ASSERT( msg != NULL );


        switch(bytes[3]){
            case SysExNonRtSdsExtendedDumpHeader:
                return unpackSysExNonRtSdsHeaderObj(bytes, length, msg);

            case SysExNonRtSdsSampleNameTransmission:
                return unpackSysExNonRtSdsExtLoopPointTransmissionObj( bytes, length, msg);

            case SysExNonRtSdsSampleNameRequest:
                return unpackSysExNonRtSdsExtLoopPointRequestObj(bytes, length, msg);

            case SysExNonRtSdsExtendedLoopPointsTransmission:
                return unpackSysExNonRtSdsExtSampleNameTransmissionObj(bytes, length, msg);

            case SysExNonRtSdsExtendedLoopPointsRequest:
                return unpackSysExNonRtSdsExtSampleNameRequestObj(bytes, length, msg);

        }

        return false;
    }

#ifdef __cplusplus
    } // extern "C"
} // namespace MidiMessage
#endif

#endif //MIDIMESSAGE_PACKERS_H
