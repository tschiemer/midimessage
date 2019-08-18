//
// Created by Philip Tschiemer on 19.07.19.
//

#include <iostream>
#include <midimessage.h>
#include "midimessage.h"


void receivedMidiMessageFromSomewhere( uint8_t * buf, int len );
void sendMidiMessageToSomewhere( uint8_t * buf, int len );

void receivedSysExMessageFromSomewhere( uint8_t * buf, int len );


void receivedMidiMessageFromSomewhere( uint8_t * buf, int len ){

    MidiMessage::Message_t msg, msg2;

    uint8_t buf2[256];
    int len2;

    static uint8_t pitch = 0;


    if (!MidiMessage::unpack( buf, len, &msg )) {
        std::cout << "Discarding invalid message" << std::endl;
        return;
    }

    if (!MidiMessage::isChannelVoiceMessage(&msg)){

        std::cout << "Discarding non-ChannelVoice message" << std::endl;

        // if above a SysEx message with data content was unpacked, we have to make sure to free the memory again.
        MidiMessage::freeMessage(&msg);

        return;
    }

    // Struct based approach to create messages
    // (the final message packaging is unified)
    msg2.Status = MidiMessage::StatusNoteOn;
    msg2.Channel = msg.Channel;
    msg2.Data.NoteOn.Key = 40 + pitch;
    msg2.Data.NoteOn.Velocity = 100;

    pitch = (pitch + 1) % 13;

    len2 = MidiMessage::pack( buf2, &msg2 );
    if (len2 > 0){

        sendMidiMessageToSomewhere( buf2, len2 );

    }

    // direct approach
    len2 = MidiMessage::packNoteOff( buf2, msg2.Channel, msg2.Data.NoteOn.Key, 50 );
    if (len2 > 0){
        sendMidiMessageToSomewhere( buf2, len2 );
    }
}

void sendMidiMessageToSomewhere( uint8_t * buf, int len ){


    std::cout << "Sending Midi message (raw bytes): ";

    for (int i = 0; i < len; i++){
        std::cout << (int)buf[i] << " ";
    }

    std::cout << std::endl;

    MidiMessage::Status_t status = MidiMessage::getStatus(buf[0]);

    std::cout << "Status " << (int)status;
    if (MidiMessage::StatusNoteOn == status) {
        std::cout << " (Note On)";
    } else if (MidiMessage::StatusNoteOff == status) {
        std::cout << " (Note Off)";
    }
    std::cout << std::endl;


    if (!MidiMessage::isSystemExclusive(status)){
        std::cout << "Channel " << (int)MidiMessage::getChannel(buf[0]) << std:: endl;
    }

    if (MidiMessage::StatusNoteOn == status || MidiMessage::StatusNoteOff == status) {
        std::cout << "key " << (int)MidiMessage::getValue(buf[1]) << std::endl;
        std::cout << "velocity " << (int)MidiMessage::getValue(buf[2]) << std::endl;
    }

    std::cout << std::endl;
}

void receivedSysExMessageFromSomewhere( uint8_t * buf, int len ) {

    MidiMessage::Message_t msg;

    if (!MidiMessage::unpack( buf, len, &msg )) {
        std::cout << "Discarding invalid message" << std::endl;
        return;
    }

    if (msg.Status != MidiMessage::StatusSystemExclusive) {
        std::cout << "Discarding non-SysEx message" << std::endl;
        return;
    }

    std::cout << "Received SysEx Message with " << (int)msg.Data.SysEx.Custom.Length << " bytes of data" << std::endl;

    if (msg.Data.SysEx.Custom.Id == MidiMessage::ReservedSystemExclusiveIdExperimental){
        std::cout << "Experimental message!" << std::endl;
    } else if (msg.Data.SysEx.Custom.Id == MidiMessage::ReservedSystemExclusiveIdRealTime) {
        std::cout << "Universal real time message!" << std::endl;
    } else if (msg.Data.SysEx.Custom.Id == MidiMessage::ReservedSystemExclusiveIdNonRealTime) {
        std::cout << "Universal non-real time message!" << std::endl;
    } else {
        std::cout << "Custom manufacturer message!" << std::endl;
    }

    for (int i = 0; i < msg.Data.SysEx.Custom.Length; i++) {
#if SYSEX_MEMORY == SYSEX_MEMORY_STATIC
        std::cout << (int)msg.Data.SysEx.Custom.Data.Bytes[i] << " ";
#elif SYSEX_MEMORY == SYSEX_MEMORY_DYNAMIC
        std::cout << (int)((uint8_t*)msg.Data.SysEx.Custom.Data)[i] << " ";
#endif
    }

    std::cout << std::endl;


    MidiMessage::freeMessage( &msg );
}


int main(int argc, char * argv[]){

    std::cout << "-------------- Voice channel msg vs SysEx msg" << std::endl;

    uint8_t buf[3] = {MidiMessage::StatusNoteOff | 1, 33, 66};
    uint8_t bufSysExManufacturer[10] = {MidiMessage::StatusSystemExclusive, 1, 2, 3, 4, 5, 6, 7, 8, 9};


    receivedMidiMessageFromSomewhere( buf, sizeof(buf) );
    receivedMidiMessageFromSomewhere( bufSysExManufacturer, sizeof(bufSysExManufacturer) );

    std::cout << "--------------" << std::endl;

    receivedSysExMessageFromSomewhere( buf, sizeof(buf) );
    receivedSysExMessageFromSomewhere( bufSysExManufacturer, sizeof(bufSysExManufacturer) );

    std::cout << "-------------- Experimental Sysex msg" << std::endl;

    uint8_t bufSysExExperimental[6] = {MidiMessage::StatusSystemExclusive, MidiMessage::ReservedSystemExclusiveIdExperimental, 1, 3, 3, 7};
    receivedSysExMessageFromSomewhere( bufSysExExperimental, sizeof(bufSysExExperimental) );

    std::cout << "-------------- Universal Realtime Sysex msg" << std::endl;

    uint8_t bufSysRealtime[10] = {MidiMessage::StatusSystemExclusive, MidiMessage::ReservedSystemExclusiveIdRealTime, 1, 3, 3, 7};
    receivedSysExMessageFromSomewhere( bufSysRealtime, sizeof(bufSysRealtime) );

    std::cout << "-------------- Universal Non-RealtimeSysex msg" << std::endl;

    uint8_t bufSysNonRealtime[10] = {MidiMessage::StatusSystemExclusive, MidiMessage::ReservedSystemExclusiveIdNonRealTime, 1, 3, 3, 7};
    receivedSysExMessageFromSomewhere( bufSysNonRealtime, sizeof(bufSysNonRealtime) );

    return 0;
}