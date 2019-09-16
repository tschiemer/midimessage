
#include <midimessage/stringifier.h>
#include <midimessage/packers.h>
#include <util-hex.h>


#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define str_eq(x,y)     (strcmp((char*)x,(char*)y) == 0)

#define assertBool(expr)    if (!(expr)) { return StringifierResultInvalidValue; }
#define assertU4(x)     if (((x) & NibbleMask) != (x)) { return StringifierResultInvalidU4; }
#define assertU7(x)         if (x > MaxU7) { return StringifierResultInvalidU7; }
#define assertU14(x)        if (x > MaxU14) { return StringifierResultInvalidU14; }
#define assertU21(x)        if (x > MaxU21) { return StringifierResultInvalidU21; }
#define assertU28(x)        if (x > MaxU28) { return StringifierResultInvalidU28; }
#define assertU35(x)        if (x > MaxU35) { return StringifierResultInvalidU35; }
#define assertI7(x)         if (x < MinI7 || MaxI7 < x) { return StringifierResultInvalidValue; }
#define assertData(bytes, len)   \
                            for(uint8_t i = 0; i < len; i++){ \
                                assertU7(bytes[i]); \
                            }
#define assertCueString(bytes) \
                            for(uint8_t i = 0; bytes[i] != '\0'; i++){ \
                                assertBool(isMscCueNumberChar(bytes[i])); \
                            }

#ifdef __cplusplus
namespace MidiMessage {
    extern "C" {
#endif


    bool readHex( uint8_t * bytes, uint8_t * length, uint8_t *argv, uint8_t expectedLength ){
        int l = strlen((char*)argv);

        if (l % 2 != 0){
            return false;
        }

        l >>= 1;

        if (expectedLength > 0 && l != expectedLength){
            return false;
        }

        if (hex_to_byte(bytes, argv, l)){
            if (length != NULL) {
                *length = l;
            }
            return true;
        }

        return false;
    }

    inline int sprintfHex( uint8_t * dst, uint8_t * src, uint8_t length){
        byte_to_hex( dst, src, length );
        dst[2*length] = '\0';
        return 2 * length;
    }

    bool readMtc(MidiTimeCode_t * mtc, uint8_t *argv[]){
        uint8_t fps = atoi((char*)argv[0]);

        switch (fps) {
            case 24:
                fps = MtcFrameRate24fps;
                break;
            case 25:
                fps = MtcFrameRate25fps;
                break;
            case 29:
                fps = MtcFrameRate29_97fps;
                break;
            case 30:
                fps = MtcFrameRate30fps;
                break;
            default:
                return false;
        }

        mtc->Fps = fps;
        mtc->Hour = atoi((char*)argv[1]);
        mtc->Minute = atoi((char*)argv[2]);
        mtc->Second = atoi((char*)argv[3]);
        mtc->Frame = atoi((char*)argv[4]);
        mtc->FractionalFrame = 0;

        if (mtc->Hour > MtcMaxHour) return false;
        if (mtc->Minute > MtcMaxMinute) return false;
        if (mtc->Second > MtcMaxSecond) return false;
        if (mtc->Frame > MtcMaxFps[fps]) return false;

        return true;
    }
    bool readMtcLong(MidiTimeCode_t * mtc, uint8_t *argv[]){
        if (readMtc(mtc, argv)){
            mtc->FractionalFrame = atoi((char*)argv[5]);
            if (mtc->FractionalFrame > MtcMaxFractionalFrame) return false;
            return true;
        }
        return false;
    }
    int sprintfMtc( uint8_t * dst, MidiTimeCode_t * mtc){

        uint8_t fps = 0;
        switch(mtc->Fps){
            case MtcFrameRate24fps: fps = 24; break;
            case MtcFrameRate25fps: fps = 25; break;
            case MtcFrameRate29_97fps: fps = 29; break;
            case MtcFrameRate30fps: fps = 30; break;
        }
        return sprintf( (char*)dst, "%d%s %d %d %d %d",
               fps,
               fps == 29 ? ".97" : "",
               mtc->Hour,
               mtc->Minute,
               mtc->Second,
               mtc->Frame
        );
    }
    int sprintfMtcLong( uint8_t * dst, MidiTimeCode_t * mtc){
        int length = sprintfMtc(dst, mtc);

        length += sprintf((char*)&dst[length], " %d", mtc->FractionalFrame);

        return length;
    }

    uint8_t readCueNumberPart( uint8_t * dst, uint8_t * argv ){

        if ( ! isValidMscCueNumberPart(argv) ){
            return 0;
        }

        // strcpy + strlen
        uint8_t i = 0;
        while( argv[i] != '\0' ){
            dst[i] = argv[i];
            i++;
        }

        dst[i] = '\0';

        return i;
    }

    bool readCueNumber( uint8_t * dst, MscCueNumber_t *cueNumber, uint8_t argc, uint8_t ** argv ){

        uint8_t len = readCueNumberPart( dst, argv[0] );

        if (len == 0) {
            return false;
        }

        cueNumber->Number = dst;

        if (argc == 1){
            cueNumber->List = NULL;
            cueNumber->Path = NULL;
            return true;
        }

        cueNumber->List = &cueNumber->Number[len];

        len = readCueNumberPart( cueNumber->List, argv[1] );

        if (len == 0) {
            return false;
        }


        if (argc == 2){
            cueNumber->Path = NULL;
            return true;
        }

        cueNumber->Path = &cueNumber->List[len];

        len = readCueNumberPart( cueNumber->Path, argv[1] );

        if (len == 0){
            return false;
        }

        return true;
    }

    uint8_t sprintfCueNumber( uint8_t * dst, MscCueNumber_t *cueNumber ){
        uint8_t len = sprintf( (char*)dst, "%s", cueNumber->Number );

        if (cueNumber->List == NULL){
            return len;
        }

        len += sprintf( (char*)&dst[len], " %s", cueNumber->List );

        if (cueNumber->Path == NULL){
            return len;
        }

        len += sprintf( (char*)&dst[len], " %s", cueNumber->Path );

        return len;
    }


    inline int ArgsToMccData( uint8_t * bytes, uint8_t * length, uint8_t argc, uint8_t ** argv){


        if (argc == 0){
            return StringifierResultWrongArgCount;
        }

        do {

//            printf("cmd =[%s]\n",  argv[0]);

            SysExRtMmcCommandData_t cmd;

            cmd.Command.Value = 0;

            if (str_eq(argv[0], "stop")) {
                cmd.Command.Bytes[0] = SysExRtMmcCommandStop;
            }
            else if (str_eq(argv[0], "play")) {
                cmd.Command.Bytes[0] = SysExRtMmcCommandPlay;
            }
            else if (str_eq(argv[0], "deferred-play")) {
                cmd.Command.Bytes[0] = SysExRtMmcCommandDeferredPlay;
            }
            else if (str_eq(argv[0], "fast-forward")) {
                cmd.Command.Bytes[0] = SysExRtMmcCommandFastForward;
            }
            else if (str_eq(argv[0], "rewind")) {
                cmd.Command.Bytes[0] = SysExRtMmcCommandRewind;
            }
            else if (str_eq(argv[0], "record-strobe")) {
                cmd.Command.Bytes[0] = SysExRtMmcCommandRecordStrobe;
            }
            else if (str_eq(argv[0], "record-exit")) {
                cmd.Command.Bytes[0] = SysExRtMmcCommandRecordExit;
            }
            else if (str_eq(argv[0], "record-pause")) {
                cmd.Command.Bytes[0] = SysExRtMmcCommandRecordPause;
            }
            else if (str_eq(argv[0], "pause")) {
                cmd.Command.Bytes[0] = SysExRtMmcCommandPause;
            }
            else if (str_eq(argv[0], "eject")) {
                cmd.Command.Bytes[0] = SysExRtMmcCommandEject;
            }
            else if (str_eq(argv[0], "chase")) {
                cmd.Command.Bytes[0] = SysExRtMmcCommandChase;
            }
            else if (str_eq(argv[0], "command-error-reset")) {
                cmd.Command.Bytes[0] = SysExRtMmcCommandCommandErrorReset;
            }
            else if (str_eq(argv[0], "mmc-reset")) {
                cmd.Command.Bytes[0] = SysExRtMmcCommandMmcReset;
            }
            else if (str_eq(argv[0], "wait")) {
                cmd.Command.Bytes[0] = SysExRtMmcCommandWait;
            }
            else if (str_eq(argv[0], "resume")) {
                cmd.Command.Bytes[0] = SysExRtMmcCommandResume;
            }
            else if (str_eq(argv[0], "variable-play")) {
                cmd.Command.Bytes[0] = SysExRtMmcCommandVariablePlay;
            }
            else if (str_eq(argv[0], "search")) {
                cmd.Command.Bytes[0] = SysExRtMmcCommandSearch;
            }
            else if (str_eq(argv[0], "shuttle")) {
                cmd.Command.Bytes[0] = SysExRtMmcCommandShuttle;
            }
            else if (str_eq(argv[0], "deferred-variable-play")) {
                cmd.Command.Bytes[0] = SysExRtMmcCommandDeferredVariablePlay;
            }
            else if (str_eq(argv[0], "record-strobe-variable")) {
                cmd.Command.Bytes[0] = SysExRtMmcCommandRecordStrobeVariable;
            }
            else if (str_eq(argv[0], "step")) {
                cmd.Command.Bytes[0] = SysExRtMmcCommandStep;
            }
            else if (str_eq(argv[0], "write")) {
                cmd.Command.Bytes[0] = SysExRtMmcCommandWrite;
            }
            else if (str_eq(argv[0], "masked-write")) {
                cmd.Command.Bytes[0] = SysExRtMmcCommandMaskedWrite;
            }
            else if (str_eq(argv[0], "read")) {
                cmd.Command.Bytes[0] = SysExRtMmcCommandRead;
            }
            else if (str_eq(argv[0], "update")) {
                cmd.Command.Bytes[0] = SysExRtMmcCommandUpdate;
            }
            else if (str_eq(argv[0], "locate")) {
                cmd.Command.Bytes[0] = SysExRtMmcCommandLocate;
            }
            else if (str_eq(argv[0], "assign-system-master")) {
                cmd.Command.Bytes[0] = SysExRtMmcCommandAssignSystemMaster;
            }
            else if (str_eq(argv[0], "generator-command")) {
                cmd.Command.Bytes[0] = SysExRtMmcCommandGeneratorCommand;
            }
            else if (str_eq(argv[0], "mtc-command")) {
                cmd.Command.Bytes[0] = SysExRtMmcCommandMtcCommand;
            }
            else if (str_eq(argv[0], "move")) {
                cmd.Command.Bytes[0] = SysExRtMmcCommandMove;
            }
            else if (str_eq(argv[0], "add")) {
                cmd.Command.Bytes[0] = SysExRtMmcCommandAdd;
            }
            else if (str_eq(argv[0], "substract")) {
                cmd.Command.Bytes[0] = SysExRtMmcCommandSubstract;
            }
            else if (str_eq(argv[0], "drop-frame-adjust")) {
                cmd.Command.Bytes[0] = SysExRtMmcCommandDropFrameAdjust;
            }
            else if (str_eq(argv[0], "procedure")) {
                cmd.Command.Bytes[0] = SysExRtMmcCommandProcedure;
            }
            else if (str_eq(argv[0], "event")) {
                cmd.Command.Bytes[0] = SysExRtMmcCommandEvent;
            }
            else if (str_eq(argv[0], "group")) {
                cmd.Command.Bytes[0] = SysExRtMmcCommandGroup;
            }
            else if (str_eq(argv[0], "command-segment")) {
                cmd.Command.Bytes[0] = SysExRtMmcCommandCommandSegment;
            }
            else {
                return StringifierResultInvalidValue;
            }


            uint8_t consumedArgs = 1; // command arg = 1
            float speed = 0.0;

            switch (cmd.Command.Bytes[0]) {

                case SysExRtMmcCommandStop:
                case SysExRtMmcCommandPlay:
                case SysExRtMmcCommandDeferredPlay:
                case SysExRtMmcCommandFastForward:
                case SysExRtMmcCommandRewind:
                case SysExRtMmcCommandRecordStrobe:
                case SysExRtMmcCommandRecordExit:
                case SysExRtMmcCommandRecordPause:
                case SysExRtMmcCommandPause:
                case SysExRtMmcCommandEject:
                case SysExRtMmcCommandChase:
                case SysExRtMmcCommandCommandErrorReset:
                case SysExRtMmcCommandMmcReset:
                case SysExRtMmcCommandWait:
                case SysExRtMmcCommandResume:
                    break;

                case SysExRtMmcCommandVariablePlay:
                case SysExRtMmcCommandSearch:
                case SysExRtMmcCommandShuttle:
                case SysExRtMmcCommandDeferredVariablePlay:
                case SysExRtMmcCommandRecordStrobeVariable:

                    if (argc < 2){
                        return StringifierResultWrongArgCount;
                    }

                    speed = atof((char*)argv[1]);

                    if (! SysExRtMmcStandardSpeedFromFloatObj( &cmd.Data.StandardSpeed, speed )){
                        return StringifierResultInvalidValue;
                    }

//                    printf("%d %d %d %04X\n", cmd.Data.StandardSpeed.Direction, cmd.Data.StandardSpeed.Resolution, cmd.Data.StandardSpeed.IntegerPart, cmd.Data.StandardSpeed.FractionalPart );
//                    exit(0);

                    consumedArgs ++;
                    break;

                case SysExRtMmcCommandStep:
                    if (argc < 2){
                        return StringifierResultWrongArgCount;
                    }

                    cmd.Data.S7 = atoi((char*)argv[1]);

                    assertI7( cmd.Data.S7 );

//                    printf("%d", cmd.Data.S7);
//                    exit(0);

                    consumedArgs ++;
                    break;

                case SysExRtMmcCommandLocate:
                    if (argc < 3){
                        return StringifierResultWrongArgCount;
                    }

                    if (str_eq(argv[1], "field")){
                        cmd.Data.Locate.SubCommand = SysExRtMmcCommandLocateSubCommandInformationField;
                        cmd.Data.Locate.InformationField = atoi((char*)argv[2]);

                        assertU7( cmd.Data.U7 );

                        consumedArgs += 2;
                    }
                    else if (str_eq(argv[1], "mtc")){
                        if (argc < 8){
                            return StringifierResultWrongArgCount;
                        }

                        cmd.Data.Locate.SubCommand = SysExRtMmcCommandLocateSubCommandTarget;
                        if ( ! readMtcLong( &cmd.Data.Locate.MidiTimeCode, &argv[2]) ){
                            assert(false);
                            return StringifierResultInvalidValue;
                        }

                        consumedArgs += 7;
                    }
                    else {
                        return StringifierResultInvalidValue;
                    }
                    break;

                case SysExRtMmcCommandAssignSystemMaster:
                    if (argc < 2){
                        return StringifierResultWrongArgCount;
                    }

                    cmd.Data.U7 = atoi((char*)argv[1]);

                    assertU7( cmd.Data.U7 );

                    consumedArgs ++;
                    break;


                case SysExRtMmcCommandWrite: ////////

                case SysExRtMmcCommandMaskedWrite:
                case SysExRtMmcCommandRead:
                case SysExRtMmcCommandUpdate:
                case SysExRtMmcCommandGeneratorCommand:
                case SysExRtMmcCommandMtcCommand:
                case SysExRtMmcCommandMove:
                case SysExRtMmcCommandAdd:
                case SysExRtMmcCommandSubstract:
                case SysExRtMmcCommandDropFrameAdjust:
                case SysExRtMmcCommandProcedure:
                case SysExRtMmcCommandEvent:
                case SysExRtMmcCommandGroup:
                case SysExRtMmcCommandCommandSegment:
                    //TODO

                default:
                    return StringifierResultGenericError;

            }

//            printf("cmd = %d [%s]\n", cmd.Command, argv[0]);
//            printf("argc = %d -> %d\n", argc, consumedArgs);

            argc -= consumedArgs;
            argv = &argv[consumedArgs];

            *length += packSysExRtMmcCommand( &bytes[*length], &cmd);

        } while( argc > 0 );

        return StringifierResultOk;
    }

    inline int MccDataToString( uint8_t * bytes, uint8_t * data, uint8_t length){

        int strLength = 0;

        for (;data != NULL && length > 0;
              data = getSysExRtMmcCommandNext(data, &length)) {

            SysExRtMmcCommandData_t cmd;

            if ( ! unpackSysExRtMmcCommand(data, length, &cmd) ){
                return 0;
            }

            // prepend next command with space
            if (strLength > 0){
                bytes[strLength++] = ' ';
                bytes[strLength] = '\0'; // safety
            }

            switch (cmd.Command.Bytes[0]) {

                case SysExRtMmcCommandStop:                     strLength += sprintf( (char*)&bytes[strLength], "stop"); break;
                case SysExRtMmcCommandPlay:                     strLength += sprintf( (char*)&bytes[strLength], "play"); break;
                case SysExRtMmcCommandDeferredPlay:             strLength += sprintf( (char*)&bytes[strLength], "deferred-play"); break;
                case SysExRtMmcCommandFastForward:              strLength += sprintf( (char*)&bytes[strLength], "fast-forward"); break;
                case SysExRtMmcCommandRewind:                   strLength += sprintf( (char*)&bytes[strLength], "rewind"); break;
                case SysExRtMmcCommandRecordStrobe:             strLength += sprintf( (char*)&bytes[strLength], "record-strobe"); break;
                case SysExRtMmcCommandRecordExit:               strLength += sprintf( (char*)&bytes[strLength], "record-exit"); break;
                case SysExRtMmcCommandRecordPause:              strLength += sprintf( (char*)&bytes[strLength], "record-pause"); break;
                case SysExRtMmcCommandPause:                    strLength += sprintf( (char*)&bytes[strLength], "pause"); break;
                case SysExRtMmcCommandEject:                    strLength += sprintf( (char*)&bytes[strLength], "eject"); break;
                case SysExRtMmcCommandChase:                    strLength += sprintf( (char*)&bytes[strLength], "chase"); break;
                case SysExRtMmcCommandCommandErrorReset:        strLength += sprintf( (char*)&bytes[strLength], "command-error-reset"); break;
                case SysExRtMmcCommandMmcReset:                 strLength += sprintf( (char*)&bytes[strLength], "mmc-reset"); break;
                case SysExRtMmcCommandWait:                     strLength += sprintf( (char*)&bytes[strLength], "wait"); break;
                case SysExRtMmcCommandResume:                   strLength += sprintf( (char*)&bytes[strLength], "resume"); break;

                case SysExRtMmcCommandVariablePlay:             strLength += sprintf( (char*)&bytes[strLength], "variable-play "); break;
                case SysExRtMmcCommandSearch:                   strLength += sprintf( (char*)&bytes[strLength], "search "); break;
                case SysExRtMmcCommandShuttle:                  strLength += sprintf( (char*)&bytes[strLength], "shuttle "); break;
                case SysExRtMmcCommandDeferredVariablePlay:     strLength += sprintf( (char*)&bytes[strLength], "deferred-variable-play "); break;
                case SysExRtMmcCommandRecordStrobeVariable:     strLength += sprintf( (char*)&bytes[strLength], "record-strobe-variable "); break;

                case SysExRtMmcCommandStep:                     strLength += sprintf( (char*)&bytes[strLength], "step "); break;

                case SysExRtMmcCommandWrite:                    strLength += sprintf( (char*)&bytes[strLength], "write "); break;

                case SysExRtMmcCommandMaskedWrite:              strLength += sprintf( (char*)&bytes[strLength], "masked-write "); break;
                case SysExRtMmcCommandRead:                     strLength += sprintf( (char*)&bytes[strLength], "read "); break;
                case SysExRtMmcCommandUpdate:                   strLength += sprintf( (char*)&bytes[strLength], "update "); break;
                case SysExRtMmcCommandLocate:                   strLength += sprintf( (char*)&bytes[strLength], "locate "); break;
                case SysExRtMmcCommandAssignSystemMaster:       strLength += sprintf( (char*)&bytes[strLength], "assign-system-master "); break;
                case SysExRtMmcCommandGeneratorCommand:         strLength += sprintf( (char*)&bytes[strLength], "generator-command "); break;
                case SysExRtMmcCommandMtcCommand:               strLength += sprintf( (char*)&bytes[strLength], "mtc-command "); break;
                case SysExRtMmcCommandMove:                     strLength += sprintf( (char*)&bytes[strLength], "move "); break;
                case SysExRtMmcCommandAdd:                      strLength += sprintf( (char*)&bytes[strLength], "add "); break;
                case SysExRtMmcCommandSubstract:                strLength += sprintf( (char*)&bytes[strLength], "substract "); break;
                case SysExRtMmcCommandDropFrameAdjust:          strLength += sprintf( (char*)&bytes[strLength], "drop-frame-adjust "); break;
                case SysExRtMmcCommandProcedure:                strLength += sprintf( (char*)&bytes[strLength], "procedure "); break;
                case SysExRtMmcCommandEvent:                    strLength += sprintf( (char*)&bytes[strLength], "event "); break;
                case SysExRtMmcCommandGroup:                    strLength += sprintf( (char*)&bytes[strLength], "group "); break;
                case SysExRtMmcCommandCommandSegment:           strLength += sprintf( (char*)&bytes[strLength], "command-segment "); break;

                default:
                    return 0;
            }

            float speed = 0.0;

            switch (cmd.Command.Bytes[0]) {

                case SysExRtMmcCommandStop:
                case SysExRtMmcCommandPlay:
                case SysExRtMmcCommandDeferredPlay:
                case SysExRtMmcCommandFastForward:
                case SysExRtMmcCommandRewind:
                case SysExRtMmcCommandRecordStrobe:
                case SysExRtMmcCommandRecordExit:
                case SysExRtMmcCommandRecordPause:
                case SysExRtMmcCommandPause:
                case SysExRtMmcCommandEject:
                case SysExRtMmcCommandChase:
                case SysExRtMmcCommandCommandErrorReset:
                case SysExRtMmcCommandMmcReset:
                case SysExRtMmcCommandWait:
                case SysExRtMmcCommandResume:
                        // no data
                    break;

                case SysExRtMmcCommandVariablePlay:
                case SysExRtMmcCommandSearch:
                case SysExRtMmcCommandShuttle:
                case SysExRtMmcCommandDeferredVariablePlay:
                case SysExRtMmcCommandRecordStrobeVariable:

//                    printf("%d %d %d %04X\n", cmd.Data.StandardSpeed.Direction, cmd.Data.StandardSpeed.Resolution, cmd.Data.StandardSpeed.IntegerPart, cmd.Data.StandardSpeed.FractionalPart );

                    speed = SysExRtMmcStandardSpeedToFloatObj( &cmd.Data.StandardSpeed );

                    strLength += sprintf( (char*)&bytes[strLength], "%f", speed );

                    break;

                case SysExRtMmcCommandStep:
                    strLength += sprintf( (char*)&bytes[strLength], "%d", cmd.Data.S7 );
                    break;

                case SysExRtMmcCommandLocate:
                    if (cmd.Data.Locate.SubCommand == SysExRtMmcCommandLocateSubCommandInformationField)  {
                        strLength += sprintf( (char*)&bytes[strLength], "field %d", cmd.Data.Locate.InformationField);
                    } else if (cmd.Data.Locate.SubCommand == SysExRtMmcCommandLocateSubCommandTarget)  {
                        strLength += sprintf( (char*)&bytes[strLength], "mtc ");
                        strLength += sprintfMtcLong(&bytes[strLength], &cmd.Data.Locate.MidiTimeCode);
                    }

                    break;

                case SysExRtMmcCommandAssignSystemMaster:
                    strLength += sprintf( (char*)&bytes[strLength], "%d", cmd.Data.U7);
                    break;

                case SysExRtMmcCommandWrite: ////////

                case SysExRtMmcCommandMaskedWrite:
                case SysExRtMmcCommandRead:
                case SysExRtMmcCommandUpdate:
                case SysExRtMmcCommandGeneratorCommand:
                case SysExRtMmcCommandMtcCommand:
                case SysExRtMmcCommandMove:
                case SysExRtMmcCommandAdd:
                case SysExRtMmcCommandSubstract:
                case SysExRtMmcCommandDropFrameAdjust:
                case SysExRtMmcCommandProcedure:
                case SysExRtMmcCommandEvent:
                case SysExRtMmcCommandGroup:
                case SysExRtMmcCommandCommandSegment:
                    //TODO

                default:
                    return 0;

            }

        }

        return strLength;
    }



    uint8_t stringToArgs(uint8_t ** argv, uint8_t maxArg, uint8_t * bytes, uint8_t length){

        uint8_t argc = 0;

        uint8_t beginning = 0;

        for(uint8_t i = 0; i < length; i++){

            // if first argument not yet detected
            if (argc == 0){
                // if reached end of string before first argument, then the input is bad.
                if (bytes[i] == '\0'){
                    break;
                }
                // trim beginning whitespace
                if (bytes[i] != ' ' && bytes[i] != '\r' && bytes[i] != '\n' && bytes[i] != '\t'){
                    argv[argc++] = &bytes[i];
                    beginning = i;
                }
                continue;
            }

            // reached end of line/input
            if (bytes[i] == '\0' || bytes[i] == '\r' || bytes[i] == '\n'){

                // make sure to properly terminate last argument
                bytes[i] = '\0';

                break;
            }

            // replace argument delimiters with EOS
            if (bytes[i] == ' '){
                bytes[i] = '\0';

                continue;
            }

            // otherwise there is  presumably valid argument character

            // if before there was an argument delimiter, register new argument
            if ( i - beginning > 0 && bytes[i - 1] == '\0'){

                ASSERT(argc < maxArg);

                argv[argc++] = &bytes[i];

                continue;
            }

            // otherwise go to next input character
        }

        return argc;
    }

    int MessagefromString(Message_t * msg, uint8_t length, uint8_t * bytes){

        uint8_t argc = 0;
        uint8_t *argv[32];

        argc = stringToArgs(argv, 32, bytes, length);

        return MessagefromArgs(msg, argc, argv);
    }

    int MessagefromArgs(Message_t * msg, uint8_t argc, uint8_t *argv[]) {

//        printf("argc = %d : ", argc);
//        for(uint8_t i = 0; i < argc; i++){
//            printf("%s ", argv[i]);
//        }
//        printf("\n");

        if (argc == 0) {
            return StringifierResultNoInput;
        }

        if (argc == 1){
            if (str_eq(argv[0], "start")) {
                msg->StatusClass = StatusClassSystemMessage;
                msg->SystemMessage = SystemMessageStart;
            }
            else if (str_eq(argv[0], "stop")) {
                msg->StatusClass = StatusClassSystemMessage;
                msg->SystemMessage = SystemMessageStop;
            }
            else if (str_eq(argv[0], "continue")) {
                msg->StatusClass = StatusClassSystemMessage;
                msg->SystemMessage = SystemMessageContinue;
            }
            else if (str_eq(argv[0], "active-sensing")) {
                msg->StatusClass = StatusClassSystemMessage;
                msg->SystemMessage = SystemMessageActiveSensing;
            }
            else if (str_eq(argv[0], "reset")) {
                msg->StatusClass = StatusClassSystemMessage;
                msg->SystemMessage = SystemMessageReset;
            }
            else if (str_eq(argv[0], "timing-clock")) {
                msg->StatusClass = StatusClassSystemMessage;
                msg->SystemMessage = SystemMessageTimingClock;
            }
            else if (str_eq(argv[0], "tune-request")) {
                msg->StatusClass = StatusClassSystemMessage;
                msg->SystemMessage = SystemMessageTuneRequest;
            }
            else {
                return StringifierResultInvalidValue;
            }

            return StringifierResultOk;
        }
        
        else if (str_eq(argv[0], "note")) {
            if (argc != 5) {
                return StringifierResultWrongArgCount;
            }
            if (str_eq(argv[1], "on")) {
                msg->StatusClass = StatusClassNoteOn;
            }
            else if (str_eq(argv[1], "off")) {
                msg->StatusClass = StatusClassNoteOff;
            }
            else {
                return  StringifierResultInvalidValue;
            }
            msg->Channel = atoi((char*)argv[2]);
            msg->Data.Note.Key = atoi((char*)argv[3]);
            msg->Data.Note.Velocity = atoi((char*)argv[4]);

            assertU7(msg->Channel);
            assertU7(msg->Data.Note.Key);
            assertU7(msg->Data.Note.Velocity);

            return StringifierResultOk;
        }

        else if (str_eq(argv[0], "cc")) {
            if (argc != 4) {
                return StringifierResultWrongArgCount;
            }
            msg->StatusClass = StatusClassControlChange;
            msg->Channel = atoi((char*)argv[1]);
            msg->Data.ControlChange.Controller = atoi((char*)argv[2]);
            msg->Data.ControlChange.Value = atoi((char*)argv[3]);

            assertU7(msg->Channel);
            assertU7(msg->Data.ControlChange.Controller);
            assertU7(msg->Data.ControlChange.Value);

            return StringifierResultOk;
        }

        else if (str_eq(argv[0], "pc")) {
            if (argc != 3) {
                return StringifierResultWrongArgCount;
            }
            msg->StatusClass = StatusClassProgramChange;
            msg->Channel = atoi((char*)argv[1]);
            msg->Data.ProgramChange.Program = atoi((char*)argv[2]);

            assertU7(msg->Channel);
            assertU7(msg->Data.ProgramChange.Program);

            return StringifierResultOk;
        }

        else if (str_eq(argv[0], "pressure")) {
            if (argc != 3) {
                return StringifierResultWrongArgCount;
            }
            msg->StatusClass = StatusClassChannelPressure;
            msg->Channel = atoi((char*)argv[1]);
            msg->Data.ChannelPressure.Pressure = atoi((char*)argv[2]);

            assertU7(msg->Channel);
            assertU7(msg->Data.ChannelPressure.Pressure);

            return StringifierResultOk;
        }

        else if (str_eq(argv[0], "pitch")) {
            if (argc != 3) {
                return StringifierResultWrongArgCount;
            }
            msg->StatusClass = StatusClassPitchBendChange;
            msg->Channel = atoi((char*)argv[1]);
            msg->Data.PitchBendChange.Pitch = atoi((char*)argv[2]);

            assertU7(msg->Channel);
            assertU14(msg->Data.PitchBendChange.Pitch);

            return StringifierResultOk;
        }

        else if (str_eq(argv[0], "poly")) {
            if (argc != 4) {
                return StringifierResultWrongArgCount;
            }
            msg->StatusClass = StatusClassPolyphonicKeyPressure;
            msg->Channel = atoi((char*)argv[1]);
            msg->Data.PolyphonicKeyPressure.Key = atoi((char*)argv[2]);
            msg->Data.PolyphonicKeyPressure.Pressure = atoi((char*)argv[3]);

            assertU7(msg->Channel);
            assertU7(msg->Data.PolyphonicKeyPressure.Key);
            assertU7(msg->Data.PolyphonicKeyPressure.Pressure);

            return StringifierResultOk;
        }

        else if (str_eq(argv[0], "quarter-frame")) {
            if (argc != 3) {
                return StringifierResultWrongArgCount;
            }
            msg->StatusClass = StatusClassSystemMessage;
            msg->SystemMessage = SystemMessageMtcQuarterFrame;
            msg->Data.MtcQuarterFrame.MessageType = atoi((char*)argv[1]);
            msg->Data.MtcQuarterFrame.Nibble = atoi((char*)argv[2]);

            assertBool(isMtcQuarterMessageType(msg->Data.MtcQuarterFrame.MessageType));
            assertU4(msg->Data.MtcQuarterFrame.Nibble);

            return StringifierResultOk;
        }
        else if (str_eq(argv[0], "song-position")) {
            if (argc != 2) {
                return StringifierResultWrongArgCount;
            }
            msg->StatusClass = StatusClassSystemMessage;
            msg->SystemMessage = SystemMessageSongPositionPointer;
            msg->Data.SongPositionPointer.Position = atoi((char*)argv[1]);

            assertU14(msg->Data.SongPositionPointer.Position);

            return StringifierResultOk;
        }
        else if (str_eq(argv[0], "song-select")) {
            if (argc != 2) {
                return StringifierResultWrongArgCount;
            }
            msg->StatusClass = StatusClassSystemMessage;
            msg->SystemMessage = SystemMessageSongSelect;
            msg->Data.SongSelect.Song = atoi((char*)argv[1]);

            assertU7(msg->Data.SongSelect.Song);

            return StringifierResultOk;
        }
        
        else if (str_eq(argv[0], "sysex")) {
            if (argc < 2) {
                return StringifierResultWrongArgCount;
            }

            msg->StatusClass = StatusClassSystemMessage;
            msg->SystemMessage = SystemMessageSystemExclusive;


            if (str_eq(argv[1], "experimental")) {
                if (argc < 2 || argc > 3) {
                    return StringifierResultWrongArgCount;
                }

                msg->Data.SysEx.Id = SysExIdExperimental;
                if (argc < 3) {
                    msg->Data.SysEx.Length = 0;
                } else {

                    if (!readHex(msg->Data.SysEx.ByteData, &msg->Data.SysEx.Length, argv[2], 0)) {
                        return StringifierResultInvalidHex;
                    }

                    assertData(msg->Data.SysEx.ByteData, msg->Data.SysEx.Length);
                }

                return StringifierResultOk;
            }
            else if (str_eq(argv[1], "manufacturer")) {
                if (argc < 3 || argc > 4) {
                    return StringifierResultWrongArgCount;
                }
                msg->Data.SysEx.Id = strtol((char*)argv[2], NULL, 16);

                if (argc < 4) {
                    msg->Data.SysEx.Length = 0;
                } else {

                    if (!readHex(msg->Data.SysEx.ByteData, &msg->Data.SysEx.Length, argv[3], 0)) {
                        return StringifierResultInvalidHex;
                    }

                    assertData(msg->Data.SysEx.ByteData, msg->Data.SysEx.Length);
                }

                return StringifierResultOk;
            }
            else if (str_eq(argv[1], "rt")) {
                if (argc < 4) {
                    return StringifierResultWrongArgCount;
                }
                msg->Channel = atoi((char*)argv[2]);
                msg->Data.SysEx.Id = SysExIdRealTime;

                if (str_eq(argv[3], "mtc")) {
                    if (argc < 5) {
                        return StringifierResultWrongArgCount;
                    }
                    msg->Data.SysEx.SubId1 = SysExRtMidiTimeCode;

                    if (str_eq(argv[4], "full-message")) {
                        if (argc < 10) {
                            return StringifierResultWrongArgCount;
                        }
                        msg->Data.SysEx.SubId2 = SysExRtMtcFullMessage;

                        if ( ! readMtc(&msg->Data.SysEx.Data.MidiTimeCode, &argv[5])){
                            return StringifierResultInvalidValue;
                        }

                        return StringifierResultOk;
                    }
                    if (str_eq(argv[4], "user-bits")) {
                        if (argc < 6) {
                            return StringifierResultWrongArgCount;
                        }
                        msg->Data.SysEx.SubId2 = SysExRtMtcUserBits;

                        if ( ! hex_to_byte(msg->Data.SysEx.ByteData, argv[5], 5) ){
                            return StringifierResultInvalidValue;
                        }
                        msg->Data.SysEx.Length = 5;

                        return StringifierResultOk;
                    }
                }
                else if (str_eq(argv[3], "cueing")) {
                    if (argc < 5) {
                        return StringifierResultWrongArgCount;
                    }
                    msg->Data.SysEx.SubId1 = SysExRtMidiTimeCodeCueing;

                    if (str_eq(argv[4], "special")) {
                        if (argc != 6) {
                            return StringifierResultWrongArgCount;
                        }
                        msg->Data.SysEx.SubId2 = SysExRtMtcCueingSpecial;

                        if (str_eq(argv[5], "system-stop")) {
                            msg->Data.SysEx.Data.Cueing.EventNumber = SysExRtMtcCueingSpecialSystemStop;
                        }
                        else {
                            return StringifierResultInvalidValue;
                        }

                        return StringifierResultOk;
                    }
                    else {
                        if (argc < 6) {
                            return StringifierResultWrongArgCount;
                        }

                        if (str_eq(argv[4], "punch-in")) {
                            msg->Data.SysEx.SubId2 = SysExRtMtcCueingPunchInPoint;
                        }
                        else if (str_eq(argv[4], "punch-out")) {
                            msg->Data.SysEx.SubId2 = SysExRtMtcCueingPunchOutPoint;
                        }
                        else if (str_eq(argv[4], "event-start")) {
                            if (argc == 6) {
                                msg->Data.SysEx.SubId2 = SysExRtMtcCueingEventStartPoint;
                            } else {
                                msg->Data.SysEx.SubId2 = SysExRtMtcCueingEventStartPointWithInfo;
                            }
                        }
                        else if (str_eq(argv[4], "event-stop")) {
                            if (argc == 6) {
                                msg->Data.SysEx.SubId2 = SysExRtMtcCueingEventStopPoint;
                            } else {
                                msg->Data.SysEx.SubId2 = SysExRtMtcCueingEventStopPointWithInfo;
                            }
                        }
                        else if (str_eq(argv[4], "cue-point")) {
                            if (argc == 6) {
                                msg->Data.SysEx.SubId2 = SysExRtMtcCueingCuePoint;
                            } else {
                                msg->Data.SysEx.SubId2 = SysExRtMtcCueingCuePointWithInfo;
                            }
                        }
                        else if (str_eq(argv[4], "event-name")) {
                            if (argc < 7) {
                                return StringifierResultWrongArgCount;
                            }
                            msg->Data.SysEx.SubId2 = SysExRtMtcCueingEventName;
                        }
                        else {
                            return StringifierResultInvalidValue;
                        }

                        msg->Data.SysEx.Data.Cueing.EventNumber = atoi((char*)argv[5]);

                        assertU14(msg->Data.SysEx.Data.Cueing.EventNumber);

                        if (isSysExRtMtcCueingWithAddInfo(msg->Data.SysEx.SubId2)) {
                            uint8_t offset = 0;
                            for (uint8_t i = 0; i < argc - 6; i++) {
                                if (i > 0) {
                                    msg->Data.SysEx.ByteData[offset++] = ' ';
                                }
                                strcpy((char *) &msg->Data.SysEx.ByteData[offset], (char*)argv[6 + i]);
                                offset += strlen((char*)argv[6 + i]);

                            }
                            msg->Data.SysEx.Length += strlen((char*) msg->Data.SysEx.ByteData);
                        }

                        return StringifierResultOk;

                    }
                }
                else if (str_eq(argv[3],"msc")){
                    if (argc < 6) {
                        return StringifierResultWrongArgCount;
                    }
                    msg->Data.SysEx.SubId1 = SysExRtMidiShowControl;

                    msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Value = 0;

                    msg->Data.SysEx.Data.MidiShowControl.Command.Value = 0;



                    if (str_eq(argv[4], "lighting")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtLighting;
                    }
                    else if (str_eq(argv[4], "moving-lights")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtMovingLights;
                    }
                    else if (str_eq(argv[4], "color-changers")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtColorChangers;
                    }
                    else if (str_eq(argv[4], "strobes")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtStrobes;
                    }
                    else if (str_eq(argv[4], "lasers")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtLasers;
                    }
                    else if (str_eq(argv[4], "chasers")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtChasers;
                    }
                    else if (str_eq(argv[4], "sound")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtSound;
                    }
                    else if (str_eq(argv[4], "music")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtMusic;
                    }
                    else if (str_eq(argv[4], "cd-players")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtCdPlayers;
                    }
                    else if (str_eq(argv[4], "eprom-playback")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtEpromPlayback;
                    }
                    else if (str_eq(argv[4], "audio-tape-machines")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtAudioTapeMachines;
                    }
                    else if (str_eq(argv[4], "intercoms")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtIntercoms;
                    }
                    else if (str_eq(argv[4], "amplifiers")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtAmplifiers;
                    }
                    else if (str_eq(argv[4], "audio-fx")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtAudioEffectsDevices;
                    }
                    else if (str_eq(argv[4], "equalizers")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtEqualizers;
                    }
                    else if (str_eq(argv[4], "machinery")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtMachinery;
                    }
                    else if (str_eq(argv[4], "rigging")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtRigging;
                    }
                    else if (str_eq(argv[4], "flys")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtFlys;
                    }
                    else if (str_eq(argv[4], "lifts")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtLifts;
                    }
                    else if (str_eq(argv[4], "turntables")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtTurntables;
                    }
                    else if (str_eq(argv[4], "trusses")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtTrusses;
                    }
                    else if (str_eq(argv[4], "robots")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtRobots;
                    }
                    else if (str_eq(argv[4], "animation")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtAnimation;
                    }
                    else if (str_eq(argv[4], "floats")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtFloats;
                    }
                    else if (str_eq(argv[4], "breakaways")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtBreakaways;
                    }
                    else if (str_eq(argv[4], "barges")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtBarges;
                    }
                    else if (str_eq(argv[4], "video")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtVideo;
                    }
                    else if (str_eq(argv[4], "video-tape-machines")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtVideoTapeMachines;
                    }
                    else if (str_eq(argv[4], "video-cassette-machines")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtVideoCassetteMachines;
                    }
                    else if (str_eq(argv[4], "video-disc-players")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtVideoDiscPlayers;
                    }
                    else if (str_eq(argv[4], "video-switchers")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtVideoSwitchers;
                    }
                    else if (str_eq(argv[4], "video-fx")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtVideoEffects;
                    }
                    else if (str_eq(argv[4], "video-char-generators")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtVideoCharacterGenerators;
                    }
                    else if (str_eq(argv[4], "video-still-stores")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtVideoStillStores;
                    }
                    else if (str_eq(argv[4], "video-monitors")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtVideoMonitors;
                    }
                    else if (str_eq(argv[4], "projection")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtProjection;
                    }
                    else if (str_eq(argv[4], "film-projects")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtFilmProjectors;
                    }
                    else if (str_eq(argv[4], "slide-projectors")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtSlideProjectors;
                    }
                    else if (str_eq(argv[4], "video-projectors")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtVideoProjectors;
                    }
                    else if (str_eq(argv[4], "dissolvers")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtDissolvers;
                    }
                    else if (str_eq(argv[4], "shutter-controls")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtShutterControls;
                    }
                    else if (str_eq(argv[4], "process-control")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtProcessControl;
                    }
                    else if (str_eq(argv[4], "hydraulic-oil")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtHydraulicOil;
                    }
                    else if (str_eq(argv[4], "h2o")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtH2O;
                    }
                    else if (str_eq(argv[4], "co2")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtCO2;
                    }
                    else if (str_eq(argv[4], "compressed-air")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtCompressedAir;
                    }
                    else if (str_eq(argv[4], "natural-gas")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtNaturalGas;
                    }
                    else if (str_eq(argv[4], "fog")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtFog;
                    }
                    else if (str_eq(argv[4], "smoke")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtSmoke;
                    }
                    else if (str_eq(argv[4], "cracked-haze")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtCrackedHaze;
                    }
                    else if (str_eq(argv[4], "pyro")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtPyro;
                    }
                    else if (str_eq(argv[4], "fireworks")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtFireworks;
                    }
                    else if (str_eq(argv[4], "explosions")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtExplosions;
                    }
                    else if (str_eq(argv[4], "flame")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtFlame;
                    }
                    else if (str_eq(argv[4], "smoke-pots")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtSmokePots;
                    }
                    else if (str_eq(argv[4], "all")) {
                        msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0] = SysExRtMscCmdFmtAllTypes;
                    }
                    else {
                        return StringifierResultInvalidValue;
                    }

                    if (str_eq(argv[5], "go")) {
                        msg->Data.SysEx.Data.MidiShowControl.Command.Bytes[0] = SysExRtMscCmdGo;
                    }
                    else if (str_eq(argv[5], "stop")) {
                        msg->Data.SysEx.Data.MidiShowControl.Command.Bytes[0] = SysExRtMscCmdStop;
                    }
                    else if (str_eq(argv[5], "resume")) {
                        msg->Data.SysEx.Data.MidiShowControl.Command.Bytes[0] = SysExRtMscCmdResume;
                    }
                    else if (str_eq(argv[5], "load")) {
                        msg->Data.SysEx.Data.MidiShowControl.Command.Bytes[0] = SysExRtMscCmdLoad;
                    }
                    else if (str_eq(argv[5], "go-off")) {
                        msg->Data.SysEx.Data.MidiShowControl.Command.Bytes[0] = SysExRtMscCmdGoOff;
                    }
                    else if (str_eq(argv[5], "go-jam-lock")) {
                        msg->Data.SysEx.Data.MidiShowControl.Command.Bytes[0] = SysExRtMscCmdGo_JamLock;
                    }
                    else if (str_eq(argv[5], "timed-go")) {
                        msg->Data.SysEx.Data.MidiShowControl.Command.Bytes[0] = SysExRtMscCmdTimedGo;
                    }
                    else if (str_eq(argv[5], "set")) {
                        msg->Data.SysEx.Data.MidiShowControl.Command.Bytes[0] = SysExRtMscCmdSet;
                    }
                    else if (str_eq(argv[5], "fire")) {
                        msg->Data.SysEx.Data.MidiShowControl.Command.Bytes[0] = SysExRtMscCmdFire;
                    }
                    else if (str_eq(argv[5], "standby+")) {
                        msg->Data.SysEx.Data.MidiShowControl.Command.Bytes[0] = SysExRtMscCmdStandbyPlus;
                    }
                    else if (str_eq(argv[5], "standby-")) {
                        msg->Data.SysEx.Data.MidiShowControl.Command.Bytes[0] = SysExRtMscCmdStandbyMinus;
                    }
                    else if (str_eq(argv[5], "sequence+")) {
                        msg->Data.SysEx.Data.MidiShowControl.Command.Bytes[0] = SysExRtMscCmdSequencePlus;
                    }
                    else if (str_eq(argv[5], "sequence-")) {
                        msg->Data.SysEx.Data.MidiShowControl.Command.Bytes[0] = SysExRtMscCmdSequenceMinus;
                    }
                    else if (str_eq(argv[5], "start-clock")) {
                        msg->Data.SysEx.Data.MidiShowControl.Command.Bytes[0] = SysExRtMscCmdStartClock;
                    }
                    else if (str_eq(argv[5], "stop-clock")) {
                        msg->Data.SysEx.Data.MidiShowControl.Command.Bytes[0] = SysExRtMscCmdStopClock;
                    }
                    else if (str_eq(argv[5], "zero-clock")) {
                        msg->Data.SysEx.Data.MidiShowControl.Command.Bytes[0] = SysExRtMscCmdZeroClock;
                    }
                    else if (str_eq(argv[5], "mtc-chase-on")) {
                        msg->Data.SysEx.Data.MidiShowControl.Command.Bytes[0] = SysExRtMscCmdMtcChaseOn;
                    }
                    else if (str_eq(argv[5], "mtc-chase-off")) {
                        msg->Data.SysEx.Data.MidiShowControl.Command.Bytes[0] = SysExRtMscCmdMtcChaseOff;
                    }
                    else if (str_eq(argv[5], "open-cue-list")) {
                        msg->Data.SysEx.Data.MidiShowControl.Command.Bytes[0] = SysExRtMscCmdOpenCueList;
                    }
                    else if (str_eq(argv[5], "close-cue-list")) {
                        msg->Data.SysEx.Data.MidiShowControl.Command.Bytes[0] = SysExRtMscCmdCloseCueList;
                    }
                    else if (str_eq(argv[5], "open-cue-path")) {
                        msg->Data.SysEx.Data.MidiShowControl.Command.Bytes[0] = SysExRtMscCmdOpenCuePath;
                    }
                    else if (str_eq(argv[5], "close-cue-path")) {
                        msg->Data.SysEx.Data.MidiShowControl.Command.Bytes[0] = SysExRtMscCmdCloseCuePath;
                    }
                    else if (str_eq(argv[5], "set-clock")) {
                        msg->Data.SysEx.Data.MidiShowControl.Command.Bytes[0] = SysExRtMscCmdSetClock;
                    }
                    else if (str_eq(argv[5], "standby")) {
                        msg->Data.SysEx.Data.MidiShowControl.Command.Bytes[0] = SysExRtMscCmdStandby;
                    }
                    else if (str_eq(argv[5], "go-2-pc")) {
                        msg->Data.SysEx.Data.MidiShowControl.Command.Bytes[0] = SysExRtMscCmdGo2Pc;
                    }
                    else if (str_eq(argv[5], "standing-by")) {
                        msg->Data.SysEx.Data.MidiShowControl.Command.Bytes[0] = SysExRtMscCmdStandingBy;
                    }
                    else if (str_eq(argv[5], "complete")) {
                        msg->Data.SysEx.Data.MidiShowControl.Command.Bytes[0] = SysExRtMscCmdComplete;
                    }
                    else if (str_eq(argv[5], "cancel")) {
                        msg->Data.SysEx.Data.MidiShowControl.Command.Bytes[0] = SysExRtMscCmdCancel;
                    }
                    else if (str_eq(argv[5], "cancelled")) {
                        msg->Data.SysEx.Data.MidiShowControl.Command.Bytes[0] = SysExRtMscCmdCancelled;
                    }
                    else if (str_eq(argv[5], "abort")) {
                        msg->Data.SysEx.Data.MidiShowControl.Command.Bytes[0] = SysExRtMscCmdAbort;
                    }
                    else if (str_eq(argv[5], "all-off")) {
                        msg->Data.SysEx.Data.MidiShowControl.Command.Bytes[0] = SysExRtMscCmdAllOff;
                    }
                    else if (str_eq(argv[5], "restore")) {
                        msg->Data.SysEx.Data.MidiShowControl.Command.Bytes[0] = SysExRtMscCmdRestore;
                    }
                    else if (str_eq(argv[5], "reset")) {
                        msg->Data.SysEx.Data.MidiShowControl.Command.Bytes[0] = SysExRtMscCmdReset;
                    }
                    else {
                        return StringifierResultInvalidValue;
                    }

                    switch(msg->Data.SysEx.Data.MidiShowControl.Command.Bytes[0]){

                        case SysExRtMscCmdGo:
                        case SysExRtMscCmdStop:
                        case SysExRtMscCmdResume:
                        case SysExRtMscCmdLoad:

                        case SysExRtMscCmdGoOff:
                        case SysExRtMscCmdGo_JamLock:
                            if (argc < 7 || 9 < argc) {
                                return StringifierResultWrongArgCount;
                            }
                            if ( ! readCueNumber( msg->Data.SysEx.ByteData, &msg->Data.SysEx.Data.MidiShowControl.CueNumber, argc - 6, &argv[6]) ){
                                return StringifierResultInvalidValue;
                            }

                            return StringifierResultOk;

                        case SysExRtMscCmdTimedGo:
                            if (argc < 13 | 15 < argc) {
                                return StringifierResultWrongArgCount;
                            }
                            if ( ! readMtcLong( &msg->Data.SysEx.Data.MidiShowControl.MidiTimeCode, &argv[6]) ){
                                return StringifierResultInvalidValue;
                            }
                            if ( ! readCueNumber( msg->Data.SysEx.ByteData, &msg->Data.SysEx.Data.MidiShowControl.CueNumber, argc - 12, &argv[12]) ){
                                return StringifierResultInvalidValue;
                            }

                            return StringifierResultOk;

                        case SysExRtMscCmdSet:
                            if (argc != 14) {
                                return StringifierResultWrongArgCount;
                            }
                            msg->Data.SysEx.Data.MidiShowControl.Controller = atoi((char*)argv[6]);
                            msg->Data.SysEx.Data.MidiShowControl.Value = atoi((char*)argv[7]);

                            assertU14(msg->Data.SysEx.Data.MidiShowControl.Controller);
                            assertU14(msg->Data.SysEx.Data.MidiShowControl.Value);

                            if ( ! readMtcLong( &msg->Data.SysEx.Data.MidiShowControl.MidiTimeCode, &argv[8]) ){
                                return StringifierResultInvalidValue;
                            }

                            return StringifierResultOk;

                        case SysExRtMscCmdFire:
                            if (argc != 7) {
                                return StringifierResultWrongArgCount;
                            }
                            msg->Data.SysEx.Data.MidiShowControl.MacroNumber = atoi((char*)argv[6]);

                            assertU7(msg->Data.SysEx.Data.MidiShowControl.MacroNumber);

                            return StringifierResultOk;

                        case SysExRtMscCmdStandbyPlus:
                        case SysExRtMscCmdStandbyMinus:
                        case SysExRtMscCmdSequencePlus:
                        case SysExRtMscCmdSequenceMinus:
                        case SysExRtMscCmdStartClock:
                        case SysExRtMscCmdStopClock:
                        case SysExRtMscCmdZeroClock:
                        case SysExRtMscCmdMtcChaseOn:
                        case SysExRtMscCmdMtcChaseOff:
                        case SysExRtMscCmdOpenCueList:
                        case SysExRtMscCmdCloseCueList:
                            if (argc != 7) {
                                return StringifierResultWrongArgCount;
                            }
                            msg->Data.SysEx.Data.MidiShowControl.CueNumber.List = msg->Data.SysEx.ByteData;
                            if ( ! readCueNumberPart(msg->Data.SysEx.Data.MidiShowControl.CueNumber.List, argv[6]) ){
                                return StringifierResultInvalidValue;
                            }
                            return StringifierResultOk;

                        case SysExRtMscCmdOpenCuePath:
                        case SysExRtMscCmdCloseCuePath:
                            if (argc != 7) {
                                return StringifierResultWrongArgCount;
                            }

                            msg->Data.SysEx.Data.MidiShowControl.CueNumber.Path = msg->Data.SysEx.ByteData;
                            if ( ! readCueNumberPart(msg->Data.SysEx.Data.MidiShowControl.CueNumber.Path, argv[6]) ){
                                return StringifierResultInvalidValue;
                            }
                            return StringifierResultOk;

                        case SysExRtMscCmdSetClock:
                            if (argc != 13) {
                                return StringifierResultWrongArgCount;
                            }

                            if ( ! readMtcLong( &msg->Data.SysEx.Data.MidiShowControl.MidiTimeCode, &argv[6]) ){
                                return StringifierResultInvalidValue;
                            }

                            msg->Data.SysEx.Data.MidiShowControl.CueNumber.List = msg->Data.SysEx.ByteData;
                            if ( ! readCueNumberPart(msg->Data.SysEx.Data.MidiShowControl.CueNumber.List, argv[12]) ){
                                return StringifierResultInvalidValue;
                            }
                            return StringifierResultOk;

                        case SysExRtMscCmdStandby:
                        case SysExRtMscCmdGo2Pc:
                            if (argc < 10 || 12 < argc) {
                                return StringifierResultWrongArgCount;
                            }

                            msg->Data.SysEx.Data.MidiShowControl.Checksum = atoi((char*)argv[6]);
                            msg->Data.SysEx.Data.MidiShowControl.SequenceNumber = atoi((char*)argv[7]);

                            assertU14(msg->Data.SysEx.Data.MidiShowControl.Checksum);
                            assertU14(msg->Data.SysEx.Data.MidiShowControl.SequenceNumber);

                            if ( ! readHex(msg->Data.SysEx.Data.MidiShowControl.Data, NULL, argv[8], 4)){
                                return StringifierResultInvalidHex;
                            }

                            if ( ! readCueNumber( msg->Data.SysEx.ByteData, &msg->Data.SysEx.Data.MidiShowControl.CueNumber, argc - 9, &argv[9]) ){
                                return StringifierResultInvalidValue;
                            }

                            return StringifierResultOk;

                        case SysExRtMscCmdStandingBy:
                            if (argc < 15 | 17 < argc) {
                                return StringifierResultWrongArgCount;
                            }

                            msg->Data.SysEx.Data.MidiShowControl.Checksum = atoi((char*)argv[6]);
                            msg->Data.SysEx.Data.MidiShowControl.SequenceNumber = atoi((char*)argv[7]);

                            assertU14(msg->Data.SysEx.Data.MidiShowControl.Checksum);
                            assertU14(msg->Data.SysEx.Data.MidiShowControl.SequenceNumber);

                            if ( ! readMtcLong( &msg->Data.SysEx.Data.MidiShowControl.MidiTimeCode, &argv[8]) ){
                                return StringifierResultInvalidValue;
                            }

                            if ( ! readCueNumber( msg->Data.SysEx.ByteData, &msg->Data.SysEx.Data.MidiShowControl.CueNumber, argc - 14, &argv[14]) ){
                                return StringifierResultInvalidValue;
                            }

                            return StringifierResultOk;

                        case SysExRtMscCmdComplete:
                        case SysExRtMscCmdCancel:
                            if (argc < 9 || 11 < argc) {
                                return StringifierResultWrongArgCount;
                            }
                            msg->Data.SysEx.Data.MidiShowControl.Checksum = atoi((char*)argv[6]);
                            msg->Data.SysEx.Data.MidiShowControl.SequenceNumber = atoi((char*)argv[7]);

                            assertU14(msg->Data.SysEx.Data.MidiShowControl.Checksum);
                            assertU14(msg->Data.SysEx.Data.MidiShowControl.SequenceNumber);

                            if ( ! readCueNumber( msg->Data.SysEx.ByteData, &msg->Data.SysEx.Data.MidiShowControl.CueNumber, argc - 8, &argv[8]) ){
                                return StringifierResultInvalidValue;
                            }

                            return StringifierResultOk;

                        case SysExRtMscCmdCancelled:
                        case SysExRtMscCmdAbort:
                            if (argc < 10 || 12 < argc) {
                                return StringifierResultWrongArgCount;
                            }
                            msg->Data.SysEx.Data.MidiShowControl.Checksum = atoi((char*)argv[6]);
                            msg->Data.SysEx.Data.MidiShowControl.Status = atoi((char*)argv[7]);
                            msg->Data.SysEx.Data.MidiShowControl.SequenceNumber = atoi((char*)argv[8]);

                            assertU14(msg->Data.SysEx.Data.MidiShowControl.Checksum);
                            assertU14(msg->Data.SysEx.Data.MidiShowControl.Status);
                            assertU14(msg->Data.SysEx.Data.MidiShowControl.SequenceNumber);

                            if ( ! readCueNumber( msg->Data.SysEx.ByteData, &msg->Data.SysEx.Data.MidiShowControl.CueNumber, argc - 9, &argv[9]) ){
                                return StringifierResultInvalidValue;
                            }

                            return StringifierResultOk;


                        case SysExRtMscCmdAllOff:
                        case SysExRtMscCmdRestore:
                        case SysExRtMscCmdReset:
                            if (argc != 6) {
                                return StringifierResultWrongArgCount;
                            }

//                            printf("cmdFmt=%02X  cmd=%02X\n", msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0], msg->Data.SysEx.Data.MidiShowControl.Command.Bytes[0]);
                            return StringifierResultOk;
                    }

                }
                else if (str_eq(argv[3], "dc")){
                    msg->Data.SysEx.SubId1 = SysExRtDeviceControl;

                    if (argc == 6){
                        if (str_eq(argv[4], "master-volume")){
                            msg->Data.SysEx.SubId2 = SysExRtDcMasterVolume;
                        } else if (str_eq(argv[4], "master-balance")){
                            msg->Data.SysEx.SubId2 = SysExRtDcMasterBalance;
                        } else if (str_eq(argv[4], "coarse-tuning")){
                            msg->Data.SysEx.SubId2 = SysExRtDcMasterCoarseTuning;
                        } else if (str_eq(argv[4], "fine-tuning")){
                            msg->Data.SysEx.SubId2 = SysExRtDcMasterFineTuning;
                        } else {
                            return StringifierResultInvalidValue;
                        }

                        msg->Data.SysEx.Data.DeviceControl.Value = atoi((char*)argv[5]);

                        assertU14(msg->Data.SysEx.Data.DeviceControl.Value);

                        return StringifierResultOk;
                    }
                    if (argc < 8){
                        return StringifierResultWrongArgCount;
                    }
                    if ( ! str_eq(argv[4], "global-parameter")){
                        return StringifierResultInvalidValue;
                    }
                    msg->Data.SysEx.SubId2 = SysExRtDcGlobalParameterControl;

                    uint8_t sw = msg->Data.SysEx.Data.DeviceControl.GlobalParameterControl.SlotPathLength = atoi((char*)argv[5]);
                    uint8_t pw = msg->Data.SysEx.Data.DeviceControl.GlobalParameterControl.ParameterIdWidth = atoi((char*)argv[6]);
                    uint8_t vw = msg->Data.SysEx.Data.DeviceControl.GlobalParameterControl.ValueWidth = atoi((char*)argv[7]);

                    assertU7(msg->Data.SysEx.Data.DeviceControl.GlobalParameterControl.SlotPathLength);
                    assertU7(msg->Data.SysEx.Data.DeviceControl.GlobalParameterControl.ParameterIdWidth);
                    assertU7(msg->Data.SysEx.Data.DeviceControl.GlobalParameterControl.ValueWidth);
                    assertBool( (pw == 0 && vw == 0) || (pw > 0 && vw > 0) );

                    // are there as many arguments as specified?
                    if  ( argc - 8 < sw || ((argc - 8 - sw) % 2) != 0 ){
                        return StringifierResultWrongArgCount;
                    }
                    // parameter count
                    uint8_t pc = (argc - 8 - sw) / 2;

                    // so we know the final total data length (2bytes per slot path + (pw+vw)bytes per parameter)
                    msg->Data.SysEx.Data.DeviceControl.GlobalParameterControl.DataLength = 2*sw + pc * (pw + vw);

                    // prepare data container;
                    msg->Data.SysEx.Data.DeviceControl.GlobalParameterControl.Data = msg->Data.SysEx.ByteData;


                    uint8_t ai = 8;

                    for (uint8_t i = 0; i < sw; i++){
                        uint16_t v = atoi((char*)argv[ai++]);

                        assertU14(v);

                        setIthGpcSlot(&msg->Data.SysEx.Data.DeviceControl.GlobalParameterControl, i, v);
                    }

                    for (uint8_t i = 0; i < pc; i++){

                        uint8_t * id = getIthGpcParameterIdAddr( &msg->Data.SysEx.Data.DeviceControl.GlobalParameterControl, i );
                        uint8_t * value = getIthGpcParameterValueAddr( &msg->Data.SysEx.Data.DeviceControl.GlobalParameterControl, i );

                        uint8_t tmp;

                        if (!readHex(id, &tmp, argv[ai++], pw)) {
                            return StringifierResultInvalidHex;
                        }

                        if (!readHex(value, &tmp, argv[ai++], vw)) {
                            return StringifierResultInvalidHex;
                        }
                    }

                    return StringifierResultOk;

                }
                else if (str_eq(argv[3],"cds")) {
                    if (argc < 7){
                        return StringifierResultWrongArgCount;
                    }

                    msg->Data.SysEx.SubId1 = SysExRtControllerDestinationSetting;

                    msg->Data.SysEx.Data.ControllerDestinationSetting.Channel = atoi((char*)argv[4]);

                    assertU4(msg->Data.SysEx.Data.ControllerDestinationSetting.Channel);

                    uint8_t ai;
                    if (str_eq(argv[5], "cc")){
                        msg->Data.SysEx.SubId2 = SysExRtCdsController;
                        msg->Data.SysEx.Data.ControllerDestinationSetting.Controller = atoi((char*)argv[6]);
                        ai = 7;
                    } else if (str_eq(argv[5], "channel-pressure")){
                        msg->Data.SysEx.SubId2 = SysExRtCdsChannelPressure;
                        ai = 6;
                    } else if (str_eq(argv[5], "channel-pressure")){
                       msg->Data.SysEx.SubId2 = SysExRtCdsChannelPressure;
                        ai = 6;
                    } else {
                        return StringifierResultInvalidValue;
                    }

                    if ((argc - ai) % 2 != 0 ){
                        return StringifierResultWrongArgCount;
                    }

                    uint8_t l = 0;
                    while( ai < argc){
                        msg->Data.SysEx.ByteData[l] = atoi((char*)argv[ai++]);

                        assertU7(msg->Data.SysEx.ByteData[l]);

                        l++;
                    }
                    msg->Data.SysEx.Length = l;

                    return StringifierResultOk;
                }

                else if (str_eq(argv[3],"keys")) {
                    if (argc < 6){
                        return StringifierResultWrongArgCount;
                    }

                    msg->Data.SysEx.SubId1 = SysExRtKeybasedInstrumentControl;
                    msg->Data.SysEx.SubId2 = SysExRtKeysBasicMessage;

                    msg->Data.SysEx.Data.KeybasedInstrumentControl.Channel = atoi((char*)argv[4]);
                    msg->Data.SysEx.Data.KeybasedInstrumentControl.Key = atoi((char*)argv[5]);

                    assertU4(msg->Data.SysEx.Data.KeybasedInstrumentControl.Channel);
                    assertU7(msg->Data.SysEx.Data.KeybasedInstrumentControl.Key);

                    assertBool( (argc - 6) % 2 == 0);

                    uint8_t l = 0;
                    for(uint8_t ai = 6; ai < argc; ai++){
                        msg->Data.SysEx.ByteData[l] = atoi( (char*)argv[ai]);

                        assertU7(msg->Data.SysEx.ByteData[l]);

                        l++;
                    }
                    msg->Data.SysEx.Length = l;

                    return StringifierResultOk;
                }
                else if (str_eq(argv[3],"mcc")) {
                    if (argc < 5) {
                        return StringifierResultWrongArgCount;
                    }

                    msg->Data.SysEx.SubId1 = SysExRtMidiMachineControlCommand;

                    // reset data
                    msg->Data.SysEx.Length = 0;

                    return ArgsToMccData( msg->Data.SysEx.ByteData, &msg->Data.SysEx.Length, argc - 4, &argv[4] );

                }
                else {
                    return StringifierResultInvalidValue;
                }

            }
            else if (str_eq(argv[1], "nonrt")) {
                if (argc < 4) {
                    return StringifierResultWrongArgCount;
                }

                msg->Channel = atoi((char*)argv[2]);
                msg->Data.SysEx.Id = SysExIdNonRealTime;

                if (argc == 5) {
                    if (str_eq(argv[3], "eof")) {
                        msg->Data.SysEx.SubId1 = SysExNonRtEndOfFile;
                        msg->Data.SysEx.Data.PacketNumber = atoi((char*)argv[4]);

                        assertU7(msg->Data.SysEx.Data.PacketNumber);

                        return StringifierResultOk;
                    }
                    else if (str_eq(argv[3], "wait")) {
                        msg->Data.SysEx.SubId1 = SysExNonRtWait;
                        msg->Data.SysEx.Data.PacketNumber = atoi((char*)argv[4]);

                        assertU7(msg->Data.SysEx.Data.PacketNumber);

                        return StringifierResultOk;
                    }
                    else if (str_eq(argv[3], "cancel")) {
                        msg->Data.SysEx.SubId1 = SysExNonRtCancel;
                        msg->Data.SysEx.Data.PacketNumber = atoi((char*)argv[4]);

                        assertU7(msg->Data.SysEx.Data.PacketNumber);

                        return StringifierResultOk;
                    }
                    else if (str_eq(argv[3], "nak")) {
                        msg->Data.SysEx.SubId1 = SysExNonRtNAK;
                        msg->Data.SysEx.Data.PacketNumber = atoi((char*)argv[4]);

                        assertU7(msg->Data.SysEx.Data.PacketNumber);

                        return StringifierResultOk;
                    }
                    else if (str_eq(argv[3], "ack")) {
                        msg->Data.SysEx.SubId1 = SysExNonRtACK;
                        msg->Data.SysEx.Data.PacketNumber = atoi((char*)argv[4]);

                        assertU7(msg->Data.SysEx.Data.PacketNumber);

                        return StringifierResultOk;
                    }
                }

                if (str_eq(argv[3], "info")) {
                    if (argc < 5) {
                        return StringifierResultWrongArgCount;
                    }
                    msg->Data.SysEx.SubId1 = SysExNonRtGeneralInformation;

                    if (str_eq(argv[4], "request")) {
                        msg->Data.SysEx.SubId2 = SysExNonRtGenInfoIdentityRequest;

                    } else if (str_eq(argv[3], "reply")) {
                        if (argc != 8) {
                            return StringifierResultWrongArgCount;
                        }
                        msg->Data.SysEx.SubId2 = SysExNonRtGenInfoIdentityReply;
                        msg->Data.SysEx.Data.GeneralInfo.ManufacturerId = strtol((char*)argv[5], NULL, 16);
                        msg->Data.SysEx.Data.GeneralInfo.DeviceFamily = atoi((char*)argv[6]);
                        msg->Data.SysEx.Data.GeneralInfo.DeviceFamilyMember = atoi((char*)argv[7]);

                        if (!readHex(msg->Data.SysEx.Data.GeneralInfo.SoftwareRevision, NULL, argv[8], 4)) {
                            return StringifierResultInvalidHex;
                        }

                        assertU14(msg->Data.SysEx.Data.GeneralInfo.DeviceFamily);
                        assertU14(msg->Data.SysEx.Data.GeneralInfo.DeviceFamilyMember);
                        assertData(msg->Data.SysEx.Data.GeneralInfo.SoftwareRevision, 4);

                    } else {
                        return StringifierResultInvalidValue;
                    }

                    return StringifierResultOk;

                }
                else if (str_eq(argv[3], "gm")) {
                    if (argc != 5) {
                        return StringifierResultWrongArgCount;
                    }
                    msg->Data.SysEx.SubId1 = SysExNonRtGeneralMidi;
                    if (str_eq(argv[4], "system-on1")) {
                        msg->Data.SysEx.SubId2 = SysExNonRtGmSystemOn1;
                    }
                    else if (str_eq(argv[4], "system-off")) {
                        msg->Data.SysEx.SubId2 = SysExNonRtGmSystemOff;
                    }
                    else if (str_eq(argv[4], "system-on2")) {
                        msg->Data.SysEx.SubId2 = SysExNonRtGmSystemOn2;
                    }
                    else {
                        return StringifierResultInvalidValue;
                    }

                    return StringifierResultOk;
                }
                else if (str_eq(argv[3], "cueing")) {
                    if (argc < 5) {
                        return StringifierResultWrongArgCount;
                    }
                    msg->Data.SysEx.SubId1 = SysExNonRtMidiTimeCode;

                    if (str_eq(argv[4], "special")) {
                        if (argc != 6) {
                            return StringifierResultWrongArgCount;
                        }
                        msg->Data.SysEx.SubId2 = SysExNonRtMtcSpecial;

                        if (str_eq(argv[5], "time-code-offset")) {
                            msg->Data.SysEx.Data.Cueing.EventNumber = SysExNonRtMtcSpecialTimeCodeOffset;
                        }
                        else if (str_eq(argv[5], "enable-event-list")) {
                            msg->Data.SysEx.Data.Cueing.EventNumber = SysExNonRtMtcSpecialEnableEventList;
                        }
                        else if (str_eq(argv[5], "disable-event-list")) {
                            msg->Data.SysEx.Data.Cueing.EventNumber = SysExNonRtMtcSpecialDisableEventList;
                        }
                        else if (str_eq(argv[5], "clear-event-list")) {
                            msg->Data.SysEx.Data.Cueing.EventNumber = SysExNonRtMtcSpecialClearEventList;
                        }
                        else if (str_eq(argv[5], "system-stop")) {
                            msg->Data.SysEx.Data.Cueing.EventNumber = SysExNonRtMtcSpecialSystemStop;
                        }
                        else if (str_eq(argv[5], "event-list-request")) {
                            msg->Data.SysEx.Data.Cueing.EventNumber = SysExNonRtMtcSpecialEventListRequest;
                        }
                        else {
                            return StringifierResultInvalidValue;
//                            msg->Data.SysEx.Data.Cueing.EventNumber = atoi((char*)argv[5]);
//
//                            assertU14(msg->Data.SysEx.Data.Cueing.EventNumber);
                        }

                        return StringifierResultOk;
                    }
                    else {
                        if (argc < 13) {
                            return StringifierResultWrongArgCount;
                        }

                        if (str_eq(argv[4], "punch-in")) {
                            if (str_eq(argv[5], "add")) {
                                msg->Data.SysEx.SubId2 = SysExNonRtMtcPunchInPoint;
                            } else if (str_eq(argv[5], "rm")) {
                                msg->Data.SysEx.SubId2 = SysExNonRtMtcDeletePunchInPoint;
                            } else {
                                return StringifierResultInvalidValue;
                            }
                        }
                        else if (str_eq(argv[4], "punch-out")) {
                            if (str_eq(argv[5], "add")) {
                                msg->Data.SysEx.SubId2 = SysExNonRtMtcPunchOutPoint;
                            } else if (str_eq(argv[5], "rm")) {
                                msg->Data.SysEx.SubId2 = SysExNonRtMtcDeletePunchOutPoint;
                            } else {
                                return StringifierResultInvalidValue;
                            }
                        }
                        else if (str_eq(argv[4], "event-start")) {
                            if (str_eq(argv[5], "add")) {
                                if (argc == 13) {
                                    msg->Data.SysEx.SubId2 = SysExNonRtMtcEventStartPoint;
                                } else {
                                    msg->Data.SysEx.SubId2 = SysExNonRtMtcEventStartPointWithInfo;
                                }
                            } else if (str_eq(argv[5], "rm")) {
                                msg->Data.SysEx.SubId2 = SysExNonRtMtcDeleteEventStartPoint;
                            } else {
                                return StringifierResultInvalidValue;
                            }
                        }
                        else if (str_eq(argv[4], "event-stop")) {
                            if (str_eq(argv[5], "add")) {
                                if (argc == 13) {
                                    msg->Data.SysEx.SubId2 = SysExNonRtMtcEventStopPoint;
                                } else {
                                    msg->Data.SysEx.SubId2 = SysExNonRtMtcEventStopPointWithInfo;
                                }
                            } else if (str_eq(argv[5], "rm")) {
                                msg->Data.SysEx.SubId2 = SysExNonRtMtcDeleteEventStopPoint;
                            } else {
                                return StringifierResultInvalidValue;
                            }
                        }
                        else if (str_eq(argv[4], "cue-point")) {
                            if (str_eq(argv[5], "add")) {
                                if (argc == 13) {
                                    msg->Data.SysEx.SubId2 = SysExNonRtMtcCuePoint;
                                } else {
                                    msg->Data.SysEx.SubId2 = SysExNonRtMtcCuePointWithInfo;
                                }
                            } else if (str_eq(argv[5], "rm")) {
                                msg->Data.SysEx.SubId2 = SysExNonRtMtcDeletePunchInPoint;
                            } else {
                                return StringifierResultInvalidValue;
                            }
                        }
                        else if (str_eq(argv[4], "event-name")) {
                            if (str_eq(argv[5], "-")) {
                                if (argc < 14) {
                                    return StringifierResultWrongArgCount;
                                }
                                msg->Data.SysEx.SubId2 = SysExNonRtMtcEventName;
                            } else {
                                return StringifierResultInvalidValue;
                            }
                        }
                        else {
                            return StringifierResultInvalidValue;
                        }


                        if ( ! readMtcLong(&msg->Data.SysEx.Data.Cueing.MidiTimeCode, &argv[6])){
                            return StringifierResultInvalidValue;
                        }

                        msg->Data.SysEx.Data.Cueing.EventNumber = atoi((char*)argv[12]);

                        if (isSysExNonRtMtcWithAddInfo(msg->Data.SysEx.SubId2)) {
                            uint8_t offset = 0;
                            for (uint8_t i = 0; i < argc - 13; i++) {
                                if (i > 0) {
                                    msg->Data.SysEx.ByteData[offset++] = ' ';
                                }
                                strcpy((char *) &msg->Data.SysEx.ByteData[offset], (char*)argv[13 + i]);
                                offset += strlen((char*)argv[13 + i]);

                            }
                            msg->Data.SysEx.Length += strlen((char*)(char *) msg->Data.SysEx.ByteData);
                        }

                        assertU14(msg->Data.SysEx.Data.Cueing.EventNumber);
                        // Additional info does not need be validate (well, expect for length maybe..) as it will be nibblized

                        return StringifierResultOk;
                    }
                }
                else {
                    return StringifierResultInvalidValue;
                }
            } else {
                return StringifierResultInvalidValue;
            }

        } else {
            return StringifierResultInvalidValue;
        }

        return StringifierResultGenericError;
    }


    int MessagetoString(uint8_t * bytes, Message_t * msg) {

        int length = 0;

        if (msg->StatusClass == StatusClassNoteOn){
            length += sprintf( (char*)&bytes[length], "note on %d %d %d", msg->Channel, msg->Data.Note.Key, msg->Data.Note.Velocity);
        }
        if (msg->StatusClass == StatusClassNoteOff){
            length += sprintf( (char*)&bytes[length], "note off %d %d %d", msg->Channel, msg->Data.Note.Key, msg->Data.Note.Velocity);
        }
        if (msg->StatusClass == StatusClassControlChange){
            length += sprintf( (char*)&bytes[length], "cc %d %d %d", msg->Channel, msg->Data.ControlChange.Controller, msg->Data.ControlChange.Value);
        }
        if (msg->StatusClass == StatusClassProgramChange){
            length += sprintf( (char*)&bytes[length], "pc %d %d", msg->Channel, msg->Data.ProgramChange.Program);
        }
        if (msg->StatusClass == StatusClassChannelPressure){
            length += sprintf( (char*)&bytes[length], "pressure %d %d", msg->Channel, msg->Data.ChannelPressure.Pressure);
        }
        if (msg->StatusClass == StatusClassPitchBendChange){
            length += sprintf( (char*)&bytes[length], "pitch %d %d", msg->Channel, msg->Data.PitchBendChange.Pitch);
        }
        if (msg->StatusClass == StatusClassPolyphonicKeyPressure){
            length += sprintf( (char*)&bytes[length], "poly %d %d %d", msg->Channel, msg->Data.PolyphonicKeyPressure.Key, msg->Data.PolyphonicKeyPressure.Pressure);
        }

        if (msg->StatusClass == StatusClassSystemMessage){

            if (msg->SystemMessage == SystemMessageMtcQuarterFrame){
                length += sprintf( (char*)&bytes[length], "quarter-frame %d %d", msg->Data.MtcQuarterFrame.MessageType, msg->Data.MtcQuarterFrame.Nibble);
            }
            if (msg->SystemMessage == SystemMessageSongPositionPointer){
                length += sprintf( (char*)&bytes[length], "song-position %d", msg->Data.SongPositionPointer.Position);
            }
            if (msg->SystemMessage == SystemMessageSongSelect){
                length += sprintf( (char*)&bytes[length], "song-select %d", msg->Data.SongSelect.Song);
            }
            if (msg->SystemMessage == SystemMessageStart){
                length += sprintf( (char*)&bytes[length], "start");
            }
            if (msg->SystemMessage == SystemMessageStop){
                length += sprintf( (char*)&bytes[length], "stop");
            }
            if (msg->SystemMessage == SystemMessageContinue){
                length += sprintf( (char*)&bytes[length], "continue");
            }
            if (msg->SystemMessage == SystemMessageActiveSensing){
                length += sprintf( (char*)&bytes[length], "active-sensing");
            }
            if (msg->SystemMessage == SystemMessageReset){
                length += sprintf( (char*)&bytes[length], "reset");
            }
            if (msg->SystemMessage == SystemMessageTimingClock){
                length += sprintf( (char*)&bytes[length], "timing-clock");
            }
            if (msg->SystemMessage == SystemMessageTuneRequest){
                length += sprintf( (char*)&bytes[length], "tune-request");
            }
            if (msg->SystemMessage == SystemMessageSystemExclusive){
                length += sprintf( (char*)&bytes[length], "sysex ");

//                printf("sysex ");

                if (msg->Data.SysEx.Id == SysExIdExperimental){
                    length += sprintf( (char*)&bytes[length], "experimental ");

                    length += sprintfHex( &bytes[length],  msg->Data.SysEx.ByteData,  msg->Data.SysEx.Length);
                }

                if (isSysExManufacturerId(msg->Data.SysEx.Id)){
                    length += sprintf( (char*)&bytes[length], "manufacturer %06X ", msg->Data.SysEx.Id);

                    length += sprintfHex( &bytes[length],  msg->Data.SysEx.ByteData,  msg->Data.SysEx.Length);
                }

                if (msg->Data.SysEx.Id == SysExIdRealTime){
                    length += sprintf( (char*)&bytes[length], "rt %d ", msg->Channel);

//                    printf("rt %d ", msg->Channel);

                    if (msg->Data.SysEx.SubId1 == SysExRtMidiTimeCode){
                        length += sprintf( (char*)&bytes[length], "mtc ");

                        if (msg->Data.SysEx.SubId2 == SysExRtMtcFullMessage){

                            length += sprintf( (char*)&bytes[length], "full-message ");

                            length += sprintfMtc( &bytes[length], &msg->Data.SysEx.Data.MidiTimeCode );

                        } else if (msg->Data.SysEx.SubId2 == SysExRtMtcUserBits){
                            length += sprintf( (char*)&bytes[length], "user-bits %02X%02X%02X%02X%02X",
                                   msg->Data.SysEx.ByteData[0],
                                   msg->Data.SysEx.ByteData[1],
                                   msg->Data.SysEx.ByteData[2],
                                   msg->Data.SysEx.ByteData[3],
                                   msg->Data.SysEx.ByteData[4]
                            );
                        }
                    }
                    else if (msg->Data.SysEx.SubId1 == SysExRtMidiTimeCodeCueing){
                        length += sprintf( (char*)&bytes[length], "cueing ");
                        if (msg->Data.SysEx.SubId2 == SysExRtMtcCueingSpecial){
                            length += sprintf( (char*)&bytes[length], "special ");
                            if (msg->Data.SysEx.Data.Cueing.EventNumber == SysExRtMtcCueingSpecialSystemStop){
                                length += sprintf( (char*)&bytes[length], "system-stop");
                            }
                            else {
                                length += sprintf( (char*)&bytes[length], "%d", msg->Data.SysEx.Data.Cueing.EventNumber);
                            }
                        }
                        else {

                            switch (msg->Data.SysEx.SubId2){
                                case SysExRtMtcCueingPunchInPoint:                 length += sprintf( (char*)&bytes[length], "punch-in "); break;
                                case SysExRtMtcCueingPunchOutPoint:                length += sprintf( (char*)&bytes[length], "punch-out "); break;
                                case SysExRtMtcCueingEventStartPoint:
                                case SysExRtMtcCueingEventStartPointWithInfo:     length += sprintf( (char*)&bytes[length], "event-start "); break;
                                case SysExRtMtcCueingEventStopPoint:
                                case SysExRtMtcCueingEventStopPointWithInfo:      length += sprintf( (char*)&bytes[length], "event-stop "); break;
                                case SysExRtMtcCueingCuePoint:
                                case SysExRtMtcCueingCuePointWithInfo:            length += sprintf( (char*)&bytes[length], "cue-point "); break;
                                case SysExRtMtcCueingEventName:                    length += sprintf( (char*)&bytes[length], "event-name "); break;
                                default:
                                    return 0;
                            }

                            length += sprintf( (char*)&bytes[length], "%d",msg->Data.SysEx.Data.Cueing.EventNumber);

                            if (msg->Data.SysEx.Length > 0){
                                length += sprintf( (char*)&bytes[length], " %s", msg->Data.SysEx.ByteData);
                            }

                            length += sprintf( (char*)&bytes[length], "");

//                        length += sprintf( (char*)&bytes[length], "%d", msg->Data.SysEx.SubId2);
                        }
                    }
                    else if (msg->Data.SysEx.SubId1 == SysExRtMidiShowControl){
                        length += sprintf( (char*)&bytes[length], "msc ");



                        switch(msg->Data.SysEx.Data.MidiShowControl.CommandFormat.Bytes[0]) {

                            case SysExRtMscCmdFmtLighting:                  length += sprintf((char *) &bytes[length], "lighting "); break;
                            case SysExRtMscCmdFmtMovingLights:              length += sprintf((char *) &bytes[length], "moving-lights "); break;
                            case SysExRtMscCmdFmtColorChangers:             length += sprintf((char *) &bytes[length], "color-changers "); break;
                            case SysExRtMscCmdFmtStrobes:                   length += sprintf((char *) &bytes[length], "strobes "); break;
                            case SysExRtMscCmdFmtLasers:                    length += sprintf((char *) &bytes[length], "lasers "); break;
                            case SysExRtMscCmdFmtChasers:                   length += sprintf((char *) &bytes[length], "chasers "); break;
                            case SysExRtMscCmdFmtSound:                     length += sprintf((char *) &bytes[length], "sound "); break;
                            case SysExRtMscCmdFmtMusic:                     length += sprintf((char *) &bytes[length], "music "); break;
                            case SysExRtMscCmdFmtCdPlayers:                 length += sprintf((char *) &bytes[length], "cd-players "); break;
                            case SysExRtMscCmdFmtEpromPlayback:             length += sprintf((char *) &bytes[length], "eprom-playback "); break;
                            case SysExRtMscCmdFmtAudioTapeMachines:         length += sprintf((char *) &bytes[length], "audio-tape-machines "); break;
                            case SysExRtMscCmdFmtIntercoms:                 length += sprintf((char *) &bytes[length], "intercoms "); break;
                            case SysExRtMscCmdFmtAmplifiers:                length += sprintf((char *) &bytes[length], "amplifiers "); break;
                            case SysExRtMscCmdFmtAudioEffectsDevices:       length += sprintf((char *) &bytes[length], "audio-fx "); break;
                            case SysExRtMscCmdFmtEqualizers:                length += sprintf((char *) &bytes[length], "equalizers "); break;
                            case SysExRtMscCmdFmtMachinery:                 length += sprintf((char *) &bytes[length], "machinery "); break;
                            case SysExRtMscCmdFmtRigging:                   length += sprintf((char *) &bytes[length], "rigging "); break;
                            case SysExRtMscCmdFmtFlys:                      length += sprintf((char *) &bytes[length], "flys "); break;
                            case SysExRtMscCmdFmtLifts:                     length += sprintf((char *) &bytes[length], "lifts "); break;
                            case SysExRtMscCmdFmtTurntables:                length += sprintf((char *) &bytes[length], "turntables "); break;
                            case SysExRtMscCmdFmtTrusses:                   length += sprintf((char *) &bytes[length], "trusses "); break;
                            case SysExRtMscCmdFmtRobots:                    length += sprintf((char *) &bytes[length], "robots "); break;
                            case SysExRtMscCmdFmtAnimation:                 length += sprintf((char *) &bytes[length], "animation "); break;
                            case SysExRtMscCmdFmtFloats:                    length += sprintf((char *) &bytes[length], "floats "); break;
                            case SysExRtMscCmdFmtBreakaways:                length += sprintf((char *) &bytes[length], "breakaways "); break;
                            case SysExRtMscCmdFmtBarges:                    length += sprintf((char *) &bytes[length], "barges "); break;
                            case SysExRtMscCmdFmtVideo:                     length += sprintf((char *) &bytes[length], "video "); break;
                            case SysExRtMscCmdFmtVideoTapeMachines:         length += sprintf((char *) &bytes[length], "video-tape-machines "); break;
                            case SysExRtMscCmdFmtVideoCassetteMachines:     length += sprintf((char *) &bytes[length], "video-cassette-machines "); break;
                            case SysExRtMscCmdFmtVideoDiscPlayers:          length += sprintf((char *) &bytes[length], "video-disc-players "); break;
                            case SysExRtMscCmdFmtVideoSwitchers:            length += sprintf((char *) &bytes[length], "video-switchers "); break;
                            case SysExRtMscCmdFmtVideoEffects:              length += sprintf((char *) &bytes[length], "video-fx "); break;
                            case SysExRtMscCmdFmtVideoCharacterGenerators:  length += sprintf((char *) &bytes[length], "video-char-generators "); break;
                            case SysExRtMscCmdFmtVideoStillStores:          length += sprintf((char *) &bytes[length], "video-still-stores "); break;
                            case SysExRtMscCmdFmtVideoMonitors:             length += sprintf((char *) &bytes[length], "video-monitors "); break;
                            case SysExRtMscCmdFmtProjection:                length += sprintf((char *) &bytes[length], "projection "); break;
                            case SysExRtMscCmdFmtFilmProjectors:            length += sprintf((char *) &bytes[length], "film-projects "); break;
                            case SysExRtMscCmdFmtSlideProjectors:           length += sprintf((char *) &bytes[length], "slide-projectors "); break;
                            case SysExRtMscCmdFmtVideoProjectors:           length += sprintf((char *) &bytes[length], "video-projectors "); break;
                            case SysExRtMscCmdFmtDissolvers:                length += sprintf((char *) &bytes[length], "dissolvers "); break;
                            case SysExRtMscCmdFmtShutterControls:           length += sprintf((char *) &bytes[length], "shutter-controls "); break;
                            case SysExRtMscCmdFmtProcessControl:            length += sprintf((char *) &bytes[length], "process-control "); break;
                            case SysExRtMscCmdFmtHydraulicOil:              length += sprintf((char *) &bytes[length], "hydraulic-oil "); break;
                            case SysExRtMscCmdFmtH2O:                       length += sprintf((char *) &bytes[length], "h2o "); break;
                            case SysExRtMscCmdFmtCO2:                       length += sprintf((char *) &bytes[length], "co2 "); break;
                            case SysExRtMscCmdFmtCompressedAir:             length += sprintf((char *) &bytes[length], "compressed-air "); break;
                            case SysExRtMscCmdFmtNaturalGas:                length += sprintf((char *) &bytes[length], "natural-gas "); break;
                            case SysExRtMscCmdFmtFog:                       length += sprintf((char *) &bytes[length], "fog "); break;
                            case SysExRtMscCmdFmtSmoke:                     length += sprintf((char *) &bytes[length], "smoke "); break;
                            case SysExRtMscCmdFmtCrackedHaze:               length += sprintf((char *) &bytes[length], "cracked-haze "); break;
                            case SysExRtMscCmdFmtPyro:                      length += sprintf((char *) &bytes[length], "pyro "); break;
                            case SysExRtMscCmdFmtFireworks:                 length += sprintf((char *) &bytes[length], "fireworks "); break;
                            case SysExRtMscCmdFmtExplosions:                length += sprintf((char *) &bytes[length], "explosions "); break;
                            case SysExRtMscCmdFmtFlame:                     length += sprintf((char *) &bytes[length], "flame "); break;
                            case SysExRtMscCmdFmtSmokePots:                 length += sprintf((char *) &bytes[length], "smoke-pots "); break;
                            case SysExRtMscCmdFmtAllTypes:                  length += sprintf((char *) &bytes[length], "all "); break;
                        }



                        switch(msg->Data.SysEx.Data.MidiShowControl.Command.Bytes[0]){

                            case SysExRtMscCmdGo:               length += sprintf( (char*)&bytes[length], "go "); break;
                            case SysExRtMscCmdStop:             length += sprintf( (char*)&bytes[length], "stop "); break;
                            case SysExRtMscCmdResume:           length += sprintf( (char*)&bytes[length], "resume "); break;
                            case SysExRtMscCmdLoad:             length += sprintf( (char*)&bytes[length], "load "); break;
                            case SysExRtMscCmdGoOff:            length += sprintf( (char*)&bytes[length], "go-off "); break;
                            case SysExRtMscCmdGo_JamLock:       length += sprintf( (char*)&bytes[length], "go-jam-lock "); break;
                            case SysExRtMscCmdTimedGo:          length += sprintf( (char*)&bytes[length], "timed-go "); break;
                            case SysExRtMscCmdSet:              length += sprintf( (char*)&bytes[length], "set "); break;
                            case SysExRtMscCmdFire:             length += sprintf( (char*)&bytes[length], "fire "); break;
                            case SysExRtMscCmdStandbyPlus:      length += sprintf( (char*)&bytes[length], "standby+ "); break;
                            case SysExRtMscCmdStandbyMinus:     length += sprintf( (char*)&bytes[length], "standby- "); break;
                            case SysExRtMscCmdSequencePlus:     length += sprintf( (char*)&bytes[length], "sequence+ "); break;
                            case SysExRtMscCmdSequenceMinus:    length += sprintf( (char*)&bytes[length], "sequence- "); break;
                            case SysExRtMscCmdStartClock:       length += sprintf( (char*)&bytes[length], "start-clock "); break;
                            case SysExRtMscCmdStopClock:        length += sprintf( (char*)&bytes[length], "stop-clock "); break;
                            case SysExRtMscCmdZeroClock:        length += sprintf( (char*)&bytes[length], "zero-clock "); break;
                            case SysExRtMscCmdMtcChaseOn:       length += sprintf( (char*)&bytes[length], "mtc-chase-on "); break;
                            case SysExRtMscCmdMtcChaseOff:      length += sprintf( (char*)&bytes[length], "mtc-chase-off "); break;
                            case SysExRtMscCmdOpenCueList:      length += sprintf( (char*)&bytes[length], "open-cue-list "); break;
                            case SysExRtMscCmdCloseCueList:     length += sprintf( (char*)&bytes[length], "close-cue-list "); break;
                            case SysExRtMscCmdOpenCuePath:      length += sprintf( (char*)&bytes[length], "open-cue-path "); break;
                            case SysExRtMscCmdCloseCuePath:     length += sprintf( (char*)&bytes[length], "close-cue-path "); break;
                            case SysExRtMscCmdSetClock:         length += sprintf( (char*)&bytes[length], "set-clock "); break;
                            case SysExRtMscCmdStandby:          length += sprintf( (char*)&bytes[length], "standby "); break;
                            case SysExRtMscCmdGo2Pc:            length += sprintf( (char*)&bytes[length], "go-2-pc "); break;
                            case SysExRtMscCmdStandingBy:       length += sprintf( (char*)&bytes[length], "standing-by "); break;
                            case SysExRtMscCmdComplete:         length += sprintf( (char*)&bytes[length], "complete "); break;
                            case SysExRtMscCmdCancel:           length += sprintf( (char*)&bytes[length], "cancel "); break;
                            case SysExRtMscCmdCancelled:        length += sprintf( (char*)&bytes[length], "cancelled "); break;
                            case SysExRtMscCmdAbort:            length += sprintf( (char*)&bytes[length], "abort "); break;
                            case SysExRtMscCmdAllOff:           length += sprintf( (char*)&bytes[length], "all-off"); break;
                            case SysExRtMscCmdRestore:          length += sprintf( (char*)&bytes[length], "restore"); break;
                            case SysExRtMscCmdReset:            length += sprintf( (char*)&bytes[length], "reset"); break;
                        }

                        switch(msg->Data.SysEx.Data.MidiShowControl.Command.Bytes[0]){

                            case SysExRtMscCmdGo:
                            case SysExRtMscCmdStop:
                            case SysExRtMscCmdResume:
                            case SysExRtMscCmdLoad:

                            case SysExRtMscCmdGoOff:
                            case SysExRtMscCmdGo_JamLock:
                                length += sprintfCueNumber( &bytes[length], &msg->Data.SysEx.Data.MidiShowControl.CueNumber );
                                break;

                            case SysExRtMscCmdTimedGo:
                                length += sprintfMtcLong( &bytes[length], &msg->Data.SysEx.Data.MidiShowControl.MidiTimeCode);
                                length += sprintf( (char*)&bytes[length], " " );
                                length += sprintfCueNumber( &bytes[length], &msg->Data.SysEx.Data.MidiShowControl.CueNumber );
                                break;

                            case SysExRtMscCmdSet:
                                length += sprintf( (char*)&bytes[length], "%d %d ", msg->Data.SysEx.Data.MidiShowControl.Controller, msg->Data.SysEx.Data.MidiShowControl.Value);
                                length += sprintfMtcLong(&bytes[length], &msg->Data.SysEx.Data.MidiShowControl.MidiTimeCode);
                                break;

                            case SysExRtMscCmdFire:
                                length += sprintf( (char*)&bytes[length], "%d", msg->Data.SysEx.Data.MidiShowControl.MacroNumber);
                                break;

                            case SysExRtMscCmdStandbyPlus:
                            case SysExRtMscCmdStandbyMinus:
                            case SysExRtMscCmdSequencePlus:
                            case SysExRtMscCmdSequenceMinus:
                            case SysExRtMscCmdStartClock:
                            case SysExRtMscCmdStopClock:
                            case SysExRtMscCmdZeroClock:
                            case SysExRtMscCmdMtcChaseOn:
                            case SysExRtMscCmdMtcChaseOff:
                            case SysExRtMscCmdOpenCueList:
                            case SysExRtMscCmdCloseCueList:
                                length += sprintf( (char*)&bytes[length], "%s", msg->Data.SysEx.Data.MidiShowControl.CueNumber.List);
                                break;

                            case SysExRtMscCmdOpenCuePath:
                            case SysExRtMscCmdCloseCuePath:
                                length += sprintf( (char*)&bytes[length], "%s", msg->Data.SysEx.Data.MidiShowControl.CueNumber.Path);
                                break;

                            case SysExRtMscCmdSetClock:
                                length += sprintfMtcLong( &bytes[length], &msg->Data.SysEx.Data.MidiShowControl.MidiTimeCode);
                                length += sprintf( (char*)&bytes[length], " %s", msg->Data.SysEx.Data.MidiShowControl.CueNumber.List);
                                break;

                            case SysExRtMscCmdStandby:
                            case SysExRtMscCmdGo2Pc:
                                length += sprintf( (char*)&bytes[length], "%d %d %02X%02X%02X%02X ",
                                                         msg->Data.SysEx.Data.MidiShowControl.Checksum,
                                                         msg->Data.SysEx.Data.MidiShowControl.SequenceNumber,
                                                         msg->Data.SysEx.Data.MidiShowControl.Data[0],
                                                         msg->Data.SysEx.Data.MidiShowControl.Data[1],
                                                         msg->Data.SysEx.Data.MidiShowControl.Data[2],
                                                         msg->Data.SysEx.Data.MidiShowControl.Data[3]
                                );
                                length += sprintfCueNumber( &bytes[length], &msg->Data.SysEx.Data.MidiShowControl.CueNumber );
                                break;

                            case SysExRtMscCmdStandingBy:
                                length += sprintf( (char*)&bytes[length], "%d %d ", msg->Data.SysEx.Data.MidiShowControl.Checksum, msg->Data.SysEx.Data.MidiShowControl.SequenceNumber);
                                length += sprintfMtcLong( &bytes[length], &msg->Data.SysEx.Data.MidiShowControl.MidiTimeCode);
                                length += sprintf( (char*)&bytes[length], " " );
                                length += sprintfCueNumber( &bytes[length], &msg->Data.SysEx.Data.MidiShowControl.CueNumber );
                                break;

                            case SysExRtMscCmdComplete:
                            case SysExRtMscCmdCancel:
                                length += sprintf( (char*)&bytes[length], "%d %d ", msg->Data.SysEx.Data.MidiShowControl.Checksum, msg->Data.SysEx.Data.MidiShowControl.SequenceNumber);
                                length += sprintfCueNumber( &bytes[length], &msg->Data.SysEx.Data.MidiShowControl.CueNumber );
                                break;

                            case SysExRtMscCmdCancelled:
                            case SysExRtMscCmdAbort:
                                length += sprintf( (char*)&bytes[length], "%d %d %d ", msg->Data.SysEx.Data.MidiShowControl.Checksum, msg->Data.SysEx.Data.MidiShowControl.Status, msg->Data.SysEx.Data.MidiShowControl.SequenceNumber);
                                length += sprintfCueNumber( &bytes[length], &msg->Data.SysEx.Data.MidiShowControl.CueNumber );
                                break;


                            default:
                            case SysExRtMscCmdAllOff:
                            case SysExRtMscCmdRestore:
                            case SysExRtMscCmdReset:
                                break;

                        }
                    }
                    else if (msg->Data.SysEx.SubId1 == SysExRtDeviceControl){
                        if (msg->Data.SysEx.SubId2 == SysExRtDcMasterVolume){
                            length += sprintf( (char*)&bytes[length], "master-volume %d", msg->Data.SysEx.Data.DeviceControl.Value);
                        }
                        else if (msg->Data.SysEx.SubId2 == SysExRtDcMasterBalance){
                            length += sprintf( (char*)&bytes[length], "master-balance %d", msg->Data.SysEx.Data.DeviceControl.Value);
                        }
                        else if (msg->Data.SysEx.SubId2 == SysExRtDcMasterCoarseTuning){
                            length += sprintf( (char*)&bytes[length], "coarse-tuning %d", msg->Data.SysEx.Data.DeviceControl.Value);
                        }
                        else if (msg->Data.SysEx.SubId2 == SysExRtDcMasterFineTuning){
                            length += sprintf( (char*)&bytes[length], "fine-tuning %d", msg->Data.SysEx.Data.DeviceControl.Value);
                        }
                        else if (msg->Data.SysEx.SubId2 == SysExRtDcGlobalParameterControl){


                            uint8_t sw = msg->Data.SysEx.Data.DeviceControl.GlobalParameterControl.SlotPathLength;
                            uint8_t pw = msg->Data.SysEx.Data.DeviceControl.GlobalParameterControl.ParameterIdWidth;
                            uint8_t vw = msg->Data.SysEx.Data.DeviceControl.GlobalParameterControl.ValueWidth;

                            length += sprintf( (char*)&bytes[length], "global-parameter-control %d %d %d", sw, pw, vw);

//                            printf("%s :: datalength = %d",
//                                   bytes,
//                                   msg->Data.SysEx.Data.DeviceControl.GlobalParameterControl.DataLength
//                            );
//                            fflush(stdout);

                            for(uint8_t i = 0; i < sw; i++){
                                length += sprintf( (char*)&bytes[length], " %d", getIthGpcSlot(&msg->Data.SysEx.Data.DeviceControl.GlobalParameterControl, i) );
                            }

                            uint8_t pc = (pw + vw) == 0 ? 0 : ((msg->Data.SysEx.Data.DeviceControl.GlobalParameterControl.DataLength - 2*sw) / (pw + vw));

                            for (uint8_t i = 0; i < pc; i++){
                                uint8_t * id = getIthGpcParameterIdAddr(&msg->Data.SysEx.Data.DeviceControl.GlobalParameterControl, i);
                                uint8_t * value = getIthGpcParameterValueAddr(&msg->Data.SysEx.Data.DeviceControl.GlobalParameterControl, i);

                                bytes[length++] = ' ';
                                length += sprintfHex( &bytes[length], id, pw );
                                bytes[length++] = ' ';
                                length += sprintfHex( &bytes[length], value, vw );
                            }
                        }
                    }
                    else if (msg->Data.SysEx.SubId1 == SysExRtControllerDestinationSetting){

                        length += sprintf( (char*)&bytes[length], "cds %d ", msg->Data.SysEx.Data.ControllerDestinationSetting.Channel);

                        if (msg->Data.SysEx.SubId2 == SysExRtCdsController){
                            length += sprintf( (char*)&bytes[length], "cc %d", msg->Data.SysEx.Data.ControllerDestinationSetting.Controller);
                        } else if (msg->Data.SysEx.SubId2 == SysExRtCdsChannelPressure) {
                            length += sprintf( (char*)&bytes[length], "channel-pressure" );
                        } else if (msg->Data.SysEx.SubId2 == SysExRtCdsPolyphonicKeyPressure) {
                            length += sprintf( (char*)&bytes[length], "key-pressure" );
                        } else {
                            return 0;
                        }

                        for(uint8_t i = 0; i < msg->Data.SysEx.Length; i++){
                            length += sprintf( (char*)&bytes[length], " %d", msg->Data.SysEx.ByteData[i]);
                        }
                    }
                    else if (msg->Data.SysEx.SubId1 == SysExRtKeybasedInstrumentControl){

                        length += sprintf( (char*)&bytes[length], "keys %d %d", msg->Data.SysEx.Data.KeybasedInstrumentControl.Channel, msg->Data.SysEx.Data.KeybasedInstrumentControl.Key);

                        for(uint8_t i = 0; i < msg->Data.SysEx.Length; i++){
                            length += sprintf( (char*)&bytes[length], " %d", msg->Data.SysEx.ByteData[i]);
                        }

                    }
                    else if (msg->Data.SysEx.SubId1 == SysExRtMidiMachineControlCommand){

                        length += sprintf( (char*)&bytes[length], "mcc ");

                        int l = MccDataToString( &bytes[length], msg->Data.SysEx.ByteData, msg->Data.SysEx.Length );

                        if (l <= 0){
                            return 0;
                        } else {
                            length += l;
                        }
                    }
                    else if (msg->Data.SysEx.SubId1 == SysExRtMidiMachineControlResponse){
                        length += sprintf( (char*)&bytes[length], "mcr ");
                    }

                }


                if (msg->Data.SysEx.Id == SysExIdNonRealTime){
                    length += sprintf( (char*)&bytes[length], "nonrt %d ", msg->Channel);


                    if (msg->Data.SysEx.SubId1 == SysExNonRtEndOfFile){
                        length += sprintf( (char*)&bytes[length], "eof %d", msg->Data.SysEx.Data.PacketNumber);
                    } else if (msg->Data.SysEx.SubId1 == SysExNonRtWait){
                        length += sprintf( (char*)&bytes[length], "wait %d", msg->Data.SysEx.Data.PacketNumber);
                    } else if (msg->Data.SysEx.SubId1 == SysExNonRtCancel){
                        length += sprintf( (char*)&bytes[length], "cancel %d", msg->Data.SysEx.Data.PacketNumber);
                    } else if (msg->Data.SysEx.SubId1 == SysExNonRtNAK){
                        length += sprintf( (char*)&bytes[length], "nak %d", msg->Data.SysEx.Data.PacketNumber);
                    } else if (msg->Data.SysEx.SubId1 == SysExNonRtACK){
                        length += sprintf( (char*)&bytes[length], "ack %d", msg->Data.SysEx.Data.PacketNumber);
                    }
                    else if (msg->Data.SysEx.SubId1 == SysExNonRtGeneralMidi){
                        length += sprintf( (char*)&bytes[length], "gm ");
                        if (msg->Data.SysEx.SubId2 == SysExNonRtGmSystemOn1){
                            length += sprintf( (char*)&bytes[length], "system-on1");
                        }
                        if (msg->Data.SysEx.SubId2 == SysExNonRtGmSystemOff){
                            length += sprintf( (char*)&bytes[length], "system-off");
                        }
                        if (msg->Data.SysEx.SubId2 == SysExNonRtGmSystemOn2){
                            length += sprintf( (char*)&bytes[length], "system-on2");
                        }
                    }
                    else if (msg->Data.SysEx.SubId1 == SysExNonRtMidiTimeCode){
                        length += sprintf( (char*)&bytes[length], "cueing ");
                        if (msg->Data.SysEx.SubId2 == SysExNonRtMtcSpecial){
                            length += sprintf( (char*)&bytes[length], "special ");
                            if (msg->Data.SysEx.Data.Cueing.EventNumber == SysExNonRtMtcSpecialTimeCodeOffset){
                                length += sprintf( (char*)&bytes[length], "time-code-offset");
                            }
                            else if (msg->Data.SysEx.Data.Cueing.EventNumber == SysExNonRtMtcSpecialEnableEventList){
                                length += sprintf( (char*)&bytes[length], "enable-event-list");
                            }
                            else if (msg->Data.SysEx.Data.Cueing.EventNumber == SysExNonRtMtcSpecialDisableEventList){
                                length += sprintf( (char*)&bytes[length], "disable-event-list");
                            }
                            else if (msg->Data.SysEx.Data.Cueing.EventNumber == SysExNonRtMtcSpecialClearEventList){
                                length += sprintf( (char*)&bytes[length], "clear-event-list");
                            }
                            else if (msg->Data.SysEx.Data.Cueing.EventNumber == SysExNonRtMtcSpecialSystemStop){
                                length += sprintf( (char*)&bytes[length], "system-stop");
                            }
                            else if (msg->Data.SysEx.Data.Cueing.EventNumber == SysExNonRtMtcSpecialEventListRequest){
                                length += sprintf( (char*)&bytes[length], "event-list-request");
                            }
                            else {
                                length += sprintf( (char*)&bytes[length], "%d", msg->Data.SysEx.Data.Cueing.EventNumber);
                            }
                        }
                        else {

                            switch (msg->Data.SysEx.SubId2){
                                case SysExNonRtMtcPunchInPoint:                 length += sprintf( (char*)&bytes[length], "punch-in add "); break;
                                case SysExNonRtMtcDeletePunchInPoint:           length += sprintf( (char*)&bytes[length], "punch-in rm "); break;
                                case SysExNonRtMtcPunchOutPoint:                length += sprintf( (char*)&bytes[length], "punch-out add "); break;
                                case SysExNonRtMtcDeletePunchOutPoint:          length += sprintf( (char*)&bytes[length], "punch-out rm "); break;
                                case SysExNonRtMtcEventStartPoint:
                                case SysExNonRtMtcEventStartPointWithInfo:     length += sprintf( (char*)&bytes[length], "event-start add "); break;
                                case SysExNonRtMtcDeleteEventStartPoint:        length += sprintf( (char*)&bytes[length], "event-start rm "); break;
                                case SysExNonRtMtcEventStopPoint:
                                case SysExNonRtMtcEventStopPointWithInfo:      length += sprintf( (char*)&bytes[length], "event-stop add "); break;
                                case SysExNonRtMtcDeleteEventStopPoint:         length += sprintf( (char*)&bytes[length], "event-stop rm "); break;
                                case SysExNonRtMtcCuePoint:
                                case SysExNonRtMtcCuePointWithInfo:            length += sprintf( (char*)&bytes[length], "cue-point add "); break;
                                case SysExNonRtMtcDeleteCuePoint:               length += sprintf( (char*)&bytes[length], "cue-point rm "); break;
                                case SysExNonRtMtcEventName:                    length += sprintf( (char*)&bytes[length], "event-name "); break;
                                default:
                                    return 0;
                            }

                            uint8_t fps = 0;
                            switch(msg->Data.SysEx.Data.Cueing.MidiTimeCode.Fps){
                                case MtcFrameRate24fps: fps = 24; break;
                                case MtcFrameRate25fps: fps = 25; break;
                                case MtcFrameRate29_97fps: fps = 29; break;
                                case MtcFrameRate30fps: fps = 30; break;
                            }
                            length += sprintf( (char*)&bytes[length], "%d%s %d %d %d %d %d %d",
                                   fps,
                                   fps == 29 ? ".97" : "",
                                   msg->Data.SysEx.Data.MidiTimeCode.Hour,
                                   msg->Data.SysEx.Data.Cueing.MidiTimeCode.Minute,
                                   msg->Data.SysEx.Data.Cueing.MidiTimeCode.Second,
                                   msg->Data.SysEx.Data.Cueing.MidiTimeCode.Frame,
                                   msg->Data.SysEx.Data.Cueing.MidiTimeCode.FractionalFrame,
                                   msg->Data.SysEx.Data.Cueing.EventNumber
                            );

                            if (msg->Data.SysEx.Length > 0){
                                length += sprintf( (char*)&bytes[length], " %s", msg->Data.SysEx.ByteData);
                            }

                            length += sprintf( (char*)&bytes[length], "");

//                        length += sprintf( (char*)&bytes[length], "%d", msg->Data.SysEx.SubId2);
                        }
                    }
                    else if (msg->Data.SysEx.SubId1 == SysExNonRtGeneralInformation){
                        length += sprintf( (char*)&bytes[length], "info ");

                        if (msg->Data.SysEx.SubId2 == SysExNonRtGenInfoIdentityRequest){
                            length += sprintf( (char*)&bytes[length], "request"
                            );
                        } else if (msg->Data.SysEx.SubId2 == SysExNonRtGenInfoIdentityReply) {
                            length += sprintf( (char*)&bytes[length], "reply %06X %d %d %02X%02X%02X%02X",
                                   msg->Data.SysEx.Data.GeneralInfo.ManufacturerId,
                                   msg->Data.SysEx.Data.GeneralInfo.DeviceFamily,
                                   msg->Data.SysEx.Data.GeneralInfo.DeviceFamilyMember,
                                   msg->Data.SysEx.Data.GeneralInfo.SoftwareRevision[0],
                                   msg->Data.SysEx.Data.GeneralInfo.SoftwareRevision[1],
                                   msg->Data.SysEx.Data.GeneralInfo.SoftwareRevision[2],
                                   msg->Data.SysEx.Data.GeneralInfo.SoftwareRevision[3]
                            );
                        }
                    }

                }
            }
        }

        return length;
    }

#ifdef __cplusplus
    } // extern "C"
} // namespace MidiMessage
#endif