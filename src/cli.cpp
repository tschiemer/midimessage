/**
 *
 */

#include <stdio.h>
#include <cstdlib>
#include <stdlib.h>
#include <string.h>
#include <cstring>
#include <unistd.h>
#include <getopt.h>
#include <sys/time.h>
#include <midimessage.h>
#include <util-hex.h>

#include "midimessage.h"

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

// Message struct
uint8_t buf[128];
Message_t msg = {
        .Data.SysEx.ByteData = buf
};

// Timed option
struct {
    bool enabled;
    Resolution_t resolution;
    unsigned long lastTimestamp;
} timedOpt = {
        .enabled = false,
        .resolution = ResolutionMicro
};

// Option running status
bool runningStatusEnabled = false;

// Options prefix/suffix
char prefix[32] = "";
char suffix[32] = "";


void generate(uint8_t argc,  char  * argv[]);
uint8_t parse(uint8_t length, uint8_t bytes[]);


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
    printf("\t u4 (data nibble) < 63 (0x0FF)\n");
    printf("\t u7 (data byte) <= 127 (0x7F)\n");
    printf("\t u14 (2byte integer) <= 16383 (0x3FFF)\n");
    printf("\t u21 (3byte integer) <= 2097151 (0x1FFFFF)\n");
    printf("\t u28 (4byte integer) <= 268435455 (0x0FFFFFFF)\n");
    printf("\t u35 (5byte integer) <= 34359738367 (0x7FFFFFFFF)\n");
    printf("\t sN ((max) N byte ascii string)\n");
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

    printf("\nSystem Exclusives*:\n");
    printf("\t sysex experimental <data (xN)>\n");
    printf("\t sysex manufacturer <manufacturer-id (x1..3)> <data (xN)>\n");
    printf("\t sysex nonrt <device-id (u7)> (eof|wait|cancel|nak|ack) <packet-number>\n");
    printf("\t sysex nonrt <device-id (u7)> info request\n");
    printf("\t sysex nonrt <device-id (u7)> info reply <manufacturer-id (x1, x3)> <device-family (u14)> <device-family-member (u14)> <software-revision (x4)>\n");
    printf("\t sysex nonrt <device-id (u7)> gm (system-on1|system-off|system-on2)\n");
    printf("\t sysex nonrt <device-id (u7)> cueing special (time-code-offset|enable-event-list|disable-event-list|clear-event-list|system-stop|event-list-request|<(u14)>)\n");
    printf("\t sysex nonrt <device-id (u7)> cueing (punch-in|punch-out) (add|rm) <fps = 24,25,29.97,30> <hour <= 23> <minute <= 59> <second <= 59> <frame < fps> <fractional-frame <= 99> <event-number (u14)>\n");
    printf("\t sysex nonrt <device-id (u7)> cueing (event-start|event-stop|cue-point) (add|rm) <fps = 24,25,29.97,30> <hour <= 23> <minute <= 59> <second <= 59> <frame < fps> <fractional-frame <= 99> <event-number (u14)> [<event-name (s) ..>]\n");
    printf("\t sysex nonrt <device-id (u7)> cueing event-name - <fps = 24,25,29.97,30> <hour <= 23> <minute <= 59> <second <= 59> <frame < fps> <fractional-frame <= 99> <event-number (u14)> <event-name (s) ..>\n");
    printf("\t sysex rt <device-id (u7)> mtc full-message <fps = 24,25,29.97,30> <hour <= 23> <minute <= 59> <second <= 59> <frame < fps>\n");
    printf("\t sysex rt <device-id (u7)> mtc user-bits <5bytes (x5)>\n");
    printf("\t sysex rt <device-id (u7)> cueing special (system-stop|<(u14)>)\n");
    printf("\t sysex rt <device-id (u7)> cueing (punch-in|punch-out) <event-number (u14)>\n");
    printf("\t sysex rt <device-id (u7)> cueing (event-start|event-stop|cue-point) <event-number (u14)> [<event-name (s) ..>]\n");
    printf("\t sysex rt <device-id (u7)> cueing event-name <event-number (u14)> <event-name (s) ..>\n");
    printf("*<device-id> := 127 is broadcast\n");


    printf("\nExamples:\n");
    printf("\t bin/midimessage-cli -g note on 60 40 1\n");
    printf("\t bin/midimessage-cli -g | bin/midimessage-cli -p\n");
    printf("\t bin/midimessage-cli -g --prefix='%%d ' --suffix=$'\\x0a'\n");
    printf("\t bin/midimessage-cli -g | bin/midimessage-cli -ptmilli > test.recording\n");
    printf("\t cat test.recording | bin/midimessage-cli -gtmilli | bin/midimessage-cli -p\n");
}



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
                if (strlen(optarg) > 0){
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
            generate( (uint8_t)(argc - optind), &argv[optind] );
        } else {
            while(optind < argc) {
                parse( (uint8_t)strlen(argv[optind]), (uint8_t*)argv[optind] );
                optind++;
            }
        }

        exit(EXIT_SUCCESS);
    }

    uint8_t pos = 0;
    uint8_t line[MAX_LEN];

    line[0] = 0;

    uint8_t argsCount = 1;
    uint8_t * args[16];

    args[0] = &line[0];

    int ch;

    // start timer
    if (timedOpt.enabled){
        // record timer
        timedOpt.lastTimestamp = getNow();
    }

    while( (ch = fgetc(stdin)) != EOF ){

        uint8_t in = (uint8_t)ch;


        if (pos >= MAX_LEN){
            printf("Buffer overflow!\n");
            return 1;
        }


        if (mode == ModeGenerate){

            if (in == '\r'){
                continue;
            }
            if (in == ' '){
                if (pos > 0 && line[pos-1] != '\0') {
                    line[pos++] = '\0';
                    args[argsCount++] = &line[pos];
                }
                continue;
            }
            if (in != '\n'){
                line[pos++] = in;
                if (pos == 1){
                    argsCount = 1;
                }
                continue;
            }
            if (in == '\n'){
                if (pos > 0 && line[pos-1] != '\0') {
                    line[pos++] = '\0';
                } else if (pos > 0 && line[pos-1] == '\0'){
                    argsCount--;
                }

                if (timedOpt.enabled){
                    unsigned long delay = std::strtoul((char*)args[0], NULL, 10);
                    unsigned long waitUntil = timedOpt.lastTimestamp + delay;
                    unsigned long now;
                    do {
                        now = getNow();
                    } while( now < waitUntil );

                    timedOpt.lastTimestamp = now;

                    generate( argsCount - 1 , (char**)&args[1] );

                } else {
                    generate( argsCount, (char**)args );
                }

                pos = 0;
                line[0] = '\0';

                argsCount = 0;
            }

        }
        if (mode == ModeParse) {

            if (pos == 0 && isDataByte(in)){
                if (runningStatusEnabled && isRunningStatus(line[0])){
                    pos++;
                } else {
                    // wait for control byte
                    continue;
                }
            }


            line[pos++] = in;

            if ( parse(pos, (uint8_t *) line) ) {
                pos = 0;
            }
        }
    }

    return EXIT_SUCCESS;
}
void generateLoop(void){

}
void parseLoop(void) {

}

void generate(uint8_t argc, char * argv[]){

    uint8_t bytes[128];
    uint8_t length = 0;

    static uint8_t runningStatus = MidiMessage_RunningStatusNotSet;

    if (argc == 0){
        return;
    }

//    for(int i = 0; i < argc; i++){
//        printf("%s ", argv[i]);
//    }
//    printf("\n");
//    fflush(stdout);

    if (strcmp(argv[0], "note") == 0){
        if (argc != 5){
            return;
        }
        if (strcmp(argv[1], "on") == 0){
            msg.StatusClass = StatusClassNoteOn;
        }
        else if (strcmp(argv[1], "off") == 0){
            msg.StatusClass = StatusClassNoteOff;
        }
        else {
            return;
        }
        msg.Channel = atoi(argv[2]);
        msg.Data.Note.Key = atoi(argv[3]);
        msg.Data.Note.Velocity = atoi(argv[4]);
    }

    else if (strcmp(argv[0], "cc") == 0){
        if (argc != 4){
            return;
        }
        msg.StatusClass = StatusClassControlChange;
        msg.Channel = atoi(argv[1]);
        msg.Data.ControlChange.Controller = atoi(argv[2]);
        msg.Data.ControlChange.Value = atoi(argv[3]);
    }

    else if (strcmp(argv[0], "pc") == 0){
        if (argc != 3){
            return;
        }
        msg.StatusClass = StatusClassProgramChange;
        msg.Channel = atoi(argv[1]);
        msg.Data.ProgramChange.Program = atoi(argv[2]);
    }

    else if (strcmp(argv[0], "pressure") == 0){
        if (argc != 3){
            return;
        }
        msg.StatusClass = StatusClassChannelPressure;
        msg.Channel = atoi(argv[1]);
        msg.Data.ChannelPressure.Pressure = atoi(argv[2]);
    }

    else if (strcmp(argv[0], "pitch") == 0){
        if (argc != 3){
            return;
        }
        msg.StatusClass = StatusClassPitchBendChange;
        msg.Channel = atoi(argv[1]);
        msg.Data.PitchBendChange.Pitch = atoi(argv[2]);
    }

    else if (strcmp(argv[0], "poly") == 0){
        if (argc != 4){
            return;
        }
        msg.StatusClass = StatusClassPolyphonicKeyPressure;
        msg.Channel = atoi(argv[1]);
        msg.Data.PolyphonicKeyPressure.Key = atoi(argv[2]);
        msg.Data.PolyphonicKeyPressure.Pressure = atoi(argv[3]);
    }

    else if (strcmp(argv[0], "quarter-frame") == 0){
        if (argc != 3){
            return;
        }
        msg.StatusClass = StatusClassSystemMessage;
        msg.SystemMessage = SystemMessageMtcQuarterFrame;
        msg.Data.MtcQuarterFrame.MessageType = atoi(argv[1]);
        msg.Data.MtcQuarterFrame.Nibble = atoi(argv[2]);
    }
    else if (strcmp(argv[0], "song-position") == 0){
        if (argc != 2){
            return;
        }
        msg.StatusClass = StatusClassSystemMessage;
        msg.SystemMessage = SystemMessageSongPositionPointer;
        msg.Data.SongPositionPointer.Position = atoi(argv[1]);
    }
    else if (strcmp(argv[0], "song-select") == 0){
        if (argc != 2){
            return;
        }
        msg.StatusClass = StatusClassSystemMessage;
        msg.SystemMessage = SystemMessageSongSelect;
        msg.Data.SongSelect.Song = atoi(argv[1]);
    }
    else if (strcmp(argv[0], "start") == 0){
        msg.StatusClass = StatusClassSystemMessage;
        msg.SystemMessage = SystemMessageStart;
    }
    else if (strcmp(argv[0], "stop") == 0){
        msg.StatusClass = StatusClassSystemMessage;
        msg.SystemMessage = SystemMessageStop;
    }
    else if (strcmp(argv[0], "continue") == 0){
        msg.StatusClass = StatusClassSystemMessage;
        msg.SystemMessage = SystemMessageContinue;
    }
    else if (strcmp(argv[0], "active-sensing") == 0){
        msg.StatusClass = StatusClassSystemMessage;
        msg.SystemMessage = SystemMessageActiveSensing;
    }
    else if (strcmp(argv[0], "reset") == 0){
        msg.StatusClass = StatusClassSystemMessage;
        msg.SystemMessage = SystemMessageReset;
    }
    else if (strcmp(argv[0], "timing-clock") == 0){
        msg.StatusClass = StatusClassSystemMessage;
        msg.SystemMessage = SystemMessageTimingClock;
    }
    else if (strcmp(argv[0], "tune-request") == 0){
        msg.StatusClass = StatusClassSystemMessage;
        msg.SystemMessage = SystemMessageTuneRequest;
    }
    else if (strcmp(argv[0], "sysex") == 0){
        if (argc < 2){
            return;
        }

        msg.StatusClass = StatusClassSystemMessage;
        msg.SystemMessage = SystemMessageSystemExclusive;


        if (strcmp(argv[1], "experimental") == 0) {
            if (argc < 2 || argc > 3){
                return;
            }

            msg.Data.SysEx.Id = SysExIdExperimental;
            if (argc < 3) {
                msg.Data.SysEx.Length = 0;
            } else {
                int len = strlen(argv[2]);
                if (len % 2 != 0){
                    return;
                }
                len >>= 1;
                if (!hex_to_byte(msg.Data.SysEx.ByteData, (uint8_t*)argv[2], len)){
                    return;
                }
                msg.Data.SysEx.Length = len;
            }
        }
        else if (strcmp(argv[1], "manufacturer") == 0) {
            if (argc < 3 || argc > 4){
                return;
            }
            msg.Data.SysEx.Id = std::strtol(argv[2], NULL, 16);

            if (argc < 4) {
                msg.Data.SysEx.Length = 0;
            } else {
                int len = strlen(argv[3]);
                if (len % 2 != 0){
                    return;
                }
                len >>= 1;
                if (!hex_to_byte(msg.Data.SysEx.ByteData, (uint8_t*)argv[3], len)){
                    return;
                }
                msg.Data.SysEx.Length = len;
            }
        }
        else if (strcmp(argv[1], "rt") == 0){
            if (argc < 4){
                return;
            }
            msg.Channel = atoi(argv[2]);
            msg.Data.SysEx.Id = SysExIdRealTime;

            if (strcmp(argv[3], "mtc") == 0){
                if (argc < 5){
                    return;
                }
                msg.Data.SysEx.SubId1 = SysExRtMidiTimeCode;

                if (strcmp(argv[4], "full-message") == 0){
                    if (argc < 10){
                        return;
                    }
                    msg.Data.SysEx.SubId2 = SysExRtMtcFullMessage;
                    uint8_t fps = atoi(argv[5]);

                    switch(fps){
                        case 24: fps = MtcFrameRate24fps; break;
                        case 25: fps = MtcFrameRate25fps; break;
                        case 29: fps = MtcFrameRate29_97fps; break;
                        case 30: fps = MtcFrameRate30fps; break;
                        default: return;
                    }

                    msg.Data.SysEx.Data.MidiTimeCode.Fps = fps;
                    msg.Data.SysEx.Data.MidiTimeCode.Fps = atoi(argv[6]);
                    msg.Data.SysEx.Data.MidiTimeCode.Minute = atoi(argv[7]);
                    msg.Data.SysEx.Data.MidiTimeCode.Second = atoi(argv[8]);
                    msg.Data.SysEx.Data.MidiTimeCode.Frame = atoi(argv[9]);
                }
                if (strcmp(argv[4], "user-bits") == 0){
                    if (argc < 6){
                        return;
                    }
                    msg.Data.SysEx.SubId2 = SysExRtMtcUserBits;
                    unsigned long long bits = strtoull(argv[5], NULL, 16);
                    msg.Data.SysEx.ByteData[0] = (bits >> 32) & DataMask;
                    msg.Data.SysEx.ByteData[1] = (bits >> 24) & DataMask;
                    msg.Data.SysEx.ByteData[2] = (bits >> 16) & DataMask;
                    msg.Data.SysEx.ByteData[3] = (bits >> 8) & DataMask;
                    msg.Data.SysEx.ByteData[4] = bits & DataMask;
                    msg.Data.SysEx.Length = 5;
                }
            }
            else if (strcmp(argv[3], "cueing") == 0){
                if (argc < 5){
                    return;
                }
                msg.Data.SysEx.SubId1 = SysExRtMidiTimeCodeCueing;

                if (strcmp(argv[4], "special") == 0){
                    if (argc != 6){
                        return;
                    }
                    msg.Data.SysEx.SubId2 = SysExRtMtcCueingSpecial;

                    if (strcmp(argv[5], "system-stop") == 0){
                        msg.Data.SysEx.Data.Cueing.EventNumber = SysExRtMtcCueingSpecialSystemStop;
                    }
                    else {
                        msg.Data.SysEx.Data.Cueing.EventNumber = atoi(argv[5]);
                    }
                }
                else {
                    if (argc < 6){
                        return;
                    }

                    if (strcmp(argv[4], "punch-in") == 0){
                        msg.Data.SysEx.SubId2 = SysExRtMtcCueingPunchInPoint;
                    }
                    else if (strcmp(argv[4], "punch-out") == 0){
                        msg.Data.SysEx.SubId2 = SysExRtMtcCueingPunchOutPoint;
                    }
                    else if (strcmp(argv[4], "event-start") == 0){
                        if (argc == 6) {
                            msg.Data.SysEx.SubId2 = SysExRtMtcCueingEventStartPoint;
                        } else {
                            msg.Data.SysEx.SubId2 = SysExRtMtcCueingEventStartPointWithInfo;
                        }
                    }
                    else if (strcmp(argv[4], "event-stop") == 0){
                        if (argc == 6){
                            msg.Data.SysEx.SubId2 = SysExRtMtcCueingEventStopPoint;
                        } else {
                            msg.Data.SysEx.SubId2 = SysExRtMtcCueingEventStopPointWithInfo;
                        }
                    }
                    else if (strcmp(argv[4], "cue-point") == 0){
                        if (argc == 6) {
                            msg.Data.SysEx.SubId2 = SysExRtMtcCueingCuePoint;
                        } else {
                            msg.Data.SysEx.SubId2 = SysExRtMtcCueingCuePointWithInfo;
                        }
                    }
                    else if (strcmp(argv[4], "event-name") == 0){
                        if (argc < 7){
                            return;
                        }
                        msg.Data.SysEx.SubId2 = SysExRtMtcCueingEventName;
                    }
                    else {
                        return;
                    }

                    msg.Data.SysEx.Data.Cueing.EventNumber = atoi(argv[5]);

                    if (isSysExRtMtcCueingWithAddInfo(msg.Data.SysEx.SubId2)){
                        uint8_t offset = 0;
                        for(uint8_t i = 0; i < argc - 6; i++){
                            if (i > 0){
                                msg.Data.SysEx.ByteData[offset++] = ' ';
                            }
                            strcpy((char*)&msg.Data.SysEx.ByteData[offset], argv[6+i]);
                            offset += strlen(argv[6+i]);

                        }
                        msg.Data.SysEx.Length += strlen((char*)msg.Data.SysEx.ByteData);
                    } else {
//                        if (argc != 13){
//                            return;
//                        }
                    }

                }
            }
            else {
                return;
            }

        }
        else if (strcmp(argv[1], "nonrt") == 0) {
            if (argc < 4){
                return;
            }

            msg.Channel = atoi(argv[2]);
            msg.Data.SysEx.Id = SysExIdNonRealTime;

            if (strcmp(argv[3], "eof") == 0){
                if (argc != 5){
                    return;
                }
                msg.Data.SysEx.SubId1 = SysExNonRtEndOfFile;
                msg.Data.SysEx.Data.PacketNumber = atoi(argv[4]);
            }
            else if (strcmp(argv[3], "wait") == 0){
                if (argc != 5){
                    return;
                }
                msg.Data.SysEx.SubId1 = SysExNonRtWait;
                msg.Data.SysEx.Data.PacketNumber = atoi(argv[4]);
            }
            else if (strcmp(argv[3], "cancel") == 0){
                if (argc != 5){
                    return;
                }
                msg.Data.SysEx.SubId1 = SysExNonRtCancel;
                msg.Data.SysEx.Data.PacketNumber = atoi(argv[4]);
            }
            else if (strcmp(argv[3], "nak") == 0){
                if (argc != 5){
                    return;
                }
                msg.Data.SysEx.SubId1 = SysExNonRtNAK;
                msg.Data.SysEx.Data.PacketNumber = atoi(argv[4]);
            }
            else if (strcmp(argv[3], "ack") == 0){
                if (argc != 5){
                    return;
                }
                msg.Data.SysEx.SubId1 = SysExNonRtACK;
                msg.Data.SysEx.Data.PacketNumber = atoi(argv[4]);
            }


            else if (strcmp(argv[3], "info") == 0){
                if (argc < 5){
                    return ;
                }
                msg.Data.SysEx.SubId1 = SysExNonRtGeneralInformation;

                if (strcmp(argv[4], "request") == 0){
                    msg.Data.SysEx.SubId2 = SysExNonRtGenInfoIdentityRequest;

                } else if (strcmp(argv[3], "reply") == 0){
                    if (argc != 8){
                        return;
                    }
                    msg.Data.SysEx.SubId2 = SysExNonRtGenInfoIdentityReply;
                    msg.Data.SysEx.Data.GeneralInfo.ManufacturerId = std::strtol(argv[5], NULL, 16);
                    msg.Data.SysEx.Data.GeneralInfo.DeviceFamily = atoi(argv[6]);
                    msg.Data.SysEx.Data.GeneralInfo.DeviceFamilyMember = atoi(argv[7]);
                    uint32_t rev = std::strtol(argv[8], NULL, 16);
                    msg.Data.SysEx.Data.GeneralInfo.SoftwareRevision[0] = (rev >> 24) & 0xFF;
                    msg.Data.SysEx.Data.GeneralInfo.SoftwareRevision[1] = (rev >> 16) & 0xFF;
                    msg.Data.SysEx.Data.GeneralInfo.SoftwareRevision[2] = (rev >> 8) & 0xFF;
                    msg.Data.SysEx.Data.GeneralInfo.SoftwareRevision[3] = rev & 0xFF;

                } else {
                    return;
                }

            }
            else if (strcmp(argv[3], "gm") == 0) {
                if (argc != 5){
                    return;
                }
                msg.Data.SysEx.SubId1 = SysExNonRtGeneralMidi;
                if (strcmp(argv[4], "system-on1") == 0){
                    msg.Data.SysEx.SubId2 = SysExNonRtGmSystemOn1;
                }
                else if (strcmp(argv[4], "system-off") == 0){
                    msg.Data.SysEx.SubId2 = SysExNonRtGmSystemOff;
                }
                else if (strcmp(argv[4], "system-on2") == 0){
                    msg.Data.SysEx.SubId2 = SysExNonRtGmSystemOn2;
                }
                else {
                    return;
                }
            }
            else if (strcmp(argv[3], "cueing") == 0){
                if (argc < 5){
                    return;
                }
                msg.Data.SysEx.SubId1 = SysExNonRtMidiTimeCode;

                if (strcmp(argv[4], "special") == 0){
                    if (argc != 6){
                        return;
                    }
                    msg.Data.SysEx.SubId2 = SysExNonRtMtcSpecial;

                    if (strcmp(argv[5], "time-code-offset") == 0){
                        msg.Data.SysEx.Data.Cueing.EventNumber = SysExNonRtMtcSpecialTimeCodeOffset;
                    }
                    else if (strcmp(argv[5], "enable-event-list") == 0){
                        msg.Data.SysEx.Data.Cueing.EventNumber = SysExNonRtMtcSpecialEnableEventList;
                    }
                    else if (strcmp(argv[5], "disable-event-list") == 0){
                        msg.Data.SysEx.Data.Cueing.EventNumber = SysExNonRtMtcSpecialDisableEventList;
                    }
                    else if (strcmp(argv[5], "clear-event-list") == 0){
                        msg.Data.SysEx.Data.Cueing.EventNumber = SysExNonRtMtcSpecialClearEventList;
                    }
                    else if (strcmp(argv[5], "system-stop") == 0){
                        msg.Data.SysEx.Data.Cueing.EventNumber = SysExNonRtMtcSpecialSystemStop;
                    }
                    else if (strcmp(argv[5], "event-list-request") == 0){
                        msg.Data.SysEx.Data.Cueing.EventNumber = SysExNonRtMtcSpecialEventListRequest;
                    }
                    else {
                        msg.Data.SysEx.Data.Cueing.EventNumber = atoi(argv[5]);
                    }
                }
                else {
                    if (argc < 13){
                        return;
                    }

                    if (strcmp(argv[4], "punch-in") == 0){
                        if (strcmp(argv[5], "add") == 0){
                            msg.Data.SysEx.SubId2 = SysExNonRtMtcPunchInPoint;
                        } else if (strcmp(argv[5], "rm") == 0){
                            msg.Data.SysEx.SubId2 = SysExNonRtMtcDeletePunchInPoint;
                        } else {
                            return;
                        }
                    }
                    else if (strcmp(argv[4], "punch-out") == 0){
                        if (strcmp(argv[5], "add") == 0){
                            msg.Data.SysEx.SubId2 = SysExNonRtMtcPunchOutPoint;
                        } else if (strcmp(argv[5], "rm") == 0){
                            msg.Data.SysEx.SubId2 = SysExNonRtMtcDeletePunchOutPoint;
                        } else {
                            return;
                        }
                    }
                    else if (strcmp(argv[4], "event-start") == 0){
                        if (strcmp(argv[5], "add") == 0){
                            if (argc == 13) {
                                msg.Data.SysEx.SubId2 = SysExNonRtMtcEventStartPoint;
                            } else {
                                msg.Data.SysEx.SubId2 = SysExNonRtMtcEventStartPointWithInfo;
                            }
                        } else if (strcmp(argv[5], "rm") == 0){
                            msg.Data.SysEx.SubId2 = SysExNonRtMtcDeleteEventStartPoint;
                        } else {
                            return;
                        }
                    }
                    else if (strcmp(argv[4], "event-stop") == 0){
                        if (strcmp(argv[5], "add") == 0){
                            if (argc == 13){
                                msg.Data.SysEx.SubId2 = SysExNonRtMtcEventStopPoint;
                            } else {
                                msg.Data.SysEx.SubId2 = SysExNonRtMtcEventStopPointWithInfo;
                            }
                        } else if (strcmp(argv[5], "rm") == 0){
                            msg.Data.SysEx.SubId2 = SysExNonRtMtcDeleteEventStopPoint;
                        } else {
                            return;
                        }
                    }
                    else if (strcmp(argv[4], "cue-point") == 0){
                        if (strcmp(argv[5], "add") == 0){
                            if (argc == 13) {
                                msg.Data.SysEx.SubId2 = SysExNonRtMtcCuePoint;
                            } else {
                                msg.Data.SysEx.SubId2 = SysExNonRtMtcCuePointWithInfo;
                            }
                        } else if (strcmp(argv[5], "rm") == 0){
                            msg.Data.SysEx.SubId2 = SysExNonRtMtcDeletePunchInPoint;
                        } else {
                            return;
                        }
                    }
                    else if (strcmp(argv[4], "event-name") == 0){
                        if (strcmp(argv[5], "-") == 0){
                            if (argc < 14){
                                return;
                            }
                            msg.Data.SysEx.SubId2 = SysExNonRtMtcEventName;
                        } else {
                            return;
                        }
                    }
                    else {
                        return;
                    }

                    uint8_t fps = atoi(argv[6]);

                    switch(fps){
                        case 24: fps = MtcFrameRate24fps; break;
                        case 25: fps = MtcFrameRate25fps; break;
                        case 29: fps = MtcFrameRate29_97fps; break;
                        case 30: fps = MtcFrameRate30fps; break;
                        default: return;
                    }

                    msg.Data.SysEx.Data.Cueing.MidiTimeCode.Fps = fps;
                    msg.Data.SysEx.Data.Cueing.MidiTimeCode.Hour = atoi(argv[7]);
                    msg.Data.SysEx.Data.Cueing.MidiTimeCode.Minute = atoi(argv[8]);
                    msg.Data.SysEx.Data.Cueing.MidiTimeCode.Second = atoi(argv[9]);
                    msg.Data.SysEx.Data.Cueing.MidiTimeCode.Frame = atoi(argv[10]);
                    msg.Data.SysEx.Data.Cueing.MidiTimeCode.FractionalFrame = atoi(argv[11]);
                    msg.Data.SysEx.Data.Cueing.EventNumber = atoi(argv[12]);

                    if (isSysExNonRtMtcWithAddInfo(msg.Data.SysEx.SubId2)){
                        uint8_t offset = 0;
                        for(uint8_t i = 0; i < argc - 13; i++){
                            if (i > 0){
                                msg.Data.SysEx.ByteData[offset++] = ' ';
                            }
                            strcpy((char*)&msg.Data.SysEx.ByteData[offset], argv[13+i]);
                            offset += strlen(argv[13+i]);

                        }
                        msg.Data.SysEx.Length += strlen((char*)msg.Data.SysEx.ByteData);
                    } else {
                        if (argc != 13){
                            return;
                        }
                    }

                }
            }
            else {
                return;
            }
        } else {
            return;
        }

    } else {
        return;
    }

    length = pack( bytes, &msg );
//    printf("packed %d..\n", length);

    if (length > 0){

        printf(prefix, length);

        if (runningStatusEnabled && updateRunningStatus( &runningStatus, bytes[0] )){
            fwrite( &bytes[1], 1, length - 1, stdout );
        } else {
            fwrite(bytes, 1, length, stdout);
        }

        printf("%s", suffix);

        fflush(stdout);
    }
}

uint8_t parse(uint8_t length, uint8_t bytes[]){

    uint8_t buf[256];

//    for(int i = 0; i < length; i++){
//        printf("%02x ", bytes[i]);
//    }
//    printf("\n");

    if ( ! unpack(bytes, length, &msg) ){
        return 0;
    }

//    printf("parsed! %02X\n", msg.StatusClass);



    if (timedOpt.enabled){

        unsigned long now = getNow();
        unsigned long diff = now - timedOpt.lastTimestamp;

        printf("%ld ", diff);

        timedOpt.lastTimestamp = now;
    }


    if (msg.StatusClass == StatusClassNoteOn){
        printf("note on %d %d %d\n", msg.Channel, msg.Data.Note.Key, msg.Data.Note.Velocity);
    }
    if (msg.StatusClass == StatusClassNoteOff){
        printf("note off %d %d %d\n", msg.Channel, msg.Data.Note.Key, msg.Data.Note.Velocity);
    }
    if (msg.StatusClass == StatusClassControlChange){
        printf("cc %d %d %d\n", msg.Channel, msg.Data.ControlChange.Controller, msg.Data.ControlChange.Value);
    }
    if (msg.StatusClass == StatusClassProgramChange){
        printf("pc %d %d\n", msg.Channel, msg.Data.ProgramChange.Program);
    }
    if (msg.StatusClass == StatusClassChannelPressure){
        printf("pressure %d %d\n", msg.Channel, msg.Data.ChannelPressure.Pressure);
    }
    if (msg.StatusClass == StatusClassPitchBendChange){
        printf("pitch %d %d\n", msg.Channel, msg.Data.PitchBendChange.Pitch);
    }
    if (msg.StatusClass == StatusClassPolyphonicKeyPressure){
        printf("poly %d %d %d\n", msg.Channel, msg.Data.PolyphonicKeyPressure.Key, msg.Data.PolyphonicKeyPressure.Pressure);
    }

    if (msg.StatusClass == StatusClassSystemMessage){

//        printf("sysmsg \n");

        if (msg.SystemMessage == SystemMessageMtcQuarterFrame){
            printf("quarter-frame %d %d\n", msg.Data.MtcQuarterFrame.MessageType, msg.Data.MtcQuarterFrame.Nibble);
        }
        if (msg.SystemMessage == SystemMessageSongPositionPointer){
            printf("song-position %d\n", msg.Data.SongPositionPointer.Position);
        }
        if (msg.SystemMessage == SystemMessageSongSelect){
            printf("song-select %d\n", msg.Data.SongSelect.Song);
        }
        if (msg.SystemMessage == SystemMessageStart){
            printf("start\n");
        }
        if (msg.SystemMessage == SystemMessageStop){
            printf("stop\n");
        }
        if (msg.SystemMessage == SystemMessageContinue){
            printf("continue\n");
        }
        if (msg.SystemMessage == SystemMessageActiveSensing){
            printf("active-sensing\n");
        }
        if (msg.SystemMessage == SystemMessageReset){
            printf("reset\n");
        }
        if (msg.SystemMessage == SystemMessageTimingClock){
            printf("timing-clock\n");
        }
        if (msg.SystemMessage == SystemMessageTuneRequest){
            printf("tune-request\n");
        }
        if (msg.SystemMessage == SystemMessageSystemExclusive){
            printf("sysex ");

            if (msg.Data.SysEx.Id == SysExIdExperimental){
                printf("experimental ");
                for(uint8_t i = 0; i < msg.Data.SysEx.Length; i++){
                    printf("%02X", msg.Data.SysEx.ByteData[i]);
                }
                printf("\n");
            }

            if (isSysExManufacturerId(msg.Data.SysEx.Id)){
                printf("manufacturer %06X ", msg.Data.SysEx.Id);
                for(uint8_t i = 0; i < msg.Data.SysEx.Length; i++){
                    printf("%02X", msg.Data.SysEx.ByteData[i]);
                }
                printf("\n");
            }

            if (msg.Data.SysEx.Id == SysExIdRealTime){
                printf("rt %d ", msg.Channel);

                if (msg.Data.SysEx.SubId1 == SysExRtMidiTimeCode){
                    printf("mtc ");

                    if (msg.Data.SysEx.SubId2 == SysExRtMtcFullMessage){
                        uint8_t fps = 0;
                        switch(msg.Data.SysEx.Data.MidiTimeCode.Fps){
                            case MtcFrameRate24fps: fps = 24; break;
                            case MtcFrameRate25fps: fps = 25; break;
                            case MtcFrameRate29_97fps: fps = 29; break;
                            case MtcFrameRate30fps: fps = 30; break;
                        }
                        printf("full-message %d%s %d %d %d %d\n",
                               fps,
                               fps == 29 ? ".97" : "",
                               msg.Data.SysEx.Data.MidiTimeCode.Hour,
                               msg.Data.SysEx.Data.MidiTimeCode.Minute,
                               msg.Data.SysEx.Data.MidiTimeCode.Second,
                               msg.Data.SysEx.Data.MidiTimeCode.Frame
                        );
                    } else if (msg.Data.SysEx.SubId2 == SysExRtMtcUserBits){
                        printf("user-bits %02X%02X%02X%02X%02X\n",
                               msg.Data.SysEx.ByteData[0],
                               msg.Data.SysEx.ByteData[1],
                               msg.Data.SysEx.ByteData[2],
                               msg.Data.SysEx.ByteData[3],
                               msg.Data.SysEx.ByteData[4]
                        );
                    }
                }
                else if (msg.Data.SysEx.SubId1 == SysExRtMidiTimeCodeCueing){
                    printf("cueing ");
                    if (msg.Data.SysEx.SubId2 == SysExRtMtcCueingSpecial){
                        printf("special ");
                        if (msg.Data.SysEx.Data.Cueing.EventNumber == SysExRtMtcCueingSpecialSystemStop){
                            printf("system-stop\n");
                        }
                        else {
                            printf("%d\n", msg.Data.SysEx.Data.Cueing.EventNumber);
                        }
                    }
                    else {

                        switch (msg.Data.SysEx.SubId2){
                            case SysExRtMtcCueingPunchInPoint:                 printf("punch-in "); break;
                            case SysExRtMtcCueingPunchOutPoint:                printf("punch-out "); break;
                            case SysExRtMtcCueingEventStartPoint:
                            case SysExRtMtcCueingEventStartPointWithInfo:     printf("event-start "); break;
                            case SysExRtMtcCueingEventStopPoint:
                            case SysExRtMtcCueingEventStopPointWithInfo:      printf("event-stop "); break;
                            case SysExRtMtcCueingCuePoint:
                            case SysExRtMtcCueingCuePointWithInfo:            printf("cue-point "); break;
                            case SysExRtMtcCueingEventName:                    printf("event-name "); break;
                            default:
                                printf("?? ");
                        }

                        printf("%d",msg.Data.SysEx.Data.Cueing.EventNumber);

                        if (msg.Data.SysEx.Length > 0){
                            printf(" %s", msg.Data.SysEx.ByteData);
                        }

                        printf("\n");

//                        printf("%d\n", msg.Data.SysEx.SubId2);
                    }
                }
            }

            if (msg.Data.SysEx.Id == SysExIdNonRealTime){
                printf("nonrt %d ", msg.Channel);


                if (msg.Data.SysEx.SubId1 == SysExNonRtEndOfFile){
                    printf("eof %d\n", msg.Data.SysEx.Data.PacketNumber);
                } else if (msg.Data.SysEx.SubId1 == SysExNonRtWait){
                    printf("wait %d\n", msg.Data.SysEx.Data.PacketNumber);
                } else if (msg.Data.SysEx.SubId1 == SysExNonRtCancel){
                    printf("cancel %d\n", msg.Data.SysEx.Data.PacketNumber);
                } else if (msg.Data.SysEx.SubId1 == SysExNonRtNAK){
                    printf("nak %d\n", msg.Data.SysEx.Data.PacketNumber);
                } else if (msg.Data.SysEx.SubId1 == SysExNonRtACK){
                    printf("ack %d\n", msg.Data.SysEx.Data.PacketNumber);
                }
                else if (msg.Data.SysEx.SubId1 == SysExNonRtGeneralMidi){
                    printf("gm ");
                    if (msg.Data.SysEx.SubId2 == SysExNonRtGmSystemOn1){
                        printf("system-on1\n");
                    }
                    if (msg.Data.SysEx.SubId2 == SysExNonRtGmSystemOff){
                        printf("system-off\n");
                    }
                    if (msg.Data.SysEx.SubId2 == SysExNonRtGmSystemOn2){
                        printf("system-on2\n");
                    }
                }
                else if (msg.Data.SysEx.SubId1 == SysExNonRtMidiTimeCode){
                    printf("cueing ");
                    if (msg.Data.SysEx.SubId2 == SysExNonRtMtcSpecial){
                        printf("special ");
                        if (msg.Data.SysEx.Data.Cueing.EventNumber == SysExNonRtMtcSpecialTimeCodeOffset){
                            printf("time-code-offset\n");
                        }
                        else if (msg.Data.SysEx.Data.Cueing.EventNumber == SysExNonRtMtcSpecialEnableEventList){
                            printf("enable-event-list\n");
                        }
                        else if (msg.Data.SysEx.Data.Cueing.EventNumber == SysExNonRtMtcSpecialDisableEventList){
                            printf("disable-event-list\n");
                        }
                        else if (msg.Data.SysEx.Data.Cueing.EventNumber == SysExNonRtMtcSpecialClearEventList){
                            printf("clear-event-list\n");
                        }
                        else if (msg.Data.SysEx.Data.Cueing.EventNumber == SysExNonRtMtcSpecialSystemStop){
                            printf("system-stop\n");
                        }
                        else if (msg.Data.SysEx.Data.Cueing.EventNumber == SysExNonRtMtcSpecialEventListRequest){
                            printf("event-list-request\n");
                        }
                        else {
                            printf("%d\n", msg.Data.SysEx.Data.Cueing.EventNumber);
                        }
                    }
                    else {

                        switch (msg.Data.SysEx.SubId2){
                            case SysExNonRtMtcPunchInPoint:                 printf("punch-in add "); break;
                            case SysExNonRtMtcDeletePunchInPoint:           printf("punch-in rm "); break;
                            case SysExNonRtMtcPunchOutPoint:                printf("punch-out add "); break;
                            case SysExNonRtMtcDeletePunchOutPoint:          printf("punch-out rm "); break;
                            case SysExNonRtMtcEventStartPoint:
                            case SysExNonRtMtcEventStartPointWithInfo:     printf("event-start add "); break;
                            case SysExNonRtMtcDeleteEventStartPoint:        printf("event-start rm "); break;
                            case SysExNonRtMtcEventStopPoint:
                            case SysExNonRtMtcEventStopPointWithInfo:      printf("event-stop add "); break;
                            case SysExNonRtMtcDeleteEventStopPoint:         printf("event-stop rm "); break;
                            case SysExNonRtMtcCuePoint:
                            case SysExNonRtMtcCuePointWithInfo:            printf("cue-point add "); break;
                            case SysExNonRtMtcDeleteCuePoint:               printf("cue-point rm "); break;
                            case SysExNonRtMtcEventName:                    printf("event-name "); break;
                            default:
                                printf("?? ");
                        }

                        uint8_t fps = 0;
                        switch(msg.Data.SysEx.Data.Cueing.MidiTimeCode.Fps){
                            case MtcFrameRate24fps: fps = 24; break;
                            case MtcFrameRate25fps: fps = 25; break;
                            case MtcFrameRate29_97fps: fps = 29; break;
                            case MtcFrameRate30fps: fps = 30; break;
                        }
                        printf("%d%s %d %d %d %d %d %d",
                               fps,
                               fps == 29 ? ".97" : "",
                               msg.Data.SysEx.Data.MidiTimeCode.Hour,
                               msg.Data.SysEx.Data.Cueing.MidiTimeCode.Minute,
                               msg.Data.SysEx.Data.Cueing.MidiTimeCode.Second,
                               msg.Data.SysEx.Data.Cueing.MidiTimeCode.Frame,
                               msg.Data.SysEx.Data.Cueing.MidiTimeCode.FractionalFrame,
                               msg.Data.SysEx.Data.Cueing.EventNumber
                        );

                        if (msg.Data.SysEx.Length > 0){
                            printf(" %s", msg.Data.SysEx.ByteData);
                        }

                        printf("\n");

//                        printf("%d\n", msg.Data.SysEx.SubId2);
                    }
                }
                else if (msg.Data.SysEx.SubId1 == SysExNonRtGeneralInformation){
                    printf("info ");

                    if (msg.Data.SysEx.SubId2 == SysExNonRtGenInfoIdentityRequest){
                        printf("request\n"
                        );
                    } else if (msg.Data.SysEx.SubId2 == SysExNonRtGenInfoIdentityReply) {
                        printf("reply %06X %d %d %02X%02X%02X%02X\n",
                               msg.Data.SysEx.Data.GeneralInfo.ManufacturerId,
                               msg.Data.SysEx.Data.GeneralInfo.DeviceFamily,
                               msg.Data.SysEx.Data.GeneralInfo.DeviceFamilyMember,
                               msg.Data.SysEx.Data.GeneralInfo.SoftwareRevision[0],
                               msg.Data.SysEx.Data.GeneralInfo.SoftwareRevision[1],
                               msg.Data.SysEx.Data.GeneralInfo.SoftwareRevision[2],
                               msg.Data.SysEx.Data.GeneralInfo.SoftwareRevision[3]
                        );
                    }
                }

            }
        }
    }

    return length;
}