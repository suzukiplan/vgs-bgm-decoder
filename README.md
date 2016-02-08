# VGS BGM Decoder
- decodes: `VGS's .BGM file` to `PCM`
- platform free

## License
[The BSD 2-Clause License](https://github.com/suzukiplan/vgs-bgm-decoder/blob/master/LICENSE.txt)

## How to use
- Create Context
- Load BGM data (from file or memory)
  - _Get Meta Information (optional)_
  - Decode
  - _Get/Set Register (optional)_
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
- `path` : path of the .BGM or .vgs file
- `data` : pointer of memory area that BGM or .vgs file data stored.
- `size` : size of the .BGM or .vgs file

#### return value
- `0` : success
- `-1` : failed

## Get Meta Information
#### prototyping
```
struct VgsMetaHeader* vgsdec_get_meta_header(void* context);
struct VgsMetaData* vgsdec_get_meta_data(void* context, int index);
```

#### arguments
- `context` : context
- `index` : index of the meta data

#### return value
- NULL: if meta header or data is not exist.
- !NULL: meta header or data

## Decode (BASIC)
#### prototyping
```
void vgsdec_execute(void* context, void* buffer, size_t size);
```

#### arguments
- `context` : context
- `buffer` : stores decoded PCM data
- `size` : size of `buffer`

#### remarks
- this function stores decoded PCM data to the buffer.
- The format of the PCM are fixed by the following:
  - sampling rate: 22050Hz
  - bit rate: 16bit
  - channels: 1 (monoral)
- ex: 1 second = 44100 byte _(22050 * 16 * 1 / 8)_
- use to sequentially call

## Decode (ASYNC)
_Async decoder function provides only for the senior programmer that fully understanding both of multiple threading mechanism and `vgsdec.c`. Please DO NOT USE, if you cannot the both or the either._

#### prototyping
```
int vgsdec_async_start(void* context);
int vgsdec_async_enqueue(void* context, void* buffer, size_t size, void (*callback)(void* context, void* buffer, size_t size));
void vgsdec_async_stop(void* context);
```

- `vgsdec_async_start` function
  - start async decoder thread
- `vgsdec_async_enqueue` function
  - enqueue a decode task
- `callback` function:
  - called by async decoder thread after decoded.
- `vgsdec_async_stop` function
  - stop async decoder thread

#### arguments
- `context` : context
- `buffer` : input/output buffer
- `size` : size of `buffer`
- `callback` : function pointer of callback after decoded

#### return value
- `0` : success
- `-1` : failed

#### remarks
- you must not call `vgsdec_execute` while running async decoder thread.

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
|`VGSDEC_REG_KEY_{0~5}`|o|-|get the [scale value](#scale-value) of Ch{0~5}|
|`VGSDEC_REG_TONE_{0~5}`|o|-|get the [tone value](#tone-value) of Ch{0~5}|
|`VGSDEC_REG_VOL_{0~5}`|o|-|get the average volume of Ch{0~5}|
|`VGSDEC_REG_PLAYING`|o|-|current playing status: `0` = not playing, `1` = playing|
|`VGSDEC_REG_INDEX`|o|-|get the playing note-index|
|`VGSDEC_REG_LOOP_INDEX`|o|-|get the note index of the loop-label (acyclic song: -1)|
|`VGSDEC_REG_LENGTH`|o|-|get the count of the notes|
|`VGSDEC_REG_TIME`|o|o|get or set the playing time _(1sec = 22050)_|
|`VGSDEC_REG_LOOP_TIME`|o|-|get the time of the loop-label _(1sec = 22050)_|
|`VGSDEC_REG_TIME_LENGTH`|o|-|get the time-length _(1sec = 22050)_|
|`VGSDEC_REG_LOOP_COUNT`|o|-|get the loop-count|
|`VGSDEC_REG_RESET`|-|o|set none-zero: reset to the state after loading the data.|
|`VGSDEC_REG_FADEOUT`|-|o|set none-zero: start fadeout.|
|`VGSDEC_REG_FADEOUT_COUNT`|o|-|get [fadeout counter](#fadeout-counter)|
|`VGSDEC_REG_VOLUME_RATE_{0~5}`|o|o|get or set the channel volume rate (0~100)|
|`VGSDEC_REG_VOLUME_RATE`|o|o|get or set the master volume rate (0~100)|
|`VGSDEC_REG_SYNTHESIS_BUFFER`|o|o|set none-zero: does not clear the buffer when called `vgsdec_execute`|
|`VGSDEC_REG_KEYON_{0~5}`|o|-|get key-on status of Ch{0~5}: `0` = key-off, `1` = key-on|
|`VGSDEC_REG_MUTE_{0~5}`|o|o|get or set the mute of Ch{0~5}: `0` = sound, `1` = mute|
|`VGSDEC_REG_ADD_KEY_{0~5}`|o|o|get or set the [scale up/down](#scale-updown) of Ch{0~5}|

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

#### scale up/down
- set VGSDEC_REG_ADD_KEY = 5: C major -> F major
- set VGSDEC_REG_ADD_KEY = -1: C major -> B major
- please exclude rhythm channel _(but specific rhythm channel is impossible...)_

#### tone value
|value|name|
|---|---|
|0|Sin wave|
|1|Square ware|
|2|Saw wave|
|3|Noize wave|

#### fadeout counter
##### means of counter value
- `0` : noop
- `1` ~ `99` : in-progress
- `100` : finished

##### mechanism
- set `VGSDEC_REG_FADEOUT` --> `VGSDEC_REG_FADEOUT_COUNT` to be `1`
- `VGSDEC_REG_FADEOUT_COUNT` increments per __1024Hz__ _(about 0.04643990929705 seconds)_ , if not `0` or `100`
- `VGSDEC_REG_FADEOUT_COUNT` equals 100 --> `VGSDEC_REG_PLAYING` to be zero

## Release Context
#### prototyping
```
void vgsdec_release_context(void* context);
```

#### arguments
- `context` : context

## Build
### example decoder
```
$ cd example
$ make
```

### DLL (Windows only)
```
# nmake /f makedll.mak
```
