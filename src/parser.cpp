
#include <midimessage/parser.h>
#include <midimessage/packers.h>

//#include <cstdio>
#ifdef __cplusplus
namespace MidiMessage {
    extern "C" {
#endif

    void parser_init(Parser_t * parser, bool runningStatusEnabled, uint8_t * buffer, uint8_t maxLength, Message_t * msg, void (*messageHandler)(Message_t * message, void * context), void (*discardingDataHandler)(uint8_t * bytes, uint8_t length, void * context), void * context) {
        parser->RunningStatusEnabled = runningStatusEnabled;

        parser->Buffer = buffer;
        parser->MaxLength = maxLength;

        parser->Message = msg;

        parser->MessageHandler = messageHandler;
        parser->DiscardingDataHandler = discardingDataHandler;

        parser->Length = 0;

        parser->Context = context;
    }

    void parser_receivedData(Parser_t * parser, uint8_t * data, uint8_t len){

        for (uint8_t i = 0; i < len; i++){

            // consume real time messages straight away (may be interleaved in other messages)
            if (isSystemRealTimeMessage(data[i])){
                // parser will definitly success
                if (unpackSystemMessageObj(&data[i], 1, parser->Message)){
                    // emit real time message
                    parser->MessageHandler( parser->Message, parser->Context );
                }

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

            // if a control byte comes after the first byte, everything before must have been a command
            if (parser->Length > 1 && isControlByte(data[i]) ){

                // sysexes may end with status bytes other than EOX
                if (parser->Buffer[0] == SystemMessageSystemExclusive){

                    parser->Buffer[ parser->Length++ ] = SystemMessageEndOfExclusive;

                    // try to parse data
                    uint8_t consumed = unpack(parser->Buffer, parser->Length, parser->Message);

                    if (consumed > 0){
                        // emit sysex
                        parser->MessageHandler( parser->Message, parser->Context );
                    }

                    // if was in fact an EOX, skip parser byte
                    if (data[i] == SystemMessageEndOfExclusive){
                        parser->Length = 0;
                        continue;
                    }
                }

                if (parser->DiscardingDataHandler != NULL){
                    parser->DiscardingDataHandler( parser->Buffer, parser->Length, parser->Context );
                }

                // discard previous data
                parser->Length = 0;
            }


            parser->Buffer[ parser->Length++ ] = data[i];

            // try to parse data straight away
            uint8_t consumed = unpack(parser->Buffer, parser->Length, parser->Message);

            if (consumed > 0){
                // emit event
                parser->MessageHandler( parser->Message, parser->Context );

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