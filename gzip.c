#include "gzip.h"
#include <stdio.h>

void compact(char *buffer, char *out_buffer, size_t out_size){
    // STEP 1.
    // deflate a into b. (that is, compress a into b)
    // zlib struct
    z_stream defstream;
    defstream.zalloc = Z_NULL;
    defstream.zfree = Z_NULL;
    defstream.opaque = Z_NULL;

    // setup "a" as the input and "b" as the compressed output
    defstream.avail_in = (uInt) strlen(buffer) + 1; // size of input, string + terminator
    defstream.next_in = (Bytef *) buffer; // input char array
    defstream.avail_out = (uInt) out_size; // size of output
    defstream.next_out = (Bytef *) out_buffer; // output char array

    // the actual compression work.
    deflateInit(&defstream, Z_NO_COMPRESSION);
    deflate(&defstream, Z_FINISH);
    deflateEnd(&defstream);
}
void extract(char *buffer, char *out_buffer, size_t in_size){
    // STEP 2.
    // inflate b into c
    // zlib struct
    z_stream infstream;
    infstream.zalloc = Z_NULL;
    infstream.zfree = Z_NULL;
    infstream.opaque = Z_NULL;

    // setup "b" as the input and "c" as the compressed output
    infstream.avail_in = (uInt) in_size; // size of input
    infstream.next_in = (Bytef *) buffer; // input char array
    infstream.avail_out = (uInt) in_size; // size of output
    infstream.next_out = (Bytef *) out_buffer; // output char array

    // the actual DE-compression work.
    inflateInit(&infstream);
    inflate(&infstream, Z_NO_FLUSH);
    inflateEnd(&infstream);
}


void md5(char *str, char *out, int length){
  int n;
  MD5_CTX c;
  unsigned char digest[16];

  MD5_Init(&c);

  while (length > 0) {
    if (length > 512) {
      MD5_Update(&c, str, 512);
    } else {
      MD5_Update(&c, str, length);
    }
    length -= 512;
    str += 512;
  }

  MD5_Final(digest, &c);

  for (n = 0; n < 16; ++n) {
    snprintf(&(out[n*2]), 16*2, "%02x", (unsigned int)digest[n]);
  }
}
