/* decoder sample (draft) */
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#define _open open
#define _write write
#define _lseek lseek
#define _close close
#endif
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "vgsdec.h"

#ifndef O_BINARY
#define O_BINARY 0
#endif

struct DataHeader {
    char riff[4];
    unsigned int fsize;
    char wave[4];
    char fmt[4];
    unsigned int bnum;
    unsigned short fid;
    unsigned short ch;
    unsigned int sample;
    unsigned int bps;
    unsigned short bsize;
    unsigned short bits;
    char data[4];
    unsigned int dsize;
};

int main(int argc, char* argv[])
{
    char buf[1024];
    void* context;
    FILE* wav;
    struct DataHeader dh;

    if (argc < 3) {
        puts("usage: decoder bgm_file wav_file");
        return 1;
    }

    /* initialize context */
    context = vgsdec_create_context();
    if (NULL == context) {
        puts("vgsdec_create_context error.");
        return 255;
    }

    /* load bgm data */
    if (vgsdec_load_bgm_from_file(context, argv[1])) {
        puts("load error.");
        vgsdec_release_context(context);
        return 2;
    }

    /* open wave file */
    wav = fopen(argv[2], "wb");
    if (NULL == wav) {
        puts("open error.");
        vgsdec_release_context(context);
        return 3;
    }

    /* initialize wave header */
    memset(&dh, 0, sizeof(dh));
    strncpy(dh.riff, "RIFF", 4);
    strncpy(dh.wave, "WAVE", 4);
    strncpy(dh.fmt, "fmt ", 4);
    strncpy(dh.data, "data", 4);
    dh.bnum = 16;
    dh.fid = 1;
    dh.ch = 1;
    dh.sample = 22050;
    dh.bps = 44100;
    dh.bsize = 2;
    dh.bits = 16;
    dh.dsize = 0;
    fwrite(&dh, 1, sizeof(dh), wav);

    /* decoding loop */
    while (vgsdec_get_value(context, VGSDEC_REG_PLAYING) && vgsdec_get_value(context, VGSDEC_REG_LOOP_COUNT) == 0) {
        vgsdec_execute(context, buf, sizeof(buf));
        fwrite(buf, sizeof(buf), 1, wav);
        dh.dsize += sizeof(buf);
    }

    /* wait the end of fadeout if looped */
    if (vgsdec_get_value(context, VGSDEC_REG_LOOP_COUNT)) {
        vgsdec_set_value(context, VGSDEC_REG_FADEOUT, 1);
        while (vgsdec_get_value(context, VGSDEC_REG_PLAYING)) {
            vgsdec_execute(context, buf, sizeof(buf));
            fwrite(buf, sizeof(buf), 1, wav);
            dh.dsize += sizeof(buf);
        }
    }

    fclose(wav);
    vgsdec_release_context(context);

    /* update wave header */
    dh.fsize = dh.dsize + sizeof(dh) - 8;
    {
        int fd = _open(argv[2], O_RDWR | O_BINARY);
        if (-1 != fd) {
            _lseek(fd, 0, 0);
            _write(fd, &dh, sizeof(dh));
            _close(fd);
        }
    }
    return 0;
}
