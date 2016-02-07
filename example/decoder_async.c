/* decoder sample (draft) */
#ifdef _WIN32
#include <windows.h>
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

static void callback(void* context, void* data, size_t size);
static FILE* wav;
static struct DataHeader dh;
static volatile int err_flag;

#ifdef _WIN32
static void usleep(int us)
{
    Sleep(us / 1000);
}
#endif

int main(int argc, char* argv[])
{
    char buf[1024];
    void* context;

    if (argc < 3) {
        puts("usage: decoder_async bgm_file wav_file");
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

    printf("DECODING...");

    /* start async decoder */
    if (vgsdec_async_start(context)) {
        puts("cannot start async decoder.");
        fclose(wav);
        vgsdec_release_context(context);
        return 4;
    }

    /* enqueue first data */
    if (vgsdec_async_enqueue(context, buf, sizeof(buf), callback)) {
        puts("cannot enqueue 1st data.");
        fclose(wav);
        vgsdec_release_context(context);
        return 4;
    }

    /* waiting for end of decoding */
    while (vgsdec_get_value(context, VGSDEC_REG_PLAYING) && !err_flag) {
        usleep(1000);
        printf(".");
    }
    if (!err_flag) puts("done");
    vgsdec_async_stop(context);

    fclose(wav);
    vgsdec_release_context(context);

    /* update wave header */
    if (!err_flag) {
        dh.fsize = dh.dsize + sizeof(dh) - 8;
        {
            int fd = _open(argv[2], O_RDWR | O_BINARY);
            if (-1 != fd) {
                _lseek(fd, 0, 0);
                _write(fd, &dh, sizeof(dh));
                _close(fd);
            }
        }
    }
    return err_flag;
}

static void callback(void* context, void* data, size_t size)
{
    /* write wav file */
    fwrite(data, size, 1, wav);
    dh.dsize += size;

    /* enqueue next data if playing */
    if (vgsdec_get_value(context, VGSDEC_REG_PLAYING)) {
        /* set fadeout if looped */
        if (0 == vgsdec_get_value(context, VGSDEC_REG_FADEOUT_COUNTER)) {
            if (vgsdec_get_value(context, VGSDEC_REG_LOOP_COUNT)) {
                vgsdec_set_value(context, VGSDEC_REG_FADEOUT, 1);
            }
        }
        /* enqueue next data */
        if (vgsdec_async_enqueue(context, data, size, callback)) {
            puts("failed in enqueue");
            err_flag = 1;
        }
    }
}
