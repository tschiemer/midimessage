#include <midimessage/simpleparser.h>
#include <midimessage/commonccs.h>

#include <cstdio>

using namespace MidiMessage;

int main(){

  #define CC(ch,c,v)          (StatusClassControlChange | ch), (c & 0x7f), (v & 0x7f)

  #define NRPN_VALUE(ch,c,v)  CC(ch, CcNonRegisteredParameterMSB, (c >> 7)), CC(ch, CcNonRegisteredParameterLSB, c), CC(ch, CcDataEntryMSB, (v >> 7)), CC(ch, CcDataEntryLSB, v)
  #define NRPN_INC(ch,c,v)    CC(ch, CcNonRegisteredParameterMSB, (c >> 7)), CC(ch, CcNonRegisteredParameterLSB, c), CC(ch, CcDataIncrement, v)
  #define NRPN_DEC(ch,c,v)    CC(ch, CcNonRegisteredParameterMSB, (c >> 7)), CC(ch, CcNonRegisteredParameterLSB, c), CC(ch, CcDataDecrement, v)

  #define RPN_VALUE(ch,c,v)   CC(ch, CcRegisteredParameterMSB, (c >> 7)), CC(ch, CcRegisteredParameterLSB, c), CC(ch, CcDataEntryMSB, (v >> 7)), CC(ch, CcDataEntryLSB, v)
  #define RPN_INC(ch,c,v)     CC(ch, CcRegisteredParameterMSB, (c >> 7)), CC(ch, CcRegisteredParameterLSB, c), CC(ch, CcDataIncrement, v)
  #define RPN_DEC(ch,c,v)     CC(ch, CcRegisteredParameterMSB, (c >> 7)), CC(ch, CcRegisteredParameterLSB, c), CC(ch, CcDataDecrement, v)



  uint8_t data[] = {
    CC(1,2,3),
    NRPN_VALUE(1, 1, 1),
    CC(1,2,3),
    NRPN_INC(2, 2, 2),
    CC(1,2,3),
    CC(1,2,3),
    NRPN_DEC(3, 3, 3),
    CC(1,2,3),
    RPN_VALUE(4, 10000, 10000),
    CC(1,2,3),
    RPN_INC(5, 1000, 1),
    CC(1,2,3),
    RPN_DEC(6, 1000, 127),
  };

  // for (int i = 0; i < sizeof(data); i++){
  //   printf("%02x", data[i]);
  // }
  // printf("\n");

  uint8_t buffer[256];
  SimpleParser_t parser;

  simpleparser_init(&parser,
    false,
    buffer, sizeof(buffer),
    [](uint8_t * data, uint16_t length, void * user_data){
        printf("other message (%d): ", length);
        for(int i = 0; i < length; i++){
          printf("%02x", data[i]);
        }
        printf("\n");
    },
    #if 1
    [](uint8_t channel, NRpnType_t type, NRpnAction_t action, uint16_t controller, uint16_t value, void * user_data){
      switch(type){
        case NRpnTypeRPN: printf("RPN ");break;
        case NRpnTypeNRPN: printf("NRPN "); break;
      }

      switch(action){
        case NRpnActionValue:     printf("ch %d controller %d value %d", channel, controller, value); break;
        case NRpnActionIncrement: printf("ch %d controller %d increment %d", channel, controller, value); break;
        case NRpnActionDecrement: printf("ch %d controller %d decrement %d", channel, controller, value); break;
      }

      printf("\n");

    },
    #else
    NULL,
    #endif
    NULL, // silently ignore discarded data,
    NULL // no user/context data
  );

  simpleparser_receivedData(&parser, data, sizeof(data));

  return EXIT_SUCCESS;
}
