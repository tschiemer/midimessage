//
// Created by Philip Tschiemer on 22.08.19.
//

#include <stdio.h>
#include <cstdlib>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#include "midimessage.h"

#define MAX_LEN 255

using namespace std;
using namespace MidiMessage;

typedef enum {
    ModeUndefined   = 0,
    ModeParse       = 1,
    ModeGenerate    = 2
} Mode_t;

void printHelp( void ) {
    printf("Usage: midimessage-cli [-h?] [--parse|-p [<data1> ..]] [--generate [<cmd> ...]]\n");
    printf("If no <data> or <cmd>.. is given reads from STDIN assuming either a continuous data stream (when parsing) or one generation command per line\n");
    printf("Output to STDOUT (when generating this will be binary).\n");
    printf("Note: parsed message output format is identical to the required generation format ;)\n");

    printf("\nVoice Commands:\n");
    printf("\t note (on|off) <key> <velocity> <channel>\n");
    printf("\t cc <controller> <value> <channel>\n");
    printf("\t pc <program> <channel>\n");
    printf("\t pressure <pressure> <channel>\n");
    printf("\t pitch <pitch> <channel>\n");
    printf("\t poly <key> <pressure> <channel>\n");

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

    printf("\nExamples:\n");
    printf("\t ./midimessage-cli -g note on 60 40 1\n");
    printf("\t ./midimessage-cli -g | ./midimessage-cli -p\n");
}

void generate(uint8_t argc,  char  * argv[]);
uint8_t parse(uint8_t length, uint8_t bytes[]);

int main(int argc, char * argv[], char * env[]){

    int c;
    int digit_optind = 0;


    Mode_t mode = ModeUndefined;

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
                {"help",    no_argument,    0,  'h' },
                {0,         0,              0,  0 }
        };

        c = getopt_long(argc, argv, "pgh?",
                        long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
            case '?':
            case 'h':
                printHelp();
                return EXIT_SUCCESS;

            case 'p':
                if (mode == ModeGenerate){
                    printf("Already set to parsing - can not do both!\n");
                    return 1;
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

            default:
                printf("?? getopt returned character code 0%o ??\n", c);
        }
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

        return EXIT_SUCCESS;
    }

    uint8_t pos = 0;
    uint8_t line[MAX_LEN];

    uint8_t argsCount = 1;
    uint8_t * args[16];

    args[0] = &line[0];

    int ch;

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

                generate( argsCount, (char**)args );

                pos = 0;
                line[0] = '\0';

                argsCount = 0;
            }

        }
        if (mode == ModeParse) {

            // wait for control byte
            if (pos == 0 && !isControlByte(in) && in != SystemMessageEndOfExclusive) {
                continue;
            }

            line[pos++] = in;

            if ( parse(pos, (uint8_t *) line) ) {
                pos = 0;
            }
        }
    }

    return EXIT_SUCCESS;
}


void generate(uint8_t argc, char * argv[]){

    Message_t msg;
    uint8_t bytes[128];
    uint8_t length = 0;

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
        msg.Data.Note.Key = atoi(argv[2]);
        msg.Data.Note.Velocity = atoi(argv[3]);
        if (strcmp(argv[1], "on") == 0){
            msg.StatusClass = StatusClassNoteOn;
        }
        else if (strcmp(argv[1], "off") == 0){
            msg.StatusClass = StatusClassNoteOff;
        }
        else {
            return;
        }
        msg.Channel = atoi(argv[4]);
    }

    else if (strcmp(argv[0], "cc") == 0){
        if (argc != 4){
            return;
        }
        msg.StatusClass = StatusClassControlChange;
        msg.Data.ControlChange.Controller = atoi(argv[1]);
        msg.Data.ControlChange.Value = atoi(argv[2]);
        msg.Channel = atoi(argv[3]);
    }

    else if (strcmp(argv[0], "pc") == 0){
        if (argc != 3){
            return;
        }
        msg.StatusClass = StatusClassProgramChange;
        msg.Data.ProgramChange.Program = atoi(argv[1]);
        msg.Channel = atoi(argv[2]);
    }

    else if (strcmp(argv[0], "pressure") == 0){
        if (argc != 3){
            return;
        }
        msg.StatusClass = StatusClassChannelPressure;
        msg.Data.ChannelPressure.Pressure = atoi(argv[1]);
        msg.Channel = atoi(argv[2]);
    }

    else if (strcmp(argv[0], "pitch") == 0){
        if (argc != 3){
            return;
        }
        msg.StatusClass = StatusClassPitchBendChange;
        msg.Data.PitchBendChange.Pitch = atoi(argv[1]);
        msg.Channel = atoi(argv[2]);
    }

    else if (strcmp(argv[0], "poly") == 0){
        if (argc != 4){
            return;
        }
        msg.StatusClass = StatusClassPolyphonicKeyPressure;
        msg.Data.PolyphonicKeyPressure.Key = atoi(argv[1]);
        msg.Data.PolyphonicKeyPressure.Pressure = atoi(argv[2]);
        msg.Channel = atoi(argv[3]);
    }

    else if (strcmp(argv[0], "quarter-frame") == 0){
        if (argc != 3){
            return;
        }
        msg.StatusClass = StatusClassSystemMessage;
        msg.Status = SystemMessageMtcQuarterFrame;
        msg.Data.MtcQuarterFrame.MessageType = atoi(argv[1]);
        msg.Data.MtcQuarterFrame.Nibble = atoi(argv[2]);
    }
    else if (strcmp(argv[0], "song-position") == 0){
        if (argc != 2){
            return;
        }
        msg.StatusClass = StatusClassSystemMessage;
        msg.Status = SystemMessageSongPositionPointer;
        msg.Data.SongPositionPointer.Position = atoi(argv[1]);
    }
    else if (strcmp(argv[0], "song-select") == 0){
        if (argc != 2){
            return;
        }
        msg.StatusClass = StatusClassSystemMessage;
        msg.Status = SystemMessageSongSelect;
        msg.Data.SongSelect.Song = atoi(argv[1]);
    }
    else if (strcmp(argv[0], "start") == 0){
        msg.StatusClass = StatusClassSystemMessage;
        msg.Status = SystemMessageStart;
    }
    else if (strcmp(argv[0], "stop") == 0){
        msg.StatusClass = StatusClassSystemMessage;
        msg.Status = SystemMessageStop;
    }
    else if (strcmp(argv[0], "continue") == 0){
        msg.StatusClass = StatusClassSystemMessage;
        msg.Status = SystemMessageContinue;
    }
    else if (strcmp(argv[0], "active-sensing") == 0){
        msg.StatusClass = StatusClassSystemMessage;
        msg.Status = SystemMessageActiveSensing;
    }
    else if (strcmp(argv[0], "reset") == 0){
        msg.StatusClass = StatusClassSystemMessage;
        msg.Status = SystemMessageReset;
    }
    else if (strcmp(argv[0], "timing-clock") == 0){
        msg.StatusClass = StatusClassSystemMessage;
        msg.Status = SystemMessageTimingClock;
    }
    else if (strcmp(argv[0], "tune-request") == 0){
        msg.StatusClass = StatusClassSystemMessage;
        msg.Status = SystemMessageTuneRequest;
    }
    else if (strcmp(argv[0], "sysex") == 0){
        msg.StatusClass = StatusClassSystemMessage;
        msg.Status = SystemMessageSystemExclusive;
        return;
    } else {
        return;
    }

//    printf("packing..\n");
//    fflush(stdout);

    length = pack( bytes, &msg );

    if (length > 0){
        fwrite(bytes, 1, length, stdout);
        fflush(stdout);
    }
}

uint8_t parse(uint8_t length, uint8_t bytes[]){

    Message_t msg;

//    for(int i = 0; i < length; i++){
//        printf("%02x ", bytes[i]);
//    }
//    printf("\n");

    if ( ! unpack(bytes, length, &msg) ){
        return 0;
    }

//    printf("parsed! %d\n", msg.StatusClass);

    if (msg.StatusClass == StatusClassNoteOn){
        printf("note on %d %d %d\n", msg.Data.Note.Key, msg.Data.Note.Velocity, msg.Channel);
    }
    if (msg.StatusClass == StatusClassNoteOff){
        printf("note off %d %d %d\n", msg.Data.Note.Key, msg.Data.Note.Velocity, msg.Channel);
    }
    if (msg.StatusClass == StatusClassControlChange){
        printf("cc %d %d %d\n", msg.Data.ControlChange.Controller, msg.Data.ControlChange.Value, msg.Channel);
    }
    if (msg.StatusClass == StatusClassProgramChange){
        printf("pc %d %d\n", msg.Data.ProgramChange.Program, msg.Channel);
    }
    if (msg.StatusClass == StatusClassChannelPressure){
        printf("pressure %d %d\n", msg.Data.ChannelPressure.Pressure, msg.Channel);
    }
    if (msg.StatusClass == StatusClassPitchBendChange){
        printf("pitch %d %d\n", msg.Data.PitchBendChange.Pitch, msg.Channel);
    }
    if (msg.StatusClass == StatusClassPolyphonicKeyPressure){
        printf("poly %d %d %d\n", msg.Data.PolyphonicKeyPressure.Key, msg.Data.PolyphonicKeyPressure.Pressure, msg.Channel);
    }

    if (msg.StatusClass == StatusClassSystemMessage){

        if (msg.Status == SystemMessageMtcQuarterFrame){
            printf("quarter-frame %d %d\n", msg.Data.MtcQuarterFrame.MessageType, msg.Data.MtcQuarterFrame.Nibble);
        }
        if (msg.Status == SystemMessageSongPositionPointer){
            printf("song-position %d\n", msg.Data.SongPositionPointer.Position);
        }
        if (msg.Status == SystemMessageSongSelect){
            printf("song-select %d\n", msg.Data.SongSelect.Song);
        }
        if (msg.Status == SystemMessageStart){
            printf("start\n");
        }
        if (msg.Status == SystemMessageStop){
            printf("stop\n");
        }
        if (msg.Status == SystemMessageContinue){
            printf("continue\n");
        }
        if (msg.Status == SystemMessageActiveSensing){
            printf("active-sensing\n");
        }
        if (msg.Status == SystemMessageReset){
            printf("reset\n");
        }
        if (msg.Status == SystemMessageTimingClock){
            printf("timing-clock\n");
        }
        if (msg.Status == SystemMessageTuneRequest){
            printf("tune-request\n");
        }
        if (msg.Status == SystemMessageSystemExclusive){
            printf("SysEx\n");
        }
    }

    return length;
}