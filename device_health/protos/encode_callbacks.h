#ifndef ENCODE_CALLBACKS_H
#define ENCODE_CALLBACKS_H

#pragma once 
#include <pb.h>
#include <pb_encode.h>
#include "string.h"
bool encode_string(pb_ostream_t *stream, const pb_field_t *field, void * const *arg) {
    char* string_to_encode = (char*)*arg;
    if (!pb_encode_tag_for_field(stream, field))
        return false;
    
    return pb_encode_string(stream, (uint8_t*)string_to_encode, strlen(string_to_encode));
}

#endif /* ENCODING_CALLBACKS_H */