/* decoder sample (draft) */
#include <stdio.h>
#include "vgsdec.h"

int main(int argc, char* argv[])
{
	char buf[1024];
	void* context;

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

	/* decoding loop */
	while (vgsdec_get_value(context, VGSDEC_REG_PLAYING) &&
	       vgsdec_get_value(context, VGSDEC_REG_LOOP_COUNT)==0) {
		vgsdec_execute(context, buf, sizeof(buf));
		// todo: appends pcm to wav file
	}

	/* wait the end of fadeout if looped */
	if (vgsdec_get_value(context, VGSDEC_REG_LOOP_COUNT)) {
		vgsdec_set_value(context, VGSDEC_REG_FADEOUT, 1);
		while (vgsdec_get_value(context, VGSDEC_REG_PLAYING)) {
			vgsdec_execute(context, buf, sizeof(buf));
			// todo: appends pcm to wav file
		}
	}

	vgsdec_release_context(context);
	return 0;
}

