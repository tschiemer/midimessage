/**
 * \file
 */

#ifndef MIDIMESSAGE_PARSER_H
#define MIDIMESSAGE_PARSER_H

#include "../midimessage.h"

namespace MidiMessage {

    /**
     * Parser for continuous parsing of incoming byte stream.
     *
     * Can deal with Running Status (if enabled) and interleaved System Real Time Messages (see MIDI spec).
     */
    struct Parser {

        bool RunningStatusEnabled;

        uint8_t * Buffer;
        uint8_t MaxLength;

        Message_t * Message;

        void (*MessageHandler)(Message_t * message);
        void (*DiscardingDataHandler)(uint8_t * bytes, uint8_t length);

        uint8_t Length;

        Parser(bool runningStatusEnabled, uint8_t * buffer, uint8_t maxLength, Message_t * msg, void (*messageHandler)(Message_t * message), void (*discardingDataHandler)(uint8_t * bytes, uint8_t length)) {
            this->RunningStatusEnabled = runningStatusEnabled;

            this->Buffer = buffer;
            this->MaxLength = maxLength;

            this->Message = msg;

            this->MessageHandler = messageHandler;
            this->DiscardingDataHandler = discardingDataHandler;

            this->Length = 0;
        }

        void reset() {
            this->Length = 0;
        }

        /**
         * @param data      incoming data buffer
         * @param len       length of incoming data
         */
        void receivedData(uint8_t * data, uint8_t len);

    };

}
#endif //MIDIMESSAGE_PARSER_H
