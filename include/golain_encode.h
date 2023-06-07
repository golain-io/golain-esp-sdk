#ifndef _GOLAIN_ENCODE_H
#define _GOLAIN_ENCODE_H

#include "pb.h"
#include "pb_encode.h"
#include "pb_decode.h"

/*------------------------------------------------------Nanopb callbacks-------------------------------------*/
bool golain_pb_decode_string(pb_istream_t *stream, const pb_field_t *field, void **arg);

bool golain_pb_encode_string(pb_ostream_t *stream, const pb_field_t *field, void * const *arg) ;

#endif