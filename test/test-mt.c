#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include "vgsdec.h"

static void* decoder(void* args);

int main(int argc, char* argv[])
{
    pthread_t tid[10];
    int i;

    if (argc < 2) {
        return 1;
    }

    for (i = 0; i < 10; i++) {
        if (pthread_create(&tid[i], NULL, decoder, argv[1])) {
            puts("pthread_create error");
            return 255;
        }
    }

    for (i = 0; i < 10; i++) {
        pthread_join(tid[i], NULL);
    }
    puts("ALL-DONE");
    return 0;
}

static void* decoder(void* args)
{
    const char* path = (const char*)args;
    char buf[1024];
    void* context;

    /* initialize context */
    context = vgsdec_create_context();
    if (NULL == context) {
        puts("vgsdec_create_context error.");
        return NULL;
    }

    /* load bgm data */
    if (vgsdec_load_bgm_from_file(context, path)) {
        puts("load error.");
        vgsdec_release_context(context);
        return NULL;
    }

    /* decoding loop */
    while (vgsdec_get_value(context, VGSDEC_REG_PLAYING) && vgsdec_get_value(context, VGSDEC_REG_LOOP_COUNT) == 0) {
        vgsdec_execute(context, buf, sizeof(buf));
    }

    /* waiting for the end of fadeout if looped */
    if (vgsdec_get_value(context, VGSDEC_REG_LOOP_COUNT)) {
        vgsdec_set_value(context, VGSDEC_REG_FADEOUT, 1);
        while (vgsdec_get_value(context, VGSDEC_REG_PLAYING)) {
            vgsdec_execute(context, buf, sizeof(buf));
        }
    }
    vgsdec_release_context(context);
    puts("DONE");
    return NULL;
}
