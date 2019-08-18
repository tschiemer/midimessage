//
// Created by Philip Tschiemer on 18.08.19.
//

#ifndef MIDIMESSAGE_STDLIB_H
#define MIDIMESSAGE_STDLIB_H

#include <cstddef>

namespace MidiMessage {

    void * memcpy( void * dst, void * src, size_t count );

    void * calloc( size_t n, size_t s );

    void free( void * ptr );

}

#endif //MIDIMESSAGE_STDLIB_H
