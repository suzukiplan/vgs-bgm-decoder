/* (C)2016, SUZUKI PLAN.
 *----------------------------------------------------------------------------
 * Description: VGSDEC - internal header
 *    Platform: Common
 *      Author: Yoji Suzuki (SUZUKI PLAN)
 *----------------------------------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include "vgsdec.h"

#define SAMPLE_RATE 22050
#define SAMPLE_BITS 16
#define SAMPLE_CH 1
#define SAMPLE_BUFS 4410

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
    unsigned char key;
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

struct _PSG {
    struct _NOTE* notes;
    unsigned char play;
    unsigned char mask;
    unsigned short mvol;
    unsigned int waitTime;
    short wav[6];
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
    int idxnum;
    int volumeRate;
};

extern short* TONE1[85];
extern short* TONE2[85];
extern short* TONE3[85];
extern short* TONE4[85];
