/**
 * \file
 *
 * Isn't it strange that typically the C is considered as a center or base note (at least when starting out in (keys) playing)?
 * This just makes sense when trying to play the most simple version leaving out the black keys.
 *
 * But quite obviously the A-note is the basis - just look at the note names ABCDEFG!
 * Why the German note name for B turned into a H just messes things up pretty badily: AHCDEFG, or when considering the C as center key: CDEFGAH wtf?
 */

#ifndef MIDIMESSAGE_NOTES_H
#define MIDIMESSAGE_NOTES_H

#include <cstdint>

namespace MidiMessage {

    const uint8_t IntervalMinorSecond   = 1;
    const uint8_t IntervalMajorSecond   = 2;
    const uint8_t IntervalMinorThird    = 3;
    const uint8_t IntervalMajorThird    = 4;
    const uint8_t IntervalFourth        = 5;
    const uint8_t IntervalTritone       = 6;
    const uint8_t IntervalFifth         = 7;
    const uint8_t IntervalMinorSixth    = 8;
    const uint8_t IntervalMajorSixth    = 9;
    const uint8_t IntervalMinorSeventh  = 10;
    const uint8_t IntervalMajorSeventh  = 11;
    const uint8_t IntervalOctave        = 12;

    typedef enum {
        NoteMin = 0,

        NoteC_1             = NoteMin, // 8.18 Hz
        NoteC_1Sharp        = 1, // 8.66 Hz
        NoteD_1             = 2, // 9.18 Hz
        NoteD_1Sharp        = 3, // 9.72 Hz
        NoteE_1             = 4, // 10.30 Hz
        NoteF_1             = 5, // 10.91 Hz
        NoteF_1Sharp        = 6, // 11.56 Hz
        NoteG_1             = 7, // 12.25 Hz
        NoteG_1Sharp        = 8, // 12.98 Hz
        NoteA_1             = 9, // 13.75 Hz
        NoteA_1Sharp        = 10, // 14.57 Hz
        NoteB_1             = 11, // 15.43 Hz

        NoteA0 = 21, // 27.5 Hz
        NoteA1 = 33, // 55 Hz
        NoteA2 = 48, // 110 Hz
        NoteA3 = 57, // 220 Hz
        
        NoteC4 = 60,
        NoteMiddleC = NoteC4,
        
        NoteA4 = 69,
        NoteConcertPitch = NoteA4, // 440 Hz

        NoteA5 = 81, // 880  Hz
        NoteA6 = 93, // 1760  Hz
        NoteA7 = 105, // 3520 Hz
        NoteA8 = 117, // 7040 Hz

        NoteG9 = 127, // 12543.85 Hz
        NoteMax = NoteG9

    } Note_t;

    inline bool isNoteC( Note_t note ){
        return (note % 12) == NoteC_1;
    }
    inline bool isNoteCSharp( Note_t note ){
        return (note % 12) == NoteC_1Sharp;
    }
    inline bool isNoteD( Note_t note ){
        return (note % 12) == NoteD_1;
    }
    inline bool isNoteDSharp( Note_t note ){
        return (note % 12) == NoteD_1Sharp;
    }
    inline bool isNoteE( Note_t note ){
        return (note % 12) == NoteE_1;
    }
    inline bool isNoteF( Note_t note ){
        return (note % 12) == NoteF_1;
    }
    inline bool isNoteFSharp( Note_t note ){
        return (note % 12) == NoteF_1Sharp;
    }
    inline bool isNoteG( Note_t note ){
        return (note % 12) == NoteG_1;
    }
    inline bool isNoteGSharp( Note_t note ){
        return (note % 12) == NoteG_1Sharp;
    }
    inline bool isNoteA( Note_t note ){
        return (note % 12) == NoteA_1Sharp;
    }
    inline bool isNoteASharp( Note_t note ){
        return (note % 12) == NoteA_1Sharp;
    }
    inline bool isNoteB( Note_t note ){
        return (note % 12) == NoteB_1;
    }

    inline bool isSubSubContraNote( Note_t note ){
        return (0 <= note && note <= 11);
    }

    inline bool isSubContraNote( Note_t note ){
        return (12 <= note && note <= 23);
    }

    inline bool isContraNote( Note_t note ){
        return (24 <= note && note <= 35);
    }

    inline bool isGreatNote( Note_t note ){
        return (26 <= note && note <= 47);
    }

    inline bool isSmallNote( Note_t note ){
        return (48 <= note && note <= 59);
    }


}

#endif //MIDIMESSAGE_NOTES_H
