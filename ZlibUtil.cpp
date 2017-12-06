/**************************************************************
 * Project          Augmented Reality Visit Assistance
 * (c) Copyright    2017
 * Company          Infinte Reality Solution LLC
 *                  All rights reserved
 **************************************************************/

#include "ZlibUtil.h"
#include <zlib.h>
#include <string.h>

int compressData(void* inData, int inSize, void* outData, int outSize)
{
   z_stream defstream;
   defstream.zalloc = Z_NULL;
   defstream.zfree = Z_NULL;
   defstream.opaque = Z_NULL;
   // setup "a" as the input and "b" as the compressed output
   defstream.avail_in = inSize;
   defstream.next_in = (Bytef *)inData;
   defstream.avail_out = (uInt)outSize;
   defstream.next_out = (Bytef *)outData;
   
   // the actual compression work.
   deflateInit(&defstream, Z_BEST_COMPRESSION);
   deflate(&defstream, Z_FINISH);
   deflateEnd(&defstream);

   return defstream.total_out;
}

int uncompressData(void* inData, int inSize, void* outData, int outSize)
{
   z_stream infstream;
   infstream.zalloc = Z_NULL;
   infstream.zfree = Z_NULL;
   infstream.opaque = Z_NULL;
   // setup "b" as the input and "c" as the compressed output
   infstream.avail_in = (uInt)inSize;
   infstream.next_in = (Bytef *)inData;
   infstream.avail_out = (uInt)outSize;
   infstream.next_out = (Bytef *)outData;
   
   // the actual DE-compression work.
   inflateInit(&infstream);
   inflate(&infstream, Z_NO_FLUSH);
   inflateEnd(&infstream);

   return infstream.total_out;
}
