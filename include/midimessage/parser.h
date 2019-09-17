/**
 * \file
 */

#ifndef MIDIMESSAGE_PARSER_H
#define MIDIMESSAGE_PARSER_H

#include "../midimessage.h"

#ifdef __cplusplus
namespace MidiMessage {
    extern "C" {
#endif

    /**
     * Parser for continuous parsing of incoming byte stream.
     *
     * Can deal with Running Status (if enabled) and interleaved System Real Time Messages (see MIDI spec).
     */

    typedef struct {

        bool RunningStatusEnabled;

        uint8_t *Buffer;
        uint8_t MaxLength;

        Message_t *Message;

        void *Context;

        void (*MessageHandler)(Message_t *message, void *context);

        void (*DiscardingDataHandler)(uint8_t *bytes, uint8_t length, void *context);

        uint8_t Length;

    } Parser_t;


    void parser_init(Parser_t * parser, bool runningStatusEnabled, uint8_t * buffer, uint8_t maxLength, Message_t * msg, void (*messageHandler)(Message_t * message, void * context), void (*discardingDataHandler)(uint8_t * bytes, uint8_t length, void * context), void * context);

    inline void parser_reset(Parser_t * parser) {
        parser->Length = 0;
        parser->Buffer[0] = 0;
    }

    /**
     * @param data      incoming data buffer
     * @param len       length of incoming data
     */
    void parser_receivedData(Parser_t * parser, uint8_t * data, uint8_t len);

#ifdef __cplusplus
    } // extern "C"
} // namespace MidiMessage
#endif

#endif //MIDIMESSAGE_PARSER_H
