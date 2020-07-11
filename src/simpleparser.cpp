#include <midimessage/simpleparser.h>
#include <midimessage/commonccs.h>

#ifdef __cplusplus
namespace MidiMessage {
    extern "C" {
#endif

    void simpleparser_receivedData(SimpleParser_t * parser, uint8_t * data, uint16_t len){

        for (uint16_t i = 0; i < len; i++){

            // consume real time messages straight away (may be interleaved in other messages)
            if (isSystemRealTimeMessage(data[i])){

                // emit real time message
                parser->MessageHandler( &data[i], 1, parser->Context );

                // skip further processing
                continue;
            }

            // discard any initial data bytes unless running status is enabled
            if (parser->Length == 0 && isDataByte(data[i])){

                if (parser->RunningStatusEnabled && isRunningStatus(parser->Buffer[0])){
                    parser->Length++;
                } else {

                    if (parser->DiscardingDataHandler != NULL){
                        parser->DiscardingDataHandler( &data[i], 1, parser->Context );
                    }

                    // wait for control byte
                    continue;
                }
            }

            // if a control byte comes after the first byte, everything before must have been an (unrecognized) command
            if (parser->Length > 1 && isControlByte(data[i]) ){

                // sysexes may end with status bytes other than EOX (so we don't test for that)
                if (parser->Buffer[0] == SystemMessageSystemExclusive){

                    // add EOX anyways
                    parser->Buffer[ parser->Length++ ] = SystemMessageEndOfExclusive;

                    // emit sysex
                    parser->MessageHandler( parser->Buffer, parser->Length, parser->Context );


                    // if was in fact an EOX, skip further processing of byte
                    if (data[i] == SystemMessageEndOfExclusive){
                        // reset buffer
                        parser->Length = 0;
                        continue;
                    }
                }

                // if it was not a sysex and no other message type was complete, then we had an incomplete (invalid)
                // message which we discard;
                else if (parser->DiscardingDataHandler != NULL){
                    parser->DiscardingDataHandler( parser->Buffer, parser->Length, parser->Context );
                }

                // discard previous data
                parser->Length = 0;
            }


            parser->Buffer[ parser->Length++ ] = data[i];

            // try to parse data straight away
            if (simpleValidate(parser->Buffer, parser->Length)){

                if (parser->NRpnHandler != NULL
                    && getStatusClass(parser->Buffer[0]) == StatusClassControlChange
                    && (parser->Buffer[1] == CcNonRegisteredParameterMSB || parser->Buffer[1] == CcNonRegisteredParameterLSB
                      || parser->Buffer[1] == CcRegisteredParameterMSB || parser->Buffer[1] == CcRegisteredParameterLSB
                      || parser->Buffer[1] == CcDataEntryMSB || parser->Buffer[1] == CcDataEntryLSB
                      || parser->Buffer[1] == CcDataIncrement || parser->Buffer[1] == CcDataDecrement)
                   ){

                    uint8_t channel = getChannel(parser->Buffer[0]);
                    uint8_t controller = parser->Buffer[1];
                    uint8_t value = parser->Buffer[2];

                    // some devices (like Yamaha 01V96) do not send the MSB first, but the LSB, so we have allow "improper sequences"
                    if (parser->NRpnMsgCount == 0){
                        if (controller == CcNonRegisteredParameterMSB || controller == CcNonRegisteredParameterLSB || controller == CcRegisteredParameterMSB || controller == CcRegisteredParameterLSB) {

                            if (controller == CcNonRegisteredParameterMSB || controller == CcNonRegisteredParameterLSB){
                              parser->NRpnType = NRpnTypeNRPN;
                            } else {
                              parser->NRpnType = NRpnTypeRPN;
                            }

                            parser->NRpnChannel = channel;

                            if (controller == CcNonRegisteredParameterMSB || controller == CcRegisteredParameterMSB){
                                parser->NRpnController = value << 7;
                            } else {
                                parser->NRpnController = value;
                            }
                            parser->NRpnMsgCount = 1;

                        } else {
                          // Reset
                          // (nothing to do)
                        }
                    }
                    else if (parser->NRpnChannel != channel) {
                        // reset filter
                        parser->NRpnMsgCount = 0;
                    }
                    else if (parser->NRpnMsgCount == 1){

                        // sanity checking (if was NRPN before, is it still NRPN?)
                        if ((parser->NRpnType == NRpnTypeNRPN && (controller == CcRegisteredParameterMSB || controller == CcRegisteredParameterLSB)) ||
                            (parser->NRpnType == NRpnTypeRPN && (controller == CcNonRegisteredParameterMSB || controller == CcNonRegisteredParameterLSB))){
                          parser->NRpnMsgCount = 0;
                        }
                        else if (controller == CcNonRegisteredParameterMSB || controller == CcRegisteredParameterMSB || controller == CcNonRegisteredParameterLSB || controller == CcRegisteredParameterLSB){


                            if (controller == CcNonRegisteredParameterMSB || controller == CcRegisteredParameterMSB){
                                parser->NRpnController |= value << 7;
                            } else {
                                parser->NRpnController |= value;
                            }

                            parser->NRpnMsgCount = 2;

                        } else {
                            // reset filter
                            parser->NRpnMsgCount = 0;
                        }
                    }
                    else if (parser->NRpnMsgCount == 2) {

                        if (controller == CcDataEntryMSB) {
                            parser->NRpnValue = value << 7;
                            parser->NRpnMsgCount = 3;

                        } else if (controller == CcDataEntryLSB) {
                            parser->NRpnValue = value;
                            parser->NRpnMsgCount = 3;

                        } else if (controller == CcDataIncrement) {
                            parser->NRpnHandler( parser->NRpnChannel, parser->NRpnType, NRpnActionIncrement, parser->NRpnController, value, parser->Context);
                            parser->NRpnMsgCount = 0;

                        } else if (controller == CcDataDecrement) {
                            parser->NRpnHandler( parser->NRpnChannel, parser->NRpnType, NRpnActionDecrement, parser->NRpnController, value, parser->Context);
                            parser->NRpnMsgCount = 0;

                        } else {
                            // reset filter
                            parser->NRpnMsgCount = 0;
                        }
                    }
                    else if (parser->NRpnMsgCount == 3) {

                        if (controller == CcDataEntryMSB) {
                          parser->NRpnValue |= value << 7;
                          parser->NRpnHandler( parser->NRpnChannel, parser->NRpnType, NRpnActionValue, parser->NRpnController, parser->NRpnValue, parser->Context);
                          parser->NRpnMsgCount = 0;

                        } else if (controller == CcDataEntryLSB) {
                          parser->NRpnValue |= value;
                          parser->NRpnHandler( parser->NRpnChannel, parser->NRpnType, NRpnActionValue, parser->NRpnController, parser->NRpnValue, parser->Context);
                          parser->NRpnMsgCount = 0;

                        } else {
                          // reset filter
                          parser->NRpnMsgCount = 0;
                        }
                    }

                    // NEVER consume message (ie always pass to normal message handler)
                    // parser->Length = 0;
                    // continue;
                }

                // emit event
                if (parser->MessageHandler != NULL) {
                    parser->MessageHandler( parser->Buffer, parser->Length, parser->Context );
                }

                // reset data
                parser->Length = 0;
            }

            // in case the buffer would overflow next time, discard current contents
            if ( parser->Length >= parser->MaxLength ){

                if (parser->DiscardingDataHandler != NULL){
                    parser->DiscardingDataHandler( parser->Buffer, parser->Length, parser->Context );
                }

                parser->Length = 0;
            }
        }

//        printf("%d ", parser->Length);
//        for(uint8_t i = 0; i < parser->Length; i++){
//            printf("%02X", parser->Buffer[i]);
//        }
//        printf("\n");
    }


#ifdef __cplusplus
  } // extern "C"
} // namespace MidiMessage
#endif
