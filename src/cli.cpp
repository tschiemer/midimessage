/**
 *
 */

#include <stdio.h>
//#include <cstdarg>
#include <stdlib.h>
//#include <stdlib.h>
//#include <string.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/time.h>
#include <time.h>

#include <util-hex.h>

#include <midimessage.h>
#include <midimessage/stringifier.h>
#include <midimessage/parser.h>
#include <midimessage/commonccs.h>


using namespace std;
using namespace MidiMessage;


///////// Defines

typedef enum {
    ModeUndefined       = 0,
    ModeParse           = 1,
    ModeGenerate        = 2,
    ModeConvert        = 3
} Mode_t;

inline bool isValidMode( Mode_t mode ){
    return (mode == ModeParse ||
            mode == ModeGenerate ||
            mode == ModeConvert
    );
}

typedef enum {
    ProcessNibblize,
    ProcessDeNibblize,
    ProcessSevenbitize,
    ProcessDeSevenbitize
} Process_t;

typedef enum {
    ResolutionMicro = 0,
    ResolutionMilli = 1
} Resolution_t;


///////// Variables

// Run mode
Mode_t mode = ModeUndefined;

// Running status option
bool runningStatusEnabled = false;

bool nrpnFilterEnabled = false;

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


bool printDiscardedData = false;

bool exitOnError = true;

int verbosity = 0;

Process_t process;

bool useHex = false;

///////// Signatures

void printHelp( void );
unsigned long getNow();

void generator(void);
void generatorError(int code, uint8_t argc, uint8_t ** argv);
void writeMidiPacket( Message_t * msg );

void parser(void);
void parsedMessage( Message_t * msg, void * context );
void discardingData( uint8_t * data, uint8_t length, void * context );



void printHelp( void ) {
    printf("Usage:\n");
    printf("\t midimessage-cli [-h?]\n");
    printf("\t midimessage-cli [--running-status|-r] [--timed|-t[milli|micro]] (--parse|-p) [-d] [--nprn-filter]\n");
    printf("\t midimessage-cli [--running-status|-r] [--timed|-t[milli|micro]] (--generate|-g) [-x0|-x1] [-v[N]] [--prefix=<prefix>] [--suffix=<suffix] [<cmd> ...]\n");
    printf("\t midimessage-cli --convert=(nibblize|denibblize|sevenbitize|desevenbitize) [--hex] [<data xN>]\n");

    printf("\nOptions:\n");
    printf("\t -h|-? \t\t\t\t show this help\n");
    printf("\t --running-status|-r \t\t Accept (when parsing) or generate messages that rely on the running status (see MIDI specs)\n");
    printf("\t --timed|-t[milli|micro] \t Enables the capture or playback of delta-time information (ie the time between messages). Optionally the time resolution (milliseconds or microseconds) can be specified (default = micro)\n");
    printf("\t --parse|-p [<binary-data>] \t Enter parse mode and optionally pass as first argument (binary) message to be parsed. If no argument is provided starts reading binary stream from STDIN. Each successfully parsed message will be printed to STDOUT and terminated with a newline.\n");
    printf("\t -d \t\t\t\t In parsing mode only, instead of silent discarding output any discarded data to STDERR.\n");
    printf("\t --nrpn-filter \t\t\t\t In parsing mode only, assume CC-sequences 99-98-96 (increment), 99-98-97 (decrement), 99-98-6-38 (data entry) are NRPN sequences, thus these will be filtered even if impartial (!! ie, 99-98-6-2 will only output the message for 2; this is a convenience feature and can not be solved for the general case) \n");
    printf("\t --generate|-g [<cmd> ...] \t Enter generation mode and optionally pass command to be generated. If no command is given, expects one command from STDIN per line. Generated (binary) messages are written to STDOUT.\n");
    printf("\t --prefix=<prefix> \t\t Prefixes given string (max 32 bytes) before each binary sequence (only when in generation mode). A single %%d can be given which will be replaced with the length of the following binary message (incompatible with running-status mode).\n");
    printf("\t --suffix=<suffix> \t\t Suffixes given string (max 32 bytes) before each binary sequence (only when in generation mode).\n");
    printf("\t -x0, -x1 \t\t\t In generation mode, exit on input error (-x1) or continue processing (-x0). Default := continue (-x0).\n");
    printf("\t -v0, -v1 \t\t\t In generation mode, print command parsing result (on error only) to STDERR. Default := do NOT print (-v0).\n");
    printf("\t --convert=.. \t\t\t Enter convertion mode, ie transform incoming STDIN using convertion method and write to STDOUT (raw bytes).\n");
    printf("\t --hex \t\t\t\t In convertion mode (only), hex input/output\n");

    printf("\nFancy pants note: the parsing output format is identical to the generation command format ;) \n");

    printf("\nData types:\n");
    printf("\t uN := N bit unsigned integer)\n");
    printf("\t\t u4 (data nibble) < 15 (0x0F)\n");
    printf("\t\t u7 <= 127 (0x7F)\n");
    printf("\t\t u14 <= 16383 (0x3FFF)\n");
    printf("\t\t u21 <= 2097151 (0x1FFFFF)\n");
    printf("\t\t u28 <= 268435455 (0x0FFFFFFF)\n");
    printf("\t\t u35 <= 34359738367 (0x7FFFFFFFF)\n");
    printf("\t sN := N bit signed integer\n");
    printf("\t strN ((max) N byte ascii string)\n");
    printf("\t xN (N byte hex string <> 2Ns) (note: data bytes must be <= 0x7F)\n");

    printf("\nVoice Commands:\n");
    printf("\t note (on|off) <channel (u4)> <key (u7)> <velocity (u7)>\n");
    printf("\t cc <channel (u4)> <controller (u7)> <value (u7)>\n");
    printf("\t pc <channel (u4)> <program (u7)>\n");
    printf("\t pressure <channel (u4)> <pressure (u7)>\n");
    printf("\t pitch <channel (u4)> <pitch (u14)>\n");
    printf("\t poly <channel (u4)> <key (u7)> <pressure (u7)>\n");

    printf("\nSystem Commands:\n");
    printf("\t start\n");
    printf("\t stop\n");
    printf("\t continue\n");
    printf("\t reset\n");
    printf("\t active-sensing\n");
    printf("\t tune-request\n");
    printf("\t timing-clock\n");
    printf("\t quarter-frame <messageType (u3)> <nibble (u4)>\n");
    printf("\t song-position <position (u14)>\n");
    printf("\t song-select <songNumber (u7)>\n");

    printf("\n(Basic) System Exclusives:\n");
    printf("\t sysex experimental <data (xN)>\n");
    printf("\t sysex manufacturer <manufacturer-id (x1..3)> <data (xN)>\n");
    printf("\t sysex nonrt <device-id* (u7)> (eof|wait|cancel|nak|ack) <packet-number (u7)>\n");
    printf("* <device-id> := 127 is all devices (broadcast)\n");

    printf("\nGeneral Information\n");
    printf("\t sysex nonrt <device-id (u7)> info request\n");
    printf("\t sysex nonrt <device-id (u7)> info reply <manufacturer-id (x1, x3)> <device-family (u14)> <device-family-member (u14)> <software-revision (x4)>\n");

    printf("\nGeneral MIDI (extension)\n");
    printf("\t sysex nonrt <device-id (u7)> gm (system-on1|system-off|system-on2)\n");

    printf("\nController Destination Setting\n");
    printf("\t sysex nonrt <device-id (u7)> cds <channel (u4)> (channel-pressure|key-pressure)  <parameter1 (u7)> <range1 (u7)> [<parameter2 (u7)> <range2 (u7)> .. <parameterN (u7)> <rangeN (u7)>]\n");
    printf("\t sysex nonrt <device-id (u7)> cds <channel (u4)> cc <controller (u7)> <parameter1 (u7)> <range1 (u7)> [<parameter2 (u7)> <range2 (u7)> .. <parameterN (u7)> <rangeN (u7)>]\n");

    printf("\nKeybased Instrument Control\n");
    printf("\t sysex nonrt <device-id (u7)> keys <channel (u7)> <key (u7)> <controller1 (u7)> <value1 (u7)> [<controller2 (u7)> <value2 (u7)> .. <controllerN (u7)> <valueN (u7)>]\n");

    printf("\nDevice Control\n");
    printf("\t sysex rt <device-id (u7)> dc** (master-volume|master-balance|coarse-tuning|fine-tuning) <value (u14)>\n");
    printf("\t sysex rt <device-id (u7)> dc global-parameter <slot-count (u7)> <parameter-id-width (u7)> <parameter-value-width (u7)> [<slot-path1 (u14)> [.. <slot-pathN (u14)>]] [<parameter-id1 (xN)> <parameter-value1 (xN)> [..  <parameter-idN (xN)> <parameter-valueN (xN)>]]\n");



    printf("\nMIDI Time Code + Cueing\n");
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

    printf("\nMIDI Show Control (MSC)\n");
    printf("\t sysex rt <device-id (u7)> msc <cmdFmt*> (all-off|restore|reset)\n");
    printf("\t sysex rt <device-id (u7)> msc <cmdFmt*> (go|stop|resume|load|go-off|go-jam-lock) <cue-number**> [<cue-list**> [<cue-path**>]]\n");
    printf("\t sysex rt <device-id (u7)> msc <cmdFmt*> timed-go <fps = 24,25,29.97,30> <hour <= 23> <minute <= 59> <second <= 59> <frame < fps> < (x1)> <cue-number**> [<cue-list**> [<cue-path**>]]\n");
    printf("\t sysex rt <device-id (u7)> msc <cmdFmt*> set <controller (u14)> <value (u14)> <fps = 24,25,29.97,30> <hour <= 23> <minute <= 59> <second <= 59> <frame < fps> < (x1)>\n");
    printf("\t sysex rt <device-id (u7)> msc <cmdFmt*> fire <macro-number (u7)>\n");
    printf("\t sysex rt <device-id (u7)> msc <cmdFmt*> (standby+|standby-|sequence+|sequence-|start-clock|stop-clock|zero-clock|mtc-chase-on|mtc-chase-off|open-cue-list|close-cue-list) <cue-list**>\n");
    printf("\t sysex rt <device-id (u7)> msc <cmdFmt*> (open-cue-path|close-cue-path) <cue-path**>\n");
    printf("\t sysex rt <device-id (u7)> msc <cmdFmt*> set-clock <fps = 24,25,29.97,30> <hour <= 23> <minute <= 59> <second <= 59> <frame < fps> < (x1)> <cue-list**>\n");
    printf("\t sysex rt <device-id (u7)> msc <cmdFmt*> (standby|go-2-pc) <checksum (u14)> <sequence-number (u14)> <data (x4)> <cue-number**> [<cue-list**> [<cue-path**>]]\n");
    printf("\t sysex rt <device-id (u7)> msc <cmdFmt*> standing-by <checksum (u14)> <sequence-number (u14)> <fps = 24,25,29.97,30> <hour <= 23> <minute <= 59> <second <= 59> <frame < fps> < (x1)> <cue-number**> [<cue-list**> [<cue-path**>]]\n");
    printf("\t sysex rt <device-id (u7)> msc <cmdFmt*> (cancelled|abort) <checksum (u14)> <status (u16)> <sequence-number (u14)> <data (x4)> <cue-number**> [<cue-list**> [<cue-path**>]]\n");

    printf("* <cmdFmt> := lighting|moving-lights|color-changers|strobes|lasers|chasers|sound|music|cd-players|eprom-playback|audio-tape-machines|intercoms|amplifiers|audio-fx|equalizers|machinery|rigging|flys|lifts|turntables|trusses|robots|animation|floats|breakaways|barges|video|video-tape-machines|video-cassette-machines|video-disc-players|video-switchers|video-fx|video-char-generators|video-still-stores|video-monitors|projection|film-projects|slide-projectors|video-projectors|dissolvers|shutter-controls|process-control|hydraulic-oil|h2o|co2|compressed-air|natural-gas|fog|smoke|cracked-haze|pyro|fireworks|explosions|flame|smoke-pots|all\n");
    printf("** <cue-number>, <cue-list>, <cue-path> := ascii numbers (0-9) and/or dots (.)\n");

    printf("\nMIDI Machine Commands (MMC)\n");
    printf("For MMC the MIDI format acts as container for a command stream of its own, where several MMC commands can be packed into one MIDI message.\n");
    printf("\n\t sysex rt <device-id (u7)> mcc <command1 ..> [<command2 ..> [ .. <commandN ..>] .. ]]\n");
    printf("\t\t <commandN ..> :\n");
    printf("\t\t (stop|play|deferred-play|fast-forward|rewind|record-strobe|record-exit|record-pause|pause|eject|chase|command-error-reset|mmc-reset|wait|resume)\n");
    printf("\t\t (variable-play|search|shuttle|deferred-variable-play|record-strobe-variable) <speed (float)>\n");
    printf("\t\t step <step (s7)>\n");
    printf("\t\t locate (field <field>|mtc <fps = 24,25,29.97,30> <hour <= 23> <minute <= 59> <second <= 59> <frame < fps>)\n");
    printf("\t\t assign-system-master <master (u7)>\n");
    printf("\t TODO:\n");
    printf("\t\t generator-command (stop|run|copy-jam)\n");
    printf("\t\t mtc-command (off|follow)\n");
    printf("\t\t write ..\n");
    printf("\t\t move ..\n");
    printf("\t\t etc\n");

    printf("\nMobile Phone Control\n");
    printf("\t sysex rt <phone-id (u7)> mpc (vibrator|led|display|keypad|all|<manufacturer-id (x1,x3)> <cmd-id (u7)>> <device-index (u7)> <reset|on|off>\n");
    printf("\t sysex rt <phone-id (u7)> mpc (vibrator|led|display|keypad|all|<manufacturer-id (x1,x3)> <cmd-id (u7)>> <device-index (u7)> <manufacturer-cmd (x1, x3)> <data (xN)>\n");
    printf("\t sysex rt <phone-id (u7)> mpc (vibrator|led|display|keypad|all|<manufacturer-id (x1,x3)> <cmd-id (u7)>> <device-index (u7)> follow-midi-channels [<channel1 (u7)> <low-note1 (u7)> <high-note1  (u7)> [ .. [<channelN (u7)> <low-noteN (u7)> <high-noteN (u7)>] .. ]\n");
    printf("\t sysex rt <phone-id (u7)> mpc (vibrator|led|display|keypad|all|<manufacturer-id (x1,x3)> <cmd-id (u7)>> <device-index (u7)> set-color <red (u7)> <green (u7)> <blue (u7)>\n");
    printf("\t sysex rt <phone-id (u7)> mpc (vibrator|led|display|keypad|all|<manufacturer-id (x1,x3)> <cmd-id (u7)>> <device-index (u7)> set-level\n");

    printf("\nSample Dump Standard (SDS)\n");
    printf("\t sysex nonrt <device-id (u7)> sds-header <sample-number (u14)> <sample-fmt (u7)> <sample-period (u21)> <sample-length (u21)> <loop-start (u21)> <loop-end (u21)> (uni-forward|bi-forward|forward-once)\n");
    printf("\t sysex nonrt <device-id (u7)> sds-request <sample-number (u14)>\n");
    printf("\t sysex nonrt <device-id (u7)> sds-data <packet-index (u7)> <data (xN)> [<checksum (x1)> [<verification-checksum (x1)>]]*\n");
    printf("\t sysex nonrt <device-id (u7)> sds-ext loop-point-tx <sample-number (u14)> <loop-number (u14)> (uni-forward|bi-forward|forward-once) <loop-start (u21)> <loop-end (u21)>\n");
    printf("\t sysex nonrt <device-id (u7)> sds-ext loop-point-request <sample-number (u14)> <loop-number (u14)>\n");
    printf("\t sysex nonrt <device-id (u7)> sds-ext ext-header <sample-number (u14)> <sample-fmt (u7)> <sample-rate-integer (u28)> <sample-rate-fraction (u28)> <sample-length (u35)> <loop-start (u35)> <loop-end (u35)> <loop-type**>\n");
    printf("\t sysex nonrt <device-id (u7)> sds-ext ext-loop-point-tx <sample-number (u14)> <loop-number (u14)> <loop-type**> <loop-start (u35)> <loop-end (u35)> \n");
    printf("\t sysex nonrt <device-id (u7)> sds-ext ext-loop-point-request <sample-number (u14)> <loop-number (u14)>\n");
    printf("\t sysex nonrt <device-id (u7)> sds-ext name-tx <sample-number (u14)> -*** <sample-name (strN) ...>\n");
    printf("\t sysex nonrt <device-id (u7)> sds-ext name-request <sample-number (u14)>\n");
    printf("* <checksum> := as sent/to be sent in message, <verification-checksum> := as computed. Both checksums are given when parsing a MIDI stream but during generation, if no <checksum> is given it is computed (recommended) otherwise its value will be used; <verification-checksum> will always be ignored.\n");
    printf("** <loop-type> := uni-forward|bi-forward|uni-forward-release|bi-forward-release|uni-backward|bi-backward|uni-backward-release|bi-backward-release|backward-once|forward-once\n");
    printf("*** In principle there is a language tag to support localization, but apart from the default (English) none are documented and thus likely not used. Thus momentarily only the default is supported which is chosen by specifying '-' as argument.\n");

    printf("\nMIDI Visual Control (MVC)\n");
    printf("\t sysex nonrt <device-id (u7)> mvc (on-off|clip-control-channel|fx-control-channel|note-msg-enabled) <data (xN)>\n");
    printf("\t sysex nonrt <device-id (u7)> mvc (playback-assign-msn|playback-assign-lsn|dissolve-assign-msn|dissolve-assign-lsn) <data (xN)>\n");
    printf("\t sysex nonrt <device-id (u7)> mvc (fx1-assign-msn|fx1-assign-lsn|fx2-assign-msn|fx2-assign-lsn|fx3-assign-msn|fx3-assign-lsn) <data (xN)>\n");
    printf("\t sysex nonrt <device-id (u7)> mvc (playback-speed-range|keyboard-range-lower|keyboard-range-upper) <data (xN)>\n");
    printf("\t sysex nonrt <device-id (u7)> mvc <parameter-address (x3)> <data (xN)>\n");

    printf("\nFile Dump\n");
    printf("\t sysex nonrt <device-id (u7)> file-dump request <sender-id (u7)> <type (str4)*> <name (strN)..>\n");
    printf("\t sysex nonrt <device-id (u7)> file-dump header <sender-id (u7)> <type (str4)*> <length (u28)> <name (strN)..>\n");
    printf("\t sysex nonrt <device-id (u7)> file-dump data <packet-number (u7)> <data (xN)> [<checksum (x1)> [<verification-checksum (x1)>]]**\n");
    printf("*<type> := four 7-bit ASCII bytes. Defined in specification: MIDI, MIEX, ESEQ, TEXT, BIN<space>, MAC<space>\n");
    printf("** <checksum> := as sent/to be sent in message, <verification-checksum> := as computed. Both checksums are given when parsing a MIDI stream but during generation, if no <checksum> is given it is computed (recommended) otherwise its value will be used; <verification-checksum> will always be ignored.\n");

    printf("\nNotation Information\n");
    printf("\t sysex rt <device-id (u7)> notation bar-number (not-running|running-unknown|<s14>)\n");
    printf("\t sysex rt <device-id (u7)> notation time-signature (immediate|delayed) <midi-clock-in-metronome-click (u7)> <32nd-notes-in-midi-quarter-note (u7)> <time-signature-nominator1 (u7)> <time-signature-denominator1 (u7)>  [<nominator2 (u7)> <denominator2 (u7)> ..]\n");

    printf("\nTuning Standard\n");
    printf("\t sysex nonrt <device-id (u7)> tuning bulk-dump-request <program (u7)>\n");
    printf("\t sysex nonrt <device-id (u7)> tuning bulk-dump-request-back <bank (u7)> <program (u7)>\n");
    printf("\t sysex nonrt <device-id (u7)> tuning bulk-dump <program (u7)> <name (str16)> (-|<semitone1 (u7)> <cents1* (u14)>) .. (-|<semitone128 (u7)> <cents128* (u14)>) [<checksum (x1)> [<verification-checksum (x1)]]\n");
    printf("\t sysex nonrt <device-id (u7)> tuning key-based-dump <bank (u7)> <preset (u7)> <name (str16)> (-|<semitone1 (u7)> <cents1* (u14)>) .. (-|<semitone128 (u7)> <cents128* (u14)>) [<checksum (x1)> [<verification-checksum (x1)]]\n");
    printf("\t sysex nonrt <device-id (u7)> tuning single-note-change-back <bank (u7)> <preset (u7)> <key1 (u7)> <semitone1 (u7)> <cents1* (u14)> [..  <keyN (u7)> <semitoneN (u7)> <centsN* (u14)>]\n");
    printf("\t sysex nonrt <device-id (u7)> tuning octave-dump-1byte <bank (u7)> <preset (u7)>  <name (str16)> <cent1** (s7)> .. <cents12** (s7)> ( [<checksum (x1)> [<verification-checksum (x1)]]\n");
    printf("\t sysex nonrt <device-id (u7)> tuning octave-dump-2byte <bank (u7)> <preset (u7)>  <name (str16)> <cent1> .. <cents12 (s7)> ( [<checksum (x1)> [<verification-checksum (x1)]]\n");
    printf("\t sysex rt <device-id (u7)> tuning single-note-change <program (u7)> <key1 (u7)> <semitone1 (u7)> <cents1* (u14)> [..  <keyN (u7)> <semitoneN (u7)> <centsN* (u14)>]\n");
    printf("\t sysex rt <device-id (u7)> tuning single-note-change-bank <bank (u7)> <preset (u7)> <key1 (u7)> <semitone1 (u7)> <cents1* (u14)> [..  <keyN (u7)> <semitoneN (u7)> <centsN* (u14)>]\n");
    printf("\t* in .0061-cent units\n");
    printf("\t** from -64 (:=0) to 63 (:=127, ie 0 := 64)\n");
    printf("\t*** in .012207-cents units\n");


    printf("\nExamples:\n");
    printf("\t bin/midimessage-cli -g note on 60 40 1\n");
    printf("\t bin/midimessage-cli -g sysex experimental 1337\n");
    printf("\t bin/midimessage-cli -g sysex rt 1 msc sound reset\n");
    printf("\t bin/midimessage-cli -g | bin/midimessage-cli -p\n");
    printf("\t bin/midimessage-cli -g --prefix='%%d ' --suffix=$'\\x0a'\n");
    printf("\t bin/midimessage-cli -g | bin/midimessage-cli -ptmilli > test.recording\n");
    printf("\t cat test.recording | bin/midimessage-cli -gtmilli | bin/midimessage-cli -p\n");
    printf("\t bin/midimessage-cli --convert=nibblize --hex 1337 > test.nibblized\n");
    printf("\t cat test.nibblized | bin/midimessage-cli --convert=denibblize --hex\n");
    printf("\t bin/midimessage-cli -v1 -g nrpn 1 128 255 | bin/midimessage-cli -p\n");
    printf("\t bin/midimessage-cli -v1 -g nrpn 1 128 256 | bin/midimessage-cli -p --nrpn-filter\n");
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

void generator(void){
    uint8_t line[255];

    uint8_t sysexBuffer[128];
    Message_t msg;
    msg.Data.SysEx.ByteData = sysexBuffer;

    // start timer
    if (timedOpt.enabled){
        // record timer
        timedOpt.lastTimestamp = getNow();
    }

    while(fgets((char*)line, sizeof(line), stdin ) != NULL){

        uint8_t *args[32];
        uint8_t argsCount = stringToArgs( args, 32, line, strlen((char*)line) );

        if (argsCount == 0){
            continue;
        }


        uint8_t ** firstArg = args;

        // if the next command is delayed, we have to wait and update the arguments correspondingly
        if (timedOpt.enabled){
            unsigned long delay = strtoul((char*)args[0], NULL, 10);
            unsigned long waitUntil = timedOpt.lastTimestamp + delay;
            unsigned long now;
            do {
                now = getNow();
            } while( now < waitUntil );

            timedOpt.lastTimestamp = now;

            argsCount--;
            firstArg = &firstArg[1];
        }


        if (strcmp((char*)firstArg[0], "nrpn") == 0){

            if (argsCount != 4) {
                generatorError(StringifierResultWrongArgCount, argsCount, firstArg);
                continue;
            }
            msg.StatusClass = StatusClassControlChange;
            msg.Channel = atoi((char*)firstArg[1]);

            if (msg.Channel > MaxU7) {
                generatorError(StringifierResultInvalidU7, argsCount, firstArg);
                continue;
            }

            uint16_t controller = atoi((char*)firstArg[2]);
            uint16_t value = atoi((char*)firstArg[3]);

            if (controller > MaxU14) {
                generatorError(StringifierResultInvalidU14, argsCount, firstArg);
                continue;
            }
            if (value > MaxU14) {
                generatorError(StringifierResultInvalidU14, argsCount, firstArg);
                continue;
            }

            msg.Data.ControlChange.Controller = CcNonRegisteredParameterMSB;
            msg.Data.ControlChange.Value = (controller >> 7) & DataMask;
            writeMidiPacket(&msg);

            msg.Data.ControlChange.Controller = CcNonRegisteredParameterLSB;
            msg.Data.ControlChange.Value = controller & DataMask;
            writeMidiPacket(&msg);

            msg.Data.ControlChange.Controller = CcDataEntryMSB;
            msg.Data.ControlChange.Value = (value >> 7) & DataMask;
            writeMidiPacket(&msg);

            msg.Data.ControlChange.Controller = CcDataEntryLSB;
            msg.Data.ControlChange.Value = value & DataMask;
            writeMidiPacket(&msg);

        } else {
            int result = MessagefromArgs( &msg,  argsCount, firstArg );

            if (StringifierResultOk == result){

                writeMidiPacket( &msg );
            } else {
                generatorError(result, argsCount, firstArg);
            }
        }
    }
}

void generatorError(int code, uint8_t argc, uint8_t ** argv){

    if (verbosity > 0) {
        switch (code) {
            case StringifierResultGenericError:
                fprintf(stderr, "Generic error: ");
                break;
            case StringifierResultInvalidValue:
                fprintf(stderr, "Invalid value: ");
                break;
            case StringifierResultWrongArgCount:
                fprintf(stderr, "Wrong arg count: ");
                break;
            case StringifierResultNoInput:
                fprintf(stderr, "No input ");
                break;
            case StringifierResultInvalidU4:
                fprintf(stderr, "Invalid U4/Nibble Value ");
                break;
            case StringifierResultInvalidU7:
                fprintf(stderr, "Invalid U7 Value ");
                break;
            case StringifierResultInvalidU14:
                fprintf(stderr, "Invalid U14 Value ");
                break;
            case StringifierResultInvalidU21:
                fprintf(stderr, "Invalid U21 Value ");
                break;
            case StringifierResultInvalidU28:
                fprintf(stderr, "Invalid U28 Value ");
                break;
            case StringifierResultInvalidU35:
                fprintf(stderr, "Invalid U35 Value ");
                break;
            case StringifierResultInvalidHex:
                fprintf(stderr, "Invalid Hex Value ");
                break;
        }

        for (uint8_t i = 0; i < argc; i++) {
            fprintf(stderr, "%s ", argv[i]);
        }
        fprintf(stderr, "\n");
    }
    if (exitOnError){
        exit(EXIT_FAILURE);
    }
}

void writeMidiPacket( Message_t * msg ){

    static uint8_t runningStatusState = MidiMessage_RunningStatusNotSet;

    uint8_t bytes[255];
    uint8_t length = pack( bytes, msg );

    if (length == 0){
        return;
    }

    printf(prefix, length);

    if (runningStatusEnabled && updateRunningStatus( &runningStatusState, bytes[0] )){
        fwrite( &bytes[1], 1, length-1, stdout);
    } else {
        fwrite(bytes, 1, length, stdout);
    }

    printf("%s", suffix);

    fflush(stdout);
}

void parser(void){

    uint8_t sysexBuffer[128];
    Message_t msg;
    msg.Data.SysEx.ByteData = sysexBuffer;

    uint8_t dataBuffer[255];
    Parser_t parser;
    parser_init(&parser, runningStatusEnabled, dataBuffer, 255, &msg, parsedMessage, discardingData, NULL );

    // start timer
    if (timedOpt.enabled){
        // record timer
        timedOpt.lastTimestamp = getNow();
    }


    int ch;
    while( (ch = fgetc(stdin)) != EOF ){
        uint8_t byte = (uint8_t)ch;

        parser_receivedData(&parser, &byte, 1 );
    }
}



typedef int (*Reader_t)(void);
typedef uint8_t(*Converter_t)(uint8_t *,uint8_t *, uint8_t);

void convert(Reader_t reader, Converter_t converter, uint8_t each){

    uint8_t src[16];
    uint8_t dst[16];
    uint8_t srcl = 0;

    void (*output)(uint8_t *,uint8_t) = [](uint8_t * data, uint8_t len){

        uint8_t buf[32];

        if (useHex){
            byte_to_hex(buf, data, len);
            fwrite( buf, 1, 2*len, stdout );
        } else {
            fwrite( data, 1, len, stdout );
        }
    };

    int ch;
    while( (ch = reader()) != EOF ){
        uint8_t byte = (uint8_t)ch;

        if (useHex){
            static uint8_t nibble = 0;
            static uint8_t nibbles[2];

            nibbles[nibble++] = byte;

            if (nibble >= 2){
                if ( ! hex_to_byte(&src[srcl], nibbles, 1) ){
                    fprintf(stderr, "Error: invalid hex input!\n");
                }
                srcl++;
                nibble = 0;
            }
        } else {
            src[srcl++] = byte;
        }

        if (srcl >= each){

            uint8_t dstl = converter(dst, src, srcl);

            output(dst, dstl);

            srcl = 0;
        }
    }
    // in case input terminated before group full
    if (srcl > 0){
        uint8_t dstl = converter(dst, src, srcl);

        output(dst, dstl);
    }

}

void parsedMessage( Message_t * msg, void * context ){

    static uint8_t nrpnMsgCount = 0;
    static uint8_t nrpnChannel = 0;
    static uint8_t nrpnValues[4];

    if (nrpnFilterEnabled && msg->StatusClass == StatusClassControlChange) {
        if (nrpnMsgCount == 0 && msg->Data.ControlChange.Controller == CcNonRegisteredParameterMSB) {
            nrpnChannel = msg->Channel;
            nrpnValues[0] = msg->Data.ControlChange.Value;
            nrpnMsgCount = 1;
            return;
        }

        uint8_t nrpnAction = 0;

        if (nrpnMsgCount > 0 && nrpnChannel != msg->Channel){
            nrpnMsgCount = 0;
        } else {
            if (nrpnMsgCount == 1) {
                if (msg->Data.ControlChange.Controller == CcNonRegisteredParameterLSB) {
                  nrpnValues[1] = msg->Data.ControlChange.Value;
                  nrpnMsgCount = 2;
                  return;
                } else {
                  nrpnMsgCount = 0;
                }
            }
            else if (nrpnMsgCount == 2) {
                if (msg->Data.ControlChange.Controller == CcDataEntryMSB) {
                  nrpnValues[2] = msg->Data.ControlChange.Value;
                  nrpnMsgCount = 3;
                  return;
                } else if (msg->Data.ControlChange.Controller == CcDataIncrement) {
                  nrpnValues[2] = msg->Data.ControlChange.Value;
                  nrpnAction = CcDataIncrement;
                } else if (msg->Data.ControlChange.Controller == CcDataDecrement) {
                  nrpnValues[2] = msg->Data.ControlChange.Value;
                  nrpnAction = CcDataDecrement;
                } else {
                  nrpnMsgCount = 0;
                }
            }
            else if (nrpnMsgCount == 3){
                if (msg->Data.ControlChange.Controller == CcDataEntryLSB) {
                  nrpnValues[3] = msg->Data.ControlChange.Value;
                  nrpnMsgCount = 4;
                  nrpnAction = CcDataEntryMSB;
                } else {
                  nrpnMsgCount = 0;
                }
            }
        }

        if (nrpnAction != 0){

            if (timedOpt.enabled){

                unsigned long now = getNow();
                unsigned long diff = now - timedOpt.lastTimestamp;

                printf("%ld ", diff);

                timedOpt.lastTimestamp = now;
            }

            uint16_t controller = (nrpnValues[0] << 7) | nrpnValues[1];

            if (nrpnAction == CcDataIncrement) {
                printf("nrpn %d %d inc %d\n", nrpnChannel, controller, nrpnValues[2]);
            } else if (nrpnAction == CcDataDecrement) {
                printf("nrpn %d %d dec %d\n", nrpnChannel, controller, nrpnValues[2]);
            } else {
                uint16_t value = (nrpnValues[2] << 7) | nrpnValues[3];
                printf("nrpn %d %d %d\n", nrpnChannel, controller, value);
            }

            fflush(stdout);

            nrpnMsgCount = 0;

            return;
        }
    }

    uint8_t stringBuffer[255];

    int length = MessagetoString(  stringBuffer, msg );

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

void discardingData( uint8_t * data, uint8_t length, void * context ){

    if (printDiscardedData == false){
        return;
    }

    fwrite( data, 1, length, stderr );

    fflush(stderr);

}

uint8_t ** gargv;

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
                {"verbose", optional_argument, 0, 'v'},
                {"exit-on-error", required_argument, 0, 'x'},
                {"help",    no_argument,    0,  'h' },
                {"convert", required_argument, 0, 0},
                {"hex", no_argument, 0, 0},
                {"nrpn-filter", no_argument, 0, 'n'},
                {0,         0,              0,  0 }
        };

        c = getopt_long(argc, argv, "pgt::rhdv::x:h?",
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
                else if (strcmp(long_options[option_index].name, "suffix") == 0) {
                    if (mode != ModeGenerate) {
                        printf("Can only use prefix when generating messages!\n");
                        exit(EXIT_FAILURE);
                    }
                    strcpy(suffix, optarg);
                }
                else if (strcmp(long_options[option_index].name, "convert") == 0) {
                    if (mode != ModeUndefined){
                        printf("Can only enter one mode!\n");
                    }
                    mode = ModeConvert;
                    if (strcmp(optarg, "nibblize") == 0){
                        process = ProcessNibblize;
                    } else if (strcmp(optarg, "denibblize") == 0){
                        process = ProcessDeNibblize;
                    } else if (strcmp(optarg, "sevenbitize") == 0){
                        process = ProcessSevenbitize;
                    } else if (strcmp(optarg, "desevenbitize") == 0){
                        process = ProcessDeSevenbitize;
                    } else {
                        printf("Conversion mode not recognized!\n");
                        exit(EXIT_FAILURE);
                    }
                }
                else if (strcmp(long_options[option_index].name, "hex") == 0){
                    useHex = true;
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

            case 'n':
                nrpnFilterEnabled = true;
                break;

            case 'd':
                printDiscardedData = true;
                break;

            case 'v':

                if (optarg != NULL && strlen(optarg) > 0){
                    verbosity = atoi(optarg);
                } else {
                    verbosity++;
                }
                break;

            case 'x':
                if (optarg != NULL && strlen(optarg) > 0){
                    exitOnError = (bool)atoi(optarg);
                }
                break;

            default:
                printf("?? getopt returned character code 0%o ??\n", c);
        }
    }

    if ( ! isValidMode( mode) ){
        printf("Must be called in either parsing or generation mode (or conversion mode, for that matter..)!\n");
        exit(EXIT_FAILURE);
    }


    if (mode == ModeGenerate) {

        // if there are additional arguments just try to generate a message from
        if (optind < argc){

            uint8_t sysexBuffer[128];
            Message_t msg;
            msg.Data.SysEx.ByteData = sysexBuffer;

            uint8_t argsCount = (uint8_t) (argc - optind);
            uint8_t ** firstArg = (uint8_t **) &argv[optind];

            int result = StringifierResultGenericError;

            if (strcmp((char*)firstArg[0], "nrpn") == 0){

                if (argsCount < 4 || 5 < argsCount) {
                    generatorError(StringifierResultWrongArgCount, argsCount, firstArg);
                    exit(EXIT_FAILURE);
                }
                msg.StatusClass = StatusClassControlChange;
                msg.Channel = atoi((char*)firstArg[1]);

                if (msg.Channel > MaxU7) {
                    generatorError(StringifierResultInvalidU7, argsCount, firstArg);
                    exit(EXIT_FAILURE);
                }

                uint16_t controller = atoi((char*)firstArg[2]);

                if (controller > MaxU14) {
                    generatorError(StringifierResultInvalidU14, argsCount, firstArg);
                    exit(EXIT_FAILURE);
                }

                uint8_t action = 0;
                uint16_t value = 0;

                if (strcmp((char*)firstArg[3], "inc") == 0){

                    action = CcDataIncrement;

                    if (argsCount == 5) {
                        value = atoi((char*)firstArg[4]);

                        if (value > MaxU7) {
                            generatorError(StringifierResultInvalidU14, argsCount, firstArg);
                            exit(EXIT_FAILURE);
                        }
                    }
                }
                else if (strcmp((char*)firstArg[3], "dec") == 0){

                    action = CcDataDecrement;

                    if (argsCount == 5) {
                        value = atoi((char*)firstArg[4]);

                        if (value > MaxU7) {
                            generatorError(StringifierResultInvalidU14, argsCount, firstArg);
                            exit(EXIT_FAILURE);
                        }
                    }

                } else {

                  if (argsCount != 4) {
                      generatorError(StringifierResultWrongArgCount, argsCount, firstArg);
                      exit(EXIT_FAILURE);
                  }

                  action = CcDataEntryMSB;
                  value = atoi((char*)firstArg[3]);

                  if (value > MaxU14) {
                      generatorError(StringifierResultInvalidU14, argsCount, firstArg);
                      exit(EXIT_FAILURE);
                  }
                }

                msg.Data.ControlChange.Controller = CcNonRegisteredParameterMSB;
                msg.Data.ControlChange.Value = (controller >> 7) & DataMask;
                writeMidiPacket(&msg);

                msg.Data.ControlChange.Controller = CcNonRegisteredParameterLSB;
                msg.Data.ControlChange.Value = controller & DataMask;
                writeMidiPacket(&msg);

                if (action == CcDataEntryMSB){
                    msg.Data.ControlChange.Controller = CcDataEntryMSB;
                    msg.Data.ControlChange.Value = (value >> 7) & DataMask;
                    writeMidiPacket(&msg);

                    msg.Data.ControlChange.Controller = CcDataEntryLSB;
                    msg.Data.ControlChange.Value = value & DataMask;
                    writeMidiPacket(&msg);
                } else {
                    msg.Data.ControlChange.Controller = action;
                    msg.Data.ControlChange.Value = value & DataMask;
                    writeMidiPacket(&msg);
                }

                exit(EXIT_SUCCESS);

            } else {
                result = MessagefromArgs(&msg, argsCount, firstArg);

                if (result == StringifierResultOk) {
                    writeMidiPacket(&msg);
                    exit(EXIT_SUCCESS);
                } else {
                    generatorError(result, argsCount, firstArg);
                    exit(EXIT_FAILURE);
                }
            }

        }

        // enter generator loop (reads stdin until eof)
        generator();
    }

    if (mode == ModeParse){
        if (optind < argc) {
            printf("Parser mode may not be called with additional arguments - data is read from stdin only.\n");
            exit(EXIT_FAILURE);
        }

        // enter parser loop (reads stdin until eof)
        parser();
    }

    if (mode == ModeConvert){

        Converter_t converter;
        uint8_t each;

        switch(process){
            case ProcessNibblize:
                converter = [](uint8_t * dst, uint8_t * src, uint8_t length){
                    return nibblize(dst, src, length);
                };
                each = 1;
                break;
            case ProcessDeNibblize:
                converter = [](uint8_t * dst, uint8_t * src, uint8_t length){
                    return denibblize(dst, src, length);
                };
                each = 2;
                break;
            case ProcessSevenbitize:
                converter = [](uint8_t * dst, uint8_t * src, uint8_t length){
                    return sevenbitize(dst, src, length);
                };
                each = 7;
                break;
            case ProcessDeSevenbitize:
                converter = [](uint8_t * dst, uint8_t * src, uint8_t length){
                    return desevenbitize(dst, src, length);
                };
                each = 8;
                break;
        }

        Reader_t reader;

        if (optind < argc){

            gargv = (uint8_t**)&argv[optind];

            reader = []() -> int {
                static int i = 0;
                static int len = strlen((char*)gargv[0]);

                if (i >= len){
                    return EOF;
                }
                return gargv[0][i++];
            };
        } else {
            reader = []() -> int {
                return fgetc(stdin);
            };
        }

        convert( reader, converter, each);
    }

    return EXIT_SUCCESS;
}
