#include <zlib.h>

static alloc_func zalloc = (alloc_func)0;
static free_func zfree = (free_func)0;
void zlib_deflate(char *next_in, char *next_out, uLong avail_in, uLong avail_out)
{
    z_stream strm;
    strm.zalloc = zalloc;
    strm.zfree = zfree;
    strm.opaque = (voidpf)0;
    deflateInit(&strm, Z_DEFAULT_COMPRESSION);
    strm.next_in = (Bytef *)next_in;
    strm.next_out = (Bytef *)next_out;
    strm.avail_in = avail_in;
    strm.avail_out = avail_out;
    deflate(&strm, Z_NO_FLUSH);
    deflate(&strm, Z_FINISH);
    next_out[strm.total_out] = '\0';
    deflateEnd(&strm);
}

void zlib_inflate(char *next_in, char *next_out, uLong avail_in, uLong avail_out)
{
    z_stream dstrm;
    dstrm.zalloc = zalloc;
    dstrm.zfree = zfree;
    dstrm.opaque = (voidpf)0;
    inflateInit(&dstrm);
    dstrm.next_in = (Bytef *)next_in;
    dstrm.next_out = (Bytef *)next_out;
    dstrm.avail_in = avail_in;
    dstrm.avail_out = avail_out;
    inflate(&dstrm, Z_NO_FLUSH);
    inflateEnd(&dstrm);
    next_out[dstrm.total_out] = '\0'; 
    inflateEnd(&dstrm); 
}