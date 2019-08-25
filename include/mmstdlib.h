/**
 * \file
 */

#ifndef MIDIMESSAGE_STDLIB_H
#define MIDIMESSAGE_STDLIB_H

#include <midimessage.h>
#include <cstddef>

namespace MidiMessage {

    void * memcpy( void * dst, void * src, size_t count );

#if SYSEX_MEMORY == SYSEX_MEMORY_DYNAMIC
    void * calloc( size_t n, size_t s );

    void free( void * ptr );
#endif

}

#endif //MIDIMESSAGE_STDLIB_H
