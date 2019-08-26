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

#include "midimessage.h"

#define MAX_LEN 255

using namespace std;
using namespace MidiMessage;

typedef enum {
    ModeUndefined   = 0,
    ModeParse       = 1,
    ModeGenerate    = 2
} Mode_t;

uint8_t buf[128];

Message_t msg = {
        .Data.SysEx.ByteData = buf
};

typedef enum {
    ResolutionMicro = 0,
    ResolutionMilli = 1
} Resolution_t;

Resolution_t resolution = ResolutionMicro;
bool timed = false;
unsigned long lastTimestamp, now;


void printHelp( void ) {
    printf("Usage: midimessage-cli [-h?] [--parse|-p [--timed|-t[milli|micro]] [<binary-data>]] [--generate|-g [--timed|-t[milli|micro]] [<cmd> ...]]\n");
    printf("If no <data> or <cmd>.. is given reads from STDIN assuming either a continuous data stream (when parsing) or one generation command per line\n");
    printf("Output to STDOUT (when generating this will be binary).\n");
    printf("Note: parsed message output format is identical to the required generation format ;)\n");

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

    printf("\nSystem Exclusives:\n");
    printf("\t experimental <n> <data-of-length-n>\n");
    printf("\t manufacturer <hex-manufacturer-id> <n> <data-of-length-n>\n");
    printf("\t nonrt info (<request> <device-id>|<reply> <device-id> <hex-manufacturer-id> <hex-device-family> <hex-device-family-member> <hex-software-revision> \n");

    printf("\nNote: Data bytes have a value range of 0-127 - anything above is considered a control byte.\n");

    printf("\nRecordings and Replay\n");
    printf("\t Using the --timed|-t option the utility will enter a record mode (when parsing) or replay mode (when generating) message. The generation commands will then have a delay since the last message in the given time scale (micro or milli seconds, default = micro).\n");

    printf("\nExamples:\n");
    printf("\t ./midimessage-cli -g note on 60 40 1\n");
    printf("\t ./midimessage-cli -g | ./midimessage-cli -p\n");
    printf("\t ./midimessage-cli -g | ./midimessage-cli -ptmilli > test.recording\n");
    printf("\t cat test.recording | ./midimessage-cli -gtmilli | ./midimessage-cli -p\n");
}

void generate(uint8_t argc,  char  * argv[]);
uint8_t parse(uint8_t length, uint8_t bytes[]);

unsigned long getNow(){

    struct timespec c;

    if (clock_gettime(CLOCK_REALTIME, &c) == -1) {
        perror("error calling clock_gettime()");
        exit(1);
    }

    if (resolution == ResolutionMilli){
        return c.tv_sec * 1000 + c.tv_nsec / 1000000;
    }

    return c.tv_sec * 1000000 + c.tv_nsec / 1000;
}




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
                {"timed", optional_argument,  0, 't'},
                {"help",    no_argument,    0,  'h' },
                {0,         0,              0,  0 }
        };

        c = getopt_long(argc, argv, "pgt::h?",
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

            case 't':
                timed = true;
                if (strlen(optarg) > 0){
                    if (strcmp(optarg, "milli") == 0){
                        resolution = ResolutionMilli;
                    } else if (strcmp(optarg, "micro") == 0) {
                        resolution = ResolutionMicro;
                    } else {
                        printf("Error: invalid time scale, either 'milli' or 'micro', %s given\n", optarg);
                        exit(EXIT_FAILURE);
                    }
                }
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

    // start timer
    if (timed){
        // record timer
        lastTimestamp = getNow();
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

                if (timed){
                    unsigned long delay = std::strtoul((char*)args[0], NULL, 10);
                    unsigned long waitUntil = lastTimestamp + delay;
                    unsigned long now;
                    do {
                        now = getNow();
                    } while( now < waitUntil );

                    lastTimestamp = now;

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
        msg.StatusClass = StatusClassSystemMessage;
        msg.SystemMessage = SystemMessageSystemExclusive;

        if (argc < 2){
            return;
        }
        if (strcmp(argv[1], "experimental") == 0) {
            msg.Data.SysEx.Id = SysExIdExperimental;
            if (argc < 4) {
                msg.Data.SysEx.Length = 0;
            } else {
                int len = atoi(argv[2]);
                memcpy((char*)msg.Data.SysEx.ByteData, argv[3], len);
                msg.Data.SysEx.Length = len;
            }
        } else if (strcmp(argv[1], "manufacturer") == 0) {
            if (argc < 3){
                return;
            }

            msg.Data.SysEx.Id = std::strtol(argv[2], NULL, 16);

            if (argc < 5) {
                msg.Data.SysEx.Length = 0;
            } else {
                int len = atoi(argv[3]);
                memcpy((char*)msg.Data.SysEx.ByteData, argv[4], len);
                msg.Data.SysEx.Length = len;
            }
        } else if (strcmp(argv[1], "rt") == 0){
            if (argc < 3){
                return;
            }
            msg.Data.SysEx.Id = SysExIdRealTime;


        } else if (strcmp(argv[1], "nonrt") == 0) {
            if (argc < 3){
                return;
            }

            msg.Data.SysEx.Id = SysExIdNonRealTime;

            if (strcmp(argv[2], "info") == 0){
                if (argc < 4){
                    return ;
                }
                msg.Data.SysEx.SubId1 = SysExNonRtGeneralInformation;

                if (strcmp(argv[3], "request") == 0){
                    if (argc < 5){
                        return ;
                    }
                    msg.Data.SysEx.SubId2 = SysExNonRtGenInfoIdentityRequest;
                    msg.Channel = atoi(argv[4]);

                } else if (strcmp(argv[3], "reply") == 0){
                    if (argc < 8){
                        return;
                    }
                    msg.Data.SysEx.SubId2 = SysExNonRtGenInfoIdentityReply;
                    msg.Channel = atoi(argv[4]);
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

            } else {
                return;
            }
        } else {
            return;
        }

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

//    for(int i = 0; i < length; i++){
//        printf("%02x ", bytes[i]);
//    }
//    printf("\n");

    if ( ! unpack(bytes, length, &msg) ){
        return 0;
    }

//    printf("parsed! %d\n", msg.StatusClass);



    if (timed){

        unsigned long now = getNow();
        unsigned long diff = now - lastTimestamp;

        printf("%ld ", diff);

        lastTimestamp = now;
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
                printf("experimental %d ", msg.Data.SysEx.Length);
                for(uint8_t i = 0; i < msg.Data.SysEx.Length; i++){
                    printf("%c", msg.Data.SysEx.ByteData[i]);
                }
                printf("\n");
            }

            if (isSysExManufacturerId(msg.Data.SysEx.Id)){
                printf("manufacturer %06X %d ", msg.Data.SysEx.Id, msg.Data.SysEx.Length);
                for(uint8_t i = 0; i < msg.Data.SysEx.Length; i++){
                    printf("%c", msg.Data.SysEx.ByteData[i]);
                }
                printf("\n");
            }

            if (msg.Data.SysEx.Id == SysExIdRealTime){
                printf("rt ");
            }

            if (msg.Data.SysEx.Id == SysExIdNonRealTime){
                printf("nonrt ");

                if (msg.Data.SysEx.SubId1 == SysExNonRtGeneralInformation){
                    printf("info ");

                    if (msg.Data.SysEx.SubId2 == SysExNonRtGenInfoIdentityRequest){
                        printf("request %d\n",
                               msg.Channel
                        );
                    } else if (msg.Data.SysEx.SubId2 == SysExNonRtGenInfoIdentityReply) {
                        printf("reply %d %06X %d %d %02X%02X%02X%02X\n",
                               msg.Channel,
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