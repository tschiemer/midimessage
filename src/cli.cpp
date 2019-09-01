/**
 *
 */

#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include <stdlib.h>
#include <string.h>
#include <cstring>
#include <unistd.h>
#include <getopt.h>
#include <sys/time.h>

#include <midimessage.h>
#include <stringifier.h>
#include <parser.h>


#define MAX_LEN 255

using namespace std;
using namespace MidiMessage;


///////// Defines

typedef enum {
    ModeUndefined   = 0,
    ModeParse       = 1,
    ModeGenerate    = 2
} Mode_t;

inline bool isValidMode( Mode_t mode ){
    return (mode == ModeParse || mode == ModeGenerate);
}

typedef enum {
    ResolutionMicro = 0,
    ResolutionMilli = 1
} Resolution_t;


///////// Variables

// Run mode
Mode_t mode = ModeUndefined;

bool runningStatusEnabled = false;

// Timed option
struct {
    bool enabled;
    Resolution_t resolution;
    unsigned long lastTimestamp;
} timedOpt = {
        .enabled = false,
        .resolution = ResolutionMicro
};



// Options prefix/suffix
char prefix[32] = "";
char suffix[32] = "";


void printHelp( void );
unsigned long getNow();
void writeMidiPacket( uint8_t * buffer, uint8_t length );
//void printfStringStream( char const * fmt, ... );



void printHelp( void ) {
    printf("Usage: midimessage-cli [-h?] [--running-status|-r] [--timed|-t[milli|micro]] (--parse|-p [<binary-data>]] | --generate|-g [--prefix=<prefix>] [--suffix=<suffix] [<cmd> ...])\n");

    printf("\nOptions:\n");
    printf("\t -h|-? \t\t\t\t show this help\n");
    printf("\t --running-status|-r \t\t Accept (when parsing) or generate messages that rely on the running status (see MIDI specs)\n");
    printf("\t --timed|-t[milli|micro] \t Enables the capture or playback of delta-time information (ie the time between messages). Optionally the time resolution (milliseconds or microseconds) can be specified (default = micro)\n");
    printf("\t --parse|-p [<binary-data>] \t Enter parse mode and optionally pass as first argument (binary) message to be parsed. If no argument is provided starts reading binary stream from STDIN. Each successfully parsed message will be printed to STDOUT and terminated with a newline.\n");
    printf("\t --generate|-g [<cmd> ...] \t Enter generation mode and optionally pass command to be generated. If no command is given, expects one command from STDIN per line. Generated (binary) messages are written to STDOUT.\n");
    printf("\t --prefix=<prefix> \t\t Prefixes given string (max 32 bytes) before each binary sequence (only when in generation mode). A single %%d can be given which will be replaced with the length of the following binary message (incompatible with running-status mode).\n");
    printf("\t --suffix=<suffix> \t\t Suffixes given string (max 32 bytes) before each binary sequence (only when in generation mode).\n");

    printf("\nNote: Data bytes have a value range of 0-127 - anything above is considered a control byte. There is no input validation!!\n");
    printf("Fancy pants note: the parsing output format is identical to the generation command format ;) \n");

    printf("\nData types:\n");
    printf("\t uN := N bit unsigned integer)\n");
    printf("\t\t u4 (data nibble) < 63 (0x0FF)\n");
    printf("\t\t u7 <= 127 (0x7F)\n");
    printf("\t\t u14 <= 16383 (0x3FFF)\n");
    printf("\t\t u21 <= 2097151 (0x1FFFFF)\n");
    printf("\t\t u28 <= 268435455 (0x0FFFFFFF)\n");
    printf("\t\t u35 <= 34359738367 (0x7FFFFFFFF)\n");
    printf("\t sN := N bit signed integer\n");
    printf("\t strN ((max) N byte ascii string)\n");
    printf("\t xN (N byte hex string <> 2Ns) (note: data bytes must be <= 0x7F)\n");

    printf("\nVoice Commands:\n");
    printf("\t note (on|off) <channel> <key> <velocity>\n");
    printf("\t cc <channel> <controller> <value>\n");
    printf("\t pc <channel> <program>\n");
    printf("\t pressure <channel> <pressure>\n");
    printf("\t pitch <channel> <pitch>\n");
    printf("\t poly <channel> <key> <pressure>\n");

    printf("\nSystem Commands:\n");
    printf("\t start\n");
    printf("\t stop\n");
    printf("\t continue\n");
    printf("\t reset\n");
    printf("\t active-sensing\n");
    printf("\t tune-request\n");
    printf("\t timing-clock\n");
    printf("\t quarter-frame <messageType> <nibble>\n");
    printf("\t song-position <position>\n");
    printf("\t song-select <songNumber>\n");

    printf("\n(General) System Exclusives*:\n");
    printf("\t sysex experimental <data (xN)>\n");
    printf("\t sysex manufacturer <manufacturer-id (x1..3)> <data (xN)>\n");
    printf("\t sysex nonrt <device-id (u7)> (eof|wait|cancel|nak|ack) <packet-number>\n");
    printf("\t sysex nonrt <device-id (u7)> info request\n");
    printf("\t sysex nonrt <device-id (u7)> info reply <manufacturer-id (x1, x3)> <device-family (u14)> <device-family-member (u14)> <software-revision (x4)>\n");
    printf("\t sysex nonrt <device-id (u7)> gm (system-on1|system-off|system-on2)\n");
    printf("* <device-id> := 127 is all devices\n");

    printf("\n MIDI Time Code + Cueing\n");
    printf("\t sysex rt <device-id (u7)> mtc full-message <fps = 24,25,29.97,30> <hour <= 23> <minute <= 59> <second <= 59> <frame < fps>\n");
    printf("\t sysex rt <device-id (u7)> mtc user-bits <5bytes (x5)>\n");
    printf("\t sysex nonrt <device-id (u7)> cueing special (time-code-offset|enable-event-list|disable-event-list|clear-event-list|system-stop|event-list-request|<(u14)>)\n");
    printf("\t sysex nonrt <device-id (u7)> cueing (punch-in|punch-out) (add|rm) <fps = 24,25,29.97,30> <hour <= 23> <minute <= 59> <second <= 59> <frame < fps> <fractional-frame <= 99> <event-number (u14)>\n");
    printf("\t sysex nonrt <device-id (u7)> cueing (event-start|event-stop|cue-point) (add|rm) <fps = 24,25,29.97,30> <hour <= 23> <minute <= 59> <second <= 59> <frame < fps> <fractional-frame <= 99> <event-number (u14)> [<event-name (str) ..>]\n");
    printf("\t sysex nonrt <device-id (u7)> cueing event-name - <fps = 24,25,29.97,30> <hour <= 23> <minute <= 59> <second <= 59> <frame < fps> <fractional-frame <= 99> <event-number (u14)> <event-name (str) ..>\n");
    printf("\t sysex rt <device-id (u7)> cueing special (system-stop|<(u14)>)\n");
    printf("\t sysex rt <device-id (u7)> cueing (punch-in|punch-out) <event-number (u14)>\n");
    printf("\t sysex rt <device-id (u7)> cueing (event-start|event-stop|cue-point) <event-number (u14)> [<event-name (str) ..>]\n");
    printf("\t sysex rt <device-id (u7)> cueing event-name <event-number (u14)> <event-name (str) ..>\n");
    printf("** <cue-number>, <cue-list>, <cue-path> := ascii numbers (0-9) and/or dots (.)\n");

    printf("\nTODO:\n");

    printf("\nMIDI Show Control (MSC)\n");
    printf("\t sysex rt <device-id (u7)> msc <cmdFmt (u7)> (all-off|restore|reset)\n");
    printf("\t sysex rt <device-id (u7)> msc <cmdFmt (u7)> (go|stop|resume|load|go-off|go-jam-lock) <cue-number**> [<cue-list**> [<cue-path**>]]\n");
    printf("\t sysex rt <device-id (u7)> msc <cmdFmt (u7)> timed-go <fps = 24,25,29.97,30> <hour <= 23> <minute <= 59> <second <= 59> <frame < fps> < (x1)> <cue-number**> [<cue-list**> [<cue-path**>]]\n");
    printf("\t sysex rt <device-id (u7)> msc <cmdFmt (u7)> set <controller (u14)> <value (u14)> <fps = 24,25,29.97,30> <hour <= 23> <minute <= 59> <second <= 59> <frame < fps> < (x1)>\n");
    printf("\t sysex rt <device-id (u7)> msc <cmdFmt (u7)> fire <macro-number (u7)>\n");
    printf("\t sysex rt <device-id (u7)> msc <cmdFmt (u7)> (standby+|standby-|sequence+|sequence-|start-clock|stop-clock|zero-clock|mtc-chase-on|mtc-chase-off|open-cue-list|close-cue-list) <cue-list**>\n");
    printf("\t sysex rt <device-id (u7)> msc <cmdFmt (u7)> (open-cue-path|close-cue-path) <cue-path**>\n");
    printf("\t sysex rt <device-id (u7)> msc <cmdFmt (u7)> set-clock <fps = 24,25,29.97,30> <hour <= 23> <minute <= 59> <second <= 59> <frame < fps> < (x1)> <cue-list**>\n");
    printf("\t sysex rt <device-id (u7)> msc <cmdFmt (u7)> (standby|go-2-pc) <checksum (u14)> <sequence-number (u14)> <data (x4)> <cue-number**> [<cue-list**> [<cue-path**>]]\n");
    printf("\t sysex rt <device-id (u7)> msc <cmdFmt (u7)> standing-by <checksum (u14)> <sequence-number (u14)> <fps = 24,25,29.97,30> <hour <= 23> <minute <= 59> <second <= 59> <frame < fps> < (x1)> <cue-number**> [<cue-list**> [<cue-path**>]]\n");
    printf("\t sysex rt <device-id (u7)> msc <cmdFmt (u7)> (cancelled|abort) <checksum (u14)> <status (u16)> <sequence-number (u14)> <data (x4)> <cue-number**> [<cue-list**> [<cue-path**>]]\n");

    printf("\nMIDI Machine Commands (MMC)\n");
    printf("For MMC the MIDI format acts as container for a command stream of its own, where several MMC commands can be packed into one MIDI message.\n");
    printf("\n\t sysex rt <device-id (u7) mmc cmd <command1 ..> [<command2 ..> [ .. <commandN ..>] .. ]]\n");
    printf("\t\t <commandN ..> :\n");
    printf("\t\t (stop|play|deferred-play|fast-forward|rewind|record-strobe|record-exit|record-pause|pause|eject|chase|cmd-error-reset|mmc-reset|wait|resume)\n");
    printf("\t\t (variable-play|search|shuttle|deferred-variable-play|record-strobe-variable) <speed (float)>\n");
    printf("\t\t step <step (s7)>\n");
    printf("\t\t write ..\n");
    printf("\t\t locate <fps = 24,25,29.97,30> <hour <= 23> <minute <= 59> <second <= 59> <frame < fps>)\n");
    printf("\t\t move ..\n");
    printf("\t\t <data (xN)>\n");




    printf("\nExamples:\n");
    printf("\t bin/midimessage-cli -g note on 60 40 1\n");
    printf("\t bin/midimessage-cli -g | bin/midimessage-cli -p\n");
    printf("\t bin/midimessage-cli -g --prefix='%%d ' --suffix=$'\\x0a'\n");
    printf("\t bin/midimessage-cli -g | bin/midimessage-cli -ptmilli > test.recording\n");
    printf("\t cat test.recording | bin/midimessage-cli -gtmilli | bin/midimessage-cli -p\n");
}


unsigned long getNow(){

    struct timespec c;

    if (clock_gettime(CLOCK_REALTIME, &c) == -1) {
        perror("error calling clock_gettime()");
        exit(EXIT_FAILURE);
    }

    if (timedOpt.resolution == ResolutionMilli){
        return c.tv_sec * 1000 + c.tv_nsec / 1000000;
    }

    return c.tv_sec * 1000000 + c.tv_nsec / 1000;
}



void writeMidiPacket( Message_t * msg ){


    uint8_t bytes[128];
    uint8_t length = pack( bytes, msg );

    if (length == 0){
        return;
    }

    printf(prefix, length);

    fwrite(bytes, 1, length, stdout);

    printf("%s", suffix);

    fflush(stdout);
}

void parsedMessage( Message_t * msg ){

    Stringifier stringifier(runningStatusEnabled);

    uint8_t stringBuffer[256];

    int length = stringifier.toString(  stringBuffer, msg );

    if ( length > 0 ) {

        if (timedOpt.enabled){

            unsigned long now = getNow();
            unsigned long diff = now - timedOpt.lastTimestamp;

            printf("%ld ", diff);

            timedOpt.lastTimestamp = now;
        }

        printf("%s\n", stringBuffer);
        
        fflush(stdout);
    }
}
//
//
//
//void printfStringStream( char const * fmt, ... ){
//    va_list args;
//    va_start(args, fmt);
//    int l = vsprintf( &stringStream.Buffer[stringStream.Length], fmt, args);
//    va_end (args);
//
//    stringStream.Buffer[stringStream.Length + l] = '\0';
//    stringStream.Length += l;
//}


int main(int argc, char * argv[], char * env[]){

    int c;
    int digit_optind = 0;

    if (argc <= 1){
        printHelp();
        return EXIT_SUCCESS;
    }
    

    while (1) {
        int this_option_optind = optind ? optind : 1;
        int option_index = 0;

        static struct option long_options[] = {
                {"parse",    no_argument,    0,  'p' },
                {"generate",   no_argument,    0,  'g' },
                {"timed", optional_argument,  0, 't'},
                {"running-status", optional_argument, 0, 'r'},
                {"prefix", required_argument, 0, 0},
                {"suffix", required_argument, 0, 0},
                {"help",    no_argument,    0,  'h' },
                {0,         0,              0,  0 }
        };

        c = getopt_long(argc, argv, "pgt::rh?",
                        long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
            case 0:

                if (strcmp(long_options[option_index].name, "prefix") == 0) {
                    if (mode != ModeGenerate) {
                        printf("Can only use prefix when generating messages!\n");
                        exit(EXIT_FAILURE);
                    }
                    strcpy(prefix, optarg);
                }
                if (strcmp(long_options[option_index].name, "suffix") == 0) {
                    if (mode != ModeGenerate) {
                        printf("Can only use prefix when generating messages!\n");
                        exit(EXIT_FAILURE);
                    }
                    strcpy(suffix, optarg);
                }
                break;

            case '?':
            case 'h':
                printHelp();
                return EXIT_SUCCESS;

            case 'p':
                if (mode == ModeGenerate){
                    printf("Already set to parsing - can not do both!\n");
                    exit(EXIT_FAILURE);
                }
                mode = ModeParse;
                break;

            case 'g':
                if (mode == ModeParse){
                    printf("Already set to generating - can not do both!\n");
                    return 1;
                }
                mode = ModeGenerate;
                break;

            case 't':
                timedOpt.enabled = true;
                if (optarg != NULL && strlen(optarg) > 0){
                    if (strcmp(optarg, "milli") == 0){
                        timedOpt.resolution = ResolutionMilli;
                    } else if (strcmp(optarg, "micro") == 0) {
                        timedOpt.resolution = ResolutionMicro;
                    } else {
                        printf("Error: invalid time scale, either 'milli' or 'micro', %s given\n", optarg);
                        exit(EXIT_FAILURE);
                    }
                }
                break;

            case 'r':
                runningStatusEnabled = true;
                break;

            default:
                printf("?? getopt returned character code 0%o ??\n", c);
        }
    }

    if ( ! isValidMode( mode) ){
        printf("Must be called in either parsing or generation mode!\n");
        exit(EXIT_FAILURE);
    }


    if (optind < argc) {
        if (mode == ModeGenerate){

            Stringifier stringifier(runningStatusEnabled);

            uint8_t sysexBuffer[128];
            Message_t msg = {
                    .Data.SysEx.ByteData = sysexBuffer
            };

            if (stringifier.fromArgs( &msg, (uint8_t)(argc - optind), (uint8_t**)&argv[optind] ) == StringifierResultOk){

                writeMidiPacket( &msg );
            }


            exit(EXIT_SUCCESS);
        } else {

            printf("Parser mode may not be called with additional arguments - data is read from stdin only.\n");
            exit(EXIT_FAILURE);
        }
    }

    if (mode == ModeParse){


        uint8_t sysexBuffer[128];
        Message_t msg = {
                .Data.SysEx.ByteData = sysexBuffer
        };

        uint8_t dataBuffer[128];
        Parser parser(runningStatusEnabled, dataBuffer, 128, &msg, parsedMessage );

        // start timer
        if (timedOpt.enabled){
            // record timer
            timedOpt.lastTimestamp = getNow();
        }


        int ch;
        while( (ch = fgetc(stdin)) != EOF ){
            uint8_t byte = (uint8_t)ch;

            parser.receivedData( &byte, 1 );
        }

        return EXIT_SUCCESS;
    }
    
    else { //if ( mode == ModeGenerate) {

        Stringifier stringifier(runningStatusEnabled);

        uint8_t sysexBuffer[128];
        Message_t msg = {
                .Data.SysEx.ByteData = sysexBuffer
        };


        // start timer
        if (timedOpt.enabled){
            // record timer
            timedOpt.lastTimestamp = getNow();
        }

        while(true){

            uint8_t *line;

            uint8_t *args[32];
            uint8_t argsCount = stringToArgs( args, 32, line, strlen((char*)line));

            if (argsCount == 0){
                continue;
            }

            int result = 0;

            if (timedOpt.enabled){
                unsigned long delay = strtoul((char*)args[0], NULL, 10);
                unsigned long waitUntil = timedOpt.lastTimestamp + delay;
                unsigned long now;
                do {
                    now = getNow();
                } while( now < waitUntil );

                timedOpt.lastTimestamp = now;

                result = stringifier.fromArgs( &msg,  argsCount - 1 , (uint8_t**)&args[1] );

            } else {
                result = stringifier.fromArgs( &msg,  argsCount, (uint8_t**)args );
            }

            if (result == StringifierResultOk){

                writeMidiPacket( &msg );
            }
        }

        return EXIT_SUCCESS;
    }
//
//    uint8_t pos = 0;
//    uint8_t line[MAX_LEN];
//
//    line[0] = 0;
//
//    uint8_t argsCount = 1;
//    uint8_t * args[16];
//
//    args[0] = &line[0];
//
////    stringStream.Length = 0;
//
//    int ch;
//
//    // start timer
//    if (timedOpt.enabled){
//        // record timer
//        timedOpt.lastTimestamp = getNow();
//    }
//
//    while( (ch = fgetc(stdin)) != EOF ){
//
//        uint8_t in = (uint8_t)ch;
//
//
//        if (pos >= MAX_LEN){
//            printf("Buffer overflow!\n");
//            return 1;
//        }
//
//
//        if (mode == ModeGenerate){
//
//            if (in == '\r'){
//                continue;
//            }
//            if (in == ' '){
//                if (pos > 0 && line[pos-1] != '\0') {
//                    line[pos++] = '\0';
//                    args[argsCount++] = &line[pos];
//                }
//                continue;
//            }
//            if (in != '\n'){
//                line[pos++] = in;
//                if (pos == 1){
//                    argsCount = 1;
//                }
//                continue;
//            }
//            if (in == '\n'){
//                if (pos > 0 && line[pos-1] != '\0') {
//                    line[pos++] = '\0';
//                } else if (pos > 0 && line[pos-1] == '\0'){
//                    argsCount--;
//                }
//
//                if (timedOpt.enabled){
//                    unsigned long delay = std::strtoul((char*)args[0], NULL, 10);
//                    unsigned long waitUntil = timedOpt.lastTimestamp + delay;
//                    unsigned long now;
//                    do {
//                        now = getNow();
//                    } while( now < waitUntil );
//
//                    timedOpt.lastTimestamp = now;
//
//                    stringifier.fromString(  argsCount - 1 , (uint8_t**)&args[1] );
//
//                } else {
//                    stringifier.fromString(  argsCount, (uint8_t**)args );
//                }
//
//                pos = 0;
//                line[0] = '\0';
//
//                argsCount = 0;
//            }
//
//        }
//        if (mode == ModeParse) {
//
//
//
//            if (pos == 0 && isDataByte(in)){
//                if (runningStatusEnabled && isRunningStatus(line[0])){
//                    pos++;
//                } else {
//                    // wait for control byte
//                    continue;
//                }
//            }
//
//
//            line[pos++] = in;
//
//            uint8_t result = stringifier.toString(  pos, (uint8_t *) line);
//
//            if ( StringifierResultOk == result ) {
//                pos = 0;
//
//                if (timedOpt.enabled){
//
//                    unsigned long now = getNow();
//                    unsigned long diff = now - timedOpt.lastTimestamp;
//
//                    printf("%ld ", diff);
//
//                    timedOpt.lastTimestamp = now;
//                }
//
//                fwrite(stringStream.Buffer, 1, stringStream.Length, stdout);
//
//                printf("\n");
//
//                stringStream.Length = 0;
//            }
//        }
//    }
//
//    return EXIT_SUCCESS;
}