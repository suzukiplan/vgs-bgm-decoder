/* (C)2015, SUZUKI PLAN.
 *----------------------------------------------------------------------------
 * Description: VGS BGM Decoder
 *    Platform: Common
 *      Author: Yoji Suzuki (SUZUKI PLAN)
 *----------------------------------------------------------------------------
 */
#include "vgsdec_internal.h"

#define ROUND(X, MIN, MAX) (X < MIN ? MIN : MAX < X ? MAX : X)

/*
 *----------------------------------------------------------------------------
 * Interface functions
 *----------------------------------------------------------------------------
 */
void* __stdcall vgsdec_create_context()
{
    struct _CONTEXT* result;
    result = (struct _CONTEXT*)malloc(sizeof(struct _CONTEXT));
    if (NULL == result) return NULL;
    memset(result, 0, sizeof(struct _CONTEXT));
#ifdef _WIN32
    InitializeCriticalSection(&(result->cs);
#else
    pthread_mutex_init(&(result->mt), NULL);
#endif
    return result;
}

int __stdcall vgsdec_load_bgm_from_file(void* context, const char* path)
{
    FILE* fp;
    void* data;
    int size;
    int result;

    fp = fopen(path, "rb");
    if (NULL == fp) {
        return -1;
    }
    if (-1 == fseek(fp, 0, SEEK_END)) {
        fclose(fp);
        return -1;
    }
    size = ftell(fp);
    if (size < 1) {
        fclose(fp);
        return -1;
    }
    if (-1 == fseek(fp, 0, SEEK_SET)) {
        fclose(fp);
        return -1;
    }
    data = malloc(size);
    if (NULL == data) {
        fclose(fp);
        return -1;
    }
    if (size != fread(data, 1, (size_t)size, fp)) {
        fclose(fp);
        free(data);
        return -1;
    }
    fclose(fp);
    result = vgsdec_load_bgm_from_memory(context, data, size);
    free(data);
    return 0;
}

int __stdcall vgsdec_load_bgm_from_memory(void* context, void* data, size_t size)
{
    struct _CONTEXT* c = (struct _CONTEXT*)context;
    uLong nblen;
    int i;

    if (NULL == c || NULL == data) return -1;
    reset_context(c);
    nblen = (uLong)sizeof(c->notes);
    memset(c->notes, 0, nblen);
    uncompress((unsigned char*)c->notes, &nblen, (const unsigned char*)data, size);
    c->idxnum = nblen / sizeof(struct _NOTE);
    if (MAX_NOTES == c->idxnum) {
        return -1;
    }
    c->loopIdx = -1;
    for (i = 0; i < c->idxnum; i++) {
        if (NTYPE_WAIT == c->notes[i].type) {
            c->timeL += c->notes[i].val;
        } else if (NTYPE_LABEL == c->notes[i].type) {
            c->timeI = c->timeL;
            c->loopIdx = c->notes[i].val;
        }
    }
    return 0;
}

void __stdcall vgsdec_execute(void* context, void* buffer, size_t size)
{
    struct _CONTEXT* c = (struct _CONTEXT*)context;
    char* buf = (char*)buffer;
    static int an;
    int i, j;
    int pw;
    int wav;
    int cs;
    short* bp;

    if (NULL == c || NULL == buf) return;
    memset(buf, 0, size);

    lock_context(c);
    for (i = 0; i < 6; i++) {
        c->wav[i] = 0;
    }
    if (c->play) {
        if (100 == c->fade2) {
            c->play = 0;
            unlock_context(c);
            return; /* end fadeout */
        }
        if (0 == c->waitTime) {
            c->waitTime = get_next_note(c);
            if (0 == c->waitTime) {
                c->play = 0;
                unlock_context(c);
                return; /* no data */
            }
        }
        if (c->mvol) {
            for (i = 0; i < (int)size; i += 2) {
                for (j = 0; j < 6; j++) {
                    if (c->ch[j].tone) {
                        c->ch[j].cur %= c->ch[j].tone[0];
                        wav = c->ch[j].tone[1 + c->ch[j].cur];
                        c->ch[j].cur += 2;
                        wav *= (c->ch[j].vol * c->mvol);
                        if (c->ch[j].keyOn) {
                            if (c->ch[j].count < c->ch[j].env1) {
                                c->ch[j].count++;
                                pw = (c->ch[j].count * 100) / c->ch[j].env1;
                            } else {
                                pw = 100;
                            }
                        } else {
                            if (c->ch[j].count < c->ch[j].env2) {
                                c->ch[j].count++;
                                pw = 100 - (c->ch[j].count * 100) / c->ch[j].env2;
                            } else {
                                pw = 0;
                            }
                        }
                        if (!c->ch[j].mute) {
                            bp = (short*)(&buf[i]);
                            wav = (wav * pw / 100);
                            if (c->ch[j].volumeRate != 100) {
                                wav *= c->ch[j].volumeRate;
                                wav /= 100;
                            }
                            wav += *bp;
                            if (32767 < wav)
                                wav = 32767;
                            else if (wav < -32768)
                                wav = -32768;
                            if (c->fade2) {
                                wav *= 100 - c->fade2;
                                wav /= 100;
                            }
                            (*bp) = (short)wav;
                            if (i) {
                                c->wav[j] += pw < 0 ? -pw : pw;
                                c->wav[j] >>= 1;
                            } else {
                                c->wav[j] = pw < 0 ? -pw : pw;
                            }
                        }
                        if (c->ch[j].pdown) {
                            c->ch[j].pcnt++;
                            if (c->ch[j].pdown < c->ch[j].pcnt) {
                                c->ch[j].pcnt = 0;
                                if (c->ch[j].toneK) {
                                    c->ch[j].toneK--;
                                }
                                set_note(c, j & 0xff, c->ch[j].toneT, c->ch[j].toneK);
                            }
                        }
                    }
                }
                if (c->volumeRate != 100) {
                    bp = (short*)(&buf[i]);
                    wav = (*bp) * c->volumeRate;
                    wav /= 100;
                    if (32767 < wav)
                        wav = 32767;
                    else if (wav < -32768)
                        wav = -32768;
                    (*bp) = (short)wav;
                }
                c->waitTime--;
                if (0 == c->waitTime) {
                    c->waitTime = get_next_note(c);
                    if (0 == c->waitTime) {
                        unlock_context(c);
                        return; /* no data */
                    }
                }
                /* fade out */
                if (c->fade2 && c->fade2 < 100) {
                    c->fcnt++;
                    if (1023 < c->fcnt) {
                        c->fcnt = 0;
                        c->fade2++;
                    }
                }
                if (c->fade) {
                    c->fcnt++;
                    if (c->fade < c->fcnt) {
                        c->mvol--;
                        c->fcnt = 0;
                    }
                }
            }
        }
    } else {
        c->stopped = 1;
    }
    unlock_context(c);
}

int __stdcall vgsdec_get_value(void* context, int type)
{
    struct _CONTEXT* c = (struct _CONTEXT*)context;
    if (NULL == c) return -1;
    switch (type) {
        case VGSDEC_REG_KEY_0:
            return c->ch[0].toneK;
        case VGSDEC_REG_KEY_1:
            return c->ch[1].toneK;
        case VGSDEC_REG_KEY_2:
            return c->ch[2].toneK;
        case VGSDEC_REG_KEY_3:
            return c->ch[3].toneK;
        case VGSDEC_REG_KEY_4:
            return c->ch[4].toneK;
        case VGSDEC_REG_KEY_5:
            return c->ch[5].toneK;
        case VGSDEC_REG_TONE_0:
            return c->ch[0].toneT;
        case VGSDEC_REG_TONE_1:
            return c->ch[1].toneT;
        case VGSDEC_REG_TONE_2:
            return c->ch[2].toneT;
        case VGSDEC_REG_TONE_3:
            return c->ch[3].toneT;
        case VGSDEC_REG_TONE_4:
            return c->ch[4].toneT;
        case VGSDEC_REG_TONE_5:
            return c->ch[5].toneT;
        case VGSDEC_REG_VOL_0:
            return c->wav[0];
        case VGSDEC_REG_VOL_1:
            return c->wav[1];
        case VGSDEC_REG_VOL_2:
            return c->wav[2];
        case VGSDEC_REG_VOL_3:
            return c->wav[3];
        case VGSDEC_REG_VOL_4:
            return c->wav[4];
        case VGSDEC_REG_VOL_5:
            return c->wav[5];
        case VGSDEC_REG_PLAYING:
            return c->play;
        case VGSDEC_REG_INDEX:
            return c->nidx;
        case VGSDEC_REG_LOOP_INDEX:
            return c->loopIdx;
        case VGSDEC_REG_LENGTH:
            return c->idxnum;
        case VGSDEC_REG_TIME:
            return c->timeP;
        case VGSDEC_REG_LOOP_TIME:
            return c->timeI;
        case VGSDEC_REG_TIME_LENGTH:
            return c->timeL;
        case VGSDEC_REG_LOOP_COUNT:
            return c->loop;
        case VGSDEC_REG_VOLUME_RATE_0:
            return c->ch[0].volumeRate;
        case VGSDEC_REG_VOLUME_RATE_1:
            return c->ch[1].volumeRate;
        case VGSDEC_REG_VOLUME_RATE_2:
            return c->ch[2].volumeRate;
        case VGSDEC_REG_VOLUME_RATE_3:
            return c->ch[3].volumeRate;
        case VGSDEC_REG_VOLUME_RATE_4:
            return c->ch[4].volumeRate;
        case VGSDEC_REG_VOLUME_RATE_5:
            return c->ch[5].volumeRate;
        case VGSDEC_REG_VOLUME_RATE:
            return c->volumeRate;
    }
    return -1;
}

void __stdcall vgsdec_set_value(void* context, int type, int value)
{
    struct _CONTEXT* c = (struct _CONTEXT*)context;

    if (NULL == c) return;
    lock_context(c);
    switch (type) {
        case VGSDEC_REG_TIME:
            if (0 <= value) jump_time(c, value);
            break;
        case VGSDEC_REG_FADEOUT:
            if (value && 0 == c->fade2) c->fade2 = 1;
            break;
        case VGSDEC_REG_RESET:
            if (value) reset_context(c);
            break;
        case VGSDEC_REG_VOLUME_RATE_0:
            c->ch[0].volumeRate = ROUND(value, 0, 100);
            break;
        case VGSDEC_REG_VOLUME_RATE_1:
            c->ch[1].volumeRate = ROUND(value, 0, 100);
            break;
        case VGSDEC_REG_VOLUME_RATE_2:
            c->ch[2].volumeRate = ROUND(value, 0, 100);
            break;
        case VGSDEC_REG_VOLUME_RATE_3:
            c->ch[3].volumeRate = ROUND(value, 0, 100);
            break;
        case VGSDEC_REG_VOLUME_RATE_4:
            c->ch[4].volumeRate = ROUND(value, 0, 100);
            break;
        case VGSDEC_REG_VOLUME_RATE_5:
            c->ch[5].volumeRate = ROUND(value, 0, 100);
            break;
        case VGSDEC_REG_VOLUME_RATE:
            c->volumeRate = ROUND(value, 0, 100);
            break;
    }
    unlock_context(c);
}

void __stdcall vgsdec_release_context(void* context)
{
    struct _CONTEXT* c = (struct _CONTEXT*)context;

    if (NULL != c) {
#ifdef _WIN32
        DeleteCriticalSection(&(c->cs);
#else
        pthread_mutex_destroy(&(c->mt));
#endif
        memset(c,0,sizeof(struct _CONTEXT));
        free(c);
    }
}

/*
 *----------------------------------------------------------------------------
 * Internal functions
 *----------------------------------------------------------------------------
 */
static void reset_context(struct _CONTEXT* c)
{
    c->play = 1;
    c->mask = 0;
    c->mvol = 0;
    c->waitTime = 0;
    memset(c->wav, 0, sizeof(c->wav));
    c->nidx = 0;
    c->stopped = 0;
    c->fade = 0;
    c->fcnt = 0;
    memset(c->ch, 0, sizeof(c->ch));
    c->mute = 0;
    c->loop = 0;
    c->fade2 = 0;
    memset(c->addKey, 0, sizeof(c->addKey));
    memset(c->addOff, 0, sizeof(c->addOff));
    c->volumeRate = 100;
    c->ch[0].volumeRate = 100;
    c->ch[1].volumeRate = 100;
    c->ch[2].volumeRate = 100;
    c->ch[3].volumeRate = 100;
    c->ch[4].volumeRate = 100;
    c->ch[5].volumeRate = 100;
}

static void lock_context(struct _CONTEXT* c)
{
#ifdef _WIN32
    EnterCriticalSection(&(c->cs);
#else
    pthread_mutex_lock(&(c->mt));
#endif
}

static void unlock_context(struct _CONTEXT* c)
{
#ifdef _WIN32
    LeaveCriticalSection(&(c->cs);
#else
    pthread_mutex_unlock(&(c->mt));
#endif
}

/* set tone and tune */
static void set_note(struct _CONTEXT* c, unsigned char cn, unsigned char t, unsigned char n)
{
    n += c->addKey[cn];
    switch (t) {
        case 0: /* SANKAKU */
            c->ch[cn].tone = TONE1[n % 85];
            break;
        case 1: /* NOKOGIR */
            c->ch[cn].tone = TONE2[n % 85];
            break;
        case 2: /* KUKEI */
            c->ch[cn].tone = TONE3[n % 85];
            break;
        default: /* NOIZE */
            c->ch[cn].tone = TONE4[n % 85];
            break;
    }
}

/* get next waittime */
static int get_next_note(struct _CONTEXT* c)
{
    int ret;
    if (c->notes[c->nidx].type == NTYPE_WAIT && 0 == c->notes[c->nidx].val) {
        return 0;
    }
    for (; NTYPE_WAIT != c->notes[c->nidx].type; c->nidx++) {
        switch (c->notes[c->nidx].type) {
            case NTYPE_ENV1: /* op1=ch, val=env1 */
                c->ch[c->notes[c->nidx].op1].env1 = c->notes[c->nidx].val;
                break;
            case NTYPE_ENV2: /* op1=ch, val=env1 */
                c->ch[c->notes[c->nidx].op1].env2 = c->notes[c->nidx].val;
                break;
            case NTYPE_VOL: /* op1=ch, val=vol */
                c->ch[c->notes[c->nidx].op1].vol = c->notes[c->nidx].val;
                break;
            case NTYPE_MVOL: /* val=mvol */
                c->mvol = c->notes[c->nidx].val;
                break;
            case NTYPE_KEYON: /* op1=ch, op2=tone, op3=key */
                c->ch[c->notes[c->nidx].op1].keyOn = 1;
                c->ch[c->notes[c->nidx].op1].count = 0;
                c->ch[c->notes[c->nidx].op1].cur = 0;
                c->ch[c->notes[c->nidx].op1].toneT = c->notes[c->nidx].op2;
                c->ch[c->notes[c->nidx].op1].toneK = c->notes[c->nidx].op3;
                set_note(c, c->notes[c->nidx].op1, c->notes[c->nidx].op2, c->notes[c->nidx].op3);
                break;
            case NTYPE_KEYOFF: /* op1=ch */
                c->ch[c->notes[c->nidx].op1].keyOn = 0;
                c->ch[c->notes[c->nidx].op1].count = 0;
                break;
            case NTYPE_PDOWN: /* op1=ch, val=Hz */
                c->ch[c->notes[c->nidx].op1].pdown = c->notes[c->nidx].val;
                c->ch[c->notes[c->nidx].op1].pcnt = 0;
                break;
            case NTYPE_JUMP: /* val=address */
                c->nidx = c->notes[c->nidx].val;
                c->loop++;
                c->timeP = c->timeI;
                break;
            case NTYPE_LABEL:
                break;
            default:
                return 0;
        }
    }
    ret = c->notes[c->nidx].val;
    if (ret) {
        c->nidx++;
    }
    c->timeP += ret;
    return ret;
}

/* jump specific time */
static void jump_time(struct _CONTEXT* c, int sec)
{
    int wt;
    reset_context(c);
    while (0 < sec) {
        wt = get_next_note(c);
        if (0 == wt) break;
        sec -= wt;
    }
}
