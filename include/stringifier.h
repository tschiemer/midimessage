/**
 * \file
 */

#ifndef MIDIMESSAGE_COMMAND_INTERFACE_H
#define MIDIMESSAGE_COMMAND_INTERFACE_H

#include "midimessage.h"

namespace MidiMessage {

    typedef enum {
        StringifierResultOk             = 0,
        StringifierResultGenericError   = 1,
        StringifierResultInvalidValue   = 2,
        StringifierResultWrongArgCount  = 3,
        StringifierResultPackError      = 4,
        StringifierResultParserFail     = 5
    } StringifierResult_t;

    struct Stringifier {
        bool RunningStatusEnabled;
        uint8_t RunningStatusState;

        void (*writeMidiStream)(uint8_t * buffer, uint8_t length);
        void (*printfStringStream)(char const * fmt, ...);


        Stringifier(bool runningStatusEnabled, void (*writeMidiStream)(uint8_t * buffer, uint8_t length), void (*printfStringStream)(char const * fmt, ...) );

        uint8_t fromString(uint8_t argc,  uint8_t * argv[]);
        uint8_t toString(uint8_t length, uint8_t bytes[]);
    };

}

#endif //MIDIMESSAGE_COMMAND_INTERFACE_H
