/* (C)2016, SUZUKI PLAN.
 *----------------------------------------------------------------------------
 * Description: VGS BGM Decoder - internal header
 *    Platform: Common
 *      Author: Yoji Suzuki (SUZUKI PLAN)
 *----------------------------------------------------------------------------
 */
#ifdef _WIN32
#include <windows.h>
#include <winsock2.h>
#else
#include <pthread.h>
#include <arpa/inet.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vgsdec.h"
#include "miniz.h"

#define NTYPE_ENV1 1
#define NTYPE_ENV2 2
#define NTYPE_VOL 3
#define NTYPE_MVOL 4
#define NTYPE_KEYON 5
#define NTYPE_KEYOFF 6
#define NTYPE_WAIT 7
#define NTYPE_PDOWN 8
#define NTYPE_JUMP 9
#define NTYPE_LABEL 10

#define MAX_NOTES 65536

struct _NOTE {
    unsigned char type;
    unsigned char op1;
    unsigned char op2;
    unsigned char op3;
    unsigned int val;
};

struct _PSGCH {
    short* tone;
    unsigned char vol;
    unsigned char keyOn;
    unsigned char mute;
    unsigned int cur;
    unsigned int count;
    unsigned int env1;
    unsigned int env2;
    unsigned char toneT;
    unsigned char toneK;
    unsigned int pdown;
    unsigned int pcnt;
    int volumeRate;
};

struct _CONTEXT {
#ifdef _WIN32
    CRITICAL_SECTION cs;
#else
    pthread_mutex_t mt;
#endif
    struct _NOTE notes[MAX_NOTES];
    struct VgsMetaHeader* mhead;
    struct VgsMetaData** mdata;
    unsigned char play;
    unsigned char mask;
    unsigned short mvol;
    unsigned int waitTime;
    int wav[6];
    int nidx;
    int stopped;
    unsigned int fade;
    unsigned int fcnt;
    struct _PSGCH ch[6];
    int mute;
    int loop;
    int fade2;
    unsigned int timeI;
    unsigned int timeL;
    unsigned int timeP;
    int addKey[6];
    int addOff[6];
    int loopIdx;
    int idxnum;
    int volumeRate;
    int synthesis;
};

extern short* TONE1[85];
extern short* TONE2[85];
extern short* TONE3[85];
extern short* TONE4[85];

static void reset_context(struct _CONTEXT* c);
static void lock_context(struct _CONTEXT* c);
static void unlock_context(struct _CONTEXT* c);
static void set_note(struct _CONTEXT* c, unsigned char cn, unsigned char t, unsigned char n);
static int get_next_note(struct _CONTEXT* c);
static void jump_time(struct _CONTEXT* c, int sec);
static size_t extract_meta_data(struct _CONTEXT* c, void* data, size_t size);
static void release_meta_data(struct _CONTEXT* c);
