/**
 * \file
 */

#ifndef MIDIMESSAGE_COMMAND_INTERFACE_H
#define MIDIMESSAGE_COMMAND_INTERFACE_H

#include "../midimessage.h"

#ifdef __cplusplus
namespace MidiMessage {
    extern "C" {
#endif

    typedef enum {
        StringifierResultOk = 0,
        StringifierResultGenericError = 1,
        StringifierResultInvalidValue = 2,
        StringifierResultWrongArgCount = 3,
        StringifierResultPackError = 4,
        StringifierResultParserFail = 5,
        StringifierResultNoInput = 6,
        StringifierResultInvalidU4 = 10,
        StringifierResultInvalidU7 = 11,
        StringifierResultInvalidU14 = 12,
        StringifierResultInvalidU21 = 13,
        StringifierResultInvalidU28 = 14,
        StringifierResultInvalidU35 = 15,
        StringifierResultInvalidHex = 20
    } StringifierResult_t;


    /**
     * Shorthand for fromArgs() when working with strings instead of argument lists
     *
     * Uses stringToArgs() to generate argument list.
     *
     * !! WARNING the passed line/string buffer is modified by the function WARNING !!
     *
     * @see stringToArgs()
     */
    int MessagefromString(Message_t *msg, uint8_t length, uint8_t *bytes);

    /**
     * Attempts to interprete given argument list and setting the given msg values.
     *
     * (Quasi-)Inversion of toString
     *
     * @param msg       Destination message to write settings to
     * @param argc      Argument count
     * @param argv      Argument list
     * @return          StringifierResult_t
     */
    int MessagefromArgs(Message_t *msg, uint8_t argc, uint8_t *argv[]);

    /**
     * Stringifies the given message unto the given destination buffer
     *
     * @param bytes     Destination buffer to write message to
     * @param msg       Source message from which to generate human-readable message
     * @return          Length of generated
     */
    int MessagetoString(uint8_t *bytes, Message_t *msg);

    /**
     * Helper function to argumentify a given command line delimited by spaces.
     *
     * Trims as follows:
     *  - any first non-whitespace character designates the beginning of the first argument
     *  - carriage return-, newline-, null- characters indicate the (post-)end of the last argument
     *
     * !! WARNING the passed line/string buffer is modified by the function WARNING !!
     *
     * @param argv      argv-structure to use
     * @param maxArg    maximal length of pointers fitting into argv
     * @param bytes     input char sequence to argumentify
     * @param length    length of input char sequence
     * @return          Number of args detected in argv
     */
    uint8_t stringToArgs(uint8_t **argv, uint8_t maxArg, uint8_t *bytes, uint8_t length);

#ifdef __cplusplus
    } // extern "C"
} // namespace MidiMessage
#endif

#endif //MIDIMESSAGE_COMMAND_INTERFACE_H
