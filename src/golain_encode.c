#include "golain_encode.h"

#include "pb.h"
#include "pb_encode.h"
#include "pb_decode.h"

#include <stdio.h>

bool golain_pb_decode_string(pb_istream_t *stream, const pb_field_t *field, void **arg){
    uint8_t buffer[1024] = {0};
    
    /* We could read block-by-block to avoid the large buffer... */
    if (stream->bytes_left > sizeof(buffer) - 1)
        return false;
    
    if (!pb_read(stream, buffer, stream->bytes_left))
        return false;
    
    sprintf((char*)*arg, "%s", buffer);
    return true;
    
}

bool golain_pb_encode_string(pb_ostream_t *stream, const pb_field_t *field, void * const *arg){
    char* string_to_encode = (char*)*arg;
    if (!pb_encode_tag_for_field(stream, field))
        return false;
    
    return pb_encode_string(stream, (uint8_t*)string_to_encode, strlen(string_to_encode));
}