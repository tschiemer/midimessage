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
    printf("If no <data> or <cmd>.. is given reads from STDIN assuming one input per line separated by a NL\n");
}

void generate(uint8_t argc, uint8_t * argv[]);
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
            generate( (uint8_t)(argc - optind), (uint8_t**)&argv[optind] );
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

    while( ! feof(stdin) ){

        uint8_t in = fgetc(stdin);

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

                generate( argsCount, args );

                pos = 0;
                line[0] = '\0';

                argsCount = 0;
            }

        }
        if (mode == ModeParse){

            line[pos++] = in;

            uint8_t consumed = parse( pos, (uint8_t*)line );

            // if part of stream were consumed, move bytes up the stream
            if (consumed > 0){
                for(int i = 0, p = pos; i < pos - consumed; i++, p++){
                    line[i] = line[p];
                }
            }

            // if receiving a control byte (that isnt a EOX) something went wrong, readjust
            else if (isControlByte(in) && in != SystemMessageEndOfExclusive && pos > 0){
                for( int i = 0, p = pos - 1; i < pos; i++,p++){
                    line[i] = line[p];
                }
            }
        }
    }

    return EXIT_SUCCESS;
}

void generate(uint8_t argc, uint8_t * argv[]){
    printf("Generating for %d\n", argc);
    for(int i = 0; i < argc; i++){
        printf(" %d %s\n", i, argv[i]);
    }
}

uint8_t parse(uint8_t length, uint8_t bytes[]){


    return length;
}