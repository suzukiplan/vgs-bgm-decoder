# VGS BGM Decoder
- decodes: `VGS's .BGM file` to `PCM`
- platform free

## How to use (draft)
- Create Context
- Load BGM data (from file or memory)
  - Decode
  - Get/Set Register
- Release Context

## Create Context
#### prototyping
```
void* vgsdec_create_context();
```

#### return value
- `!NULL` : context
- `NULL` : error

## Load BGM data
#### prototyping
```
int vgsdec_load_bgm_from_file(void* context, const char* path);
int vgsdec_load_bgm_from_memory(void* context, void* data, size_t size);
```

#### arguments
- `context` : context
- `path` : path of the BGM file
- `data` : pointer of memory area that BGM file data stored.
- `size` : size of the BGM file

#### return value
- `0` : success
- `-1` : failed

## Decode
#### prototyping
```
void vgsdec_execute(void* context, void* buffer, size_t size);
```

#### arguments
- `context` : context
- `buffer` : stores decoded PCM data
- `size` : size of buffer

#### remarks
- this function stores decoded PCM data to the buffer.
- The format of the PCM are fixed by the following:
  - sampling rate: 22050Hz
  - bit rate: 16bit
  - channels: 1 (monoral)
- ex: 1 second = 44100 byte _(22050 * 16 * 1 / 8)_
- use to sequentially call

## Get/Set Register
#### prototyping
```
int vgsdec_get_value(void* context, int type);
void vgsdec_set_value(void* context, int type, int value);
```

#### arguments
- `context` : context
- `type` : register type
- `value` : register value

#### return value
- register value

#### type
|define|get|set|description|
|:---|:---:|:---:|:---|
|`VGSDEC_REG_KEY_{0~5}`|o|-|get the scale value of Ch{0~5}|
|`VGSDEC_REG_TONE_{0~5}`|o|-|get the tone value of Ch{0~5}|
|`VGSDEC_REG_VOL_{0~5}`|o|-|get the average volume of Ch{0~5}|
|`VGSDEC_REG_PLAYING`|o|-|current playing status: `0` = not playing, `1` = playing|
|`VGSDEC_REG_INDEX`|o|o|get or set the playing note-index|
|`VGSDEC_REG_LOOP_INDEX`|o|-|get the note index of the loop-label|
|`VGSDEC_REG_LENGTH`|o|-|get the count of the notes|
|`VGSDEC_REG_TIME`|o|o|get or set the playing time _(1sec = 22050)_|
|`VGSDEC_REG_LOOP_TIME`|o|-|get the time of the loop-label _(1sec = 22050)_|
|`VGSDEC_REG_TIME_LENGTH`|o|-|get the time-length _(1sec = 22050)_|
|`VGSDEC_REG_LOOP_COUNT`|o|-|get the loop-count|
|`VGSDEC_REG_RESET`|-|o|set none-zero: reset to the state after loading the data.|
|`VGSDEC_REG_FADEOUT`|-|o|set none-zero: start fadeout.|

#### scale value
|Octave|`A`|`A#`|`B`|`C`|`C#`|`D`|`D#`|`E`|`F`|`F#`|`G`|`G#`|
|:---|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
|0|0|1|2|3|4|5|6|7|8|9|10|11|
|1|12|13|14|15|16|17|18|19|20|21|22|23|
|2|24|25|26|27|28|29|30|31|32|33|34|35|
|3|36|37|38|39|40|41|42|43|44|45|46|47|
|4|48|49|50|51|52|53|54|55|56|57|58|59|
|5|60|61|62|63|64|65|66|67|68|69|70|71|
|6|72|73|74|75|76|77|78|79|80|81|82|83|
|7|84|-|-|-|-|-|-|-|-|-|-|-|

#### tone value
|value|name|
|0|Sin wave|
|1|Square ware|
|2|Saw wave|
|3|Noize wave|

## Release Context
#### prototyping
```
void vgsdec_release_context(void* context);
```

#### arguments
- `context` : context
