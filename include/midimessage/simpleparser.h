/**
 * \file
 */

#ifndef MIDIMESSAGE_SIMPLEPARSER_H
#define MIDIMESSAGE_SIMPLEPARSER_H

#include "../midimessage.h"

#ifdef __cplusplus
namespace MidiMessage {
    extern "C" {
#endif

    typedef struct {

        bool RunningStatusEnabled;

        uint8_t *Buffer;
        uint8_t MaxLength;

        void *Context;

        void (*MessageHandler)(uint8_t *message, uint8_t length, void *context);

        void (*DiscardingDataHandler)(uint8_t *bytes, uint8_t length, void *context);

        uint8_t Length;

    } SimpleParser_t;


    inline void simpleparser_init(SimpleParser_t * parser, bool runningStatusEnabled, uint8_t * buffer, uint8_t maxLength, void (*messageHandler)(uint8_t *message, uint8_t length, void * context), void (*discardingDataHandler)(uint8_t * bytes, uint8_t length, void * context), void * context){

        parser->RunningStatusEnabled = runningStatusEnabled;

        parser->Buffer = buffer;
        parser->MaxLength = maxLength;

        parser->MessageHandler = messageHandler;
        parser->DiscardingDataHandler = discardingDataHandler;

        parser->Length = 0;

        parser->Context = context;
    }

    inline void simpleparser_reset(SimpleParser_t * parser) {
        parser->Length = 0;
        parser->Buffer[0] = 0;
    }

    /**
     * @param data      incoming data buffer
     * @param len       length of incoming data
     */
    void simpleparser_receivedData(SimpleParser_t * parser, uint8_t * data, uint8_t len);

#ifdef __cplusplus
    } // extern "C"
} // namespace MidiMessage
#endif

#endif /* MIDIMESSAGE_SIMPLEPARSER_H */
