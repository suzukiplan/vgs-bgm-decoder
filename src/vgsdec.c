/* (C)2015, SUZUKI PLAN.
 *----------------------------------------------------------------------------
 * Description: VGS mk-II SR - application program interface
 *    Platform: Common
 *      Author: Yoji Suzuki (SUZUKI PLAN)
 *----------------------------------------------------------------------------
 */
#include "vgs2.h"
#include "miniz.h"

/*
 *----------------------------------------------------------------------------
 * Global variables
 *----------------------------------------------------------------------------
 */
int BN;
struct _BINREC* BR;
struct _SLOT _slot[256];
struct _EFF _eff[256];
char* _note[256];
static uLong _notelen[256];
struct _NOTE _notebuf[65536];
struct _VRAM _vram;
struct _TOUCH _touch;
struct _PSG _psg;
unsigned char _mute;
unsigned char _pause;
unsigned char _interlace = 1;

/*
 *----------------------------------------------------------------------------
 * file static functions
 *----------------------------------------------------------------------------
 */
static void setNote(unsigned char cn, unsigned char t, unsigned char n);
static int getNextNote();
static char* getbin(const char* name, int* size);
static int gclip(unsigned char n, int* sx, int* sy, int* xs, int* ys, int* dx, int* dy);
static float myatan2(int a, int b);
static float mysqrt(float x);

/*
 *----------------------------------------------------------------------------
 * sound buffering
 *----------------------------------------------------------------------------
 */
void sndbuf(char* buf, size_t size)
{
    static int an;
    int i, j;
    int pw;
    int wav;
    int cs;
    short* bp;

    an = 1 - an;

    memset(buf, 0, size);
    if (_pause || _mute) {
        return;
    }

    for (i = 0; i < 256; i++) {
        if (_eff[i].flag) {
            if (1 < _eff[i].flag) {
                eff_pos(&_eff[i], 0);
                eff_flag(&_eff[i], 1);
            }
            /* calc copy size */
            cs = _eff[i].size - _eff[i].pos;
            if (size < (size_t)cs) {
                cs = (int)size;
            }
            /* buffering */
            for (j = 0; j < cs; j += 2) {
                bp = (short*)(&buf[j]);
                wav = *bp;
                wav += *((short*)&(_eff[i].dat[_eff[i].pos + j]));
                if (32767 < wav)
                    wav = 32767;
                else if (wav < -32768)
                    wav = -32768;
                (*bp) = (short)wav;
            }
            /* change position */
            eff_pos(&_eff[i], _eff[i].pos + cs);
            if (_eff[i].size <= _eff[i].pos) {
                eff_flag(&_eff[i], 0); /* end sound */
            }
        } else {
            eff_pos(&_eff[i], 0);
        }
    }

    /* BGM */
    lock();
    if (_psg.play && _psg.notes) {
        if (0 == _psg.waitTime) {
            _psg.waitTime = getNextNote();
            if (0 == _psg.waitTime) {
                unlock();
                return; /* no data */
            }
        }
        if (_psg.mvol) {
            for (i = 0; i < (int)size; i += 2) {
                for (j = 0; j < 6; j++) {
                    if (_psg.ch[j].tone) {
                        _psg.ch[j].cur %= _psg.ch[j].tone[0];
                        wav = _psg.ch[j].tone[1 + _psg.ch[j].cur];
                        _psg.ch[j].cur += 2;
                        wav *= (_psg.ch[j].vol * _psg.mvol);
                        if (_psg.ch[j].keyOn) {
                            if (_psg.ch[j].count < _psg.ch[j].env1) {
                                _psg.ch[j].count++;
                                pw = (_psg.ch[j].count * 100) / _psg.ch[j].env1;
                            } else {
                                pw = 100;
                            }
                        } else {
                            if (_psg.ch[j].count < _psg.ch[j].env2) {
                                _psg.ch[j].count++;
                                pw = 100 - (_psg.ch[j].count * 100) / _psg.ch[j].env2;
                            } else {
                                pw = 0;
                            }
                        }
                        if (!_psg.ch[j].mute) {
                            bp = (short*)(&buf[i]);
                            wav = (wav * pw / 100);
                            if (_psg.ch[j].volumeRate != 100) {
                                wav *= _psg.ch[j].volumeRate;
                                wav /= 100;
                            }
                            wav += *bp;
                            if (32767 < wav)
                                wav = 32767;
                            else if (wav < -32768)
                                wav = -32768;
                            if (_psg.fade2) {
                                wav *= 100 - _psg.fade2;
                                wav /= 100;
                            }
                            (*bp) = (short)wav;
                            _psg.wav[j] = pw;
                        }
                        if (_psg.ch[j].pdown) {
                            _psg.ch[j].pcnt++;
                            if (_psg.ch[j].pdown < _psg.ch[j].pcnt) {
                                _psg.ch[j].pcnt = 0;
                                if (_psg.ch[j].toneK) {
                                    _psg.ch[j].toneK--;
                                }
                                setNote(j & 0xff, _psg.ch[j].toneT, _psg.ch[j].toneK);
                            }
                        }
                    }
                }
                if (_psg.volumeRate != 100) {
                    bp = (short*)(&buf[i]);
                    wav = (*bp) * _psg.volumeRate;
                    wav /= 100;
                    if (32767 < wav)
                        wav = 32767;
                    else if (wav < -32768)
                        wav = -32768;
                    (*bp) = (short)wav;
                }
                _psg.waitTime--;
                if (0 == _psg.waitTime) {
                    _psg.waitTime = getNextNote();
                    if (0 == _psg.waitTime) {
                        unlock();
                        return; /* no data */
                    }
                }
                /* fade out */
                if (_psg.fade2 && _psg.fade2 < 100) {
                    _psg.fcnt++;
                    if (1023 < _psg.fcnt) {
                        _psg.fcnt = 0;
                        _psg.fade2++;
                    }
                }
                if (_psg.fade) {
                    _psg.fcnt++;
                    if (_psg.fade < _psg.fcnt) {
                        _psg.mvol--;
                        _psg.fcnt = 0;
                    }
                }
            }
        }
    } else {
        for (i = 0; i < 6; i++) {
            _psg.wav[i] = 0;
        }
        _psg.stopped = 1;
    }
    unlock();
}

/*
 *----------------------------------------------------------------------------
 * set tone and tune
 *----------------------------------------------------------------------------
 */
static void setNote(unsigned char cn, unsigned char t, unsigned char n)
{
    n += _psg.addKey[cn];
    switch (t) {
        case 0: /* SANKAKU */
            _psg.ch[cn].tone = TONE1[n % 85];
            break;
        case 1: /* NOKOGIR */
            _psg.ch[cn].tone = TONE2[n % 85];
            break;
        case 2: /* KUKEI */
            _psg.ch[cn].tone = TONE3[n % 85];
            break;
        default: /* NOIZE */
            _psg.ch[cn].tone = TONE4[n % 85];
            break;
    }
}

/*
 *----------------------------------------------------------------------------
 * get next waittime
 *----------------------------------------------------------------------------
 */
static int getNextNote()
{
    int ret;
    if (_psg.notes[_psg.nidx].type == NTYPE_WAIT && 0 == _psg.notes[_psg.nidx].val) {
        return 0;
    }
    for (; NTYPE_WAIT != _psg.notes[_psg.nidx].type; _psg.nidx++) {
        switch (_psg.notes[_psg.nidx].type) {
            case NTYPE_ENV1: /* op1=ch, val=env1 */
                _psg.ch[_psg.notes[_psg.nidx].op1].env1 = _psg.notes[_psg.nidx].val;
                break;
            case NTYPE_ENV2: /* op1=ch, val=env1 */
                _psg.ch[_psg.notes[_psg.nidx].op1].env2 = _psg.notes[_psg.nidx].val;
                break;
            case NTYPE_VOL: /* op1=ch, val=vol */
                _psg.ch[_psg.notes[_psg.nidx].op1].vol = _psg.notes[_psg.nidx].val;
                break;
            case NTYPE_MVOL: /* val=mvol */
                _psg.mvol = _psg.notes[_psg.nidx].val;
                break;
            case NTYPE_KEYON: /* op1=ch, op2=tone, op3=key */
                _psg.ch[_psg.notes[_psg.nidx].op1].keyOn = 1;
                _psg.ch[_psg.notes[_psg.nidx].op1].count = 0;
                _psg.ch[_psg.notes[_psg.nidx].op1].cur = 0;
                _psg.ch[_psg.notes[_psg.nidx].op1].toneT = _psg.notes[_psg.nidx].op2;
                _psg.ch[_psg.notes[_psg.nidx].op1].toneK = _psg.notes[_psg.nidx].op3;
                setNote(_psg.notes[_psg.nidx].op1, _psg.notes[_psg.nidx].op2, _psg.notes[_psg.nidx].op3);
                break;
            case NTYPE_KEYOFF: /* op1=ch */
                _psg.ch[_psg.notes[_psg.nidx].op1].keyOn = 0;
                _psg.ch[_psg.notes[_psg.nidx].op1].count = 0;
                break;
            case NTYPE_PDOWN: /* op1=ch, val=Hz */
                _psg.ch[_psg.notes[_psg.nidx].op1].pdown = _psg.notes[_psg.nidx].val;
                _psg.ch[_psg.notes[_psg.nidx].op1].pcnt = 0;
                break;
            case NTYPE_JUMP: /* val=address */
                _psg.nidx = _psg.notes[_psg.nidx].val;
                _psg.loop++;
                _psg.timeP = _psg.timeI;
                break;
            case NTYPE_LABEL:
                break;
            default:
                return 0;
        }
    }
    ret = _psg.notes[_psg.nidx].val;
    if (ret) {
        _psg.nidx++;
    }
    _psg.timeP += ret;
    return ret;
}

/*
 *----------------------------------------------------------------------------
 * load a BGM file (direct)
 *----------------------------------------------------------------------------
 */
int bload_direct(unsigned char n, const char* name)
{
    FILE* fp;
    int size;
    fp = fopen(name, "rb");
    if (NULL == fp) {
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    if (size < 1) {
        fclose(fp);
        return -1;
    }
    fseek(fp, 0, SEEK_SET);
    _note[n] = (char*)malloc(size);
    if (NULL == _note[n]) {
        fclose(fp);
        return -1;
    }
    fread(_note[n], size, 1, fp);
    fclose(fp);
    _notelen[n] = (uLong)size;
    return 0;
}

/*
 *----------------------------------------------------------------------------
 * load a BGM from memory (direct)
 *----------------------------------------------------------------------------
 */
int bload_direct2(unsigned char n, const char* src, int size)
{
    _note[n] = (char*)malloc(size);
    if (NULL == _note[n]) {
        return -1;
    }
    memcpy(_note[n], src, size);
    _notelen[n] = (uLong)size;
    return 0;
}

/*
 *----------------------------------------------------------------------------
 * free a slot (direct)
 *----------------------------------------------------------------------------
 */
void bfree_direct(unsigned char n)
{
    vgs2_bstop();
    lock();
    if (_note[n]) {
        free(_note[n]);
        _notelen[n] = 0;
    }
    unlock();
}

/*
 *----------------------------------------------------------------------------
 * play BGM
 *----------------------------------------------------------------------------
 */
void vgs2_bplay(unsigned char n)
{
    uLong nblen;
    int i;
    vgs2_bstop();
    lock();
    memset(&_psg, 0, sizeof(_psg));
    _psg.volumeRate = 100;
    _psg.ch[0].volumeRate = 100;
    _psg.ch[1].volumeRate = 100;
    _psg.ch[2].volumeRate = 100;
    _psg.ch[3].volumeRate = 100;
    _psg.ch[4].volumeRate = 100;
    _psg.ch[5].volumeRate = 100;
    _psg.notes = _notebuf;
    nblen = (uLong)sizeof(_notebuf);
    uncompress((unsigned char*)_notebuf, &nblen, (const unsigned char*)_note[n], _notelen[n]);
    _psg.idxnum = nblen / sizeof(struct _NOTE);
    for (i = 0; i < _psg.idxnum; i++) {
        if (NTYPE_WAIT == _notebuf[i].type) {
            _psg.timeL += _notebuf[i].val;
        } else if (NTYPE_LABEL == _notebuf[i].type) {
            _psg.timeI = _psg.timeL;
        }
    }
    unlock();
    _psg.play = 1;
}

/*
 *----------------------------------------------------------------------------
 * stop BGM
 *----------------------------------------------------------------------------
 */
void vgs2_bstop()
{
    lock();
    if (_psg.play && _psg.notes) {
        _psg.stopped = 0;
        _psg.play = 0;
    }
    unlock();
}

/*
 *----------------------------------------------------------------------------
 * resume playing the BGM
 *----------------------------------------------------------------------------
 */
void vgs2_bresume()
{
    lock();
    if (0 == _psg.play) {
        _psg.play = 1;
    }
    unlock();
}

/*
 *----------------------------------------------------------------------------
 * fade out the BGM (2)
 *----------------------------------------------------------------------------
 */
void vgs2_bfade2()
{
    lock();
    _psg.fade2 = 1;
    unlock();
}

/*
 *----------------------------------------------------------------------------
 * chage key
 *----------------------------------------------------------------------------
 */
void vgs2_bkey(int n)
{
    int i;
    for (i = 0; i < 6; i++) {
        if (_psg.addOff[i] == 0) {
            _psg.addKey[i] = n;
        }
    }
}

/*
 *----------------------------------------------------------------------------
 * not chage key
 *----------------------------------------------------------------------------
 */
void vgs2_bkoff(int cn, int off)
{
    if (cn < 0 || 6 <= cn) return;
    _psg.addOff[cn] = off;
    if (off) {
        _psg.addKey[cn] = 0;
    }
}

/*
 *----------------------------------------------------------------------------
 * skip notes
 *----------------------------------------------------------------------------
 */
void vgs2_bjump(int sec)
{
    int hz = 0;
    if (NULL == _psg.notes) return;
    lock();
    _psg.timeP = 0;
    _psg.loop = 0;
    _psg.nidx = 0;
    while (0 < sec) {
        hz += getNextNote();
        while (22050 <= hz) {
            hz -= 22050;
            sec--;
        }
    }
    unlock();
}

/*
 *----------------------------------------------------------------------------
 * mute channel
 *----------------------------------------------------------------------------
 */
void vgs2_bmute(int ch)
{
    if (ch < 0 || 5 < ch) return;
    lock();
    _psg.ch[ch].mute = 1 - _psg.ch[ch].mute;
    unlock();
}

/*
 *----------------------------------------------------------------------------
 * master volume
 *----------------------------------------------------------------------------
 */
void vgs2_bmvol(int rate)
{
    if (rate < 0)
        rate = 0;
    else if (100 < rate)
        rate = 100;
    _psg.volumeRate = rate;
}

/*
 *----------------------------------------------------------------------------
 * channel volume
 *----------------------------------------------------------------------------
 */
void vgs2_bcvol(int ch, int rate)
{
    if (ch < 0 || 5 < ch) return;
    if (rate < 0)
        rate = 0;
    else if (100 < rate)
        rate = 100;
    _psg.ch[ch].volumeRate = rate;
}
