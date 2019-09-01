/**
 * \file
 */

#ifndef MIDIMESSAGE_COMMAND_INTERFACE_H
#define MIDIMESSAGE_COMMAND_INTERFACE_H

#include "../midimessage.h"

namespace MidiMessage {

    typedef enum {
        StringifierResultOk             = 0,
        StringifierResultGenericError   = 1,
        StringifierResultInvalidValue   = 2,
        StringifierResultWrongArgCount  = 3,
        StringifierResultPackError      = 4,
        StringifierResultParserFail     = 5,
        StringifierResultNoInput        = 6
    } StringifierResult_t;

    struct Stringifier {
        bool RunningStatusEnabled;
        uint8_t RunningStatusState;

        Stringifier(bool runningStatusEnabled){
            this->RunningStatusEnabled = runningStatusEnabled;

            this->RunningStatusState = MidiMessage_RunningStatusNotSet;
        }



        int fromString(Message_t * msg, uint8_t length, uint8_t * bytes);
        int fromArgs(Message_t * msg, uint8_t argc,  uint8_t * argv[]);
        int toString(uint8_t * bytes, Message_t * msg);
    };

    uint8_t stringToArgs(uint8_t ** argv, uint8_t max, uint8_t * bytes, uint8_t length);
}

#endif //MIDIMESSAGE_COMMAND_INTERFACE_H
