//
// Created by Philip Tschiemer on 21.08.19.
//

#ifndef MIDIMESSAGE_NOTES_H
#define MIDIMESSAGE_NOTES_H

namespace MidiMessage {

    typedef enum {
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
        NoteG9 = 127 // 12543.85 Hz
    } Note_t;

}

#endif //MIDIMESSAGE_NOTES_H
