/**
 * \file
 */

#ifndef MIDIMESSAGE_PARSER_H
#define MIDIMESSAGE_PARSER_H

#include "midimessage.h"

namespace MidiMessage {

    struct Parser {

        bool RunningStatusEnabled;

        uint8_t * Buffer;
        uint8_t MaxLen;

        Message_t * Message;

        void (*MessageHandler)(Message_t * message);

        uint8_t Length;

        Parser(bool runninStatusEnabled, uint8_t * buffer, uint8_t maxLen, Message_t * msg, void (*messageHandler)(Message_t * message)) {
            this->RunningStatusEnabled = runninStatusEnabled;

            this->Buffer = buffer;
            this->MaxLen = maxLen;

            this->Message = msg;

            this->MessageHandler = messageHandler;

            this->Length = 0;
        }

        void reset() {
            this->Length = 0;
        }

        void receivedData(uint8_t * data, uint8_t len);

    };

}
#endif //MIDIMESSAGE_PARSER_H
