/**
 * \file
 */

#ifndef MIDIMESSAGE_SIMPLEPARSER_H
#define MIDIMESSAGE_SIMPLEPARSER_H

#include "midimessage/midimessage.h"

#ifdef __cplusplus
namespace MidiMessage {
    extern "C" {
#endif


    typedef enum {
      NRpnTypeRPN       = 8,
      NRpnTypeNRPN      = 16
    } NRpnType_t;

    typedef enum {
      NRpnActionValue     = 1,
      NRpnActionIncrement = 2,
      NRpnActionDecrement = 4
    } NRpnAction_t;

  // typedef void (*nrpn_callback)(nrpn_type_t type, uint16_t controller, uint16_t value);


    typedef struct {

        bool RunningStatusEnabled;

        uint8_t *Buffer;
        uint16_t MaxLength;

        void *Context;

        void (*MessageHandler)(uint8_t *message, uint16_t length, void *context);
        void (*NRpnHandler)(uint8_t channel, NRpnType_t type, NRpnAction_t action, uint16_t controller, uint16_t value, void *context);

        void (*DiscardingDataHandler)(uint8_t *bytes, uint16_t length, void *context);

        uint16_t Length;

        NRpnType_t NRpnType;
        uint8_t NRpnMsgCount;
        uint8_t NRpnChannel;
        uint16_t NRpnController;
        uint16_t NRpnValue;

    } SimpleParser_t;


    inline void simpleparser_reset(SimpleParser_t * parser) {
        parser->Length = 0;
        parser->Buffer[0] = 0;

        parser->NRpnMsgCount = 0;
    }

    inline void simpleparser_init(
      SimpleParser_t * parser,
      bool runningStatusEnabled,
      uint8_t * buffer,
      uint16_t maxLength,
      void (*messageHandler)(uint8_t *message, uint16_t length, void * context),
      void (*nrpnHandler)(uint8_t channel, NRpnType_t type, NRpnAction_t action,  uint16_t controller, uint16_t value, void *context),
      void (*discardingDataHandler)(uint8_t * bytes, uint16_t length, void * context),
      void * context){

        parser->RunningStatusEnabled = runningStatusEnabled;

        parser->Buffer = buffer;
        parser->MaxLength = maxLength;

        parser->MessageHandler = messageHandler;
        parser->NRpnHandler = nrpnHandler;
        parser->DiscardingDataHandler = discardingDataHandler;

        parser->Context = context;

        simpleparser_reset(parser);
    }


    /**
     * @param data      incoming data buffer
     * @param len       length of incoming data
     */
    void simpleparser_receivedData(SimpleParser_t * parser, uint8_t * data, uint16_t len);

#ifdef __cplusplus
    } // extern "C"
} // namespace MidiMessage
#endif

#endif /* MIDIMESSAGE_SIMPLEPARSER_H */
