# Changes

## Version 1.09
- deleted modulator envelope because unused

## Version 1.08
- corrected warning in the 64bit platforms

## Version 1.07
- bugfix: crush when setting KOBUSHI mode

## Version 1.06
- added getter of the KOBUSHI mode
- bugfix: reset playing time when context_reset
- bugfix: reset time length before loading a song from memory

## Version 1.05
- bugfix: reset tone when context_reset
- added KOBUSHI mode

how to use:
```
vgsdec_set_value(context, VGSDEC_REG_KOBUSHI, 1);
```

_Kobushi occurs when the pitch of the singer's voice fluctuates irregularly within one scale degree: This compares with vibrato, which vibrates in a regular cycle._

## Version 1.04
- splits the declaration and code in miniz: https://github.com/suzukiplan/vgs-bgm-decoder/issues/31

## Version 1.03
- need include guard in vgsdec_internal.h: https://github.com/suzukiplan/vgs-bgm-decoder/issues/29

## Version 1.02
- correct the notation shaking: data -> buffer: https://github.com/suzukiplan/vgs-bgm-decoder/issues/27

## Version 1.01
- do not declare `#define __stdcall` in the vgsdec.h: https://github.com/suzukiplan/vgs-bgm-decoder/issues/25

## Version 1.00
- support async decoding: https://github.com/suzukiplan/vgs-bgm-decoder/issues/2
- performance optimizing: https://github.com/suzukiplan/vgs-bgm-decoder/issues/17 
- complete getter/setter: https://github.com/suzukiplan/vgs-bgm-decoder/issues/19
- added test: https://github.com/suzukiplan/vgs-bgm-decoder/issues/20

## Version 0.03 (beta)
- support windows: https://github.com/suzukiplan/vgs-bgm-decoder/issues/15 

## Version 0.02 (beta)
- support change volume rate: https://github.com/suzukiplan/vgs-bgm-decoder/issues/3
- support synthesis mode: https://github.com/suzukiplan/vgs-bgm-decoder/issues/7
- support loading .vgs file: https://github.com/suzukiplan/vgs-bgm-decoder/issues/8
- bugfix: https://github.com/suzukiplan/vgs-bgm-decoder/issues/4
- bugfix: https://github.com/suzukiplan/vgs-bgm-decoder/issues/11

## Version 0.01 (beta)
- first beta version
- there is a possibility of a destructive change

