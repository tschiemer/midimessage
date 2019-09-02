
#include <midimessage/parser.h>
#include <midimessage/packers.h>

//#include <cstdio>

namespace MidiMessage {
    void Parser::receivedData(uint8_t * data, uint8_t len){

        for (uint8_t i = 0; i < len; i++){

            // consume real time messages straight away (may be interleaved in other messages)
            if (isSystemRealTimeMessage(data[i])){
                // this will definitly success
                if (unpackSystemMessage(&data[i], 1, this->Message)){
                    // emit real time message
                    this->MessageHandler( this->Message );
                }

                // skip further processing
                continue;
            }

            // discard any initial data bytes unless running status is enabled
            if (this->Length == 0 && isDataByte(data[i])){

                if (this->RunningStatusEnabled && isRunningStatus(this->Buffer[0])){
                    this->Length++;
                } else {
                    // wait for control byte
                    continue;
                }
            }

            // if a control byte comes after the first byte, everything before must have been a command
            if (this->Length > 1 && isControlByte(data[i]) ){

                // sysexes may end with status bytes other than EOX
                if (this->Buffer[0] == SystemMessageSystemExclusive){

                    this->Buffer[ this->Length++ ] = SystemMessageEndOfExclusive;

                    // try to parse data
                    uint8_t consumed = unpack(this->Buffer, this->Length, this->Message);

                    if (consumed > 0){
                        // emit sysex
                        this->MessageHandler( this->Message );
                    }

                    // if was in fact an EOX, skip this byte
                    if (data[i] == SystemMessageEndOfExclusive){
                        this->Length = 0;
                        continue;
                    }
                }

                // discard previous data
                this->Length = 0;
            }

            this->Buffer[ this->Length++ ] = data[i];

            // try to parse data straight away
            uint8_t consumed = unpack(this->Buffer, this->Length, this->Message);

            if (consumed > 0){
                // emit event
                this->MessageHandler( this->Message );

                // reset data
                this->Length = 0;
            }
        }

//        printf("%d ", this->Length);
//        for(uint8_t i = 0; i < this->Length; i++){
//            printf("%02X", this->Buffer[i]);
//        }
//        printf("\n");
    }
}