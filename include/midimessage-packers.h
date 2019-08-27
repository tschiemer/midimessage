/**
 * \file
 */

#ifndef MIDIMESSAGE_PACKERS_H
#define MIDIMESSAGE_PACKERS_H

#include "midimessage.h"

namespace MidiMessage {


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

    inline uint8_t packNoteOff(uint8_t *bytes, Message_t *msg) {
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

    inline bool unpackNoteOff(uint8_t *bytes, uint8_t len, Message_t *msg) {
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

    inline uint8_t packNoteOn(uint8_t *bytes, Message_t *msg) {
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

    inline bool unpackNoteOn(uint8_t *bytes, uint8_t len, Message_t *msg) {
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

    inline uint8_t packNote(uint8_t *bytes, Message_t *msg) {
        ASSERT(msg != NULL);

        if (msg->StatusClass == StatusClassNoteOff) {
            return packNoteOff(bytes, msg);
        }
        if (msg->StatusClass == StatusClassNoteOn) {
            return packNoteOn(bytes, msg);
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

    inline bool unpackNote(uint8_t *bytes, uint8_t len, Message_t *msg) {
        ASSERT(msg != NULL);

        if (unpackNoteOff(bytes, len, msg)) {
            return true;
        }

        if (unpackNoteOn(bytes, len, msg)) {
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

    inline uint8_t packPolyphonicKeyPressure(uint8_t *bytes, Message_t *msg) {
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

    inline bool unpackPolyphonicKeyPressure(uint8_t *bytes, uint8_t len, Message_t *msg) {
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
        ASSERT((value & DataMask) == value);

        bytes[0] = StatusClassControlChange | (channel & ChannelMask);
        bytes[1] = controller & DataMask;
        bytes[2] = value & DataMask;

        return MsgLenControlChange;
    }

    inline uint8_t packControlChange(uint8_t *bytes, Message_t *msg) {
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

    inline bool unpackControlChange(uint8_t *bytes, uint8_t len, Message_t *msg) {
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


    inline uint8_t packProgramChange(uint8_t *bytes, Message_t *msg) {
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

    inline bool unpackProgramChange(uint8_t *bytes, uint8_t len, Message_t *msg) {
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

    inline uint8_t packChannelPressure(uint8_t *bytes, Message_t *msg) {
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

    inline bool unpackChannelPressure(uint8_t *bytes, uint8_t len, Message_t *msg) {
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
        ASSERT(pitch <= MaxDoubleValue);

        bytes[0] = StatusClassPitchBendChange | (channel & ChannelMask);
        packDoubleValue(&bytes[1], pitch);

        return MsgLenPitchBendChange;
    }

    inline uint8_t packPitchBendChange(uint8_t *bytes, Message_t *msg) {
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
        *pitch = unpackDoubleValue(&bytes[1]);

        return true;
    }

    inline bool unpackPitchBendChange(uint8_t *bytes, uint8_t len, Message_t *msg) {
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

    inline bool isAllSoundOff(uint8_t *bytes) {
        ASSERT(bytes != NULL);

        return isAllSoundOff(bytes[0], bytes[1], bytes[2]);
    }

    inline bool isAllSoundOff(Message_t *msg) {
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

    inline bool isResetAllControllers(uint8_t *bytes) {
        ASSERT(bytes != NULL);

        return isResetAllControllers(bytes[0], bytes[1], bytes[2]);
    }

    inline bool isResetAllControllers(Message_t *msg) {
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

    inline bool isLocalControl(uint8_t *bytes) {
        ASSERT(bytes != NULL);

        return isLocalControl(bytes[0], bytes[1]);
    }

    inline bool isLocalControl(Message_t *msg) {
        ASSERT(msg != NULL);

        return isLocalControl(msg->StatusClass, msg->Data.ControlChange.Controller);
    }

    inline bool getLocalControlState(uint8_t *bytes) {
        ASSERT(bytes != NULL);
        ASSERT(isLocalControl(bytes));

        return (bytes[3] & DataMask) > 0;
    }

    inline bool getLocalControlState(Message_t *msg) {
        ASSERT(msg != NULL);
        ASSERT(isLocalControl(msg));

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

    inline bool isAllNotesOff(uint8_t *bytes) {
        ASSERT(bytes != NULL);

        return isAllNotesOff(bytes[0], bytes[1], bytes[2]);
    }

    inline bool isAllNotesOff(Message_t *msg) {
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

    inline bool isOmniMode(uint8_t *bytes) {
        ASSERT(bytes != NULL);

        return isOmniMode(bytes[0], bytes[1]);
    }

    inline bool isOmniMode(Message_t *msg) {
        ASSERT(msg != NULL);

        return isOmniMode(msg->StatusClass, msg->Data.ControlChange.Controller);
    }

    inline bool getOmniModeState(uint8_t *bytes) {
        ASSERT(bytes != NULL);
        ASSERT(isOmniMode(bytes));

        return bytes[3] == ChannelModeControllerOmniModeOn;
    }

    inline bool getOmniModeState(Message_t *msg) {
        ASSERT(msg != NULL);
        ASSERT(isOmniMode(msg));

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

    inline bool isMonoMode(uint8_t *bytes) {
        ASSERT(bytes != NULL);

        return isMonoMode(bytes[0], bytes[1]);
    }

    inline bool isMonoMode(Message_t *msg) {
        ASSERT(msg != NULL);

        return isMonoMode(msg->StatusClass, msg->Data.ControlChange.Controller);
    }

    inline uint8_t getMonoModeNumberOfChannels(uint8_t *bytes) {
        ASSERT(bytes != NULL);
        ASSERT(isMonoMode(bytes[0], bytes[1]));

        return bytes[2] & DataMask;
    }

    inline uint8_t getMonoModeNumberOfChannels(Message_t *msg) {
        ASSERT(msg != NULL);
        ASSERT(isMonoMode(msg));

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

    inline bool isPolyModeOn(uint8_t *bytes) {
        ASSERT(bytes != NULL);

        return isPolyModeOn(bytes[0], bytes[1], bytes[2]);
    }

    inline bool isPolyModeOn(Message_t *msg) {
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

    inline uint8_t packMtcQuarterFrame(uint8_t *bytes, Message_t *msg) {
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

    inline bool unpackMtcQuarterFrame(uint8_t *bytes, uint8_t len, Message_t *msg) {
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
        ASSERT(position <= MaxDoubleValue);

        bytes[0] = SystemMessageSongPositionPointer;
        packDoubleValue(&bytes[1], position);

        return MsgLenSongPositionPointer;
    }

    inline uint8_t packSongPositionPointer(uint8_t *bytes, Message_t *msg) {
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

        *position = unpackDoubleValue(&bytes[1]);

        return true;
    }

    inline bool unpackSongPositionPointer(uint8_t *bytes, uint8_t len, Message_t *msg) {
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

    inline uint8_t packSongSelect(uint8_t *bytes, Message_t *msg) {
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

    inline bool unpackSongSelect(uint8_t *bytes, uint8_t len, Message_t *msg) {
        ASSERT(msg != NULL);

        if (unpackSongSelect(bytes, len, &msg->Data.SongSelect.Song)) {
            msg->StatusClass = StatusClassSystemMessage;
            msg->SystemMessage = SystemMessageSongSelect;
            return true;
        }

        return false;
    }


///////////// System Exclusive Messages             /////////////
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

    inline uint8_t packSysExExperimentalMessage(uint8_t *bytes, Message_t *msg) {
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

    inline bool unpackSysExExperimentalMessage(uint8_t *bytes, uint8_t len, Message_t *msg) {
        ASSERT(msg != NULL);

        if (unpackSysExExperimentalMessage(bytes, len, msg->Data.SysEx.ByteData, &msg->Data.SysEx.Length)) {
            msg->StatusClass = StatusClassSystemMessage;
            msg->SystemMessage = SystemMessageSystemExclusive;
            msg->Data.SysEx.Id = SysExIdExperimental;
            return true;
        }

        return false;
    }


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

    inline uint8_t packSysExManufacturerMessage( uint8_t *bytes, Message_t * msg ){
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

    inline bool unpackSysExManufacturerMessage( uint8_t *bytes, uint8_t length, Message_t * msg ){
        ASSERT( msg != NULL );

        if ( unpackSysExManufacturerMessage(bytes, length, &msg->Data.SysEx.Id, msg->Data.SysEx.ByteData, &msg->Data.SysEx.Length) ){
            msg->StatusClass = StatusClassSystemMessage;
            msg->SystemMessage = SystemMessageSystemExclusive;
            return true;
        }

        return false;
    }

    inline uint8_t packSysExRtMtcFullMessage(uint8_t *bytes, uint8_t deviceId, uint8_t fps, uint8_t hour, uint8_t minute,
                                           uint8_t second, uint8_t frame) {
        ASSERT(bytes != NULL);
        ASSERT(deviceId <= MaxValue);
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

    inline uint8_t packSysExRtMtcFullMessage(uint8_t *bytes, uint8_t deviceId, MidiTimeCode_t *mtc) {
        ASSERT(bytes != NULL);
        ASSERT(mtc != NULL);

        return packSysExRtMtcFullMessage(bytes, deviceId, getFps(mtc->FpsHour), getHour(mtc->FpsHour), mtc->Minute,
                                       mtc->Second, mtc->Frame);
    }

    inline uint8_t packSysExRtMtcFullMessage(uint8_t *bytes, Message_t *msg) {
        ASSERT(msg != NULL);

        return packSysExRtMtcFullMessage(bytes, msg->Channel, &msg->Data.SysEx.Data.MidiTimeCode);
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

    inline bool unpackSysExRtMtcFullMessage(uint8_t *bytes, uint8_t len, uint8_t *deviceId, MidiTimeCode_t *mtc) {
        ASSERT(bytes != NULL);
        ASSERT(deviceId != NULL);
        ASSERT(mtc != NULL);

        MtcFrameRate_t fps = MtcFrameRate24fps;

        if (!unpackSysExRtMtcFullMessage(bytes, len, deviceId, (uint8_t *) &fps, &mtc->FpsHour, &mtc->Minute,
                                       &mtc->Second, &mtc->Frame)) {
            return false;
        }

        mtc->FpsHour |= setFps(fps);
        mtc->FractionalFrame = 0;

        return true;
    }

    inline bool unpackSysExRtMtcFullMessage(uint8_t *bytes, uint8_t len, Message_t *msg) {
        ASSERT(msg != NULL);

        if (unpackSysExRtMtcFullMessage(bytes, len, &msg->Channel, &msg->Data.SysEx.Data.MidiTimeCode)) {
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
        ASSERT(deviceId <= MaxValue);
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

    inline uint8_t packSysExRtMtcUserBits(uint8_t *bytes, Message_t *msg) {
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

    inline bool unpackSysExRtMtcUserBits(uint8_t *bytes, uint8_t len, Message_t *msg) {
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

    inline uint8_t packSysExRtMidiTimeCode( uint8_t *bytes, Message_t *msg){
        ASSERT( msg != NULL );


        switch (msg->Data.SysEx.SubId2) {

            case SysExRtMtcFullMessage:
                return packSysExRtMtcFullMessage(bytes, msg);

            case SysExRtMtcUserBits:
                return packSysExRtMtcUserBits(bytes, msg);

        }

        ASSERT( false ); // should never reach here

        return 0;
    }

    inline bool unpackSysExRtMidiTimeCode(uint8_t * bytes, uint8_t length, Message_t *msg){
        ASSERT( msg != NULL );

        if (length < 5) {
            return false;
        }

        switch (bytes[4]) {
            case SysExRtMtcFullMessage:
                return unpackSysExRtMtcFullMessage( bytes, length, msg );

            case SysExRtMtcUserBits:
                return unpackSysExRtMtcUserBits(bytes, length, msg);
        }
        return false;
    }


    inline uint8_t packSysExNonRtMtcCueingSetupMessage(uint8_t * bytes, uint8_t msgType, uint8_t deviceId,
                                                       uint8_t fps, uint8_t hour, uint8_t minute, uint8_t second,
                                                       uint8_t frame, uint8_t fractionalFrame, uint16_t eventNumber,
                                                       uint8_t * addInfo, uint8_t addInfoLen) {
        ASSERT(isSysExNonRtMtc(msgType));
        ASSERT(!isSysExNonRtMtcWithAddInfo(msgType) || addInfoLen == 0);
        ASSERT(bytes != NULL);
        ASSERT(deviceId <= MaxValue);
        ASSERT(isMtcFrameRate(fps));
        ASSERT(hour <= MtcMaxHour);
        ASSERT(minute <= MtcMaxMinute);
        ASSERT(second <= MtcMaxSecond);
        ASSERT(frame <= MtcMaxFps[fps]);
        ASSERT(fractionalFrame <= MtcMaxFractionalFrame);
        ASSERT(eventNumber <= MaxEventNumber);

        int msgLen = MsgLenSysExNonRtMtcCueingSetupMessageMin - 1; // min message length (w/o EOX)

        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = SysExIdNonRealTimeByte;
        bytes[2] = deviceId & DataMask;
        bytes[3] = SysExNonRtMidiTimeCode;
        bytes[4] = msgType;
        bytes[5] = ((fps << MtcFpsOffset) & MtcFpsMask) | (hour & MtcHourMask);
        bytes[6] = minute & MtcMinuteMask;
        bytes[7] = second & MtcSecondMask;
        bytes[8] = frame & MtcFrameMask;
        bytes[9] = fractionalFrame & MtcFractionalFrameMask;

        packDoubleValue(&bytes[10], eventNumber);

        if (isSysExNonRtMtcWithAddInfo(msgType)) {
            msgLen += nibblize(&bytes[7], addInfo, addInfoLen);
        }

        bytes[msgLen++] = SystemMessageEndOfExclusive;

        return msgLen;
    }

    inline uint8_t packSysExNonRtMtcCueingSetupMessage(uint8_t * bytes, uint8_t msgType, uint8_t deviceId,
                                                       MtcCueingData_t * cueing, uint8_t * addInfo,
                                                       uint8_t addInfoLen) {
        ASSERT( cueing != NULL );

        return packSysExNonRtMtcCueingSetupMessage(bytes, msgType, deviceId, getFps(cueing->MidiTimeCode.FpsHour),
                                                   cueing->MidiTimeCode.FpsHour & MtcHourMask, cueing->MidiTimeCode.Minute, cueing->MidiTimeCode.Second, cueing->MidiTimeCode.Frame,
                                                   cueing->MidiTimeCode.FractionalFrame, cueing->EventNumber, addInfo, addInfoLen);
    }

    inline uint8_t packSysExNonRtMtcCueingSetupMessage( uint8_t * bytes, Message_t * msg ){
        ASSERT( msg != NULL );

        return packSysExNonRtMtcCueingSetupMessage( bytes, msg->Data.SysEx.SubId2, msg->Channel, &msg->Data.SysEx.Data.Cueing, msg->Data.SysEx.ByteData, msg->Data.SysEx.Length );
    }

    inline bool unpackSysExNonRtMtcCueingSetupMessage(uint8_t * bytes, uint8_t len, uint8_t  * msgType,
                                                      uint8_t * deviceId, uint8_t * fps, uint8_t * hour, uint8_t * minute,
                                                      uint8_t * second, uint8_t * frame, uint8_t * fractionalFrame,
                                                      uint16_t * eventNumber, uint8_t * addInfo, uint8_t * addInfoLen) {
        ASSERT(bytes != NULL);
        ASSERT(deviceId != NULL);
        ASSERT(fps != NULL);
        ASSERT(hour != NULL);
        ASSERT(minute != NULL);
        ASSERT(second != NULL);
        ASSERT(frame != NULL);
        ASSERT(fractionalFrame != NULL);
        ASSERT(eventNumber != NULL);
        ASSERT(addInfo != NULL);
        ASSERT(addInfoLen != NULL);


        if (len < MsgLenSysExNonRtMtcCueingSetupMessageMin - 1) {
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
        if (bytes[3] != SysExNonRtMidiTimeCode) {
            return false;
        }
        if (!isSysExNonRtMtc(bytes[4])) {
            return false;
        }

        *deviceId = bytes[2] & DataMask;
        *msgType = (SysExNonRtMtc_t) (bytes[4]);
        *fps = (bytes[5] >> 5) & 0b11;
        *hour = bytes[5] & MtcHourMask;
        *minute = bytes[6] & MtcMinuteMask;
        *second = bytes[7] & MtcSecondMask;
        *frame = bytes[8] & MtcFrameMask;
        *fractionalFrame = bytes[9] & MtcFractionalFrameMask;

        *eventNumber = unpackDoubleValue(&bytes[10]);

        if (isSysExNonRtMtc(bytes[4])) {

            if (bytes[len - 1] != SystemMessageEndOfExclusive) {
                len++;
            }

            *addInfoLen = denibblize(addInfo, &bytes[12], len - MsgLenSysExNonRtMtcCueingSetupMessageMin);
        }

        return true;
    }

    inline bool unpackSysExNonRtMtcCueingSetupMessage( uint8_t * bytes, uint8_t len, uint8_t * msgType, uint8_t * deviceId, MtcCueingData_t * cueing, uint8_t *addInfo, uint8_t *addInfoLen){
        ASSERT( cueing != NULL );

        MtcFrameRate_t fps = MtcFrameRate24fps;

        if ( unpackSysExNonRtMtcCueingSetupMessage( bytes, len, msgType, deviceId, (uint8_t*)&fps, &cueing->MidiTimeCode.FpsHour, &cueing->MidiTimeCode.Minute, &cueing->MidiTimeCode.Second, &cueing->MidiTimeCode.Frame, &cueing->MidiTimeCode.FractionalFrame, &cueing->EventNumber, addInfo, addInfoLen) ){
            return false;
        }

        cueing->MidiTimeCode.FpsHour |= setFps(fps);

        return true;
    }

    inline bool unpackSysExNonRtMtcCueingSetupMessage(uint8_t * bytes, uint8_t len, Message_t * msg ){
        ASSERT( msg != NULL );

        if ( unpackSysExNonRtMtcCueingSetupMessage(bytes, len, &msg->Channel, &msg->Data.SysEx.SubId2, &msg->Data.SysEx.Data.Cueing, msg->Data.SysEx.ByteData, &msg->Data.SysEx.Length ) ){
            msg->StatusClass = StatusClassSystemMessage;
            msg->SystemMessage = SystemMessageSystemExclusive;
            msg->Data.SysEx.Id = SysExIdNonRealTimeByte;
            msg->Data.SysEx.SubId1 = SysExNonRtMidiTimeCode;
            return true;
        }

        return false;
    }


    inline uint8_t packSysExRtMtcCueingSetupMessage(uint8_t *bytes, uint8_t msgType, uint8_t deviceId,
                                                    uint16_t eventNumber, uint8_t *addInfo, uint32_t addInfoLen) {
        ASSERT( isSysExRtMtcCueing(msgType) );
        ASSERT( addInfoLen == 0 || !isSysExRtMtcCueing(msgType)  );
        ASSERT( bytes != NULL );
        ASSERT( deviceId <= MaxValue );
        ASSERT( eventNumber <= MaxEventNumber );

        int msgLen = MsgLenSysExRtMtcCueingSetupMessageMin - 1; // min message length (w/o EOX)

        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = SysExIdRealTimeByte;
        bytes[2] = deviceId & DataMask;
        bytes[3] = SysExNonRtMidiTimeCode;
        bytes[4] = msgType;

        packDoubleValue(&bytes[5], eventNumber);

        if (isSysExRtMtcCueingWithAddInfo(msgType)) {
            msgLen += nibblize(&bytes[7], addInfo, addInfoLen);
        }

        bytes[msgLen++] = SystemMessageEndOfExclusive;

        return msgLen;
    }

    inline uint8_t packSysExRtMtcCueingSetupMessage( uint8_t * bytes, Message_t * msg ){
        ASSERT( msg != NULL );

        return packSysExRtMtcCueingSetupMessage( bytes, msg->Data.SysEx.SubId2, msg->Channel, msg->Data.SysEx.Data.Cueing.EventNumber, msg->Data.SysEx.ByteData, msg->Data.SysEx.Length );
    }


    inline bool unpackSysExRtMtcCueingSetupMessage(uint8_t *bytes, uint8_t len, uint8_t *msgType,
                                                   uint8_t *deviceId, uint16_t *eventNumber, uint8_t *addInfo,
                                                   uint8_t *addInfoLen) {
        ASSERT(bytes != NULL);
        ASSERT(deviceId != NULL);
        ASSERT(eventNumber != NULL);
        ASSERT(addInfo != NULL);
        ASSERT(addInfoLen != NULL);


        if (len < MsgLenSysExRtMtcCueingSetupMessageMin - 1) {
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
        if (!isSysExRtMtcCueing(bytes[4])) {
            return false;
        }


        *deviceId = bytes[2] & DataMask;
        *msgType = bytes[4];

        *eventNumber = unpackDoubleValue(&bytes[10]);

        if (isSysExRtMtcCueingWithAddInfo(bytes[4])) {

            if (bytes[len - 1] != SystemMessageEndOfExclusive) {
                len++;
            }

            *addInfoLen = denibblize(addInfo, &bytes[12], len - MsgLenSysExRtMtcCueingSetupMessageMin);
        }

        return true;
    }

    inline bool unpackSysExRtMtcCueingSetupMessage(uint8_t *bytes, uint8_t length, Message_t * msg){
        ASSERT( msg != NULL );

        if (unpackSysExRtMtcCueingSetupMessage( bytes, length, &msg->Data.SysEx.SubId1, &msg->Channel, &msg->Data.SysEx.Data.Cueing.EventNumber, msg->Data.SysEx.ByteData, &msg->Data.SysEx.Length) ){
            msg->StatusClass = StatusClassSystemMessage;
            msg->SystemMessage = SystemMessageSystemExclusive;
            msg->Data.SysEx.Id = SysExIdRealTimeByte;
            msg->Data.SysEx.SubId1 = SysExRtMidiTimeCode;
            return true;
        }

        return false;
    }

    inline uint8_t packSysExNonRtGenInfoIdentityRequest(uint8_t *bytes, uint8_t deviceId) {
        ASSERT(bytes != NULL);
        ASSERT(deviceId <= MaxValue);

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
        ASSERT(deviceId <= MaxValue);
        ASSERT(deviceFamily <= MaxDoubleValue);
        ASSERT(deviceFamilyMember <= MaxDoubleValue);

        uint32_t len = 5;

        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = SysExIdNonRealTimeByte;
        bytes[2] = deviceId & DataMask;
        bytes[3] = SysExNonRtGeneralInformation;
        bytes[4] = SysExNonRtGenInfoIdentityReply;

        len += packSysExId(&bytes[5], manufacturerId);

        packDoubleValue(&bytes[len], deviceFamily);
        len += 2;

        packDoubleValue(&bytes[len], deviceFamilyMember);
        len += 2;

        bytes[len++] = softwareRevision[0];
        bytes[len++] = softwareRevision[1];
        bytes[len++] = softwareRevision[2];
        bytes[len++] = softwareRevision[3];

        bytes[len] = SystemMessageEndOfExclusive;

        return len;
    }


    inline uint8_t packSysExNonRtGeneralInformation( uint8_t *bytes, Message_t *msg ){
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

        *deviceFamily = unpackDoubleValue(&bytes[l]);
        l += 2;

        *deviceFamilyMember = unpackDoubleValue(&bytes[l]);
        l += 2;

        softwareRevision[0] = bytes[l++];
        softwareRevision[1] = bytes[l++];
        softwareRevision[2] = bytes[l++];
        softwareRevision[3] = bytes[l++];

        return true;
    }

    inline bool unpackSysExNonRtGeneralInformation( uint8_t *bytes, uint8_t length, Message_t *msg){
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

    inline uint8_t packSysExRtDcBasic(uint8_t *bytes, uint8_t deviceId, SysExRtDc_t type, uint16_t value) {
        ASSERT(bytes != NULL);
        ASSERT(isSysExRtDeviceControl(type) && type != SysExRtDcGlobalParameterControl);
        ASSERT(type != SysExRtDcMasterVolume || value <= MaxDoubleValue);
        ASSERT(type != SysExRtDcMasterBalance || value <= MaxDoubleValue);
        ASSERT(type != SysExRtDcMasterCoarseTuning || value <= CoarseTuningMax); // MaxDoubleValue
        ASSERT(type != SysExRtDcMasterFineTuning || value <= FineTuningMax);

        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = SysExIdRealTimeByte;
        bytes[2] = deviceId & DataMask;
        bytes[3] = SysExRtDeviceControl;
        bytes[4] = type;

        packDoubleValue(&bytes[5], value);

        bytes[7] = SystemMessageEndOfExclusive;

        return MsgLenSysExNonRtDeviceControl;
    }

    inline uint8_t packSysExRtDcGlobalParameterControl(uint8_t *bytes, uint8_t deviceId, GlobalParameterControl_t *gpc) {
        ASSERT(bytes != NULL);
        ASSERT(deviceId <= MaxValue);
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

        for (int i = 0; i < gpc->SlotPathLength; i++) {
            bytes[len++] = getMsNibble(gpc->Data[i]);
            bytes[len++] = getLsNibble(gpc->Data[i]);
        }

        for (int i = gpc->SlotPathLength; i < gpc->DataLength; i++) {
            bytes[len++] = gpc->Data[i];
        }

        bytes[len++] = SystemMessageEndOfExclusive;

        return len;
    }

    inline uint8_t packSysExRtDeviceControl(uint8_t *bytes, Message_t * msg ){
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

        *value = unpackDoubleValue(&bytes[5]);

        return true;
    }


    inline bool unpackSysExRtDcGlobalParameterControl(uint8_t *bytes, uint8_t len, uint8_t *deviceId,
                                             GlobalParameterControl_t *gpc) {
        ASSERT(bytes != NULL);
        ASSERT(deviceId != NULL);
        ASSERT(gpc != NULL);

        if (len < 8) {
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
        for (int s = 0, i = 8; s < gpc->SlotPathLength; s++, l++) {
            gpc->Data[l] = (bytes[i++] & NibbleMask) << 4;
            gpc->Data[l] |= bytes[i++] & NibbleMask;
        }

        for (int i = 8 + 2 * gpc->SlotPathLength; i < len && bytes[i] != SystemMessageEndOfExclusive; i++, l++) {
            gpc->Data[l] = bytes[i];
        }

        gpc->DataLength = l;

        return true;
    }

    inline bool unpackSysExRtDeviceControl(uint8_t *bytes, uint8_t length, Message_t *msg){
        ASSERT( msg != NULL );

        if (length < 5){
            return false;
        }

        switch(bytes[4]) {
            case SysExRtDcMasterVolume:
            case SysExRtDcMasterBalance:
            case SysExRtDcMasterCoarseTuning:
            case SysExRtDcMasterFineTuning:
                return unpackSysExRtDcBasic( bytes, length, &msg->Channel, &msg->Data.SysEx.SubId2, &msg->Data.SysEx.Data.DeviceControl.Value );

            case SysExRtDcGlobalParameterControl:

                // use sysex raw byte data container for gpc
                msg->Data.SysEx.Data.DeviceControl.GlobalParameterControl.Data = msg->Data.SysEx.ByteData;

                if ( ! unpackSysExRtDcGlobalParameterControl( bytes, length, &msg->Channel, &msg->Data.SysEx.Data.DeviceControl.GlobalParameterControl )){
                    return false;
                }

                msg->Data.SysEx.Length = msg->Data.SysEx.Data.DeviceControl.GlobalParameterControl.DataLength;

                return true;
        }

        return false;
    }

    inline uint8_t packSysExRtMmcCommand(uint8_t *bytes, uint8_t deviceId, uint8_t command, uint8_t *data,
                                  uint8_t dataLen) {
        ASSERT(bytes != NULL);
        ASSERT(deviceId <= MaxValue);
        ASSERT(isSysExRtMmcCommand(command));
        ASSERT((dataLen == 0) || (data != NULL));
        ASSERT(dataLen <= MaxValue);

        uint32_t len = 5;

        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = SysExIdRealTimeByte;
        bytes[2] = deviceId & DataMask;
        bytes[3] = SysExRtMidiMachineControlCommand;
        bytes[4] = command;

        for (uint8_t i = 0; i < dataLen; i++) {
            bytes[len++] = data[i];
        }

        bytes[len++] = SystemMessageEndOfExclusive;

        return len;
    }

    inline uint8_t packSysExRtMmcCommand( uint8_t *bytes, Message_t * msg ){
        ASSERT( msg != NULL );

        return packSysExRtMmcCommand( bytes, msg->Channel, msg->Data.SysEx.SubId2, msg->Data.SysEx.ByteData, msg->Data.SysEx.Length );
    }

    inline bool unpackSysExRtMmcCommand(uint8_t *bytes, uint8_t len, uint8_t *deviceId, uint8_t *command,
                                 uint8_t *data, uint8_t *dataLen) {
        ASSERT(bytes != NULL);
        ASSERT(deviceId != NULL);
        ASSERT(command != NULL);
        ASSERT(data != NULL);
        ASSERT(dataLen != NULL);


        if (len < MsgLenSysExRtMmcCommandWithoutInfo - 1) {
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
        *command = bytes[4];

        if (bytes[len - 1] == SystemMessageEndOfExclusive) {
            len--;
        }

        *dataLen = 0;

        if (len > 5) {
            for (int i = 5; i < len; i++, (*dataLen)++) {
                data[(*dataLen)++] = bytes[i];
            }
        }

        return true;
    }


    inline bool unpackSysExRtMmcCommand(uint8_t *bytes, uint8_t length, Message_t * msg){
        ASSERT( msg != NULL );

        if ( unpackSysExRtMmcCommand( bytes, length, &msg->Channel, &msg->Data.SysEx.SubId2, msg->Data.SysEx.ByteData, &msg->Data.SysEx.Length) ){
            msg->StatusClass = StatusClassSystemMessage;
            msg->SystemMessage = SystemMessageSystemExclusive;
            msg->Data.SysEx.Id = SysExIdRealTimeByte;
            msg->Data.SysEx.SubId1 = SysExRtMidiMachineControlCommand;
            return true;
        }

        return false;
    }

    inline uint8_t packSysExRtMidiShowControl( uint8_t * bytes, uint8_t deviceId, MidiShowControlData_t * msc ){
        ASSERT( bytes != NULL );
        ASSERT( deviceId <= MaxValue );
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
                packDoubleValue( &bytes[len], msc->Controller );
                len += 2;
                packDoubleValue( &bytes[len], msc->Value );
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
                packDoubleValue( &bytes[len], msc->Checksum );
                len += 2;
                packDoubleValue( &bytes[len], msc->SequenceNumber );
                len += 2;
                bytes[len++] = msc->Data[0];
                bytes[len++] = msc->Data[1];
                bytes[len++] = msc->Data[2];
                bytes[len++] = msc->Data[3];
                len += packMscCueNumber( &bytes[len], msc->CueNumber.Number, msc->CueNumber.List, msc->CueNumber.Path );
                break;

            case SysExRtMscCmdStandingBy:
                packDoubleValue( &bytes[len], msc->Checksum );
                len += 2;
                packDoubleValue( &bytes[len], msc->SequenceNumber );
                len += 2;
                len += packMidiTimeCodeLong( &bytes[len], &msc->MidiTimeCode );
                len += packMscCueNumber( &bytes[len], msc->CueNumber.Number, msc->CueNumber.List, msc->CueNumber.Path );
                break;

            case SysExRtMscCmdComplete:
            case SysExRtMscCmdCancel:
                packDoubleValue( &bytes[len], msc->Checksum );
                len += 2;
                packDoubleValue( &bytes[len], msc->SequenceNumber );
                len += 2;
                len += packMscCueNumber( &bytes[len], msc->CueNumber.Number, msc->CueNumber.List, msc->CueNumber.Path );
                break;

            case SysExRtMscCmdCancelled:
            case SysExRtMscCmdAbort:
                packDoubleValue( &bytes[len], msc->Checksum );
                len += 2;
                len += packSysExRtMscStatus( &bytes[len], msc->Status );
                packDoubleValue( &bytes[len], msc->SequenceNumber );
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

    inline uint8_t packSysExRtMidiShowControl( uint8_t * bytes, Message_t * msg ){
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

        if ( ! isSysExRtMscCmd(bytes[len]) ){
            return false;
        }

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
                msc->Controller = unpackDoubleValue( &bytes[pos] );
                pos += 2;
                msc->Value = unpackDoubleValue( &bytes[pos] );
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
                msc->Checksum = unpackDoubleValue( &bytes[pos] );
                pos += 2;
                msc->SequenceNumber = unpackDoubleValue( &bytes[pos] );
                pos += 2;
                msc->Data[0] = bytes[pos++];
                msc->Data[1] = bytes[pos++];
                msc->Data[2] = bytes[pos++];
                msc->Data[3] = bytes[pos++];
                pos += unpackMscCueNumber( &bytes[pos], len - pos, &msc->CueNumber.Number, &msc->CueNumber.List, &msc->CueNumber.Path );
                break;

            case SysExRtMscCmdStandingBy:
                msc->Checksum = unpackDoubleValue( &bytes[pos] );
                pos += 2;
                msc->SequenceNumber = unpackDoubleValue( &bytes[pos]);
                pos += 2;
                pos += unpackMidiTimeCodeLong( &bytes[pos], &msc->MidiTimeCode );
                pos += unpackMscCueNumber( &bytes[pos], len - pos, &msc->CueNumber.Number, &msc->CueNumber.List, &msc->CueNumber.Path );
                break;

            case SysExRtMscCmdComplete:
            case SysExRtMscCmdCancel:
                msc->Checksum = unpackDoubleValue( &bytes[pos]);
                pos += 2;
                msc->SequenceNumber = unpackDoubleValue( &bytes[pos]);
                pos += 2;
                pos += unpackMscCueNumber( &bytes[pos], len - pos, &msc->CueNumber.Number, &msc->CueNumber.List, &msc->CueNumber.Path );
                break;

            case SysExRtMscCmdCancelled:
            case SysExRtMscCmdAbort:
                msc->Checksum = unpackDoubleValue( &bytes[pos]);
                pos += 2;
                pos += unpackSysExRtMscStatus( &bytes[pos], &msc->Status );
                msc->SequenceNumber = unpackDoubleValue( &bytes[pos]);
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

    inline bool unpackSysExRtMidiShowControl(uint8_t * bytes, uint8_t length, Message_t * msg ) {
        ASSERT( msg != NULL );

        if ( unpackSysExRtMidiShowControl(bytes, length, &msg->Channel, &msg->Data.SysEx.Data.MidiShowControl) ){
            msg->StatusClass = StatusClassSystemMessage;
            msg->SystemMessage = SystemMessageSystemExclusive;
            msg->Data.SysEx.Id = SysExIdRealTimeByte;
            msg->Data.SysEx.SubId1 = SysExRtMidiShowControl;
            return true;
        }

        return false;
    }

    inline uint8_t packSysExNonRtSampleDumpHeader(uint8_t *bytes, uint8_t deviceId, uint16_t sampleNumber, uint8_t sampleFormat, uint32_t samplePeriod, uint32_t sampleLength, uint32_t loopStartPoint, uint32_t loopEndPoint, uint8_t loopType ){

        ASSERT( bytes!= NULL );
        ASSERT( deviceId <= MaxValue );
        ASSERT( sampleNumber <= MaxDoubleValue );
        ASSERT( sampleFormat <= MaxValue );
        ASSERT( samplePeriod <= MaxTripleValue );
        ASSERT( sampleLength <= MaxTripleValue );
        ASSERT( loopEndPoint <= MaxTripleValue );
        ASSERT( loopStartPoint <= MaxTripleValue );
        ASSERT( isSampleDumpLoopType(loopType) );


        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = SysExIdNonRealTimeByte;
        bytes[2] = deviceId;
        bytes[3] = SysExNonRtSampleDumpHeader;

        packDoubleValue( &bytes[4], sampleNumber );

        bytes[6] = sampleFormat;

        packTripleValue( &bytes[7], samplePeriod );
        packTripleValue( &bytes[10], sampleLength );
        packTripleValue( &bytes[13], loopStartPoint );
        packTripleValue( &bytes[16], loopEndPoint );

        bytes[19] = (uint8_t)loopType;

        bytes[20] = SystemMessageEndOfExclusive;

        return MsgLenSysExNonRtSampleDumpHeader;
    }

    inline uint8_t packSysExNonRtSampleDumpHeader(uint8_t *bytes, uint8_t deviceId, SampleDumpHeaderData_t *header ){
        ASSERT( header != NULL );

        return packSysExNonRtSampleDumpHeader(bytes, deviceId, header->SampleNumber, (uint8_t)header->SampleFormat, header->SampleLength, header->SampleLength, header->LoopStartPoint, header->LoopEndPoint, header->LoopType );
    }

    inline uint8_t packSysExNonRtSampleDumpHeader(uint8_t * bytes, Message_t *msg){
        ASSERT( msg != NULL);

        return packSysExNonRtSampleDumpHeader(bytes, msg->Channel, &msg->Data.SysEx.Data.SampleDump.Header);
    }


    inline uint8_t unpackSysExNonRtSampleDumpHeader(uint8_t *bytes, uint8_t length, uint8_t *deviceId, uint16_t *sampleNumber, uint8_t *sampleFormat, uint32_t *samplePeriod, uint32_t *sampleLength, uint32_t *loopStartPoint, uint32_t *loopEndPoint, uint8_t *loopType ){

        ASSERT( bytes!= NULL );
        ASSERT( deviceId != NULL );
        ASSERT( sampleNumber != NULL );
        ASSERT( sampleFormat != NULL );
        ASSERT( samplePeriod != NULL );
        ASSERT( sampleLength != NULL );
        ASSERT( loopEndPoint != NULL );
        ASSERT( loopStartPoint != NULL );
        ASSERT( loopType != NULL );


        if (length != MsgLenSysExNonRtSampleDumpHeader || ! isControlByte(bytes[length-1]) ){
            return false;
        }
        if ( bytes[0] != SystemMessageSystemExclusive || bytes[1] != SysExIdNonRealTimeByte || bytes[3] != SysExNonRtSampleDumpHeader){
            return false;
        }

        *deviceId = bytes[2];

        *sampleNumber = unpackDoubleValue( &bytes[4] );

        *sampleFormat = bytes[6];

        *samplePeriod = unpackTripleValue( &bytes[7] );
        *sampleLength = unpackTripleValue( &bytes[10] );
        *loopStartPoint = unpackTripleValue( &bytes[13] );
        *loopEndPoint  = unpackTripleValue( &bytes[16]);

        *loopType  = bytes[19];

        return true;
    }

    inline uint8_t unpackSysExNonRtSampleDumpHeader(uint8_t *bytes, uint8_t length, uint8_t *deviceId, SampleDumpHeaderData_t *header ){
        ASSERT( header != NULL );

        return unpackSysExNonRtSampleDumpHeader(bytes, length, deviceId, &header->SampleNumber, &header->SampleFormat, &header->SampleLength, &header->SampleLength, &header->LoopStartPoint, &header->LoopEndPoint, &header->LoopType );
    }

    inline uint8_t unpackSysExNonRtSampleDumpHeader(uint8_t * bytes, uint8_t length, Message_t *msg){
        ASSERT( msg != NULL);

        if (unpackSysExNonRtSampleDumpHeader(bytes, length, &msg->Channel, &msg->Data.SysEx.Data.SampleDump.Header)){
            msg->StatusClass = StatusClassSystemMessage;
            msg->SystemMessage = SystemMessageSystemExclusive;
            msg->Data.SysEx.Id = SysExIdNonRealTimeByte;
            msg->Data.SysEx.SubId1 = SysExNonRtSampleDumpHeader;
            return true;
        }

        return false;
    }

    inline uint8_t packSysExNonRtSampleDumpData(uint8_t *bytes, uint8_t deviceId, uint8_t runningPacketCount, uint8_t * data, uint8_t dataLen ){

        ASSERT( bytes!= NULL );
        ASSERT( deviceId <= MaxValue );
        ASSERT( runningPacketCount <= MaxValue );
        ASSERT( dataLen == 0 || data != NULL );
        ASSERT( dataLen <= MaxValue );

        uint8_t length = 5;

        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = SysExIdNonRealTimeByte;
        bytes[2] = deviceId;
        bytes[3] = SysExNonRtSampleDumpHeader;

        bytes[4] = runningPacketCount;

        for (uint8_t i = 0; i < dataLen; i++, length++){
            bytes[length] = data[i];
        }

        bytes[length] = xorChecksum( bytes, length );
        length++;

        bytes[length++] = SystemMessageEndOfExclusive;

        return length;
    }

    inline uint8_t packSysExNonRtSampleDumpData(uint8_t *bytes, uint8_t deviceId, SampleDumpDataPacket_t * data){
        ASSERT( data != NULL );

        return packSysExNonRtSampleDumpData(bytes, deviceId, data->RunningPacketCount, data->Data, data->Length );
    }

    inline uint8_t packSysExNonRtSampleDumpData(uint8_t * bytes, Message_t *msg){
        ASSERT( msg != NULL);

        return packSysExNonRtSampleDumpData(bytes, msg->Channel, &msg->Data.SysEx.Data.SampleDump.Data);
    }


    inline uint8_t unpackSysExNonRtSampleDumpData(uint8_t *bytes, uint8_t length, uint8_t *deviceId, uint8_t *runningPacketCount, uint8_t * data, uint8_t *dataLen, uint8_t *checksum ){

        ASSERT( bytes!= NULL );
        ASSERT( deviceId != NULL );
        ASSERT( runningPacketCount != NULL );
        ASSERT( data != NULL );
        ASSERT( dataLen != NULL );
        ASSERT( checksum != NULL );

        if (length < 5 || ! isControlByte(bytes[length-1]) ){
            return false;
        }
        if (bytes[0] != SystemMessageSystemExclusive || bytes[1] != SysExIdNonRealTimeByte || bytes[3] != SysExNonRtSampleDumpHeader){
            return false;
        }


        *deviceId = bytes[2];
        *runningPacketCount = bytes[4];

        *dataLen = length - 7;

        uint8_t pos = 5;

        for (uint8_t i = 0; i < *dataLen; i++, pos++){
            bytes[pos] = data[i];
        }

        *checksum = bytes[pos];

        return length;
    }

    inline uint8_t unpackSysExNonRtSampleDumpData(uint8_t *bytes,uint8_t length,  uint8_t * deviceId, SampleDumpDataPacket_t * data){
        ASSERT( data != NULL );

        return unpackSysExNonRtSampleDumpData(bytes, length, deviceId, &data->RunningPacketCount, data->Data, &data->Length, &data->Checksum );
    }

    inline uint8_t unpackSysExNonRtSampleDumpData(uint8_t * bytes, uint8_t length, Message_t *msg){
        ASSERT( msg != NULL);

        msg->Data.SysEx.Data.SampleDump.Data.Data = msg->Data.SysEx.ByteData;

        if (unpackSysExNonRtSampleDumpData(bytes, length, &msg->Channel, &msg->Data.SysEx.Data.SampleDump.Data)){
            msg->StatusClass = StatusClassSystemMessage;
            msg->SystemMessage = SystemMessageSystemExclusive;
            msg->Data.SysEx.Id = SysExIdNonRealTimeByte;
            msg->Data.SysEx.SubId1 = SysExNonRtSampleDataPacket;
            return true;
        }

        return false;
    }


    inline uint8_t packSysExNonRtSampleDumpRequest(uint8_t *bytes, uint8_t deviceId, uint16_t requestedSample ){

        ASSERT( bytes!= NULL );
        ASSERT( deviceId <= MaxValue );
        ASSERT( requestedSample <= MaxDoubleValue );

        bytes[0] = SystemMessageSystemExclusive;
        bytes[1] = SysExIdNonRealTimeByte;
        bytes[2] = deviceId;
        bytes[3] = SysExNonRtSampleDumpRequest;

        packDoubleValue( &bytes[4], requestedSample );

        bytes[6] = SystemMessageEndOfExclusive;

        return MsgLenSysExNonRtSampleDumpRequest;
    }

    inline uint8_t packSysExNonRtSampleDumpRequest(uint8_t * bytes, Message_t *msg){
        ASSERT( msg != NULL);

        return packSysExNonRtSampleDumpRequest(bytes, msg->Channel, msg->Data.SysEx.Data.SampleDump.Request.RequestedSample);
    }


    inline uint8_t unpackSysExNonRtSampleDumpRequest(uint8_t *bytes, uint8_t length, uint8_t * deviceId, uint16_t * requestedSample ){

        ASSERT( bytes!= NULL );
        ASSERT( deviceId != NULL );
        ASSERT( requestedSample != NULL );

        if (length != MsgLenSysExNonRtSampleDumpRequest || ! isControlByte(bytes[length-1]) ){
            return false;
        }
        if (bytes[0] != SystemMessageSystemExclusive || bytes[1] != SysExIdNonRealTimeByte || bytes[3] != SysExNonRtSampleDumpRequest){
            return false;
        }

        *deviceId = bytes[2];
        *requestedSample = unpackDoubleValue( &bytes[4] );

        return true;
    }

    inline uint8_t unpackSysExNonRtSampleDumpRequest(uint8_t * bytes, uint8_t length, Message_t *msg){
        ASSERT( msg != NULL);

        if (unpackSysExNonRtSampleDumpRequest(bytes, length, &msg->Channel, &msg->Data.SysEx.Data.SampleDump.Request.RequestedSample)){
            msg->StatusClass = StatusClassSystemMessage;
            msg->SystemMessage = SystemMessageSystemExclusive;
            msg->Data.SysEx.Id = SysExIdNonRealTimeByte;
            msg->Data.SysEx.SubId1 = SysExNonRtSampleDumpRequest;
            return true;
        }

        return false;
    }


}

#endif //MIDIMESSAGE_PACKERS_H
