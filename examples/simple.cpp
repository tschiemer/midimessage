//
// Created by Philip Tschiemer on 19.07.19.
//

#include <iostream>
#include <midimessage.h>
#include <midimessage/packers.h>


void receivedMidiMessageFromSomewhere( uint8_t * buf, int len );
void sendMidiMessageToSomewhere( uint8_t * buf, int len );

void receivedSysExMessageFromSomewhere( uint8_t * buf, int len );


void receivedMidiMessageFromSomewhere( uint8_t * buf, int len ){

    MidiMessage::Message_t msg, msg2;
    uint8_t msgBuf[128], msgBuf2[128];

    msg.Data.SysEx.ByteData = msgBuf;
    msg2.Data.SysEx.ByteData = msgBuf2;

    uint8_t buf2[256];
    int len2;

    static uint8_t pitch = 0;


    if (!MidiMessage::unpack( buf, len, &msg )) {
        std::cout << "Discarding invalid message" << std::endl;
        return;
    }

    if (!MidiMessage::isChannelVoiceMessageObj(&msg)){

        std::cout << "Discarding non-ChannelVoice message" << std::endl;

        return;
    }

    // Struct based approach to create messages
    // (the final message packaging is unified)
    msg2.StatusClass = MidiMessage::StatusClassNoteOn;
    msg2.Channel = msg.Channel;
    msg2.Data.Note.Key = 40 + pitch;
    msg2.Data.Note.Velocity = 100;

    pitch = (pitch + 1) % 13;

    len2 = MidiMessage::pack( buf2, &msg2 );
    if (len2 > 0){
        sendMidiMessageToSomewhere( buf2, len2 );
    }

    // direct approach
    len2 = MidiMessage::packNoteOff( buf2, msg2.Channel, msg2.Data.Note.Key, 50 );
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

    MidiMessage::StatusClass_t statusClass = MidiMessage::getStatusClass(buf[0]);

    std::cout << "Status " << (int)statusClass;
    if (MidiMessage::StatusClassNoteOn == statusClass) {
        std::cout << " (Note On)";
    } else if (MidiMessage::StatusClassNoteOff == statusClass) {
        std::cout << " (Note Off)";
    }
    std::cout << std::endl;


    if (MidiMessage::StatusClassSystemMessage != statusClass){
        std::cout << "Channel " << (int)MidiMessage::getChannel(buf[0]) << std:: endl;
    }

    if (MidiMessage::StatusClassNoteOn == statusClass || MidiMessage::StatusClassNoteOff == statusClass) {
        std::cout << "key " << (int)buf[1] << std::endl;
        std::cout << "velocity " << (int)buf[2] << std::endl;
    }

    std::cout << std::endl;
}

void receivedSysExMessageFromSomewhere( uint8_t * buf, int len ) {

    MidiMessage::Message_t msg;
    uint8_t msgBuf[128];

    msg.Data.SysEx.ByteData = msgBuf;

    if (!MidiMessage::unpack( buf, len, &msg )) {
        std::cout << "Discarding invalid message" << std::endl;
        return;
    }

    if (msg.StatusClass != MidiMessage::StatusClassSystemMessage || msg.SystemMessage != MidiMessage::SystemMessageSystemExclusive) {
        std::cout << "Discarding non-SysEx message" << std::endl;
        return;
    }


    if (msg.Data.SysEx.Id == MidiMessage::SysExIdExperimental){
        std::cout << "Experimental message with data (" <<  (int)msg.Data.SysEx.Length << "): ";

        for (int i = 0; i < msg.Data.SysEx.Length; i++) {
            std::cout << (int)msg.Data.SysEx.ByteData[i] << " ";
        }
    } else if (msg.Data.SysEx.Id == MidiMessage::SysExIdRealTime) {
        std::cout << "Universal real time message! device-id = " << (int)msg.Channel << " subid1 = " << (int) msg.Data.SysEx.SubId1;
    } else if (msg.Data.SysEx.Id == MidiMessage::SysExIdNonRealTime) {
        std::cout << "Universal non-real time message! device-id = " << (int)msg.Channel << " subid1 = " << (int) msg.Data.SysEx.SubId1 ;
    } else {
        std::cout << "Custom manufacturer message! Manufacturer Id = ";
        printf("%06X data(%d) = ", msg.Data.SysEx.Id, msg.Data.SysEx.Length);

        for (int i = 0; i < msg.Data.SysEx.Length; i++) {
            std::cout << (int)msg.Data.SysEx.ByteData[i] << " ";
        }
    }


    std::cout << std::endl;
}


int main(int argc, char * argv[]){

    std::cout << "-------------- Voice channel msg vs SysEx msg" << std::endl;

    uint8_t buf[3] = {MidiMessage::StatusClassNoteOff | 1, 33, 66};

    uint8_t bufSysExManufacturer[10] = {MidiMessage::SystemMessageSystemExclusive, MidiMessage::SysExIdManufacturerExtensionByte, 0x20, 0x11, 4, 5, 6, 7, 8, MidiMessage::SystemMessageEndOfExclusive};
    uint8_t bufSysExExperimental[7] = {MidiMessage::SystemMessageSystemExclusive, MidiMessage::SysExIdExperimentalByte, 1, 3, 3, 7, MidiMessage::SystemMessageEndOfExclusive};
    uint8_t bufSysRealtime[8] = {MidiMessage::SystemMessageSystemExclusive, MidiMessage::SysExIdRealTimeByte, 1, MidiMessage::SysExRtDeviceControl, MidiMessage::SysExRtDcMasterVolume, 0xa, 0, MidiMessage::SystemMessageEndOfExclusive};
    uint8_t bufSysNonRealtime[6] = {MidiMessage::SystemMessageSystemExclusive, MidiMessage::SysExIdNonRealTimeByte, 2, MidiMessage::SysExNonRtACK, 5, MidiMessage::SystemMessageEndOfExclusive};

//    for(uint8_t i = 0; i < sizeof(bufSysRealtime); i++){
//        printf("%02X ", bufSysRealtime[i]);
//    }
//    printf("\n");
//    exit(1);


    receivedMidiMessageFromSomewhere( buf, sizeof(buf) );
    receivedMidiMessageFromSomewhere( bufSysExManufacturer, sizeof(bufSysExManufacturer) );

    std::cout << "--------------" << std::endl;

    receivedSysExMessageFromSomewhere( buf, sizeof(buf) );
    receivedSysExMessageFromSomewhere( bufSysExManufacturer, sizeof(bufSysExManufacturer) );

    std::cout << "-------------- Experimental Sysex msg" << std::endl;

    receivedSysExMessageFromSomewhere( bufSysExExperimental, sizeof(bufSysExExperimental) );

    std::cout << "-------------- Universal Realtime Sysex msg" << std::endl;

    receivedSysExMessageFromSomewhere( bufSysRealtime, sizeof(bufSysRealtime) );

    std::cout << "-------------- Universal Non-RealtimeSysex msg" << std::endl;

    receivedSysExMessageFromSomewhere( bufSysNonRealtime, sizeof(bufSysNonRealtime) );

    return 0;
}