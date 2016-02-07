#include <stdio.h>
#include <string.h>
#include "vgsdec.h"

int main(int argc, char* argv[])
{
    char buf[1024];
    void* context;
    int i, j;

    if (argc < 2) {
        return 1;
    }

    /* initialize context */
    for (j = 0; j < 2; j++) {
        context = vgsdec_create_context();
        if (NULL == context) {
            puts("vgsdec_create_context error.");
            return 255;
        }

        for (i = 0; i < 2; i++) {
            /* load bgm data */
            if (vgsdec_load_bgm_from_file(context, argv[1])) {
                puts("load error.");
                vgsdec_release_context(context);
                return 2;
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
        }
        vgsdec_release_context(context);
    }
    puts("DONE");
    return 0;
}
