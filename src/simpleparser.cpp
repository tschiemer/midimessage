#include <midimessage/simpleparser.h>

#ifdef __cplusplus
namespace MidiMessage {
    extern "C" {
#endif

    void simpleparser_receivedData(SimpleParser_t * parser, uint8_t * data, uint8_t len){

        for (uint8_t i = 0; i < len; i++){

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

                // emit event
                parser->MessageHandler( parser->Buffer, parser->Length, parser->Context );

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
