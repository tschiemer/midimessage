#include "mmstdlib.h"
#include <cstdlib>
#include <string>

namespace MidiMessage {

    void * memcpy( void * dst, void * src, size_t count ) {
        return std::memcpy( dst, src, count );
    }

    void * calloc( size_t n, size_t s ) {
        return std::calloc( n, s );
    }

    void free( void * ptr ) {
        std::free( ptr );
    }

}